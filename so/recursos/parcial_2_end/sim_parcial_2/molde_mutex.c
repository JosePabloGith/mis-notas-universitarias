// Molde Semaforo unitario
// Autor: Jose Pablo :D

#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/types.h>
#include <unistd.h>
/*
 sem.h nos da esta estructura
 struct sembuf {
   int sem_num;  // El índice del semáforo en el arreglo (0, 1, 2...)
   int sem_op;  // La operación: -1 (Wait), 1 (Signal), o 0
   short sem_flg; // Banderas de control (como IPC_NOWAIT o SEM_UNDO)
 };
 */

// estructura tipica de los semaforos
//  recordando que necesitamos
//  una Union para poder manipular los semaforos
//
//  Recordando que los semaforos viven
//  100% en el kernel
//
//  La memoria compartida vive en el espacio del usuario
//  por ejemplo, al usar shmat(), el kernel le ordena al MMU
//  que conecte ese bloque de la RAM directamente al mapa de memoria
//  virtual programa. Una vez conectada, esa memoria
//  se vuelve parte del espacio de usuario.

union semun {
  int val; // se usa para indicar el valor del semaforo 0 abierto o 1 cerrado
  struct semid_ds *buf; // se usa para indicar el identificador del semaforo
  // Esta estructura es interna y se encuentra dentro del sistema.
  // No se puede acceder directamete a ella. Digamos
  //  que su funcion es una especie de hoja de datos
  //  que el kernel lee y actualiza cada vez que alguien usa el semaforo

  unsigned short *array; // esto se usa para inicializar un arreglo
  // completo de semaforos, con una sola linea.
};

// funiciones auxiliares para manejar el semaforo
void wait_p_sem(int semid) {
  // nececitamos que nos manden el identificado del semaforo
  struct sembuf operacion; // Estructura que nos permite manipular el semaforo

  operacion.sem_num = 0; // indica el numero del semaforo
  // como es mutex, solo hay uno, asi que inicia en cero :D

  operacion.sem_op = -1; // indica que vamos a decrementar el semaforo
  /*este valor puede ser mayor a 1, indicando que recursos disponibles
   * simplemente restara 1 unidad de esos recursos
   *
   * si no hay nada "0" y llega este -1, implica que el recurso
   * esta ocupado, entoces nos ponemos en suspencion, esperando
   * a que el otro proceso use sem_op = 1, para que al encontrar
   * que este recurso se encuentra en suspencion, nos despierte y nos permita
   * acceder al recurso.
   * */

  operacion.sem_flg = SEM_UNDO; // se usa para evitar que en caso
  // de que el proceso termine de manera inesperada, el kernel
  //  se encargue de liberar el semaforo, de manera automatica.

  // podemos dejarlo en 0, para indicar que no hay banderas de control.

  // ejecutamos la operacion encapsulado en un if

  if (semop(semid, &operacion, 1) == -1) {
    // recordando que el 1 del final es para indicar el numero de
    // opreaciones que vamos a realizar, en este caso solo uno.
    perror("error, no se pudi enecutar la operacion wait_p_sem");
    exit(1);
  }
}

// funcion para realizar signal v "para liberar el semaforo"

void signal_v_sem(int semid) {
  struct sembuf operacion;

  operacion.sem_num = 0; // indica el numero del semaforo
  operacion.sem_op = 1;  // indica que vamos a incrementar el semaforo
  // en uno, para liberarlo y permitir que el proceso pueda acceder al recurso
  //  que se encuentra en suspencion.

  operacion.sem_flg = SEM_UNDO;

  if (semop(semid, &operacion, 1) == -1) {
    perror("error, no se pudo realizar la operacion signal_v_sem");
    exit(1);
  }
}

int main() {
  // declaramos los identificadores de los semaforos
  int semid;
  union semun argumento;

  key_t llave = ftok("/tmp", 's');
  // #1 creamos el MDF semaforo
  semid = semget(llave, 1, IPC_CREAT | 0666);
  if (semid == -1) {
    perror("no se puede crear el semadoro");
    exit(1);
  }

  // #2 inicializamos el semadoro. MUTEX
  argumento.val = 1; // inicializamos el semaforo en 1 "lugares libres"
  // tiene sentido para un semaforo binario.
  if (semctl(semid, 0, SETVAL, argumento) == -1) {
    perror("no se pudo inicializar el semaforo");
    exit(1);
  }

  printf("Semaforo listo. Se puede entrar a la memoria compartida\n");

  // --- demo de uso del semaforo ---
  printf("\n proceso intentando entrar a la zona critica\n");
  wait_p_sem(semid);

  // ======================
  // seccion cirtica
  printf("estamos en la zona critica\n");
  // =======================

  printf("salimos de la zona critica\n");
  signal_v_sem(semid);

  // limpiamos recursos
  if (semctl(semid, 0, IPC_RMID) == -1) {
    perror("no se pudo eliminar el semaforo");
    exit(1);
  }

  return 0;
}
