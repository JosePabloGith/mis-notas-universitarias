/*
 *  El productor debe de generar
 *  cinco numeros aleatorui
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <unistd.h>
#define TAM 3

union semun {
  int val;
  // struct semid_ds *buf;
  // unsigned short *array;
};

int main() {
  key_t clave;
  int id_semaforo, id_memoria, *buffer;
  union semun operacion;

  clave = ftok("/tmp", 'P');
  if (clave == -1) {
    perror("Error al generar la clave");
    exit(EXIT_FAILURE);
  }

  id_memoria = shmget(clave, sizeof(int), IPC_CREAT | 0666);
  if (id_memoria == -1) {
    perror("Error al obtener el ID de memoria compartida");
    exit(EXIT_FAILURE);
  }

  buffer = (int *)shmat(id_memoria, NULL, 0);
  if (buffer == (int *)-1) {
    perror("Error al adjuntar la memoria compartida");
    exit(EXIT_FAILURE);
  }

  id_semaforo = semget(clave, 1, IPC_CREAT | 0666);
  if (id_semaforo == -1) {
    perror("Error al obtener el ID del semáforo");
    exit(EXIT_FAILURE);
  }

  // semaforo 1 empty, represeta cuantos espacios vacios hay en el buffer
  arg.val = TAM; // inicializamos el semaforo en 3, ya que el buffer tiene 3
                 // espacios vacios
  semctl(id_semaforo, 0, SETVAL, arg);

  // semaforo 2 full, representa cuantos itmes hay en el buffer
  arg.val = 0;
  semctl(id_semaforo, 1, SETVAL, arg);

  // semaforo 3 mutex
  arg.val = 1;
  semctl(id_semaforo, 2, SETVAL, arg);

  int numero_paquete = 100;

  for (int i = 0; i < TAM; i++) {
    // wait en empty
    operacion.sem_num = 0;
    operacion.sem_op = -1;
    operacion.sem_flg = 0;
    semop(id_semaforo, &operacion, 1);

    // wait en mutex
    operacion.sem_num = 2;
    operacion.sem_op = -1;
    operacion.sem_flg = 0;
    semop(id_semaforo, &operacion, 1);

    // seccion critica
    buffer[i] = numero_paquete;
    printf("Productor: produciendo paquete %d en buffer[%d]\n", numero_paquete,
           i);

    // mostrar el estado del buffer
    printf("Estado del buffer: ");
    for (int j = 0; j < TAM; j++) {
      printf("%d ", buffer[j]);
    }

    printf("\n");

    // fin de seccion critica
    //  SIGNAL en mutex salimos de la seccion critica
    operacion.sem_num = 2;
    operacion.sem_op = 1;
    semop(id_semaforo, &operacion, 1);

    // signal en full, indicamos que hay un item en el buffer
    operacion.sem_num = 1;
    operacion.sem_op = 1;
    operacion.sem_flg = 0;
    semop(id_semaforo, &operacion, 1);
    numero_paquete++;
    sleep(1);
  }

  prntgf("buffer lleno, despues deñ consumidor regresa y presiona enter \n");
  getchar();

  shmdt(buffer);
  shmctl(id_memoria, IPC_RMID, NULL);
  semctl(id_semaforo, 0, IPC_RMID);

  printf("memoria y semaforos eliminados\n");
  return 0;
}
