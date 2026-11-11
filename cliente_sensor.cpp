// =================================================================
// clientes/cliente_sensor.cpp
// Nodo sensor de prueba: envia UNA muestra al servidor SMTI.
// Uso:  cliente_sensor <id> <TEMP|PRESION|CAUDAL> <valor> [bin|texto] [ip] [puerto]
// Ej.:  cliente_sensor 101 TEMP 92.5
//       cliente_sensor 102 PRESION 12.3 texto
// =================================================================
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <stdexcept>
#include <string>

#include "PlataformaRed.h"
#include "MuestraSensor.h"

static void enviarMuestra(const std::string& ip, int puerto, int id,
                          const std::string& tipo, double valor, bool texto) {
    InicioRed inicio;
    socket_t sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == SOCKET_INVALIDO) throw std::runtime_error("Cliente: no se pudo crear el socket.");

    sockaddr_in serv;
    std::memset(&serv, 0, sizeof(serv));
    serv.sin_family = AF_INET;
    serv.sin_port   = htons(puerto);
    if (inet_pton(AF_INET, ip.c_str(), &serv.sin_addr) <= 0) {
        cerrarSocket(sock);
        throw std::runtime_error("Cliente: direccion IP invalida.");
    }
    if (connect(sock, (struct sockaddr*)&serv, sizeof(serv)) < 0) {
        cerrarSocket(sock);
        throw std::runtime_error("Cliente: fallo la conexion con " + ip + ":" + std::to_string(puerto) +
                                 " (el servidor esta corriendo?).");
    }

    if (texto) {
        std::string linea = "TRAMA:" + std::to_string(id) + ";" + tipo + ";" +
                            std::to_string(valor) + "\n";
        send(sock, linea.c_str(), (int)linea.length(), 0);
    } else {
        MuestraSensor m;
        std::memset(&m, 0, sizeof(m));
        m.idSensor = id;
        m.valor    = valor;
        std::strncpy(m.tipo, tipo.c_str(), sizeof(m.tipo) - 1);
        send(sock, (const char*)&m, (int)sizeof(m), 0);
    }

    char buffer[256] = {0};
    ssize_red n = recv(sock, buffer, sizeof(buffer) - 1, 0);
    if (n > 0) std::cout << "[RESPUESTA SERVIDOR]: " << buffer;
    cerrarSocket(sock);
}

int main(int argc, char* argv[]) {
    if (argc < 4) {
        std::cerr << "Uso: " << argv[0]
                  << " <id> <TEMP|PRESION|CAUDAL> <valor> [bin|texto] [ip] [puerto]\n";
        return 1;
    }
    try {
        int id          = std::atoi(argv[1]);
        std::string tp  = argv[2];
        double valor    = std::atof(argv[3]);
        bool texto      = (argc > 4 && std::string(argv[4]) == "texto");
        std::string ip  = (argc > 5) ? argv[5] : "127.0.0.1";
        int puerto      = (argc > 6) ? std::atoi(argv[6]) : 8080;
        enviarMuestra(ip, puerto, id, tp, valor, texto);
    } catch (const std::exception& e) {
        std::cerr << "[EXCEPCION CLIENTE]: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}
