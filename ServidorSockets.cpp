// =================================================================
// src/ServidorSockets.cpp
// CAPA 3: Servidor TCP SMTI (Unidad 9)
// =================================================================
#include "ServidorSockets.h"
#include "ExcepcionesSMTI.h"
#include "SensorTemperatura.h"
#include "SensorPresion.h"
#include "SensorCaudal.h"

#include <cstring>
#include <iostream>
#include <sstream>
#include <string>
#include <thread>
#include <vector>

namespace {
const char* PREFIJO_TEXTO = "TRAMA:";   // Tramas de texto: "TRAMA:id;tipo;valor\n"
const size_t LONG_PREFIJO = 6;
}

ServidorSMTI::ServidorSMTI(int _puerto, ListaGen<Sensor*>& _lista)
    : server_fd(SOCKET_INVALIDO), puerto(_puerto), lista(_lista) {}

ServidorSMTI::~ServidorSMTI() { detener(); }

void ServidorSMTI::detener() {
    if (server_fd != SOCKET_INVALIDO) {
        cerrarSocket(server_fd);
        server_fd = SOCKET_INVALIDO;
    }
}

// -----------------------------------------------------------------
// 1) socket()  2) bind()  3) listen()
// -----------------------------------------------------------------
void ServidorSMTI::iniciar() {
    // 1. Crear socket AF_INET (IPv4), SOCK_STREAM (TCP)
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == SOCKET_INVALIDO) {
        throw ErrorSocketSMTI("No se pudo instanciar el socket del servidor.");
    }

#ifndef _WIN32
    // Reutilizar el puerto inmediatamente tras cerrar el servidor
    // (en Windows SO_REUSEADDR permite "robar" puertos, por eso se omite).
    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
#endif

    // 2. Configurar sockaddr_in y vincular con bind()
    sockaddr_in addr;
    std::memset(&addr, 0, sizeof(addr));
    addr.sin_family      = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);   // cualquier interfaz
    addr.sin_port        = htons(puerto);       // Big-Endian de red

    if (bind(server_fd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        detener();
        throw ErrorSocketSMTI("Enlace (bind) fallido en puerto " +
                              std::to_string(puerto) + " (puerto ocupado o sin permisos).");
    }

    // 3. Modo de escucha pasiva
    if (listen(server_fd, 5) < 0) {
        detener();
        throw ErrorSocketSMTI("Fallo la operacion listen().");
    }

    std::cout << "[SERVIDOR SMTI] Escuchando telemetria en puerto "
              << puerto << "..." << std::endl;
}

// -----------------------------------------------------------------
// 4) accept() en bucle + un hilo por cliente
// -----------------------------------------------------------------
void ServidorSMTI::atender(int maxClientes) {
    if (server_fd == SOCKET_INVALIDO) {
        throw ErrorSocketSMTI("El servidor no ha sido iniciado (llame a iniciar()).");
    }

    std::vector<std::thread> hilos;
    for (int i = 0; i < maxClientes; ++i) {
        sockaddr_in cliente_addr;
        socklen_t   addr_len = sizeof(cliente_addr);
        socket_t client_fd = accept(server_fd, (struct sockaddr*)&cliente_addr, &addr_len);
        if (client_fd == SOCKET_INVALIDO) {
            for (auto& h : hilos) h.join();   // no dejar hilos huerfanos
            throw ErrorSocketSMTI("No se pudo aceptar la conexion del cliente.");
        }
        {
            std::lock_guard<std::mutex> lk(candado);
            std::cout << "[SERVIDOR SMTI] Cliente " << (i + 1) << "/" << maxClientes
                      << " conectado." << std::endl;
        }
        hilos.emplace_back(&ServidorSMTI::manejarCliente, this, client_fd);
    }
    for (auto& h : hilos) h.join();
}

