// =================================================================
// include/ServidorSockets.h
// CAPA 3: Servidor TCP de telemetria (Unidad 9)
// Clase ServidorSMTI: socket(), bind(), listen(), accept(), recv()
// =================================================================
#ifndef SERVIDOR_SOCKETS_H
#define SERVIDOR_SOCKETS_H

#include <mutex>
#include <string>
#include "PlataformaRed.h"
#include "ListaGen.h"
#include "Sensor.h"
#include "MuestraSensor.h"

class ServidorSMTI {
private:
    InicioRed          inicioRed;     // WSAStartup en Windows, nada en Linux
    socket_t           server_fd;
    int                puerto;
    ListaGen<Sensor*>& lista;         // Lista generica en el Heap (Capa 2)
    std::mutex         candado;       // Protege la lista entre hilos

    void    manejarCliente(socket_t client_fd);
    void    procesarMuestra(const MuestraSensor& m);
    void    procesarLinea(const std::string& linea);
    Sensor* crearSensor(int id, const std::string& tipo, double valor);

public:
    ServidorSMTI(int _puerto, ListaGen<Sensor*>& _lista);
    ~ServidorSMTI();

    ServidorSMTI(const ServidorSMTI&)            = delete;
    ServidorSMTI& operator=(const ServidorSMTI&) = delete;

    // socket() + bind() + listen(); lanza std::runtime_error si falla
    void iniciar();

    // Acepta 'maxClientes' conexiones (un hilo por cliente) y espera a
    // que todos terminen. Lanza std::runtime_error si accept() falla.
    void atender(int maxClientes);

    void detener();
};

#endif
