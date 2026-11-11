# Evaluación 3 — Proyecto Integrador SMTI

**Curso:** Programación Orientada a Objetos en C++ — 2026-2
**Docente:** Ing. Paola Andrea Bedoya Toro — Universidad del Cauca
**Estudiantes:** Sebastian Rivera · Sebastian Quiñones
**Tema:** Unidad 9 — Sockets TCP/UDP, Programación Genérica y Excepciones

## Contenido del entregable

```
Entregable/
├── README.md                      Este archivo
├── Proyecto_SMTI/                 Proyecto integrador (Sección 3 de la guía)
│   ├── include/                   Cabeceras (.h)
│   │   ├── MuestraSensor.h        struct MuestraSensor { int idSensor; double valor; char tipo[12]; }
│   │   ├── Sensor.h               Capa 1: clase base abstracta (virtual ~Sensor, virtual puro, operator<<)
│   │   ├── SensorTemperatura.h    Capa 1: derivada con override
│   │   ├── SensorPresion.h        Capa 1: derivada con override
│   │   ├── SensorCaudal.h         Capa 1: derivada con override
│   │   ├── ListaGen.h             Capa 2: template <typename T> class ListaGen (Heap, destructor limpio)
│   │   ├── ServidorSockets.h      Capa 3: class ServidorSMTI (socket, bind, listen, accept, recv)
│   │   ├── ExcepcionesSMTI.h      Capa 3: excepciones derivadas de std::runtime_error
│   │   └── PlataformaRed.h        Compatibilidad Linux (POSIX) / Windows (Winsock)
│   ├── src/                       Implementación (.cpp) + main.cpp
│   ├── clientes/cliente_sensor.cpp  Nodo sensor de prueba (envía tramas al servidor)
│   ├── Makefile                   Compilación en Linux/GCC
│   └── prueba.sh                  Prueba automática servidor + 5 clientes
├── Ejercicios_Resueltos/          Sección 2 de la guía (revisados y compilados)
│   ├── ej1_servidor_tcp_eco.cpp   2.1 Servidor TCP eco con excepciones
│   ├── ej2_cliente_tcp_telemetria.cpp   2.2 Cliente TCP emisor
│   ├── ej3_emisor_udp.cpp         2.3 Emisor UDP (sendto)
│   └── ej3_receptor_udp.cpp       2.3 Receptor UDP (recvfrom) para probar el emisor
└── Documentacion/
    └── SUSTENTACION.md            Preguntas orales, explicación de vtable, referencias APA
```

## Cómo compilar y ejecutar (Linux / GCC — entorno oficial de la evaluación)

```bash
cd Proyecto_SMTI
make                      # genera bin/SMTI y bin/cliente_sensor
./bin/SMTI 3              # servidor en puerto 8080, atiende 3 clientes y procesa

# En OTRA terminal:
./bin/cliente_sensor 101 TEMP 92.5
./bin/cliente_sensor 102 PRESION 12.3 texto     # trama de texto
./bin/cliente_sensor 103 CAUDAL 3.2
```

Prueba automática (servidor + 5 clientes, uno con trama inválida): `bash prueba.sh`.
Verificación de memoria: `valgrind --leak-check=full ./bin/SMTI 3`.

## Uso en ZinjaI

1. **Archivo → Nuevo → Proyecto** (C++), nómbrelo `SMTI` y agregue los archivos de `src/` y `include/`.
2. **Proyecto → Configuración → Opciones de compilación:** estándar `c++14`, directorio de includes `include`, y la opción `-pthread` (el servidor usa `std::thread` y `std::mutex`).
3. En **Windows** además enlace la librería `ws2_32` (`-lws2_32`). Para `std::thread`/`std::mutex` en MinGW instale la actualización que indica la profesora (**Actualización Zinjai uso de hilos-Socket**):
   1. Descargar `zinjai-add-mingw64-gcc7-win-20180222.zcp` (SourceForge).
   2. Arrastrarlo a la ventana principal de ZinjaI y dar permisos de administrador.
   3. Cerrar y abrir ZinjaI de nuevo.
   4. *Archivo → Preferencias… → Programa/Proyecto →* herramientas de compilación = **mingw64-gcc7** → Aceptar.
   5. Ejecutar el código.
4. La guía menciona un archivo `SMTI.zpr`; ZinjaI lo genera solo al crear el proyecto en el paso 1 (no se incluye uno hecho a mano porque no se pudo probar ZinjaI desde el entorno donde se preparó este entregable).

> La evaluación es sobre **Linux**. La capa `PlataformaRed.h` permite practicar en Windows, pero esa ruta (Winsock) **no pudo probarse** al preparar el entregable; en Linux todo fue compilado con `-Wall -Wextra` sin avisos y probado.

## Cumplimiento de los requerimientos de la guía

| Requerimiento (guía §3.2) | Dónde está |
|---|---|
| `struct MuestraSensor { int idSensor; double valor; char tipo[12]; }` | `include/MuestraSensor.h` |
| `template <typename T> class ListaGen` en el Heap, destructor limpio | `include/ListaGen.h` (`~ListaGen`, `liberarDato`) |
| `ServidorSMTI` en puerto 8080: `socket()`, `bind()`, `listen()`, `recv()` | `src/ServidorSockets.cpp` (`iniciar`, `atender`, `manejarCliente`) |
| Excepciones con `throw std::runtime_error` y captura en `main()` | `ExcepcionesSMTI.h`, `ServidorSockets.cpp`, `main.cpp` (`catch`) |
| Capa 1: abstracta, `virtual ~Sensor()`, virtual puro, `operator<<` friend, derivadas con `override` | `Sensor.*`, `SensorTemperatura/Presion/Caudal.*` |
| Capa 2: `ListaGen<Sensor*>`, recorrido, búsqueda por ID, purga en destructor | `ListaGen.h` (`procesarTodo`, `buscarPorId`) |
| Capa 3: recibir trama, validar, `new SensorX(...)` e insertar en la lista | `ServidorSMTI::crearSensor` |
| Encarpetado `include/` y `src/` | `Proyecto_SMTI/` |
| Ejercicios 2.1, 2.2, 2.3 | `Ejercicios_Resueltos/` |
| Referencias bibliográficas | `Documentacion/SUSTENTACION.md` |

## Protocolo de tramas del servidor

- **Binaria (por defecto):** el cliente envía la `struct MuestraSensor` completa (`tipo` = `TEMP`, `PRESION` o `CAUDAL`).
- **Texto:** líneas `TRAMA:id;tipo;valor\n`, por ejemplo `TRAMA:102;PRESION;12.3`.
- El servidor responde `ACK ...` si la trama es válida o `NACK ...` si no lo es (tipo desconocido, id ≤ 0, formato incorrecto).

## Nota sobre el alcance

La guía pide un servidor que reciba tramas por `recv()`; el esqueleto de la profesora solo simula la recepción. Aquí se implementó la recepción real, con un hilo por cliente y un `std::mutex` que protege la lista (basado en la actualización de ZinjaI sobre hilos que adjuntó la profesora). Si en la evaluación se pide la versión más simple (un solo `accept()` + `recv()` sin hilos), basta con llamar `atender(1)`: `ServidorSMTI` ya funciona así.
