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
// regresamos una copua del proceso que se encuentre
// si es nulo podremos manejar que no hay nada en la parte frente de la cola
Proceso ver_frente(struct Cola *cola) { return cola->frente->dato; }

// ============================================================================
// 3. COLA DE PRIORIDAD (La magia principal de este algoritmo)
// ============================================================================
//
void insertar_por_prioridad(struct Cola *cola_listos, Proceso p) {

  struct Nodo *nuevo = (struct Nodo *)malloc(sizeof(struct Nodo));
  nuevo->dato = p;
  nuevo->siguiente = NULL;

  // CASO 1 : La cola esta vacio o el nuevo elemento es ma importante que el que
  // ya esta enfrente
  //
  // si la cola esta vacia "no hay nada en la frente"
  // entonces el letrero final debe de apuntar a a este nuevo nodo.
  // recordando que aqui las prioridades pequeñas son las
  // mas importantes.
  // vasicamente comprobamos si esta vacio o si el nuevo elemento es mas
  // importante que el que ya esta
  if (cola_listos->frente == NULL ||
      p.prioridad < cola_listos->frente->dato.prioridad) {
    nuevo->siguiente = cola_listos->frente;
    cola_listos->frente = nuevo;

    if (cola_listos->final == NULL) {

      cola_listos->final = nuevo;
    }
    return;
  }

  // si no es vacio y no es mas importante
  // usamos este nodo auxiliar
  struct Nodo *actual = cola_listos->frente;

  // el proceso debe de insrtarse en algun lado, en medio o alfinal de la fila.
  // el objetico de este while es caminar a lo largo de los vagones y encontrar
  // el lugar exacto donde menter este nuevo proceso sin romper
  // el orden de pripridad.
  //  como la lista es de un solo sentido
  //  actual->siguiente ==null indicaria que nos pasamos
  //  actual->siguiente->dato.prioridad <= p.prioridad idica que sigua avanzando
  //  mientras el que este adelante sea mas urgente que p
  //  [frente]->[]->[]->[]->[]->[final]->null [RECORDANDO QUE SALEN POR FRENTE]
  //  vasicamente vamos iterando la flechita.
  //  no vamo a ir fijando en el siguoente nodo
  //  simpre buscando un numero mayor que la prioridad de p
  while (actual->siguiente != NULL &&
         actual->siguiente->dato.prioridad <= p.prioridad) {
    actual = actual->siguiente;
  }

  // el vagon nuevo estira el brazo derecho y agarra al vagon que esta
  // adelante de actual
  // con esto no perdermos la direccion del siguiente nodo y no rompemos la
  // lista enlazada
  nuevo->siguiente = actual->siguiente;

  // el vagon actual suelta el brazo del siguiente viejo y ahora
  // agarra al vagon nuevo
  actual->siguiente = nuevo;

  // esto es por si lo que insertamos era el menos urgente y se inserto
  // al final de la cola enlzada
  // debemos de mover el letrero a este elemento que ahora es el final.
  if (nuevo->siguiente == NULL) {
    cola_listos->final = nuevo;
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
                                  int reloj, Proceso terminados[],
                                  int *contador_terminados);

void imprimir_metricas(Proceso terminados[], int total_procesos);

// ============================================================
// 5. PROGRAMA PRINCIPAL (El SO en funcionamiento)
// ============================================================
int main() {
  int reloj_global = 0;
  bool cpu_ocupada = false;
  Proceso proc_cpu;

  struct Cola *cola_llegadas = crear_cola();
  struct Cola *cola_listos = crear_cola();

  // Aquí guardaremos a los procesos que vayan terminando para el reporte
  Proceso procesos_terminados[NUM_PROCESOS];
  int contador_terminados = 0;

  // Carga inicial de procesos
  encolar_fifo(cola_llegadas, (Proceso){1, 2, 0, 5, 5, -1, 0, 0, 0, 0});
  encolar_fifo(cola_llegadas, (Proceso){2, 1, 2, 3, 3, -1, 0, 0, 0, 0});
  encolar_fifo(cola_llegadas, (Proceso){3, 3, 4, 2, 2, -1, 0, 0, 0, 0});

  /*
   otro ejemplo
   encolar_fifo(cola_llegadas, (Proceso){1, 3, 0, 4, 4, -1, 0, 0, 0, 0});
   encolar_fifo(cola_llegadas, (Proceso){2, 1, 0, 3, 3, -1, 0, 0, 0, 0});
   encolar_fifo(cola_llegadas, (Proceso){3, 2, 0, 2, 2, -1, 0, 0, 0, 0});

   trp = 5.67
   tep: 2.67
   * */

  printf("=== INICIO DE LA SIMULACION (PRIORIDAD APROPIATIVA) ===\n\n");

  // El Sistema Operativo nunca duerme mientras haya trabajo pendiente
  // mientras haya un proceso esperando turno "listos", o la cpu eses
  // atendiendo a alguien
  while (hay_procesos(cola_llegadas) == true ||
         hay_procesos(cola_listos) == true || cpu_ocupada == true) {

    // usamos funciones con apuntadores a las
    // variables controladoras de la funcion main para acceder
    // externamente, de esta forma evitamos propagar codigo spagetti en la
    // funcion principal

    // NOTE: preguntamos si llego algún proceso nuevo justo en el segundo actual
    // de simulacion.
    gestionar_llegadas(cola_llegadas, cola_listos, reloj_global);

    // NOTE: si entro uno con mas prioridad que el que esta atendiendo la cpu
    //       lo desalojamos.
    verificar_desalojo(cola_listos, &proc_cpu, &cpu_ocupada);

    // NOTE: Si la CPU esta libre, entonces sacara al primero de la cola de
    // listos
    //       y ponlo a chambear
    asignar_cpu_si_libre(cola_listos, &proc_cpu, &cpu_ocupada, reloj_global);

    // NOTE: pasara un segundo
    ejecutar_un_segundo(&proc_cpu, &cpu_ocupada, &reloj_global);

    // NOTE: si ya no queda tiempo, el proceso termina
    finalizar_proceso_si_termina(&proc_cpu, &cpu_ocupada, reloj_global,
                                 procesos_terminados, &contador_terminados);
  }

  printf("\n=== SIMULACION TERMINADA ===\n");
  printf("Tiempo total: %d segundos\n", reloj_global);

  imprimir_metricas(procesos_terminados, NUM_PROCESOS);

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

    // Si entra por primera vez, calculamos su métrica de Respuesta
    // con el -1 sabemos que es la primera vez que entra a la CPU
    if (proc_cpu->t_inicio == -1) {

      // el tiempo de inicio corresponde al reloj actual
      proc_cpu->t_inicio = reloj;

      // calculamos el tiempo de respuesta usando la forrmula
      // tiempo de respuesta = tiempo de inicio - tiempo de llegada
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
                                  int reloj, Proceso terminados[],
                                  int *contador_terminados) {
  if (*cpu_ocupada == true && proc_cpu->rafaga_restante == 0) {
    proc_cpu->t_fin = reloj;
    proc_cpu->t_retorno = proc_cpu->t_fin - proc_cpu->t_llegada;
    proc_cpu->t_espera = proc_cpu->t_retorno - proc_cpu->rafaga_total;

    printf(
        "[Reloj: %ds] P%d TERMINO su ejecucion. [Espera: %ds, Retorno: %ds]\n",
        reloj, proc_cpu->id, proc_cpu->t_espera, proc_cpu->t_retorno);

    // ¡NUEVO! Guardamos la estructura final del proceso en nuestro arreglo
    // histórico
    terminados[*contador_terminados] = *proc_cpu;
    *contador_terminados = *contador_terminados + 1;

    *cpu_ocupada = false; // La CPU queda libre para el siguiente ciclo
  }
}

// ============================================================
// 7. FUNCION PARA GENERAR LA TABLA DE METRICAS
// ============================================================
void imprimir_metricas(Proceso terminados[], int total_procesos) {

  // 1. Ordenamos el arreglo de terminados por el ID (P1, P2, P3...)
  // Usamos un Método de Burbuja muy explícito porque los procesos
  // pudieron haber terminado en desorden debido a la prioridad.
  for (int i = 0; i < total_procesos - 1; i = i + 1) {
    for (int j = 0; j < total_procesos - i - 1; j = j + 1) {

      if (terminados[j].id > terminados[j + 1].id) {
        // Intercambiamos los procesos
        Proceso temporal = terminados[j];
        terminados[j] = terminados[j + 1];
        terminados[j + 1] = temporal;
      }
    }
  }

  // 2. Calculamos las sumatorias para los promedios
  float suma_retorno = 0;
  float suma_espera = 0;

  for (int i = 0; i < total_procesos; i = i + 1) {
    suma_retorno = suma_retorno + terminados[i].t_retorno;
    suma_espera = suma_espera + terminados[i].t_espera;
  }

  float tiempo_retorno_promedio = suma_retorno / total_procesos;
  float tiempo_espera_promedio = suma_espera / total_procesos;

  // 3. Imprimimos la tabla con el formato exacto solicitado
  printf("\n\n=== REPORTE DE METRICAS (OSTEP) ===\n");
  printf("ID\tTF\tTR\tTE\t|\tTRP\tTEP\n");
  printf("-----------------------------------------------------------\n");

  for (int i = 0; i < total_procesos; i = i + 1) {

    // Imprimimos los datos individuales del proceso usando tabulaciones (\t)
    printf("P%d\t%d\t%d\t%d\t|", terminados[i].id, terminados[i].t_fin,
           terminados[i].t_retorno, terminados[i].t_espera);

    // Solo en la primera fila (i == 0) imprimimos los promedios del lado
    // derecho
    if (i == 0) {
      printf("\t%.1f\t%.1f\n", tiempo_retorno_promedio, tiempo_espera_promedio);
    } else {
      printf("\n");
    }
  }
  printf("===========================================================\n");
}
