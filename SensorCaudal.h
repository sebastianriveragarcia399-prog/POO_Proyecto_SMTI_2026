// =================================================================
// include/SensorCaudal.h
// CAPA 1: Clase derivada polimorfica (Unidad 8)
// =================================================================
#ifndef SENSOR_CAUDAL_H
#define SENSOR_CAUDAL_H

#include "Sensor.h"

class SensorCaudal : public Sensor {
private:
    double caudalMinimo;   // L/min (alerta si el flujo cae por debajo)

public:
    SensorCaudal(int _id, std::string _ub, double _lect, double _min = 5.0);

    std::string getTipo() const override;
    void procesarLectura() override;
};

#endif
