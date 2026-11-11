// =================================================================
// include/Sensor.h
// CAPA 1: Clase Base Abstracta Sensor (Unidad 8)
// Autores: Sebastian Rivera y Sebastian Quinones
// =================================================================
#ifndef SENSOR_H
#define SENSOR_H

#include <iostream>
#include <string>

class Sensor {
protected:
    int         id;
    std::string ubicacion;
    double      lectura;

public:
    Sensor(int _id, std::string _ub, double _lect);

    // Destructor VIRTUAL obligatorio: permite que "delete" sobre un
    // Sensor* destruya tambien la parte derivada (evita Memory Leaks).
    virtual ~Sensor();

    int                getId() const;
    double             getLectura() const;
    const std::string& getUbicacion() const;

    // Nombre del tipo de sensor (metodo virtual puro)
    virtual std::string getTipo() const = 0;

    // Metodo Virtual Puro -> convierte a Sensor en Clase Abstracta
    virtual void procesarLectura() = 0;

    // Sobrecarga de operador amiga
    friend std::ostream& operator<<(std::ostream& os, const Sensor& s);
};

#endif
