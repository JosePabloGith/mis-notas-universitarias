#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/types.h>
struct sembuf operacion;

int main() {

  key_t clave;
  int id_semaforo;

  clave = ftok("/tmp", 'C');
  // verificamos si se creo correctamente la clave
  if (clave == -1) {
    perror("Error al crear la clave");
    exit(EXIT_FAILURE);
  }

  // creamos el semaforo contador
  // ¿que chamucos es esto y por que se parece a shmget?
  // porque es un contdor y no binaro ?
  id_semaforo = semget(clave, 1, IPC_CREAT | 0666);

  // verificamos si se creo correctamente el semaforo
  if (id_semaforo == -1) {
    perror("Error al crear el semaforo");
    exit(EXIT_FAILURE);
  }

  printf("\n---proceso [pid=%d]\n", getpid());
  printf("solicitando nucleos\n");

  // wait p solicitar un nucleo
  operacion.

      return 0;
}
