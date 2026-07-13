/*
 * NOTE: Simulador del Planificador de CPU: Round Robin
 * Basado en los conceptos de OSTEP Capítulo 7: Scheduling.
 *
 * Aspectos simulados:
 * - Cola de listos con política FIFO.
 * - Reloj global del sistema que avanza dinámicamente.
 * - Control de estados de la CPU (Ejecución de Quantum y CPU Ociosa).
 * - Recolección ordenada y cálculo de métricas de rendimiento.
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

  // Datos iniciales (parámetros de nacimiento del proceso)
  int t_llegada;
  int rafaga_total;

  // Variables de control dinámico (cambian en tiempo de ejecución)
  int rafaga_restante;
  int t_inicio; // -1 indica que la CPU no lo ha atendido aún
  int t_fin;

  // Métricas de rendimiento finales (calculadas al terminar)
  int t_retorno;
  int t_espera;
  int t_respuesta;
} Proceso;

// ============================================================
// 2. COLA (Lista Ligada FIFO)
// sera la vagoneta, cada elemento de nuestra fila es un Nodo
// el cual tiene 2 cosas, la informacion de cada proceso
// y el brazo que apunta al siguiente nodo.
// ============================================================
struct Nodo {
  Proceso dato;
  struct Nodo *siguiente;
};

// almacenamos la informacion del
// inicio y final de la cola enlzada.
struct Cola {
  // el priemro
  struct Nodo *frente;
  // el ultimo
  struct Nodo *final;
};

struct Cola *crear_cola() {
  struct Cola *cola = (struct Cola *)malloc(sizeof(struct Cola));
  cola->frente = NULL;
  cola->final = NULL;
  return cola;
}

int hay_procesos_en_cola(struct Cola *cola) {
  if (cola->frente == NULL) {
    return 0;
  } else {
    return 1;
  }
}

// agregar al final de la cola [frente]->[]->[]->[final]->null
void encolar(struct Cola *cola, Proceso p) {

  // creamos un nodo auxiliar.
  //  Considerando que va a ser el ultimo su brazo apunta a NULL
  struct Nodo *nuevoNodo = (struct Nodo *)malloc(sizeof(struct Nodo));
  nuevoNodo->dato = p;
  nuevoNodo->siguiente = NULL;

  if (hay_procesos_en_cola(cola) == 0) {
    cola->frente = nuevoNodo;
    cola->final = nuevoNodo;
  } else {
    cola->final->siguiente = nuevoNodo;
    cola->final = nuevoNodo;
  }
}

Proceso desencolar(struct Cola *cola) {

  // nececitamos este temporal
  // para no perder la referencia al primer elemento de la cola
  // al realizar cola->frente = cola->frente->siguiente
  struct Nodo *temp = cola->frente;
  // copiamos el dato del primer elemento de la cola
  // para poder retornarlo.
  Proceso dato = temp->dato;

  cola->frente = cola->frente->siguiente;

  // esto es para saber si sacamos el ultimo elemento
  // dentro de la cola, si es asi debemos de poner el final en NULL
  if (cola->frente == NULL) {
    // si no lo hacemos, se quedaria apuntando a un elemento muerto
    // ya que vamos a usar free
    cola->final = NULL;
  }

  free(temp);
  return dato;
}

Proceso ver_frente(struct Cola *cola) {
  // determinamos si el frente de nuestra cola tiene o no elemento.
  return cola->frente->dato;
}

// ============================================================
// 3. FUNCIONES AUXILIARES DE PLANIFICACIÓN (Modularización)
// ============================================================

/*
 * Mueve los procesos que acaban de llegar de la "cola de espera"
 * a la "cola de listos", basándose en el tiempo del reloj global.
 */
void gestionar_nuevas_llegadas(struct Cola *espera, struct Cola *listos,
                               int reloj_global) {
  // ver_frente nos regresa una copia del primer elemento de la cola "proceso"
  while (hay_procesos_en_cola(espera) == 1 &&
         ver_frente(espera).t_llegada <= reloj_global) {

    // si se cumple la considicion entonces desencolamos el primer elemento
    // de la cola de espera y lo colocamos al final de la cola de listos.
    Proceso llegado = desencolar(espera);
    encolar(listos, llegado);
    printf("[Reloj: %ds] Proceso %d llega y entra a la cola de listos\n",
           reloj_global, llegado.id);
  }
}

/*
 * Registra el primer contacto del proceso con la CPU.
 * Calcula el Tiempo de Respuesta (t_respuesta = t_inicio - t_llegada).
 */
void registrar_primer_contacto(Proceso *proc, int reloj_global) {

  // solo registramos el primer proceso que entra a la cpu (t_inicio == -1)
  // establecemos que si es -1 significa que es la primera vez que
  // entra al cpu.
  if (proc->t_inicio == -1) {
    // dado que es un proceso que apenas esta entrando
    // esta parte de abajo se cumple
    proc->t_inicio = reloj_global;

    // calculamos el tiempo de respuesta del proyecto
    // usando la formula
    proc->t_respuesta = proc->t_inicio - proc->t_llegada;
  }
}

