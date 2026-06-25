#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#define TAM 5

int main() {

  key_t clave;
  int id_memoria;
  int *arreglo;

  clave = ftok("/tmp", 'A');

  if (clave == -1) {
    perror("error al crear la llave");
    exit(1);
  }

  id_memoria = shmget(clave, TAM * sizeof(int), IPC_CREAT | 0666);
  if (id_memoria == -1) {
    perror("Error al crear la memrmoria compartida");
    exit(1);
  }

  arreglo = (int *)shmat(id_memoria, NULL, 0);

  if (arreglo == (int *)(-1)) {
    perror("error al adjuntarel segmento al espacio de direcciones");
    exit(1);
  }

  printf("ingresa los elementos del arreglo: \n");
  for (int i = 0; i < TAM; i++) {
    scanf("%d", &arreglo[i]);
  }

  printf("\nArreglo almacenado en memoria compartida\n");

  // des adjuntar el segmento de memoria
  shmdt(arreglo);

  // eliminar el segmento [!]
  // shmctl(id_memoria, IPC_RMID, NULL);

  return 0;
}
