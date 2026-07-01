// Este código esta pensado para ejecutarse
// en conjunto con los códigos de ejemplo
// vistos en clase.
// vamos a arreglar el control de
// acceso a la memmoria compartido
// usando el semáforo
// aumenta.c

#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>

#define TAM 5
#define REP 100000

int main() {
  key_t clave;
  int id_memoria, id_sem;
  int *arreglo;

  struct sembuf operacion;

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

  // conexion al semaforo
  id_sem = semget(clave, 1, 0666);
  if (id_sem == -1) {
    perror("error al conectarse al semaforo");
    exit(1);
  }

  arreglo = (int *)shmat(id_memoria, NULL, 0);
  if (arreglo == (int *)(-1)) {
    perror("Error al adjuntar el segmento");
    exit(1);
  }

  // cambios importantes para el control de acceso a la memoria compartida
  for (int r = 0; r < REP; r++) {
    // wait p bloquo antes de
    // entrar, imagina que es un baño, si hay alguien adentro, tu esperas afuera

    operacion.sem_num = 0;        // utilizar el semaforo 0
    operacion.sem_op = -1;        // operacion wait
    operacion.sem_flg = 0;        // sin banderas
    semop(id_sem, &operacion, 1); // realiza la operacion

    // este es area critica, solo un proceso puede estar aqui adentro a la vez
    // exclusion mutua, solo un proceso puede estar en esta seccion de codigo a
    // la vez
    for (int i = 0; i < TAM; i++) {
      arreglo[i] = arreglo[i] + 1;
    }

    // signal v libera el semaforo, imagina que sales del baño y le dices a
    // alguien afuera que ya puede entrar
    operacion.sem_op = 1;         // operacion signal
    semop(id_sem, &operacion, 1); // realiza la operacion
  }

  shmdt(arreglo);
  return 0;
}
