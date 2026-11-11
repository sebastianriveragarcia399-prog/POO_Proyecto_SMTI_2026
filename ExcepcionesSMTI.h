// =================================================================
// include/ExcepcionesSMTI.h
// CAPA 3: Excepciones del sistema (Unidad 9)
// Todas derivan de std::runtime_error, por lo que pueden capturarse
// con catch (const std::runtime_error&) o catch (const std::exception&).
// =================================================================
#ifndef EXCEPCIONES_SMTI_H
#define EXCEPCIONES_SMTI_H

#include <stdexcept>
#include <string>

// Falla de infraestructura de red (socket, bind, listen, accept...)
class ErrorSocketSMTI : public std::runtime_error {
public:
    explicit ErrorSocketSMTI(const std::string& msg)
        : std::runtime_error("Error POSIX: " + msg) {}
};

// Trama recibida con formato invalido
class ErrorTramaSMTI : public std::runtime_error {
public:
    explicit ErrorTramaSMTI(const std::string& msg)
        : std::runtime_error("Trama invalida: " + msg) {}
};

#endif
