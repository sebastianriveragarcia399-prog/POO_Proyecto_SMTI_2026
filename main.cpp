// =================================================================
// src/main.cpp  -  Punto de entrada integrador SMTI (Evaluacion 3)
// Sistema de Monitoreo y Telemetria Industrial
// Autores: Sebastian Rivera y Sebastian Quinones
// Uso: ./SMTI [numero_de_clientes=3]
// =================================================================
#include <iostream>
#include <stdexcept>
#include <cstdlib>

#include "Sensor.h"
#include "ListaGen.h"
#include "ServidorSockets.h"

int main(int argc, char* argv[]) {
    std::cout << "==================================================" << std::endl;
    std::cout << " SMTI: SERVIDOR DE TELEMETRIA DISTRIBUIDA (UNIDAD 9)" << std::endl;
    std::cout << "==================================================" << std::endl;

    int maxClientes = (argc > 1) ? std::atoi(argv[1]) : 3;
    if (maxClientes <= 0) maxClientes = 3;

    // Capa 2: lista generica de punteros polimorficos en el Heap
    ListaGen<Sensor*> listaPlanta;

    try {
        // Capa 3: servidor TCP en el puerto 8080
        ServidorSMTI servidor(8080, listaPlanta);
        servidor.iniciar();               // socket + bind + listen
        servidor.atender(maxClientes);    // accept + recv (un hilo por cliente)

        std::cout << "\n--- PROCESAMIENTO POLIMORFICO DE TELEMETRIA ("
                  << listaPlanta.tamano() << " muestras) ---" << std::endl;
        listaPlanta.procesarTodo();       // Dynamic Binding via vtable

        // Busqueda por ID en el TAD generico
        Sensor* s = listaPlanta.buscarPorId(101);
        if (s) std::cout << "\n[BUSQUEDA] Sensor 101 encontrado: " << *s << std::endl;

        std::cout << "\n--- CIERRE: ~ListaGen() liberara el Heap ---" << std::endl;
    } catch (const std::runtime_error& e) {
        // Fallas de socket / puerto ocupado: mensaje amigable
        std::cerr << "[EXCEPCION GRAVE SMTI] " << e.what() << std::endl;
        return 1;
    } catch (const std::exception& e) {
        std::cerr << "[ERROR INESPERADO] " << e.what() << std::endl;
        return 2;
    }

    return 0;   // Al salir de scope, ~ListaGen() libera la memoria Heap
}
