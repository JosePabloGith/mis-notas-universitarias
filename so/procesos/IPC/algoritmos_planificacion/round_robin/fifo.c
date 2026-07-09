/*
 * aqui modelaremos el algoritmo FIFO
 *
 * */
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

// para representar cada nodo de la cola
struct Nodo {
  int dato;
  // apuntador a la siguiente estructura
  // nos ayudara a enzar los nodos de la cola
  struct Nodo *siguiente;
};

// para representar la cola
// son datos de control
struct Cola {
  // apuntamos a la primera vagoneta de la cola
  struct Nodo *frente;
  // apuntamos a la ultima vagoneta de la cola
  struct Nodo *final;
};

// reservamos memoria para la cola y la inicializamos
struct Cola *crear_cola() {
  struct Cola *cola = (struct Cola *)malloc(sizeof(struct Cola));
  cola->frente = NULL;
  cola->final = NULL;
  return cola;
};

// si se cumple retorna true
// usamos puntero para controlar el dato real de la cola
int esta_vacia(struct Cola *cola) {
  // si la primera vagoneta es nula, implica que no hay elementos en la cola
  return (cola->frente == NULL);
}

void encolar(struct Cola *cola, int dato) {
  // creamos un nuevo nodo
  // debemos de asegurar que el espacio reservado por malloc sea del tipo Nodo
  // NOTA: al encolar, los vagones que ya estan en la cola
  //       no se tocan. Solo cramos uno nuevo
  //       hacemos que el ultimo vagon actual
  //       apunte a él y luego movemos el marcador de final
  struct Nodo *nuevoNodo = (struct Nodo *)malloc(sizeof(struct Nodo));

  // metemos el número que queremos encolar
  nuevoNodo->dato = dato;
  // como sera el ultimo vagon, el siguiente debe de ser nulo
  nuevoNodo->siguiente = NULL;

  // si la cola esta vacia, lo metemos en el primer lugar
  if (esta_vacia(cola)) {
    // cola->frente = cola->final = nuevoNodo;
    cola->final = nuevoNodo;
    cola->frente = nuevoNodo;

    // NOTE: el vagón, tiene un engnache trasero representado por
    //       el campo siguiente.
    //
    // BUG: frente y final no son enganches, son etiquetas que
    //      señalan a vagones completos.

  } else {
    // aquí implica que la cola tiene elementos
    //  cola->final está apuntando al último vagon actual
    //  tenemos que enganches el nuevo vagon al final
    //  y luego actualizamos el final
    cola->final->siguiente = nuevoNodo;
    cola->final = nuevoNodo;
  }
}

int desencolar(struct Cola *cola) {

  // verificamos primero si hay algo que se pueda desencolar
  if (esta_vacia(cola)) {
    printf("La cola esta vacia\n");
    return -1;

  } else {
    // se encontro almenos un elemento
    // cola->frente apunta a la primera estructura de la cola
    // es al que tenemos que eliminar
    // PRIMERO EN ENTRAR PRIMERO EN SALIR
    struct Nodo *temp = cola->frente;
    int dato = temp->dato;

    // actualizamos el frente de la cola
    cola->frente = cola->frente->siguiente;
    // liberamos la memoria del nodo
    free(temp);
    return dato;
  }
}

void imprimir_cola(struct Cola *cola) {
  struct Nodo *actual = cola->frente;
  printf("*********\n");
  printf("\n***Elementos de la cola***\n");
  while (actual != NULL) {
    printf("%d\n", actual->dato);
    actual = actual->siguiente;
  }
  printf("*********\n");
}

int main() {
  struct Cola *cola = crear_cola();
  int operacion, dato;

  do {
    printf("MENU\n");
    printf("1. añadir un elemento a la cola\n");
    printf("2. quitar un elemento de la cola\n");
    printf("3. imprimir la cola\n");
    printf("4. salir\n");
    printf("Escriba su opcion: ");
    scanf("%d", &operacion);

    switch (operacion) {
    case 1:
      printf("Escriba el elemento a añadir: ");
      scanf("%d", &dato);
      encolar(cola, dato);
      imprimir_cola(cola);
      break;
    case 2:
      dato = desencolar(cola);
      printf("El elemento %d ha sido eliminado\n", dato);
      imprimir_cola(cola);
      break;
    case 3:
      imprimir_cola(cola);
      break;
    case 4:
      printf("Gracias por usar el programa\n");
      exit(0);
      break;
    default:
      printf("Opcion invalida\n");
      break;
    }

  } while (operacion != 4);
  return 0;
}
