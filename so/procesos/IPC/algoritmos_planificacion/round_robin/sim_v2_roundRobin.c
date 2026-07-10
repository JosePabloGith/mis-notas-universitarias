/*
 * NOTE: Para poder simular adecuadamente un algoritmo
 *       round robin, es necesario contemplar los siguientes asoectos:
 *
 *NOTE: * se nececita una cola FIFO, para enlistar los procesos en orden

        * un reloj que avanza solo cuando hay trabajo, o salta
          al siguiente evento de llegada si la cola esta vacia.

        * manejar 3 momentos: llegada de procesos, ejecucion de quantum
          y fin de proceso.
 *
*/

#include <stdio.h>
#include <stdlib.h>

#define QUANTUM 2
#define NUM_PROCESOS 3

// ============================================================
// 1. ESTRUCTURA DEL PROCESO
// ============================================================
typedef struct {
  int id;

  // Datos iniciales (con los que "nace" el proceso)
  int t_llegada;
  int rafaga_total;

  // Variables de control (cambian mientras se ejecuta)
  int rafaga_restante;
  int t_inicio; // -1 significa "todavia no ha tocado la CPU"
  int t_fin;

  // Metricas finales (se calculan cuando el proceso TERMINA)
  int t_retorno;
  int t_espera;
  int t_respuesta;
} Proceso;

// ============================================================
// 2. COLA (lista ligada) DE PROCESOS
// ============================================================
struct Nodo {
  Proceso dato;
  struct Nodo *siguiente;
};

struct Cola {
  struct Nodo *frente;
  struct Nodo *final;
};

// Reserva memoria para una una cola vacia y retorna un puntero a la misma
struct Cola *crear_cola() {
  struct Cola *cola = (struct Cola *)malloc(sizeof(struct Cola));
  cola->frente = NULL;
  cola->final = NULL;
  return cola;
}

// Retorna 1 si hay al menos un proceso en la cola, 0 si esta vacia
int hay_procesos_en_cola(struct Cola *cola) {
  if (cola->frente == NULL) {
    return 0;
  } else {
    return 1;
  }
}

// Mete un proceso al FINAL de la cola
void encolar(struct Cola *cola, Proceso p) {
  struct Nodo *nuevoNodo = (struct Nodo *)malloc(sizeof(struct Nodo));
  nuevoNodo->dato = p;
  nuevoNodo->siguiente = NULL;

  if (hay_procesos_en_cola(cola) == 0) {
    // Cola vacia: el nuevo nodo es frente Y final al mismo tiempo
    cola->frente = nuevoNodo;
    cola->final = nuevoNodo;
  } else {
    cola->final->siguiente = nuevoNodo;
    cola->final = nuevoNodo;
  }
}

// Saca y elimina al proceso que esta en el FRENTE de la cola
Proceso desencolar(struct Cola *cola) {
  struct Nodo *temp = cola->frente;
  Proceso dato = temp->dato;

  cola->frente = cola->frente->siguiente;
  if (cola->frente == NULL) {
    // Si ya no queda nadie, el final tambien debe quedar en NULL
    cola->final = NULL;
  }

  free(temp);
  return dato;
}

// NOTE: "Espia" quien esta al frente SIN sacarlo de la cola.
//   ->  solo debemos de llamarla si verificamos que la cola no esta vacia.
Proceso ver_frente(struct Cola *cola) {
  // si no hay nada, devolvemos un proceso vacio de error
  return cola->frente->dato;
}

// ============================================================
// 3. MANEJO DE LLEGADAS
// ============================================================
// Mueve de "cola_espera" a "cola_listos" a todos los procesos cuyo
// t_llegada ya se cumplio (t_llegada <= reloj_global).
// Asume que cola_espera esta ordenada por t_llegada ascendente.
// Esto es para simular que los procesos que llegan estan listos ejecutarse
// WARNING:
void verificar_llegadas(struct Cola *espera, struct Cola *listos,
                        int reloj_global) {

  // nececitamos comprobar que t_llegada sea menor o igual al reloj_global
  //
  while (hay_procesos_en_cola(espera) &&
         ver_frente(espera).t_llegada <= reloj_global) {

    Proceso llegado = desencolar(espera);
    encolar(listos, llegado);
    printf("[Reloj: %ds] Proceso %d llega y entra a la cola de listos\n",
           reloj_global, llegado.id);
  }
}

// ============================================================
// 4. CARGA DE PROCESOS DE PRUEBA (llegadas manuales, no aleatorias)
// ============================================================
// Tabla de procesos:
//   P1: Llegada 0 | Rafaga 5
//   P2: Llegada 4 | Rafaga 2
//   P3: Llegada 5 | Rafaga 4
// IMPORTANTE: se meten en "espera", NO directo en "listos", porque
// todavia no han llegado. t_inicio nace en -1 (bandera de "no atendido").
void llenar_cola_automaticamente(struct Cola *espera) {
  Proceso p1 = {1, 0, 5, 5, -1, 0, 0, 0, 0};
  Proceso p2 = {2, 4, 2, 2, -1, 0, 0, 0, 0};
  Proceso p3 = {3, 5, 4, 4, -1, 0, 0, 0, 0};

  encolar(espera, p1);
  encolar(espera, p2);
  encolar(espera, p3);
}

