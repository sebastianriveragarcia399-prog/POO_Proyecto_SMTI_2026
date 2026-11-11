#include "SensorCaudal.h"

SensorCaudal::SensorCaudal(int _id, std::string _ub, double _lect, double _min)
    : Sensor(_id, _ub, _lect), caudalMinimo(_min) {}

std::string SensorCaudal::getTipo() const { return "CAUDAL"; }

void SensorCaudal::procesarLectura() {
    std::cout << "[CAUDAL POLIMORFICO] " << *this << " L/min";
    if (lectura < caudalMinimo) {
        std::cout << " --> ALERTA: CAUDAL BAJO MINIMO!";
    }
    std::cout << std::endl;
}
