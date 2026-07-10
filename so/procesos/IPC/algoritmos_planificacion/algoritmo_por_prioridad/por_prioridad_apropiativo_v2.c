/*
 * ============================================================================
 * ALGORITMO DE PLANIFICACIÓN: PRIORIDAD APROPIATIVA (PREEMPTIVE)
 * ============================================================================
 * * OBJETIVO DEL ALGORITMO:
 *
 * Simular cómo un Sistema Operativo moderno (como Linux o Windows) decide qué
 * programa usar en la CPU. En lugar de repartir el tiempo equitativamente
 * (como Round Robin), este algoritmo es jerárquico.
 *
 * * * REGLA DE ORO: La CPU SIEMPRE debe estar ejecutando el proceso con la
 * prioridad más alta (el número más bajo representa la urgencia máxima).
 *
 * * * ¿QUÉ SIGNIFICA APROPIATIVO (PREEMPTIVE)?
 * Significa "Desalojo". Si la CPU está ejecutando un proceso de prioridad 3,
 * y de repente el usuario abre un proceso de prioridad 1, el SO no espera.
 * Inmediatamente "le quita" (apropia) la CPU al proceso 3, lo manda a la fila
 * de espera, y pone a ejecutar al proceso 1.
 */

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#define NUM_PROCESOS 3

// ============================================================
// 1. ESTRUCTURA DEL PROCESO (El "Paciente" o Programa)
// ============================================================
typedef struct {
  int id;        // Identificador único (PID en Linux)
  int prioridad; // Nivel de urgencia (Ej. 1 = Máxima urgencia)

  int t_llegada;       // Momento en que el usuario abre el programa
  int rafaga_total;    // Tiempo total de CPU que el programa necesita
  int rafaga_restante; // Tiempo que le falta por ejecutar

  int t_inicio; // Primera vez que entra a CPU (-1 si no ha entrado)
  int t_fin;    // Momento en que termina por completo

  int t_retorno;   // Tiempo de vida total (t_fin - t_llegada)
  int t_espera;    // Tiempo formado en la fila (t_retorno - rafaga_total)
  int t_respuesta; // Latencia inicial (t_inicio - t_llegada)
} Proceso;

// ============================================================================
// 2. ESTRUCTURAS DE DATOS: LISTAS ENLAZADAS (Linked Lists)
// ============================================================================
struct Nodo {
  Proceso dato;
  struct Nodo *siguiente;
};

struct Cola {
  struct Nodo *frente;
  struct Nodo *final;
};

struct Cola *crear_cola() {
  struct Cola *cola = (struct Cola *)malloc(sizeof(struct Cola));
  cola->frente = NULL;
  cola->final = NULL;
  return cola;
}

bool hay_procesos(struct Cola *cola) {
  if (cola->frente != NULL) {
    return true;
  } else {
    return false;
  }
}

void encolar_fifo(struct Cola *cola, Proceso p) {
  struct Nodo *nuevo = (struct Nodo *)malloc(sizeof(struct Nodo));
  nuevo->dato = p;
  nuevo->siguiente = NULL;

  // Si el cola está vacía, lo agregamos al principio
  if (hay_procesos(cola) == false) {
    cola->frente = nuevo;
    cola->final = nuevo;
  } else {
    // Si no, lo agregamos al final
    // recordando que cola posee 2 punteros tipo Nodo : frente y final
    // y a su vez cada Nodo posee un proceso y la direccion del sigiiente nodo
    // "lista enlazada"
    cola->final->siguiente = nuevo;
    cola->final = nuevo;
  }
}

Proceso desencolar(struct Cola *cola) {

  // recordando que emulamos un FIFO (primero en llegar, primero en salir)
  struct Nodo *temp = cola->frente;
  // dado que temp es un nodo, que a su vez posee un proceso y la direccion del
  // siguiente nodo
  Proceso dato = temp->dato;

  cola->frente = cola->frente->siguiente;
  if (cola->frente == NULL) {
    cola->final = NULL;
  }

  free(temp);
  return dato;
}

// sirve para ver el proceso que esta al frente de la cola sin hacerle nada
Proceso ver_frente(struct Cola *cola) { return cola->frente->dato; }

