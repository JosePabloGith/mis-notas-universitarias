// Este código esta pensado para ejecutarse
// en conjunto con los códigos de ejemplo
// vistos en clase.
// aumenta.c

#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#define TAM 5
#define REP 100000

int main() {
  key_t clave;
  int id_memoria;
  int *arreglo;

  clave = ftok("/tmp", 'A');
  if (clave == -1) {

    perror("error al crear la llave");
    exit(1);
  }

  id_memoria = shmget(clave, TAM * sizeof(int), 0666);

  if (id_memoria == -1) {
    perror("error al conectarse a la memoria compartida");
    exit(1);
  }

  arreglo = (int *)shmat(id_memoria, NULL, 0);
  if (arreglo == (int *)(-1)) {
    perror("Error al adjuntar el segmento");
    exit(1);
  }

  for (int r = 0; r < REP; r++) {

    for (int i = 0; i < TAM; i++) {
      arreglo[i] = arreglo[i] + 1;
    }
  }

  shmdt(arreglo);
  return 0;
}
