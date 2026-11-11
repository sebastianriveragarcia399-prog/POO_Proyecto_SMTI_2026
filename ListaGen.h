// =================================================================
// include/ListaGen.h
// CAPA 2: TAD Lista Enlazada Generica con Templates (Unidades 7 y 9)
// Memoria dinamica en el Heap + destructor que purga todo.
// =================================================================
#ifndef LISTAGEN_H
#define LISTAGEN_H

#include <iostream>
#include <stdexcept>
#include <type_traits>

template <typename T>
struct NodoGen {
    T            dato;
    NodoGen<T>*  siguiente;
    NodoGen(T _d) : dato(_d), siguiente(nullptr) {}
};

template <typename T>
class ListaGen {
private:
    NodoGen<T>* head;
    int         cantidad;

    // Si T es un puntero (ej. Sensor*), libera el objeto apuntado en el Heap.
    static void liberarDato(T& d, std::true_type)  { delete d; }
    // Si T es un valor (ej. int, MuestraSensor), no hay nada que liberar.
    static void liberarDato(T&, std::false_type)   {}

public:
    ListaGen() : head(nullptr), cantidad(0) {}

    // Una lista duena de punteros NO debe copiarse (doble delete).
    ListaGen(const ListaGen&)            = delete;
    ListaGen& operator=(const ListaGen&) = delete;

    // Destructor Generico: purga total del Heap
    ~ListaGen() {
        NodoGen<T>* actual = head;
        while (actual != nullptr) {
            NodoGen<T>* aux = actual->siguiente;
            liberarDato(actual->dato, std::is_pointer<T>());  // objeto apuntado
            delete actual;                                    // el nodo
            actual = aux;
        }
        head = nullptr;
        cantidad = 0;
    }

    // Insercion al inicio en ListaGen<T>
    void insertar(T nuevoDato) {
        NodoGen<T>* nuevo = new NodoGen<T>(nuevoDato);
        nuevo->siguiente = head;
        head = nuevo;
        ++cantidad;
    }

    int tamano() const { return cantidad; }
    bool vacia() const { return head == nullptr; }

    // Recorrido polimorfico de la lista (solo para T = puntero a clase
    // con procesarLectura()). Al ser template, solo se instancia si se usa.
    void procesarTodo() {
        NodoGen<T>* actual = head;
        while (actual != nullptr) {
            actual->dato->procesarLectura();   // Dynamic Binding (vtable)
            actual = actual->siguiente;
        }
    }

    // Busqueda generica: devuelve el primer dato que cumpla el predicado
    // o nullptr si no existe (T debe ser puntero).
    template <typename Pred>
    T buscarSi(Pred cumple) const {
        for (NodoGen<T>* p = head; p != nullptr; p = p->siguiente)
            if (cumple(p->dato)) return p->dato;
        return nullptr;
    }

    // Busqueda por ID (T debe ser puntero a clase con getId()).
    T buscarPorId(int id) const {
        return buscarSi([id](T d) { return d->getId() == id; });
    }
};

#endif