// ============================================================
// 5. PROGRAMA PRINCIPAL
// ============================================================
int main() {

  int reloj_global = 0;

  struct Cola *cola_espera = crear_cola(); // procesos que aun no llegan
  struct Cola *cola_listos = crear_cola(); // procesos listos para ser atendidos

  printf("=== INICIO DE LA SIMULACION ROUND ROBIN ===\n");
  printf("Quantum: %d | Procesos: %d\n\n", QUANTUM, NUM_PROCESOS);

  llenar_cola_automaticamente(cola_espera);
  printf("--------------------------------------\n");

  // El ciclo sigue mientras quede alguien esperando O alguien listo
  while (hay_procesos_en_cola(cola_listos) ||
         hay_procesos_en_cola(cola_espera)) {

    // WARNING: PASO 1: pasar a "listos" a quien ya le tocaba llegar
    verificar_llegadas(cola_espera, cola_listos, reloj_global);

    // PASO 2: si aun asi no hay nadie listo, el CPU esta ocioso.
    // Adelantamos el reloj directo hasta la proxima llegada.
    if (hay_procesos_en_cola(cola_listos) == 0) {
      reloj_global = ver_frente(cola_espera).t_llegada;
      printf("[Reloj: %ds] CPU ociosa, esperando la siguiente llegada...\n",
             reloj_global);
      continue;
    }

    // WARNING: PASO 3: sacamos al siguiente proceso de la fila de listos
    //
    // NOTE:: estes es el proceso que se va a ejecutar
    Proceso proc_actual = desencolar(cola_listos);

    // PASO 4: si es su PRIMERA vez en la CPU, registramos inicio y
    // tiempo de respuesta (t_respuesta = t_inicio - t_llegada)
    if (proc_actual.t_inicio == -1) {
      //
      proc_actual.t_inicio = reloj_global;

      // esto es de la formula t_respuesta = t_inicio - t_llegada
      proc_actual.t_respuesta = proc_actual.t_inicio - proc_actual.t_llegada;
    }

    printf("[Reloj: %ds] CPU ejecuta Proceso %d (Restante: %ds)\n",
           reloj_global, proc_actual.id, proc_actual.rafaga_restante);

    // PASO 5: comparamos su rafaga restante contra el QUANTUM
    // se presentaran 2 esenarios, uno donde el quantum es suficiente
    // y otro cuando no lo es
    if (proc_actual.rafaga_restante <= QUANTUM) {

      // CASO 1: EL PROCESO TERMINA SU EJECUCION
      // indicamos que el proseo se termino consumiendo su tiempo "queda
      // resgistrado en tiempo_global"
      //     lo ponemos en modo de sumatoria
      reloj_global = reloj_global + proc_actual.rafaga_restante;

      // dado que este proceso fue atnedido completamentem su rafaga restante es
      // de 0
      proc_actual.rafaga_restante = 0;

      // ============================================
      // INFO: RETO 6 Paso necesario
      // 1. Asigna el valor del reloj_global a t_fin.
      // llenamos los datos del proceso que haya entrado en esta situacion
      proc_actual.t_fin = reloj_global;
      // 2. Calcula t_retorno = t_fin - t_llegada
      proc_actual.t_retorno = proc_actual.t_fin - proc_actual.t_llegada;
      // 3. Calcula t_espera  = t_retorno - rafaga_total
      proc_actual.t_espera = proc_actual.t_retorno - proc_actual.rafaga_total;
      // ============================================

      printf("  -> Proceso %d TERMINO. [Retorno: %ds | Espera: %ds | "
             "Respuesta: %ds]\n",
             proc_actual.id, proc_actual.t_retorno, proc_actual.t_espera,
             proc_actual.t_respuesta);

      // No se vuelve a encolar: ya termino.

    } else {

      // CASO 2: EL PROCESO AGOTA SU QUANTUM PERO NO TERMINA
      // consumimos el tiempo de proceso "quantum"
      // WARNING: Aqui el proceso aun no a terminado, por lo que se reencolara
      // al final de la cola de
      reloj_global = reloj_global + QUANTUM;

      // a la rafaga restante le quitamos el quantum que se ha consumido
      proc_actual.rafaga_restante = proc_actual.rafaga_restante - QUANTUM;

      printf("  -> Proceso %d agoto Quantum. Vuelve a la cola (Faltan: %ds)\n",
             proc_actual.id, proc_actual.rafaga_restante);

      // INFO: PASO 6: revisamos llegadas nuevas ANTES de reencolar al
      //  proceso actual, para respetar el orden justo de la fila.
      //
      //  WARNING: El orden de la cola debe de reflejar el orden de llegada, o
      //  de espera.
      //           Si un nuevo proceso llega mientras el proceso actual estaba
      //           en la cpu. ESE NUEVO PROCESO ES EL QUE DEBE DE SER ATENDIDO
      //           ANTES QUE ESTE PROCESO QUE ESTAMOS POR ENCOLAR
      // INFO: como el reloj_global ya avanzo, durante la ejecucion del quantum
      // es posible
      //       que hayan nuevas llegadas, por lo que debemos de verificar antes
      //       de reencolar
      verificar_llegadas(cola_espera, cola_listos, reloj_global);

      // ya qye verificamos si de casualidad se presento un proceso
      // justo en el tiempo en el que vamos a encolar, para asegurarnos que este
      // se coloque al final de la cola, procedemos a:
      encolar(cola_listos, proc_actual);
    }
  }

  printf("\n=== SIMULACION TERMINADA ===\n");
  printf("Tiempo total de uso de CPU: %d segundos\n", reloj_global);

  // Nota: cola_espera y cola_listos ya deberian estar vacias aqui,
  // pero libera cola_espera y cola_listos (los structs 'Cola' en si)
  // con free() antes de terminar, para no dejar fugas de memoria.
  free(cola_espera);
  free(cola_listos);

  return 0;
}
