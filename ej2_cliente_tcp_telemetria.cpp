// =================================================================
// Ejercicio 2.2 - Cliente TCP Emisor de Telemetria
// Compilar: g++ -std=c++14 -Wall -o ej2_cliente ej2_cliente_tcp_telemetria.cpp
// Probar: primero ./ej1_servidor y luego ./ej2_cliente
// =================================================================
#include <iostream>
#include <string>
#include <cstring>
#include <stdexcept>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>

class ClienteTCPTelemetria {
private:
    int sock_fd;
    std::string ip_servidor;
    int puerto;

public:
    ClienteTCPTelemetria(std::string ip, int p) : sock_fd(-1), ip_servidor(ip), puerto(p) {}

    void enviarTrama(std::string mensaje) {
        sock_fd = socket(AF_INET, SOCK_STREAM, 0);
        if (sock_fd < 0) {
            throw std::runtime_error("Error Cliente: No se pudo crear el socket.");
        }

        sockaddr_in serv_addr;
        std::memset(&serv_addr, 0, sizeof(serv_addr));
        serv_addr.sin_family = AF_INET;
        serv_addr.sin_port = htons(puerto);

        // Convertir direccion IP de formato texto a binario
        if (inet_pton(AF_INET, ip_servidor.c_str(), &serv_addr.sin_addr) <= 0) {
            close(sock_fd);
            throw std::runtime_error("Error Cliente: Direccion IP invalida o no soportada.");
        }

        // Conectarse al Servidor
        if (connect(sock_fd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
            close(sock_fd);
            throw std::runtime_error("Error Cliente: Fallo la conexion con el servidor en " + ip_servidor);
        }

        std::cout << "[CLIENTE SMTI] Conectado al servidor. Enviando muestra...\n";
        send(sock_fd, mensaje.c_str(), mensaje.length(), 0);

        char buffer[1024] = {0};
        ssize_t bytes = recv(sock_fd, buffer, sizeof(buffer) - 1, 0);
        if (bytes > 0) {
            std::cout << "[RESPUESTA SERVIDOR]: " << buffer << "\n";
        }
        close(sock_fd);
    }
};

int main() {
    try {
        ClienteTCPTelemetria cliente("127.0.0.1", 8080);
        cliente.enviarTrama("SENSOR_TEMP_01: 45.8 C | ESTADO: OK");
    } catch (const std::exception& e) {
        std::cerr << "[EXCEPCION CLIENTE]: " << e.what() << "\n";
    }
    return 0;
}
