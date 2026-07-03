#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <unistd.h>
#define TAM 3

void mostrarParImpar(int x) {
  if (x & 2 == 1) {
    printf("Impar");
  } else {
    printf("Par");
  }
}

void mostrarPrioridad(int x) {}

int main() {
  key_t clave;
  int id_memoria, id_sem, *buffer; // añadir el id del semaforo
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

  id_sem = semget(clave, 1, IPC_CREAT | 0666); // crear semaforo
  if (id_sem == -1) {
    perror("Error al crear el semaforo");
    exit(1);
  }

  printf("CONSUMIDOR\n\nEsperando paquetes disponibles...\n\n");
  for (int i = 0; i < TAM; i++) {
    // WAIT(full) - esperar paquete disponible
    operacion.sem_num = 1;
    operacion.sem_op = -1;
    semop(id_sem, &operacion, 1);

    // WAIT (mutex) - proteger buffer
    operacion.sem_num = 2;
    operacion.sem_op = -1;
    semop(id_sem, &operacion, 1);

    // region critica
    int paquete = buffer[i];
    buffer[i] = 0; // marcar casilla como vacia
    printf("Paquete %d extraído de buffer [%d]\n", paquete, i);

    // mostrar estado actual del buffer:
    printf("estado del buffer: ");
    for (int j = 0; j < TAM; j++) {
      printf("%d ", buffer[j]);
    }
    printf("\n");

    // SIGNAL(mutex) - avisar espacio libre
    operacion.sem_num = 0;
    operacion.sem_op = 1;
    semop(id_sem, &operacion, 1);

    printf("Procesando paquete...\n");
    sleep(3);
  }

  // desadjuntar el segmento de memoria:
  shmdt(buffer);
  printf("\nTodos los paquetes fueron consumidos\nConsumidor terminó\n");

  return 0;
}
