// hola Ordenador.c
// utizo un struct :D
// Este programa se encargara de conectarse a la memoria compartida
// ordenar los numeros que se encuentren ahí y posteriormente
// desconectarse

#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>

//struct "id_del_escritor lo encontre en un foro, aun esta 
// en desarrollo para el tema de semaforos :P"
typedef struct {
  int id_del_escritor;
  int numeros[5];
} CajaCompartida;

void ordenar_caja_asecendente(CajaCompartida *laCaja);
void ordenar_caja_descendente(CajaCompartida *laCaja);

int main() {

  CajaCompartida *caja;
  // Creamos la clave para la memoria compartida
  key_t llave = ftok("/tmp", 'K');
  if (llave == -1) {
    perror("Error al crear la clave para la memoria compartida");
    exit(1);
  }

  // Accedemos a la memoria compartida, no usamoes IPC_CREAT porque no queremos
  // crearla, solo acceder a ella
  int ticket_id_memory = shmget(llave, sizeof(CajaCompartida), 0666);
  if (ticket_id_memory == -1) {
    perror("Error al acceder a la memoria compartida");
    exit(1);
  }

  // conectamos la variavle caja a la memoria compartida
  // Recordamos que vamos a obtener un puntero a la memoria compartida, por lo
  // que debemos castear el puntero a la estructura que estamos usando
  //  "recuerda que tenemos que llamar al albañil de memoria compartida para que
  //  nos de la direccion de memoria compartida"
  caja = (CajaCompartida *)shmat(ticket_id_memory, NULL, 0);

  if (caja == (void *)-1) {
    perror("Error al conectar a la memoria compartida");
    exit(1);
  }
  // si llegamos aqui significa que nos conectamos a la memoria compartida,
  // ahora podemos acceder a los numeros que se encuentran ahi

  // Ordenamos los numeros en la memoria compartida usando el algoritmo de
  // dependiendo que es lo que nos pidean, podemos
  // llamar a ordenar_caja_asecendente(caja); o a
  // ordenar_caja_descendente(caja);
  //
  // por defaut dejare el oredenamiento de forma ascendente
  ordenar_caja_asecendente(caja);

  printf("Los numeros en la memoria compartida han sido ordenados de forma "
         "ascendente\n");
  printf("Arreglo ordenado: ");
  for(int i=0; i<5; i++){
  	printf("%d " ,caja->numeros[i]);
  }

  // dalimos eliminando la puerta de acceso a la memoria compartida, pero no la
  // memoria compartida

  shmdt(caja);

  return 0;
}

void ordenar_caja_asecendente(CajaCompartida *laCaja) {

  int cantidadNumerosCaja =
      sizeof(laCaja->numeros) / sizeof(laCaja->numeros[0]);

  // usaremos 2 bucles para comparar todos los numeros entre si
  // i decidira el turno, cada turno se encargara de una posicion y la deja con
  // el mumero correcto para siempre
  for (int i = 0; i < cantidadNumerosCaja - 1; i++) {

    // este j sale a buscar, recorrera todo lo que queda a la derecha de i
    // pregunando ¿hay algo mas chiquito que yo?
    // si lo encuentra lo secuestra y lo pone en la posicion de i, asi i
    // queda con el numero mas chiquito para siempre
    // el itercabio usa a temporal como mano extra para guardar el numero que
    // vamos a mover
    //
    for (int j = i + 1; j < cantidadNumerosCaja; j++) {

      // si el numero de la izquierda es MAYOR que el de la derecha, los
      // intercambiamos
      if (laCaja->numeros[i] > laCaja->numeros[j]) {

        // entonces laCaja->numeros[i] es mayor que laCaja->numeros[j], por lo
        // que debemos intercambiarlos de esta forma nos quedamos con el numero
        // mas chiquito en la posicion i y el mas grande en la posicion j
        int auxiliar = laCaja->numeros[i];

        laCaja->numeros[i] = laCaja->numeros[j];

        laCaja->numeros[j] = auxiliar;
	printf("%d\n", auxiliar);
      }
    }
  }
}

void ordenar_caja_descendente(CajaCompartida *laCaja) {

  int cantidadNumerosCaja =
      sizeof(laCaja->numeros) / sizeof(laCaja->numeros[0]);

  // usaremos 2 bucles para comparar todos los numeros entre si
  // i decidira el turno, cada turno se encargara de una posicion y la deja con
  // el mumero correcto para siempre
  for (int i = 0; i < cantidadNumerosCaja - 1; i++) {

    // este j sale a buscar, recorrera todo lo que queda a la derecha de i
    // pregunando ¿hay algo mas grande que yo?
    // si lo encuentra lo secuestra y lo pone en la posicion de i, asi i
    // queda con el numero mas grande para siempre
    // el itercabio usa a temporal como mano extra para guardar el numero que
    // vamos a mover
    //
    for (int j = i + 1; j < cantidadNumerosCaja; j++) {

      // si el numero de la izquierda es MENOR que el de la derecha, los
      // intercambiamos
      if (laCaja->numeros[i] < laCaja->numeros[j]) {

        // entonces laCaja->numeros[i] es menor que laCaja->numeros[j], por lo
        // que debemos intercambiarlos de esta forma nos quedamos con el numero
        // mas grande en la posicion i y el mas chico en la posicion j
        int auxiliar = laCaja->numeros[i];

        laCaja->numeros[i] = laCaja->numeros[j];

        laCaja->numeros[j] = auxiliar;
	printf("%d\n", auxiliar);
      }
    }
  }
}


