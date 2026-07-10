/*
 * ALGORITMO DE PLANIFICACIÓN: PRIORIDAD APROPIATIVA (PREEMPTIVE)
 * * Regla de oro: El procesador siempre lo tiene el proceso con la prioridad
 * más alta (el número MÁS BAJO = mayor prioridad).
 * Si llega un proceso más importante, el actual es desalojado inmediatamente.
 */
/*
 * ============================================================================
 * ALGORITMO DE PLANIFICACIÓN: PRIORIDAD APROPIATIVA (PREEMPTIVE)
 * ============================================================================
 * * OBJETIVO DEL ALGORITMO:
 * Simular cómo un Sistema Operativo moderno (como Linux o Windows) decide qué
 * programa usar en la CPU. En lugar de repartir el tiempo equitativamente
 * (como Round Robin), este algoritmo es jerárquico.
 * * REGLA DE ORO: La CPU SIEMPRE debe estar ejecutando el proceso con la
 * prioridad más alta (el número más bajo representa la urgencia máxima).
 * * ¿QUÉ SIGNIFICA APROPIATIVO (PREEMPTIVE)?
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

/*
 * ============================================================================
 * 2. ESTRUCTURAS DE DATOS: LISTAS ENLAZADAS (Linked Lists)
 * ============================================================================
 * ¿Por qué usamos Listas Enlazadas y no Arreglos normales?
 * 1. Dinamismo: Un SO no sabe cuántos procesos se van a abrir. La lista puede
 * crecer o encogerse sin desperdiciar memoria.
 * 2. Inserción Eficiente: Como ordenamos por prioridad, frecuentemente
 * necesitaremos "colar" procesos en el medio de la fila. En una lista
 * enlazada solo reasignamos punteros, en un arreglo tendríamos que
 * desplazar todos los elementos.
 */

// El Nodo es el "Vagón" del tren. Lleva al pasajero (Proceso) y un enganche al
// siguiente.
struct Nodo {
  Proceso dato;
  struct Nodo *siguiente;
};

// La Cola es la "Estación". Solo necesita saber dónde empieza y dónde termina
// el tren.
struct Cola {
  struct Nodo *frente;
  struct Nodo *final;
};

// Inicializa la estación (crea una cola vacía)
struct Cola *crear_cola() {
  struct Cola *cola = (struct Cola *)malloc(sizeof(struct Cola));
  cola->frente = NULL;
  cola->final = NULL;
  return cola;
}

// Verifica si hay vagones en la estación
bool hay_procesos(struct Cola *cola) {
  if (cola->frente != NULL) {
    return true;
  } else {
    return false;
  }
}

// Mete un proceso estrictamente al FINAL de la fila (First In, First Out).
// NOTA: Esta función SOLO se usa para la fila de "Llegadas Originales",
// porque ahí solo nos importa el orden del tiempo, no la prioridad.
void encolar_fifo(struct Cola *cola, Proceso p) {
  struct Nodo *nuevo = (struct Nodo *)malloc(sizeof(struct Nodo));
  nuevo->dato = p;
  nuevo->siguiente = NULL;

  if (hay_procesos(cola) == false) {
    cola->frente = nuevo;
    cola->final = nuevo;
  } else {
    cola->final->siguiente = nuevo;
    cola->final = nuevo;
  }
}

// Saca y elimina al proceso que está al INICIO de la fila.
Proceso desencolar(struct Cola *cola) {
  struct Nodo *temp = cola->frente;
  Proceso dato = temp->dato;

  cola->frente = cola->frente->siguiente;
  if (cola->frente == NULL) {
    cola->final = NULL; // Si se fue el último, limpiamos el final
  }

  free(temp);
  return dato;
}

// Permite ver quién está al frente de la fila sin sacarlo de ella.
Proceso ver_frente(struct Cola *cola) { return cola->frente->dato; }

/*
 * ============================================================================
 * 3. COLA DE PRIORIDAD (La magia principal de este algoritmo)
 * ============================================================================
 * Esta función es el corazón del planificador. No forma a los procesos al final
 * de la fila, sino que busca su lugar correspondiente basado en su urgencia.
 */
void insertar_por_prioridad(struct Cola *cola, Proceso p) {
  struct Nodo *nuevo = (struct Nodo *)malloc(sizeof(struct Nodo));
  nuevo->dato = p;
  nuevo->siguiente = NULL;

  // CASO A: Si la fila está vacía, o si el nuevo proceso es MÁS URGENTE
  // que el primero de la fila, se mete hasta adelante. (Menor número = Más
  // urgente)
  if (cola->frente == NULL || p.prioridad < cola->frente->dato.prioridad) {
    nuevo->siguiente = cola->frente;
    cola->frente = nuevo;
    if (cola->final == NULL) {
      cola->final = nuevo;
    }
    return;
  }

  // CASO B: Recorremos los "vagones" buscando a alguien menos urgente que
  // nosotros
  struct Nodo *actual = cola->frente;
  while (actual->siguiente != NULL &&
         actual->siguiente->dato.prioridad <= p.prioridad) {
    actual = actual->siguiente;
  }

  // Al encontrarlo, "rompemos" el enganche y nos metemos en medio
  nuevo->siguiente = actual->siguiente;
  actual->siguiente = nuevo;

  // Actualizamos el final si quedamos hasta atrás
  if (nuevo->siguiente == NULL) {
    cola->final = nuevo;
  }
}

