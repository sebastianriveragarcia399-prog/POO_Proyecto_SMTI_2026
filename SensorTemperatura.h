// =================================================================
// include/SensorTemperatura.h
// CAPA 1: Clase derivada polimorfica (Unidad 8)
// =================================================================
#ifndef SENSOR_TEMPERATURA_H
#define SENSOR_TEMPERATURA_H

#include "Sensor.h"

class SensorTemperatura : public Sensor {
private:
    double umbralMaximo;   // Grados Celsius

public:
    SensorTemperatura(int _id, std::string _ub, double _lect, double _umbral = 80.0);

    std::string getTipo() const override;
    void procesarLectura() override;
};

#endif
