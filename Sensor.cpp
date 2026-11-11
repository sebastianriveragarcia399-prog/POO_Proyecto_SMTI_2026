#include "Sensor.h"

Sensor::Sensor(int _id, std::string _ub, double _lect)
    : id(_id), ubicacion(_ub), lectura(_lect) {}

// Destructor Virtual: se ejecuta al hacer delete sobre un Sensor*
Sensor::~Sensor() {
    std::cout << "[DESTRUCTOR] Sensor ID " << id << " liberado del Heap." << std::endl;
}

int                Sensor::getId() const        { return id; }
double             Sensor::getLectura() const   { return lectura; }
const std::string& Sensor::getUbicacion() const { return ubicacion; }

std::ostream& operator<<(std::ostream& os, const Sensor& s) {
    os << "Sensor ID: " << s.id << " | Ubicacion: " << s.ubicacion
       << " | Valor: " << s.lectura;
    return os;
}
