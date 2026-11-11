// =================================================================
// include/SensorPresion.h
// CAPA 1: Clase derivada polimorfica (Unidad 8)
// =================================================================
#ifndef SENSOR_PRESION_H
#define SENSOR_PRESION_H

#include "Sensor.h"

class SensorPresion : public Sensor {
private:
    double presionMaxima;   // bar

public:
    SensorPresion(int _id, std::string _ub, double _lect, double _max = 10.0);

    std::string getTipo() const override;
    void procesarLectura() override;
};

#endif