/*
 * Simula la ejecución de un Quantum en la CPU.
 * Maneja los dos escenarios posibles:
 * - CASO 1: El proceso termina antes o justo en el límite del Quantum.
 * - CASO 2: El proceso agota el Quantum y debe reencolarse.
 */
void ejecutar_quantum(Proceso *proc, struct Cola *espera, struct Cola *listos,
                      int *reloj_global, Proceso terminados[],
                      int *contador_terminados) {

  printf("[Reloj: %ds] CPU ejecuta Proceso %d (Restante: %ds)\n", *reloj_global,
         proc->id, proc->rafaga_restante);

  // esto implica que se terminara de ejecutar
  // en solo el tiempo del cuantum
  if (proc->rafaga_restante <= QUANTUM) {
    // --------------------------------------------------------
    // CASO 1: EL PROCESO TERMINA SU TRABAJO
    // --------------------------------------------------------
    // usamos *al inicio para acceder a la variable que esta dentro de
    // main. basicamente lo que asemos es sumar la rafaga restante del
    // proceso que se terminara en un solo Quantum.
    *reloj_global = *reloj_global + proc->rafaga_restante;
    // cambiamos la rafaga restante a cero, ya que no se repetira mas :D
    proc->rafaga_restante = 0;

    // Cálculo de métricas finales de rendimiento
    proc->t_fin = *reloj_global;

    // usamos la formula tretorno = t_fin - t_llegada
    proc->t_retorno = proc->t_fin - proc->t_llegada;
    // usamos la formula para calcular el t_espera = t_retorno - rafaga_total
    proc->t_espera = proc->t_retorno - proc->rafaga_total;

    // Almacenamos el proceso completado en nuestro registro histórico
    // esto lo nececitaremos para imprimirlo despues
    terminados[*contador_terminados] = *proc;
    // incrementamos el contador de terminados "obvio, porque ya se termino"
    *contador_terminados = *contador_terminados + 1;

    printf("  -> Proceso %d TERMINO. [Retorno: %ds | Espera: %ds | Respuesta: "
           "%ds]\n",
           proc->id, proc->t_retorno, proc->t_espera, proc->t_respuesta);

  } else {
    // --------------------------------------------------------
    // CASO 2: EL PROCESO ES DESALOJADO POR AGOTAR EL QUANTUM
    // aqui lo que sucedera es que el quantum no es suficiente para que
    // se termine el proceso, por loque quedara rafaga_restante >0
    // --------------------------------------------------------
    // ya que no termin, implica que sumamos el quantum completo
    *reloj_global = *reloj_global + QUANTUM;
    // y a su rafaga restante le restamos un quantum
    proc->rafaga_restante = proc->rafaga_restante - QUANTUM;

    printf("  -> Proceso %d agoto Quantum. Vuelve a la cola (Faltan: %ds)\n",
           proc->id, proc->rafaga_restante);

    // REGLA DE ORO: Antes de reencolar al proceso que acaba de salir,
    // debemos revisar si llegaron procesos NUEVOS en este lapso de tiempo.
    // esta parte se me habia olvidado y no me salian los calculos :P
    // verificamos de pura casualida si no llego otro proceso al sumar el
    // quantum
    gestionar_nuevas_llegadas(espera, listos, *reloj_global);

    // Ahora sí, el proceso desalojado vuelve al final de la cola de listos
    // ya que verificamos que no llego ningun otro proceso a la lista de listos
    // entonces encolamos
    // LA COLA DE ESPERA ES EL FUTURO, PARA PROCESOS QUE NO HAN NACIDO
    // LA COLA DE LISTOS ES PARA PROCESOS QUE YA NACIERON Y ESTAN LISTOS PARA
    // EJECUTARSE
    encolar(listos, *proc);
  }
}

// ============================================================
// 4. CARGA DE PROCESOS DE PRUEBA
// ============================================================
void cargar_procesos_ejemplo(struct Cola *espera) {
  // Configuración clásica de prueba:
  // P1: Llegada 0 | Ráfaga 5
  // P2: Llegada 4 | Ráfaga 2
  // P3: Llegada 5 | Ráfaga 4
  Proceso p1 = {1, 0, 5, 5, -1, 0, 0, 0, 0};
  Proceso p2 = {2, 4, 2, 2, -1, 0, 0, 0, 0};
  Proceso p3 = {3, 5, 4, 4, -1, 0, 0, 0, 0};

  encolar(espera, p1);
  encolar(espera, p2);
  encolar(espera, p3);

  /*
    Otro ejemplo
      Proceso p1 = {1, 0, 4, 4, -1, 0, 0, 0, 0};
      Proceso p2 = {2, 0, 5, 5, -1, 0, 0, 0, 0};
      Proceso p3 = {3, 0, 3, 3, -1, 0, 0, 0, 0};

      encolar(espera, p1);
      encolar(espera, p2);
      encolar(espera, p3);

      TRP = 10.33
      TEP = 6.33

   * */
}