// -----------------------------------------------------------------
// 5) recv(): lectura de tramas de un cliente (se ejecuta en un hilo)
//    - Modo binario: struct MuestraSensor de tamano fijo.
//    - Modo texto  : lineas "TRAMA:id;tipo;valor".
//    TCP es un flujo de bytes: se acumula hasta tener tramas completas.
// -----------------------------------------------------------------
void ServidorSMTI::manejarCliente(socket_t client_fd) {
    std::string acumulado;
    char buffer[512];
    bool modoDecidido = false, modoTexto = false;

    auto responder = [&](const std::string& msg) {
        send(client_fd, msg.c_str(), (int)msg.length(), 0);
    };
    // Procesa una trama y responde ACK/NACK; las excepciones NO escapan del hilo.
    auto intentar = [&](auto&& accion) {
        try {
            accion();
            responder("ACK SMTI: Datos recibidos correctamente en el servidor.\n");
        } catch (const std::runtime_error& e) {
            {
                std::lock_guard<std::mutex> lk(candado);
                std::cerr << "[SERVIDOR SMTI] " << e.what() << std::endl;
            }
            responder(std::string("NACK SMTI: ") + e.what() + "\n");
        }
    };

    while (true) {
        ssize_red n = recv(client_fd, buffer, sizeof(buffer), 0);
        if (n < 0) {
            std::lock_guard<std::mutex> lk(candado);
            std::cerr << "[SERVIDOR SMTI] Error de lectura (recv) en un cliente." << std::endl;
            break;
        }
        if (n == 0) break;                       // cliente cerro la conexion
        acumulado.append(buffer, (size_t)n);

        if (!modoDecidido && acumulado.size() >= LONG_PREFIJO) {
            modoTexto = (acumulado.compare(0, LONG_PREFIJO, PREFIJO_TEXTO) == 0);
            modoDecidido = true;
        }
        if (!modoDecidido) continue;

        if (modoTexto) {
            size_t pos;
            while ((pos = acumulado.find('\n')) != std::string::npos) {
                std::string linea = acumulado.substr(0, pos);
                acumulado.erase(0, pos + 1);
                intentar([&] { procesarLinea(linea); });
            }
        } else {
            while (acumulado.size() >= sizeof(MuestraSensor)) {
                MuestraSensor m;
                std::memcpy(&m, acumulado.data(), sizeof(m));
                acumulado.erase(0, sizeof(m));
                intentar([&] { procesarMuestra(m); });
            }
        }
    }
    cerrarSocket(client_fd);
}

// -----------------------------------------------------------------
// Validacion e instanciacion dinamica (new) del objeto polimorfico
// -----------------------------------------------------------------
void ServidorSMTI::procesarMuestra(const MuestraSensor& m) {
    // Garantizar terminador nulo antes de construir el std::string
    size_t len = 0;
    while (len < sizeof(m.tipo) && m.tipo[len] != '\0') ++len;
    if (len == sizeof(m.tipo)) throw ErrorTramaSMTI("campo 'tipo' sin terminador nulo.");
    crearSensor(m.idSensor, std::string(m.tipo, len), m.valor);
}

void ServidorSMTI::procesarLinea(const std::string& linea) {
    if (linea.compare(0, LONG_PREFIJO, PREFIJO_TEXTO) != 0)
        throw ErrorTramaSMTI("la linea no inicia con 'TRAMA:'.");

    std::stringstream ss(linea.substr(LONG_PREFIJO));
    std::string sId, tipo, sValor;
    if (!std::getline(ss, sId, ';') || !std::getline(ss, tipo, ';') ||
        !std::getline(ss, sValor, ';'))
        throw ErrorTramaSMTI("se esperaba 'TRAMA:id;tipo;valor'.");

    int id; double valor;
    try {
        id    = std::stoi(sId);
        valor = std::stod(sValor);
    } catch (const std::exception&) {
        throw ErrorTramaSMTI("id o valor no numericos en '" + linea + "'.");
    }
    crearSensor(id, tipo, valor);
}

Sensor* ServidorSMTI::crearSensor(int id, const std::string& tipo, double valor) {
    if (id <= 0) throw ErrorTramaSMTI("idSensor debe ser positivo.");

    Sensor* s = nullptr;
    std::string ubic = "Nodo-" + std::to_string(id);
    if      (tipo == "TEMP")    s = new SensorTemperatura(id, ubic, valor);
    else if (tipo == "PRESION") s = new SensorPresion(id, ubic, valor);
    else if (tipo == "CAUDAL")  s = new SensorCaudal(id, ubic, valor);
    else throw ErrorTramaSMTI("tipo de sensor desconocido '" + tipo + "'.");

    std::lock_guard<std::mutex> lk(candado);   // la lista no es thread-safe
    lista.insertar(s);
    std::cout << "[SERVIDOR SMTI] Muestra recibida -> " << tipo << " ID " << id
              << " valor " << valor << " (en lista: " << lista.tamano() << ")" << std::endl;
    return s;
}
