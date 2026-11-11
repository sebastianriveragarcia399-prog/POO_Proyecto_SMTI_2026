// =================================================================
// include/PlataformaRed.h
// Capa de compatibilidad minima: POSIX (Linux, entorno oficial de la
// evaluacion) y Winsock (Windows + ZinjaI/MinGW, para practicar en casa).
// En Linux usa exactamente las funciones POSIX de la guia.
// =================================================================
#ifndef PLATAFORMA_RED_H
#define PLATAFORMA_RED_H

#ifdef _WIN32
    #include <winsock2.h>
    #include <ws2tcpip.h>
    typedef SOCKET socket_t;
    typedef int    ssize_red;
    #define SOCKET_INVALIDO INVALID_SOCKET
    inline void cerrarSocket(socket_t s) { closesocket(s); }
    // Winsock exige WSAStartup antes de usar sockets (RAII)
    struct InicioRed {
        InicioRed()  { WSADATA w; WSAStartup(MAKEWORD(2, 2), &w); }
        ~InicioRed() { WSACleanup(); }
    };
#else
    #include <unistd.h>
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <arpa/inet.h>
    typedef int     socket_t;
    typedef ssize_t ssize_red;
    #define SOCKET_INVALIDO (-1)
    inline void cerrarSocket(socket_t s) { close(s); }
    struct InicioRed { InicioRed() {} };   // En POSIX no hace falta inicializar nada
#endif

#endif