// ============================================================================
// 3. COLA DE PRIORIDAD (La magia principal de este algoritmo)
// ============================================================================
void insertar_por_prioridad(struct Cola *cola, Proceso p) {
  struct Nodo *nuevo = (struct Nodo *)malloc(sizeof(struct Nodo));
  nuevo->dato = p;
  nuevo->siguiente = NULL;

  if (cola->frente == NULL || p.prioridad < cola->frente->dato.prioridad) {
    nuevo->siguiente = cola->frente;
    cola->frente = nuevo;
    if (cola->final == NULL) {

      cola->final = nuevo;
    }
    return;
  }

  struct Nodo *actual = cola->frente;
  while (actual->siguiente != NULL &&
         actual->siguiente->dato.prioridad <= p.prioridad) {
    actual = actual->siguiente;
  }

  nuevo->siguiente = actual->siguiente;
  actual->siguiente = nuevo;

  if (nuevo->siguiente == NULL) {
    cola->final = nuevo;
  }
}

// ============================================================
// 4. PROTOTIPOS DE LAS FUNCIONES DEL PLANIFICADOR
// ============================================================
void gestionar_llegadas(struct Cola *llegadas, struct Cola *listos, int reloj);
void verificar_desalojo(struct Cola *listos, Proceso *proc_cpu,
                        bool *cpu_ocupada);
void asignar_cpu_si_libre(struct Cola *listos, Proceso *proc_cpu,
                          bool *cpu_ocupada, int reloj);
void ejecutar_un_segundo(Proceso *proc_cpu, bool *cpu_ocupada, int *reloj);
void finalizar_proceso_si_termina(Proceso *proc_cpu, bool *cpu_ocupada,
                                  int reloj);

// ============================================================
// 5. PROGRAMA PRINCIPAL (El SO en funcionamiento)
// ============================================================
int main() {
  int reloj_global = 0;
  bool cpu_ocupada = false;
  Proceso proc_cpu;

  struct Cola *cola_llegadas = crear_cola();
  struct Cola *cola_listos = crear_cola();

  // Carga inicial de procesos (Simulamos los "doble clic" del usuario)
  encolar_fifo(cola_llegadas, (Proceso){1, 2, 0, 5, 5, -1, 0, 0, 0, 0});
  encolar_fifo(cola_llegadas, (Proceso){2, 1, 2, 3, 3, -1, 0, 0, 0, 0});
  encolar_fifo(cola_llegadas, (Proceso){3, 3, 4, 2, 2, -1, 0, 0, 0, 0});

  printf("=== INICIO DE LA SIMULACION (PRIORIDAD APROPIATIVA) ===\n\n");

  // El Sistema Operativo nunca duerme mientras haya trabajo pendiente
  while (hay_procesos(cola_llegadas) == true ||
         hay_procesos(cola_listos) == true || cpu_ocupada == true) {

    // usamos funciones con apuntadores a las
    // variables controladoras de la funcion main para acceder
    // externamente, de esta forma evitamos propagar codigo spagetti en la
    // funcion principal
    gestionar_llegadas(cola_llegadas, cola_listos, reloj_global);

    verificar_desalojo(cola_listos, &proc_cpu, &cpu_ocupada);

    asignar_cpu_si_libre(cola_listos, &proc_cpu, &cpu_ocupada, reloj_global);

    ejecutar_un_segundo(&proc_cpu, &cpu_ocupada, &reloj_global);

    finalizar_proceso_si_termina(&proc_cpu, &cpu_ocupada, reloj_global);
  }

  printf("\n=== SIMULACION TERMINADA ===\n");
  printf("Tiempo total: %d segundos\n", reloj_global);

  free(cola_llegadas);
  free(cola_listos);
  return 0;
}

// ============================================================
// 6. IMPLEMENTACIÓN DE LAS FUNCIONES DEL PLANIFICADOR
// ============================================================