// ============================================================
// 5. REPORTE DE METRICAS FINALES
// ============================================================
void imprimir_reporte_final(Proceso terminados[], int total_procesos) {
  // Ordenamos los procesos por su ID (P1, P2, P3...) usando algoritmo Burbuja
  for (int i = 0; i < total_procesos - 1; i = i + 1) {
    for (int j = 0; j < total_procesos - i - 1; j = j + 1) {
      if (terminados[j].id > terminados[j + 1].id) {
        Proceso temporal = terminados[j];
        terminados[j] = terminados[j + 1];
        terminados[j + 1] = temporal;
      }
    }
  }

  // Sumatorias para cálculo de promedios globales
  float suma_retorno = 0;
  float suma_espera = 0;

  for (int i = 0; i < total_procesos; i = i + 1) {
    suma_retorno = suma_retorno + terminados[i].t_retorno;
    suma_espera = suma_espera + terminados[i].t_espera;
  }

  float trp = suma_retorno / total_procesos;
  float tep = suma_espera / total_procesos;

  // Imprimir tabla resumen de salida
  printf("\n\n=== REPORTE DE METRICAS ===\n");
  printf("     TF  TR  TE       |  TRP   TEP\n");

  for (int i = 0; i < total_procesos; i = i + 1) {
    if (i == 0) {
      printf("p%d   %d   %d    %d        | %.1f   %.1f\n", terminados[i].id,
             terminados[i].t_fin, terminados[i].t_retorno,
             terminados[i].t_espera, trp, tep);
    } else {
      printf("p%d   %d   %d    %d\n", terminados[i].id, terminados[i].t_fin,
             terminados[i].t_retorno, terminados[i].t_espera);
    }
  }
}

// ============================================================
// 6. PROGRAMA PRINCIPAL
// ============================================================
int main() {

  int reloj_global = 0;
  int contador_terminados = 0;
  Proceso terminados[NUM_PROCESOS];

  struct Cola *cola_espera = crear_cola();

  struct Cola *cola_listos = crear_cola();

  printf("=== INICIO SIMULACION ROUND ROBIN ===\n");
  printf("Quantum: %d | Procesos: %d\n\n", QUANTUM, NUM_PROCESOS);

  cargar_procesos_ejemplo(cola_espera);

  printf("--------------------------------------\n");

  // BUCLE DE PLANIFICACIÓN CENTRAL
  // hay procesos en cola igual a 1 significa que si hay almenos un elemento en
  // frente de la cola.
  while (hay_procesos_en_cola(cola_listos) == 1 ||
         hay_procesos_en_cola(cola_espera) == 1) {

    // PASO 1: Transferir de "espera" a "listos" a los procesos que ya llegaron
    gestionar_nuevas_llegadas(cola_espera, cola_listos, reloj_global);

    // PASO 2: Control de CPU Ociosa (Si la cola de listos esta vacia, avanzamos
    // al sig. evento)
    // si se da este caso significa que no encontro procesos en la cola de
    // listos por lo que debemos de galar el reloj "usando de referencia el
    // tiempo del proceso que se encuentre dentro de la cola de espera en la
    // posicion de frente, de esta forma el tiempo global se ajustaria para que
    // la proxima iteracion el proceso pueda entrar a la lista de cola_listos"
    // aqui ssaltamos para no quedarnos de 1 en 1
    //
    if (hay_procesos_en_cola(cola_listos) == 0) {
      reloj_global = ver_frente(cola_espera).t_llegada;
      printf("[Reloj: %ds] CPU ociosa, esperando la siguiente llegada...\n",
             reloj_global);
      // esto hace que los demas pasos se omitan, y regresa nuevamente
      // al ciclo whilw para ejecutar nuevamente sus pasos, de
      // esa manera hacemos que el procesador no se
      // quede perdiendo valiosotiempo.
      continue;
    }

    // PASO 3: Selección de proceso (Desencolar de listos)
    // llegar aqui significa que si hay procesos en la cola de listos
    // ya que nos blindamos usando la instruccion continue
    // en el paso anterior.
    // OBTENEMOS EL PROCESO QUE ESTA EN EL FRENTE DE LA COLA DE LISTOS
    Proceso proc_actual = desencolar(cola_listos);

    // PASO 4: Registrar tiempos de inicio y respuesta si es la primera vez que
    // entra a CPU
    // recordando que se debe de cumplir la condicion de tiempo_inicio == -1
    registrar_primer_contacto(&proc_actual, reloj_global);

    // PASO 5: Ejecución del Quantum en CPU
    // Esto es importante, aqui debemos de registra los terminados en caso de
    // que terminen claro esta, es por ello que lo mandamos por si las moscas
    ejecutar_quantum(&proc_actual, cola_espera, cola_listos, &reloj_global,
                     terminados, &contador_terminados);
  }

  printf("\n=== SIMULACION TERMINADA ===\n");
  printf("Tiempo total de uso de CPU: %d segundos\n", reloj_global);

  // Impresión de las estadísticas finales en formato de tabla
  imprimir_reporte_final(terminados, NUM_PROCESOS);

  // Liberación de recursos de memoria para evitar fugas
  free(cola_espera);
  free(cola_listos);

  return 0;
}
