// =================================================================
// Ejercicio 2.3 - Emision UDP de Datagramas Industriales
// Compilar: g++ -std=c++14 -Wall -o ej3_emisor ej3_emisor_udp.cpp
// (el receptor para probarlo esta en ej3_receptor_udp.cpp)
// =================================================================
#include <iostream>
#include <cstring>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

struct TramaUDP {
    int idSensor;
    double lectura;
};

// Emisor UDP de Telemetria
int main() {
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        std::cerr << "Error al crear socket UDP\n";
        return 1;
    }

    sockaddr_in dest_addr;
    std::memset(&dest_addr, 0, sizeof(dest_addr));
    dest_addr.sin_family = AF_INET;
    dest_addr.sin_port = htons(9090);
    inet_pton(AF_INET, "127.0.0.1", &dest_addr.sin_addr);

    TramaUDP datos = {101, 88.4};

    // Transmision directa sin handshake previo
    sendto(sockfd, &datos, sizeof(datos), 0, (struct sockaddr*)&dest_addr, sizeof(dest_addr));
    std::cout << "[EMISOR UDP] Trama enviada correctamente al puerto 9090.\n";

    close(sockfd);
    return 0;
}
