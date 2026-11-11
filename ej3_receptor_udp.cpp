// =================================================================
// Ejercicio 2.3 (complemento) - Receptor UDP con recvfrom()
// No viene en la guia: se agrega para poder PROBAR el emisor.
// Compilar: g++ -std=c++14 -Wall -o ej3_receptor ej3_receptor_udp.cpp
// Orden: 1) ./ej3_receptor   2) ./ej3_emisor
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

int main() {
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        std::cerr << "Error al crear socket UDP\n";
        return 1;
    }

    sockaddr_in local;
    std::memset(&local, 0, sizeof(local));
    local.sin_family = AF_INET;
    local.sin_addr.s_addr = INADDR_ANY;
    local.sin_port = htons(9090);

    // UDP no usa listen()/accept(): basta con bind() y recvfrom()
    if (bind(sockfd, (struct sockaddr*)&local, sizeof(local)) < 0) {
        std::cerr << "Error en bind (puerto 9090 ocupado?)\n";
        close(sockfd);
        return 1;
    }
    std::cout << "[RECEPTOR UDP] Esperando datagrama en el puerto 9090...\n";

    TramaUDP datos;
    sockaddr_in origen;
    socklen_t len = sizeof(origen);
    ssize_t n = recvfrom(sockfd, &datos, sizeof(datos), 0, (struct sockaddr*)&origen, &len);
    if (n == (ssize_t)sizeof(datos)) {
        char ip[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &origen.sin_addr, ip, sizeof(ip));
        std::cout << "[RECEPTOR UDP] Desde " << ip << " -> Sensor " << datos.idSensor
                  << " lectura " << datos.lectura << "\n";
    } else {
        std::cerr << "Datagrama incompleto o error (" << n << " bytes)\n";
    }
    close(sockfd);
    return 0;
}
