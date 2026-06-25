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
    perror("Error al crear la llave");
    exit(1);
  }

  // observa que aquí se quito el create en comparación
  id_memoria = shmget(clave, TAM * sizeof(int), 0666);
  if (id_memoria == -1) {
    perror("Error al conectarse a la memoria compartida");
    exit(1);
  }

  arreglo = (int *)shmat(id_memoria, NULL, 0);
  if (arreglo == (int *)(-1)) {
    perror("error al adjuntar el segmento");
    exit(1);
  }

  printf("arreglo leido desde la memoria compartida\n");

  for (int i = 0; i < TAM; i++) {
    printf("%d ", arreglo[i]);
  }

  printf("\n");
  if (shmdt(arreglo) == -1) {
    perror("error al desadjuntar el segmento de memoria");
  }

  // eliminar el segmento de memoria
  // aquí si se elimina la memoria compartida, ya no la usaremos
  // por ahora :D
  shmctl(id_memoria, IPC_RMID, NULL);
  return 0;
}
