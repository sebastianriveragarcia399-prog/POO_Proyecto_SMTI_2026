# Guía de sustentación oral — Evaluación 3 (SMTI)

La rúbrica reserva parte del puntaje del criterio 4 a la **explicación oral de punteros y vtable**. Estas son las preguntas más probables, con respuestas apoyadas en el código del proyecto.

## 1. Sockets POSIX (criterio 1)

**¿Qué hace cada función y en qué orden se llaman en el servidor?**
`socket()` crea el descriptor → `bind()` lo asocia a IP:puerto (`sockaddr_in`) → `listen()` lo pone en modo pasivo con una cola de conexiones → `accept()` bloquea hasta que llega un cliente y devuelve un **nuevo** descriptor para esa sesión → `recv()`/`send()` intercambian bytes → `close()`. El cliente solo hace `socket()` → `connect()` → `send()`/`recv()` → `close()`.

**¿Para qué sirve `htons`?** Convierte el puerto al orden de bytes de red (Big-Endian). Sin eso un puerto 8080 se leería como otro número en una CPU Little-Endian.

**¿Por qué `recv()` no garantiza traer una trama completa?** TCP es un *flujo* de bytes, no de mensajes. Por eso `manejarCliente` acumula lo recibido en un `std::string` y solo procesa cuando hay `sizeof(MuestraSensor)` bytes (o una línea completa en modo texto).

**¿Qué pasa si `recv()` devuelve 0? ¿Y -1?** 0 = el cliente cerró la conexión (se sale del bucle). -1 = error.

**¿Qué hace `SO_REUSEADDR`?** Permite reiniciar el servidor sin esperar el estado TIME_WAIT del puerto. No permite dos servidores simultáneos: el segundo falla en `bind()` y se lanza la excepción (comprobado en las pruebas).

**TCP vs UDP:** TCP orientado a conexión (handshake de 3 vías, entrega ordenada y retransmisión) → alarmas y configuración. UDP por datagramas sin conexión (`sendto`/`recvfrom`, sin `listen`/`accept`) → lecturas masivas donde perder una muestra no es crítico.

## 2. Jerarquía polimórfica y vtable (criterio 2)

**¿Por qué `Sensor` es abstracta?** Porque tiene métodos virtuales puros (`procesarLectura() = 0` y `getTipo() = 0`); no se puede hacer `new Sensor(...)`.

**¿Por qué el destructor es virtual?** Los sensores se guardan como `Sensor*`. Si el destructor no fuera virtual, `delete ptr` solo ejecutaría `~Sensor()` y no el de la clase derivada → comportamiento indefinido / fuga. Con `virtual ~Sensor()` se destruye primero la parte derivada y luego la base (se ve en la salida `[DESTRUCTOR] Sensor ID ... liberado`).

**¿Qué es la vtable?** Cada clase con métodos virtuales tiene una tabla de punteros a función (una por método virtual). Cada objeto guarda un puntero oculto (*vptr*) a la tabla de **su clase real**. Al ejecutar `dato->procesarLectura()` el programa lee el vptr del objeto, busca la entrada en la tabla y salta a la función correcta: eso es el *dynamic binding* en tiempo de ejecución. Por eso el mismo recorrido de `ListaGen<Sensor*>` ejecuta código distinto para temperatura, presión o caudal.

**¿Qué hace `override`?** Pide al compilador verificar que realmente se está sobrescribiendo un virtual de la base (si la firma no coincide, error de compilación).

**¿Por qué `operator<<` es `friend`?** Necesita leer los atributos protegidos (`id`, `ubicacion`, `lectura`) pero no puede ser método de `Sensor` porque su operando izquierdo es `std::ostream`.

## 3. TAD genérico y Heap (criterio 3)

**¿Por qué `template <typename T>`?** Una sola implementación de la lista sirve para cualquier tipo (`ListaGen<int>`, `ListaGen<Sensor*>`). El compilador genera el código para cada `T` usado. Por eso la implementación vive en el `.h`.

**¿Dónde vive cada cosa?** Nodos y sensores se crean con `new` → **Heap** (persisten hasta el `delete`). La variable `ListaGen<Sensor*> listaPlanta` está en el **Stack** de `main`; al terminar su ámbito se ejecuta `~ListaGen()`.

**¿Cómo evita fugas el destructor?** Recorre la lista; por cada nodo hace `delete` del objeto apuntado (dispara el destructor virtual) y luego `delete` del nodo. Se verificó con `valgrind --leak-check=full`: *All heap blocks were freed*.

**¿Por qué se borró el constructor de copia?** La lista es dueña de punteros: copiarla duplicaría los punteros y provocaría **doble `delete`**.

**¿Por qué `liberarDato` con `std::is_pointer`?** Para que `ListaGen<int>` o `ListaGen<MuestraSensor>` también compilen: solo se hace `delete` del dato cuando `T` es un puntero.

**¿Por qué hay un `std::mutex`?** Cada cliente se atiende en un hilo y todos insertan en la misma lista; sin candado habría condición de carrera.

## 4. Excepciones (criterio 4)

**¿Dónde se lanzan y dónde se capturan?** Se lanzan con `throw` dentro de `ServidorSMTI` (`iniciar`, `atender`) cuando falla `socket`, `bind` (puerto ocupado), `listen` o `accept`. Se capturan en `main()` con `catch (const std::runtime_error&)` e imprimen un mensaje amigable. Las excepciones propias (`ErrorSocketSMTI`, `ErrorTramaSMTI`) heredan de `std::runtime_error`.

**¿Por qué se captura por referencia constante?** Evita copiar el objeto y el *slicing* (perder la parte derivada).

**¿Qué pasa con las tramas inválidas?** Se lanza `ErrorTramaSMTI` dentro del hilo del cliente y se captura ahí mismo (una excepción no puede cruzar el límite de un hilo): el servidor responde `NACK` y sigue funcionando.

**¿Se liberan los recursos si hay excepción?** Sí: `~ServidorSMTI()` cierra el socket y al desenrollarse la pila se destruye `listaPlanta` (RAII).

## 5. Demostración rápida en máquina

1. `make` y `./bin/SMTI 3`.
2. Tres clientes: `./bin/cliente_sensor 101 TEMP 92.5`, `... 102 PRESION 12.3`, `... 103 CAUDAL 3.2`.
3. Mostrar el recorrido polimórfico, la búsqueda por ID y los mensajes `[DESTRUCTOR]` al cierre.
4. Mostrar el error amigable: abrir un segundo servidor con el puerto 8080 ocupado.
5. `valgrind --leak-check=full ./bin/SMTI 3` → 0 fugas.

## 6. Referencias bibliográficas (formato de la guía)

- Comer, D. E. (2014). *Computer Networks and Internets* (6th ed.). Pearson Education.
- Deitel, P., & Deitel, H. (2017). *C++ How to Program* (10th ed.). Pearson Education.
- Kurose, J. F., & Ross, K. W. (2021). *Computer Networking: A Top-Down Approach* (8th ed.). Pearson.
- Lippman, S. B., Lajoie, J., & Moo, B. E. (2013). *C++ Primer* (5th ed.). Addison-Wesley.
- Stevens, W. R., Fenner, B., & Rudoff, A. M. (2004). *UNIX Network Programming, Volume 1: The Sockets Networking API* (3rd ed.). Addison-Wesley.
- Stroustrup, B. (2013). *The C++ Programming Language* (4th ed.). Addison-Wesley.
