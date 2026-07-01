/*
 * escritor.c
 * Arreglamos el caso de solapamiento de datos
 * en multiples llamadas usando un semáforo
 * binario para sincronizar el acceso a la memoria compartida.
 * */

#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/types.h>
#define TAM 5

// cambio, union para los semáforos semctl
union semun {
  int val;               // valor para SETVAL
  struct semid_ds *buf;  // buffer para IPC_STAT, IPC_SET
  unsigned short *array; // array para GETALL, SETALL
  // struct seminfo *__buf;  // buffer para IPC_INFO (Linux-specific)
};

int main() {

  key_t clave;
  int id_memoria, id_sem;
  int *arreglo;
  union semun arg; // cambio, union para los semáforos semctl

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

  // cambio, creamos el semaforo
  id_sem = semget(clave, 1, IPC_CREAT | 0666);
  // verificamos si hubo error al crear el semaforo
  if (id_sem == -1) {
    perror("Error al crear el semaforo");
    exit(1);
  }

  // inicializamos el semaforo para que este disponible
  // el recurso
  arg.val = 1; // valor de la union
  int r = semctl(id_sem, 0, SETVAL, arg);
  if (r == -1) {
    perror("Error al inicializar el semaforo");
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
