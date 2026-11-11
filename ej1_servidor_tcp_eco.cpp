// =================================================================
// Ejercicio 2.1 - Servidor TCP Eco Basico con manejo de excepciones
// Compilar: g++ -std=c++14 -Wall -o ej1_servidor ej1_servidor_tcp_eco.cpp
// =================================================================
#include <iostream>
#include <string>
#include <cstring>
#include <stdexcept>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>

class ServidorTCPEco {
private:
    int server_fd;
    int puerto;

public:
    ServidorTCPEco(int p) : server_fd(-1), puerto(p) {}

    void iniciar() {
        // 1. Crear Socket AF_INET (IPv4), SOCK_STREAM (TCP)
        server_fd = socket(AF_INET, SOCK_STREAM, 0);
        if (server_fd < 0) {
            throw std::runtime_error("Error critico: No se pudo crear el socket del servidor.");
        }

        // Permitir reutilizar la direccion IP/Puerto de inmediato
        int opt = 1;
        setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

        // 2. Configurar la estructura de direccion sockaddr_in
        sockaddr_in direccion;
        std::memset(&direccion, 0, sizeof(direccion));
        direccion.sin_family = AF_INET;
        direccion.sin_addr.s_addr = INADDR_ANY;   // Escuchar en cualquier interfaz de red
        direccion.sin_port = htons(puerto);       // Puerto a Byte Order de Red (Big-Endian)

        // 3. Vincular el socket con bind()
        if (bind(server_fd, (struct sockaddr*)&direccion, sizeof(direccion)) < 0) {
            close(server_fd);
            throw std::runtime_error("Error critico: Fallo el bind en el puerto " + std::to_string(puerto));
        }

        // 4. Habilitar modo de escucha pasiva con listen()
        if (listen(server_fd, 5) < 0) {
            close(server_fd);
            throw std::runtime_error("Error critico: Fallo la operacion listen.");
        }

        std::cout << "[SERVIDOR SMTI] Servidor TCP listo y escuchando en el puerto " << puerto << "...\n";

        // 5. Aceptar cliente entrante con accept()
        sockaddr_in cliente_addr;
        socklen_t addr_len = sizeof(cliente_addr);
        int client_fd = accept(server_fd, (struct sockaddr*)&cliente_addr, &addr_len);
        if (client_fd < 0) {
            close(server_fd);
            throw std::runtime_error("Error critico: No se pudo aceptar la conexion del cliente.");
        }
        std::cout << "[SERVIDOR SMTI] Cliente conectado con exito.\n";

        // 6. Leer mensaje del cliente
        char buffer[1024] = {0};
        ssize_t bytes_leidos = recv(client_fd, buffer, sizeof(buffer) - 1, 0);
        if (bytes_leidos > 0) {
            std::cout << "[MENSAJE RECIBIDO]: " << buffer << "\n";
            std::string respuesta = "ACK SMTI: Datos recibidos correctamente en el servidor.";
            send(client_fd, respuesta.c_str(), respuesta.length(), 0);
        }

        // 7. Cierre limpio de sockets
        close(client_fd);
        close(server_fd);
        std::cout << "[SERVIDOR SMTI] Sesion finalizada y sockets liberados.\n";
    }
};

int main() {
    try {
        ServidorTCPEco servidor(8080);
        servidor.iniciar();
    } catch (const std::exception& e) {
        std::cerr << "[EXCEPCION CAPTURADA]: " << e.what() << "\n";
    }
    return 0;
}
