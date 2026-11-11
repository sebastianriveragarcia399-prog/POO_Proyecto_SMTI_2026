#include "SensorPresion.h"

SensorPresion::SensorPresion(int _id, std::string _ub, double _lect, double _max)
    : Sensor(_id, _ub, _lect), presionMaxima(_max) {}

std::string SensorPresion::getTipo() const { return "PRESION"; }

void SensorPresion::procesarLectura() {
    std::cout << "[PRESION POLIMORFICO] " << *this << " bar";
    if (lectura > presionMaxima) {
        std::cout << " --> ALERTA: SOBREPRESION EN LINEA!";
    }
    std::cout << std::endl;
}