// ============================================================
// 4. PROGRAMA PRINCIPAL (El SO en funcionamiento)
// ============================================================
int main() {
  int reloj_global = 0; // Nuestro cronómetro del mundo real

  // Variables de control de la CPU
  bool cpu_ocupada = false;
  Proceso proc_cpu; // Guarda el proceso que está corriendo actualmente

  // cola_llegadas: Guarda los procesos que el usuario va a abrir en el futuro.
  // cola_listos:   Guarda los procesos que ya llegaron y están compitiendo por
  // la CPU.
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

    /* =============================================================
     * PASO VITAL 1: VERIFICAR LA "PUERTA DE ENTRADA" (Llegadas)
     * En este segundo exacto (reloj_global), revisamos si algún programa
     * nuevo acaba de ser abierto por el usuario.
     * ============================================================= */
    while (hay_procesos(cola_llegadas) == true &&
           ver_frente(cola_llegadas).t_llegada == reloj_global) {
      Proceso recien_llegado = desencolar(cola_llegadas);
      // Lo formamos en la fila de Listos respetando su nivel de urgencia
      insertar_por_prioridad(cola_listos, recien_llegado);
      printf("[Reloj: %ds] LLEGO P%d (Prioridad %d, Rafaga %d)\n", reloj_global,
             recien_llegado.id, recien_llegado.prioridad,
             recien_llegado.rafaga_total);
    }

    /* =============================================================
     * PASO VITAL 2: VERIFICAR "DESALOJO" (Preemption)
     * Aquí se aplica la regla "Apropiativa". Comparamos al proceso
     * que está en la CPU contra el primero de la fila de Listos.
     * Si el de la fila es MÁS urgente, pateamos al de la CPU.
     * ============================================================= */
    if (cpu_ocupada == true && hay_procesos(cola_listos) == true) {
      if (ver_frente(cola_listos).prioridad < proc_cpu.prioridad) {
        printf("  [!] PREEMPTION: P%d (Pri %d) es DESALOJADO por un proceso "
               "mas importante.\n",
               proc_cpu.id, proc_cpu.prioridad);

        // Mandamos al proceso desalojado de vuelta a la fila
        insertar_por_prioridad(cola_listos, proc_cpu);
        cpu_ocupada = false;
      }
    }

    /* =============================================================
     * PASO VITAL 3: ASIGNAR LA CPU AL MÁS URGENTE
     * Si la CPU está libre (porque el anterior terminó o fue desalojado),
     * pasamos al proceso que esté al frente de la fila de Listos.
     * ============================================================= */
    if (cpu_ocupada == false && hay_procesos(cola_listos) == true) {
      proc_cpu = desencolar(cola_listos);
      cpu_ocupada = true;

      // Si entra por primera vez, calculamos su métrica de Respuesta (OSTEP)
      if (proc_cpu.t_inicio == -1) {
        proc_cpu.t_inicio = reloj_global;
        proc_cpu.t_respuesta = proc_cpu.t_inicio - proc_cpu.t_llegada;
      }
      printf("  -> CPU asignada a P%d (Prioridad %d)\n", proc_cpu.id,
             proc_cpu.prioridad);
    }

    /* =============================================================
     * PASO VITAL 4: EJECUTAR 1 SEGUNDO DE CPU
     * El tiempo avanza inevitablemente. El proceso en la CPU trabaja
     * y reducimos el tiempo que le falta para terminar.
     * ============================================================= */
    if (cpu_ocupada == true) {
      proc_cpu.rafaga_restante--;
      reloj_global++; // Avanza el cronómetro

      // Si ya no le queda tiempo, el proceso completó su tarea
      if (proc_cpu.rafaga_restante == 0) {
        proc_cpu.t_fin = reloj_global;
        proc_cpu.t_retorno = proc_cpu.t_fin - proc_cpu.t_llegada;
        proc_cpu.t_espera = proc_cpu.t_retorno - proc_cpu.rafaga_total;

        printf("[Reloj: %ds] P%d TERMINO su ejecucion. [Espera: %ds, Retorno: "
               "%ds]\n",
               reloj_global, proc_cpu.id, proc_cpu.t_espera,
               proc_cpu.t_retorno);

        cpu_ocupada = false; // La CPU queda libre para el siguiente segundo
      }
    } else {
      // Si no hay procesos ejecutando ni esperando, la CPU está Ociosa (Idle).
      // Simplemente dejamos pasar el tiempo hasta que llegue algo.
      reloj_global++;
    }
  }

  printf("\n=== SIMULACION TERMINADA ===\n");
  printf("Tiempo total: %d segundos\n", reloj_global);

  // Limpieza de la memoria dinámica utilizada por las estaciones de la Cola
  free(cola_llegadas);
  free(cola_listos);
  return 0;
}