/* PASO VITAL 1: Revisa si algún nuevo proceso  */
// esto revisara si hay algun proceso que tenga programada su llegada
// en este momento de relog "revisamos antes"
// NOTE: * Le mandamos el puntero a la cola que contiene los procesos que aun
//       no han llegado "guardados en orden de llegada FIFO"
//       * le madandamos un puntero a la cola donde estan los procesos que ya
//       llegaron
//         y que estan esperando su turno de CPU
//       * le mandamos una copia del segundo actual de simulacion
//         solo queremos saber la hora, no necesitamos modificarla
void gestionar_llegadas(struct Cola *llegadas, struct Cola *listos, int reloj) {

  // aqui revisamos si hay algun proceso que tenga programada su llegada
  // NOTE: preguntamos si hay procesos esperando el la cola de futuras llegadas
  //         no no hay no hay nada que hacer, pero si hay
  //         REVISAMOS
  //
  while (hay_procesos(llegadas) == true &&
         ver_frente(llegadas).t_llegada == reloj) {

    Proceso recien_llegado = desencolar(llegadas);
    // Lo formamos en la fila respetando su nivel de urgencia
    insertar_por_prioridad(listos, recien_llegado);
    printf("[Reloj: %ds] LLEGO P%d (Prioridad %d, Rafaga %d)\n", reloj,
           recien_llegado.id, recien_llegado.prioridad,
           recien_llegado.rafaga_total);
  }
}

/* PASO VITAL 2: Si el primero de la fila es MÁS urgente, pateamos al de la CPU
 */
void verificar_desalojo(struct Cola *listos, Proceso *proc_cpu,
                        bool *cpu_ocupada) {
  if (*cpu_ocupada == true && hay_procesos(listos) == true) {
    // Usamos -> porque proc_cpu ahora es un puntero
    if (ver_frente(listos).prioridad < proc_cpu->prioridad) {
      printf("  [!] PREEMPTION: P%d (Pri %d) es DESALOJADO por un proceso mas "
             "importante.\n",
             proc_cpu->id, proc_cpu->prioridad);

      insertar_por_prioridad(
          listos, *proc_cpu); // Dereferenciamos con * para guardar el struct
      *cpu_ocupada = false;
    }
  }
}

/* PASO VITAL 3: Si la CPU está libre, pasamos al proceso que esté al frente de
 * la fila */
void asignar_cpu_si_libre(struct Cola *listos, Proceso *proc_cpu,
                          bool *cpu_ocupada, int reloj) {
  if (*cpu_ocupada == false && hay_procesos(listos) == true) {
    *proc_cpu = desencolar(listos);
    *cpu_ocupada = true;

    // Si entra por primera vez, calculamos su métrica de Respuesta (OSTEP)
    if (proc_cpu->t_inicio == -1) {
      proc_cpu->t_inicio = reloj;
      proc_cpu->t_respuesta = proc_cpu->t_inicio - proc_cpu->t_llegada;
    }
    printf("  -> CPU asignada a P%d (Prioridad %d)\n", proc_cpu->id,
           proc_cpu->prioridad);
  }
}

/* PASO VITAL 4: El tiempo avanza inevitablemente y el proceso trabaja */
// para acceder directamente a las variables controladoras que estan en main
//  uso el operador * para acceder *directamente_al_contenido de la variable que
//  esta dentro de main.
void ejecutar_un_segundo(Proceso *proc_cpu, bool *cpu_ocupada, int *reloj) {
  if (*cpu_ocupada == true) {
    proc_cpu->rafaga_restante--; // El proceso ejecuta una instrucción
  }
  (*reloj)++; // Avanzamos el reloj de la vida real (usamos paréntesis para no
              // generar problemas)
}

/* PASO VITAL 5: Si ya no le queda tiempo, el proceso completó su tarea y se va
 * a casa */
void finalizar_proceso_si_termina(Proceso *proc_cpu, bool *cpu_ocupada,
                                  int reloj) {
  if (*cpu_ocupada == true && proc_cpu->rafaga_restante == 0) {
    proc_cpu->t_fin = reloj;
    proc_cpu->t_retorno = proc_cpu->t_fin - proc_cpu->t_llegada;
    proc_cpu->t_espera = proc_cpu->t_retorno - proc_cpu->rafaga_total;

    printf(
        "[Reloj: %ds] P%d TERMINO su ejecucion. [Espera: %ds, Retorno: %ds]\n",
        reloj, proc_cpu->id, proc_cpu->t_espera, proc_cpu->t_retorno);

    *cpu_ocupada = false; // La CPU queda libre para el siguiente ciclo
  }
}
