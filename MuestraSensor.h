// =================================================================
// include/MuestraSensor.h
// Estructura de Trama Industrial (Requerimiento 1 - Evaluacion 3)
// Autores: Sebastian Rivera y Sebastian Quinones
// =================================================================
#ifndef MUESTRASENSOR_H
#define MUESTRASENSOR_H

// Trama binaria que viaja por el socket TCP entre el nodo sensor
// (cliente) y el servidor SMTI. El cliente y el servidor DEBEN
// compilarse con esta misma cabecera para que el layout en memoria
// (incluido el padding del compilador) sea identico.
struct MuestraSensor {
    int    idSensor;   // Identificador unico del sensor
    double valor;      // Lectura medida
    char   tipo[12];   // "TEMP", "PRESION" o "CAUDAL" (terminado en '\0')
};

#endif
