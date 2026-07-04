// productor.c
// TODO: El productor deberá generar cinco números aleatorios en el rango de 1
// a 200 y almacenarlos en el buffer compartido.

#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <time.h>
#include <unistd.h>

#define TAM 5 // Tamaño actualizado a 5

union semun {
  int val;
};

int main() {
  srand(time(NULL)); // Semilla para números aleatorios
  key_t clave;
  int id_sem, id_memoria, *buffer;
  union semun arg;
  struct sembuf operacion;

  clave = ftok("/tmp", 'P');

  if (clave == -1) {
    perror("Error al crear la llave");
    exit(1);
  }

  id_memoria = shmget(clave, TAM * sizeof(int), IPC_CREAT | 0666);
  if (id_memoria == -1) {
    perror("Error al crear la memoria compartida");
    exit(1);
  }

  buffer = (int *)shmat(id_memoria, NULL, 0);
  if (buffer == (int *)(-1)) {
    perror("Error al Adjuntar el buffer");
    exit(1);
  }

  id_sem = semget(clave, 3, IPC_CREAT | 0666);

  if (id_sem == -1) {
    perror("Error al crear el semaforo");
    exit(1);
  }

  arg.val = TAM;
  semctl(id_sem, 0, SETVAL, arg);

  arg.val = 0;
  semctl(id_sem, 1, SETVAL, arg);

  arg.val = 1;
  semctl(id_sem, 2, SETVAL, arg);

  // Ciclo principal del productor
  for (int i = 0; i < TAM; i++) {
    operacion.sem_num = 0;
    operacion.sem_op = -1;
    operacion.sem_flg = 0;
    semop(id_sem, &operacion, 1);

    operacion.sem_num = 2;
    operacion.sem_op = -1;
    operacion.sem_flg = 0;
    semop(id_sem, &operacion, 1);

    // --- SECCIÓN CRÍTICA ---
    int numero_aleatorio = (rand() % 200) + 1;
    buffer[i] = numero_aleatorio;
    printf("PRODUCTOR\n Paquete %d guardado en el buffer[%d]\n",
           numero_aleatorio, i);

    printf("Estado del buffer: ");
    for (int j = 0; j < TAM; j++) {
      printf("%d ", buffer[j]);
    }
    printf("\n");
    // --- FIN SECCIÓN CRÍTICA ---

    operacion.sem_num = 2;
    operacion.sem_op = 1;
    semop(id_sem, &operacion, 1);

    operacion.sem_num = 1;
    operacion.sem_op = 1;
    operacion.sem_flg = 0;
    semop(id_sem, &operacion, 1);

    sleep(1);
  }

  printf("\n Buffer lleno. Despues del consumidor regresa y presiona Enter\n");
  getchar();

  shmdt(buffer);
  shmctl(id_memoria, IPC_RMID, NULL);
  semctl(id_sem, 0, IPC_RMID);

  printf("Memoria y semaforos eliminados\n");

  return 0;
}
