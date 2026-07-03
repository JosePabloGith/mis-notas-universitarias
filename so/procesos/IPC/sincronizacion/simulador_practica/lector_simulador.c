/*
 * lector_simulador.c
 *
 * Este es el ultimo paso de la practica: lee el resultado final de
 * la memoria compartida y limpia TODOS los recursos que crearon
 * escritor_simulacionP.c y las 5 instancias de
 * aumentador_simulacion.c (memoria compartida + semaforo binario +
 * semaforo contador).
 *
 * IMPORTANTE: corre este programa solo DESPUES de que las 5
 * instancias de aumentador_simulacion.c hayan terminado. Si lo
 * corres mientras algun hilo sigue trabajando, le quitas el piso
 * (le eliminas la memoria o el semaforo que esta usando en ese
 * momento) y ese hilo va a fallar con error.
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/types.h>

#define TAM 5

int main() {

  key_t clave_memoria, clave_sem_binario, clave_sem_contador;
  int id_memoria, id_sem_binario, id_sem_contador;
  int *arreglo;

  // --- 1. mismas llaves que el resto de la practica ---
  clave_memoria = ftok("/tmp", 'A');
  clave_sem_binario = ftok("/tmp", 'B');
  clave_sem_contador = ftok("/tmp", 'C');

  if (clave_memoria == -1 || clave_sem_binario == -1 ||
      clave_sem_contador == -1) {
    perror("Error al generar alguna de las llaves");
    exit(1);
  }

  // --- 2. nos conectamos a los recursos existentes (sin IPC_CREAT) ---
  id_memoria = shmget(clave_memoria, TAM * sizeof(int), 0666);
  if (id_memoria == -1) {
    perror("Error al conectarse a la memoria compartida");
    exit(1);
  }

  id_sem_binario = semget(clave_sem_binario, 1, 0666);
  if (id_sem_binario == -1) {
    perror("Error al conectarse al semaforo binario");
    exit(1);
  }

  id_sem_contador = semget(clave_sem_contador, 1, 0666);
  if (id_sem_contador == -1) {
    perror("Error al conectarse al semaforo contador");
    exit(1);
  }

  // --- 3. leemos el resultado final del arreglo ---
  arreglo = (int *)shmat(id_memoria, NULL, 0);
  if (arreglo == (int *)(-1)) {
    perror("Error al adjuntar el segmento de memoria");
    exit(1);
  }

  printf("arreglo final despues de que los 5 hilos trabajaron:\n");
  for (int i = 0; i < TAM; i++) {
    printf("%d ", arreglo[i]);
  }
  printf("\n");

  // --- 4. chequeo de salud de los semaforos ---
  // GETVAL regresa el valor actual del semaforo. si todos los hilos
  // liberaron correctamente lo que pidieron, el binario debe haber
  // vuelto a 1 y el contador debe haber vuelto a 3 (sus valores
  // iniciales). si ves otro numero, algun hilo se quedo a medias
  // (crash, no llego al V, etc).
  int valor_binario = semctl(id_sem_binario, 0, GETVAL);
  int valor_contador = semctl(id_sem_contador, 0, GETVAL);
  printf("\nvalor final semaforo binario:  %d (deberia ser 1)\n",
         valor_binario);
  printf("valor final semaforo contador: %d (deberia ser 3)\n", valor_contador);

  // --- 5. desadjuntamos y eliminamos TODO ---
  shmdt(arreglo);

  shmctl(id_memoria, IPC_RMID, NULL);   // elimina la memoria compartida
  semctl(id_sem_binario, 0, IPC_RMID);  // elimina el semaforo binario
  semctl(id_sem_contador, 0, IPC_RMID); // elimina el semaforo contador

  printf("\nrecursos liberados: memoria compartida y ambos semaforos "
         "eliminados\n");

  return 0;
}
