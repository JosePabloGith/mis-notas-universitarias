// Este codigo esta pensado
// para que sea capas de acceder a la memoria compartida
// que creamos con shMemo_escritor.c

// Lector.c
#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>

typedef struct {
  int id_del_escritor;
  int datos[5];
} CajaCompartida;

// prototipo
void imprimirNuemeros(CajaCompartida *cajaDeNumeros);

int main() {

  // declaramos la llave
  key_t llave;
  CajaCompartida *cajaDeNumeros;

  llave = ftok("/tmp", 'K');

  if (llave == -1) {
    perror("no se pudo generar la llave");
    exit(1);
  }

  // ahora accedemos a la memoria ya fabricada
  // AQUI NO SE DA EL FLAG IPC_CREATE, simplemente se coloca el permiso de
  // acceso
  int ticket_id_memoria = shmget(llave, sizeof(CajaCompartida), 0666);

  if (ticket_id_memoria == -1) {
    perror("no se pudo acceder a la memoria compartida");
    exit(1);
  }

  // en este punto en teoria ya tenemos acceso a la memoria compartida, ahora
  // necesitamos mapearla a nuestro espacio de direcciones
  // conectamos cajaDeNumeros a la memoria compartida
  cajaDeNumeros = (CajaCompartida *)shmat(ticket_id_memoria, NULL, 0);

  if (cajaDeNumeros == (void *)-1) {
    perror("no se pudo mapear la memoria compartida");
    exit(1);
  }

  // en este punto cajaDeNuemros apunta a la memoria compartida, ahora podemos
  // leer los datos
  printf("Tengo acceso a los numeros, los lei desde la memoria compartida");

  imprimirNuemeros(cajaDeNumeros);

  // en este punto ya no nececitamos la coneccion a la puerta de la memoria
  // compartida, por lo que podemos desconectarnos
  shmdt(cajaDeNumeros);

  // dado que en este punto no realizaremos nada mas
  // destruimsos la memoria compartida

  shmctl(ticket_id_memoria, IPC_RMID, NULL);

  return 0;
}

void imprimirNuemeros(CajaCompartida *cajaDeNumeros) {

  int cantidadDeNumeros =
      sizeof(cajaDeNumeros->datos) / sizeof(cajaDeNumeros->datos[0]);

  printf("Los numeros son: \n");
  for (int i = 0; i < cantidadDeNumeros; i++) {
    printf("%d\n", cajaDeNumeros->datos[i]);
  }
}
