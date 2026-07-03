/*
 * El objetivo de esta practica es simular
 * mediante semáforos binarios y por conteo un procesador
 * de 5 hilos
 * */

#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/types.h>

union semun {
  int val;
  struct semid_ds *buf;
  unsigned short *array;
};

int main() {

  key_t clave;
  int id_semaforo;
  union semun arg; // ¿que chamucos es esto?

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

  // iniciamos el semaforo en "3 nucleos de moemento"
  arg.val = 3; // 3 recursos "mucho ojito"

  int r = semctl(id_semaforo, 0, SETVAL, arg); // ¿que chamucos?
  // verificamos si se inicializo correctamente el semaforo
  if (r == -1) {
    perror("Error al inicializar el semaforo");
    exit(EXIT_FAILURE);
  }

  printf("semáforo contador inicializado exitsosamente con valor: %d\n",
         arg.val);
  printf("este es el ID del semaforo: %d\n", id_semaforo);
  printf("estos son los nucleos de momento disponibles: %d\n", arg.val);

  return 0;
}
