/*
 * aumentador_simulacion.c
 *
 * Este es uno de los "5 hilos" del procesador simulado. Se conecta
 * a los recursos que ya creo escritor_simulacionP.c (memoria
 * compartida + semaforo binario + semaforo contador) y compite por
 * ellos.
 *
 * IMPORTANTE: este programa representa UN hilo. Para simular los
 * 5 hilos reales, este mismo binario se corre 5 veces al mismo
 * tiempo (5 procesos), por ejemplo:
 *
 *   gcc aumentador_simulacion.c -o aumentador_simulacion
 *   for i in $(seq 1 5); do ./aumentador_simulacion & done
 *   wait
 *
 * Cada proceso usa su PID para identificarse en la salida, asi
 * puedes ver en consola quien tiene el nucleo y quien esta esperando.
 *
 * Estructura de cada ciclo de trabajo (revisa el diagrama que armamos):
 *
 *   P(contador)  -> pide un nucleo. si los 3 estan ocupados, aqui
 *                   se bloquea hasta que alguien libere uno.
 *     P(binario) -> entra a la seccion critica (memoria compartida)
 *       ... modifica el arreglo ...
 *     V(binario) -> sale de la seccion critica
 *     ... "trabaja" (sleep) mientras conserva el nucleo ...
 *   V(contador)  -> libera el nucleo
 *
 * Nota clave: el sleep() que simula el trabajo pesado va FUERA del
 * binario y DENTRO del contador. Si lo pusieramos dentro del binario,
 * solo un hilo podria "trabajar" a la vez en todo el sistema (eso
 * seria incorrecto: el binario debe proteger solo el instante breve
 * de escribir en memoria, no todo el tiempo de trabajo). Asi, hasta
 * 3 hilos pueden estar "dormidos/trabajando" en paralelo, pero se
 * turnan brevemente para tocar el arreglo.
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <unistd.h>

#define TAM 5            // tamaño del arreglo compartido (igual que el creador)
#define TRABAJOS 3       // cuantas veces este hilo intenta tomar un nucleo
#define TIEMPO_TRABAJO 2 // segundos que "tarda" en trabajar con el nucleo

int main() {

  key_t clave_memoria, clave_sem_binario, clave_sem_contador;
  int id_memoria, id_sem_binario, id_sem_contador;
  int *arreglo;
  struct sembuf op_binario, op_contador;
  pid_t mi_pid = getpid(); // identificador de este "hilo" en la salida

  // --- 1. mismas llaves que uso escritor_simulacionP.c ---
  // usamos las mismas letras (A, B, C) para apuntar a los mismos
  // recursos que el creador ya dejo listos.
  clave_memoria = ftok("/tmp", 'A');
  clave_sem_binario = ftok("/tmp", 'B');
  clave_sem_contador = ftok("/tmp", 'C');

  if (clave_memoria == -1 || clave_sem_binario == -1 ||
      clave_sem_contador == -1) {
    perror("Error al generar alguna de las llaves");
    exit(1);
  }

  // --- 2. nos conectamos a los recursos SIN crearlos ---
  // observa que aqui no llevan IPC_CREAT: si escritor_simulacionP.c
  // no se corrio antes, estas llamadas van a fallar (a proposito,
  // asi sabes que olvidaste correr el creador primero).
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

  arreglo = (int *)shmat(id_memoria, NULL, 0);
  if (arreglo == (int *)(-1)) {
    perror("Error al adjuntar el segmento de memoria");
    exit(1);
  }

  // --- 3. el hilo intenta trabajar TRABAJOS veces ---
  for (int t = 0; t < TRABAJOS; t++) {

    printf("[hilo %d] esperando un nucleo libre (intento %d de %d)\n", mi_pid,
           t + 1, TRABAJOS);

    // P sobre el contador: "hay algun nucleo libre?"
    op_contador.sem_num = 0;
    op_contador.sem_op = -1; // wait
    op_contador.sem_flg = 0;
    semop(id_sem_contador, &op_contador, 1);

    printf("[hilo %d] nucleo asignado\n", mi_pid);

    // P sobre el binario: entra a tocar la memoria compartida
    op_binario.sem_num = 0;
    op_binario.sem_op = -1; // wait
    op_binario.sem_flg = 0;
    semop(id_sem_binario, &op_binario, 1);

    // ---- seccion critica: breve, solo escribir en el arreglo ----
    printf("[hilo %d] escribiendo en memoria compartida...\n", mi_pid);
    for (int i = 0; i < TAM; i++) {
      arreglo[i] = arreglo[i] + 1;
    }
    // ---- fin de la seccion critica ----

    // V sobre el binario: ya termino de tocar la memoria
    op_binario.sem_op = 1; // signal
    semop(id_sem_binario, &op_binario, 1);

    // "trabaja" con el nucleo (ya no toca memoria, por eso esto va
    // fuera del binario: otros hilos pueden escribir mientras tanto)
    printf("[hilo %d] trabajando con el nucleo (%ds)...\n", mi_pid,
           TIEMPO_TRABAJO);
    sleep(TIEMPO_TRABAJO);

    // V sobre el contador: libera el nucleo para que otro hilo entre
    op_contador.sem_op = 1; // signal
    semop(id_sem_contador, &op_contador, 1);

    printf("[hilo %d] nucleo liberado\n", mi_pid);
  }

  shmdt(arreglo);
  return 0;
}
