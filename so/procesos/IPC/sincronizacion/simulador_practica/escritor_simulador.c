/*
 * escritor_simulacionP.c
 *
 * Este es el archivo "creador" de la practica del procesador de
 * 5 hilos. Prepara todos los recursos que despues van a usar
 * aumentador.c (los "hilos") y lector.c.
 *
 * Aqui se crean:
 *   1) La memoria compartida        (igual que en el ejercicio anterior)
 *   2) El semaforo BINARIO  -> exclusion mutua: protege la memoria
 *      compartida para que solo un hilo la modifique a la vez.
 *   3) El semaforo CONTADOR -> simula cuantos "nucleos" hay
 *      disponibles al mismo tiempo: permite que varios hilos
 *      trabajen en paralelo, pero nunca mas de NUCLEOS a la vez.
 *
 * Analogia (la misma que uso el profesor en incrementa.c):
 *   - El semaforo BINARIO es como un baño de 1 sola persona:
 *     "hay alguien adentro? espera afuera."
 *   - El semaforo CONTADOR es como un estacionamiento con N lugares:
 *     "hay lugar libre? entra. si no, espera a que alguien salga."
 *
 * Este programa SOLO crea e inicializa. No hace ningun trabajo con
 * los hilos todavia -- eso le toca a aumentador.c.
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/types.h>

#define TAM 5 // tamaño del arreglo compartido (una posicion por hilo)
#define NUCLEOS                                                                \
  3 // "nucleos" disponibles -> valor inicial del semaforo contador

// union requerida por semctl() para pasarle el valor de inicializacion.
// desde hace años glibc ya no la trae definida por default, asi que
// hay que declararla nosotros mismos (igual que en tu escritor.c original).
union semun {
  int val;               // valor para SETVAL (el que usaremos aqui)
  struct semid_ds *buf;  // buffer para IPC_STAT / IPC_SET
  unsigned short *array; // arreglo para GETALL / SETALL
};

int main() {

  key_t clave_memoria, clave_sem_binario, clave_sem_contador;
  int id_memoria, id_sem_binario, id_sem_contador;
  int *arreglo;
  union semun arg;

  // --- 1. generamos 3 llaves distintas, una por cada recurso ---
  // ftok combina una ruta que ya existe en el sistema ("/tmp") mas
  // un caracter, para generar una key_t "unica". usamos letras
  // distintas (A, B, C) para que cada recurso tenga su propia
  // llave y no se mezclen entre si.
  clave_memoria = ftok("/tmp", 'A');      // memoria compartida
  clave_sem_binario = ftok("/tmp", 'B');  // semaforo binario (mutex)
  clave_sem_contador = ftok("/tmp", 'C'); // semaforo contador (nucleos)

  if (clave_memoria == -1 || clave_sem_binario == -1 ||
      clave_sem_contador == -1) {
    perror("Error al crear alguna de las llaves");
    exit(1);
  }

  // --- 2. creamos la memoria compartida ---
  // IPC_CREAT: si no existe la crea; si ya existe (por ejemplo porque
  // quedo de una corrida anterior que no se limpio) nos conectamos
  // a la que ya esta.
  id_memoria = shmget(clave_memoria, TAM * sizeof(int), IPC_CREAT | 0666);
  if (id_memoria == -1) {
    perror("Error al crear la memoria compartida");
    exit(1);
  }

  // --- 3. creamos el semaforo BINARIO (mutex) ---
  // el segundo argumento es 1: un conjunto con un solo semaforo adentro.
  id_sem_binario = semget(clave_sem_binario, 1, IPC_CREAT | 0666);
  if (id_sem_binario == -1) {
    perror("Error al crear el semaforo binario");
    exit(1);
  }

  // lo inicializamos en 1 -> "recurso libre, puede entrar exactamente 1"
  arg.val = 1;
  if (semctl(id_sem_binario, 0, SETVAL, arg) == -1) {
    perror("Error al inicializar el semaforo binario");
    exit(1);
  }

  // --- 4. creamos el semaforo CONTADOR (nucleos disponibles) ---
  id_sem_contador = semget(clave_sem_contador, 1, IPC_CREAT | 0666);
  if (id_sem_contador == -1) {
    perror("Error al crear el semaforo contador");
    exit(1);
  }

  // lo inicializamos en NUCLEOS (3) -> "hasta 3 hilos pueden estar
  // trabajando a la vez; el resto se queda esperando en la fila"
  arg.val = NUCLEOS;
  if (semctl(id_sem_contador, 0, SETVAL, arg) == -1) {
    perror("Error al inicializar el semaforo contador");
    exit(1);
  }

  // --- 5. adjuntamos la memoria compartida a nuestro espacio de direcciones
  // ---
  arreglo = (int *)shmat(id_memoria, NULL, 0);
  if (arreglo == (int *)(-1)) {
    perror("Error al adjuntar el segmento de memoria");
    exit(1);
  }

  // --- 6. capturamos los datos iniciales, igual que en escritor.c ---
  printf("ingresa los %d elementos iniciales del arreglo:\n", TAM);
  for (int i = 0; i < TAM; i++) {
    scanf("%d", &arreglo[i]);
  }

  printf("\narreglo inicial guardado en memoria compartida\n");
  printf("id memoria compartida: %d\n", id_memoria);
  printf("id semaforo binario:   %d (valor inicial: 1)\n", id_sem_binario);
  printf("id semaforo contador:  %d (valor inicial: %d)\n", id_sem_contador,
         NUCLEOS);

  // --- 7. nos desconectamos, pero NO destruimos nada todavia ---
  // la memoria y los semaforos deben seguir vivos en el kernel para
  // que aumentador.c y lector.c los puedan usar despues. la limpieza
  // final (IPC_RMID de todo) le toca a lector.c, al final del flujo.
  shmdt(arreglo);

  return 0;
}
