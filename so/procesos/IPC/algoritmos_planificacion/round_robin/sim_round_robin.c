/*
Emularemos el algoritmo Round Robin (RR)
simularemos un doctor que tiene que checar a sus pacientes "procesos",
pero hay un tiempo limite para atender a cada paciente (QUANTUM).
*/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define QUANTUM 4
#define PACIENTES 3

// Establecemos la estructura de los pacientes
typedef struct {
  int id_proceso;
  int tiempo_rafaga;   // Tiempo total original
  int tiempo_restante; // Tiempo que le falta
  int tt_alta;         // Tiempo de Retorno (Turnaround Time)
} Proceso;

// Declaración de función
void pedirPacientes(Proceso listaPacientes[]);

int main() {
  srand(time(NULL));

  Proceso listaPacientes[PACIENTES];
  pedirPacientes(listaPacientes);

  int cantidad_de_pacientes = PACIENTES;
  int reloj_global = 0;
  int cnt_paciente = 0;

  printf("=== INICIO DE LA SIMULACION ROUND ROBIN ===\n");
  printf("Quantum: %d | Pacientes: %d\n\n", QUANTUM, PACIENTES);

  // --- INICIO DE ROUND ROBIN ---
  while (cantidad_de_pacientes > 0) {

    // Solo atendemos si el paciente aún necesita tiempo
    if (listaPacientes[cnt_paciente].tiempo_restante > 0) {

      printf("[Tiempo %d] Llama al Paciente %d (Le restan %d seg)\n",
             reloj_global, listaPacientes[cnt_paciente].id_proceso,
             listaPacientes[cnt_paciente].tiempo_restante);

      // [caso 1] El tiempo que le resta es MENOR o IGUAL al Quantum
      //         caso ideal y rapido.
      if (listaPacientes[cnt_paciente].tiempo_restante <= QUANTUM) {

        // guardams el tiempo que tomo atender al paciente dentro de la variable
        //  reloj_global
        reloj_global =
            reloj_global + listaPacientes[cnt_paciente].tiempo_restante;

        listaPacientes[cnt_paciente].tiempo_restante = 0;

        // Registro del Tiempo de Retorno
        listaPacientes[cnt_paciente].tt_alta = reloj_global;

        printf("  -> Paciente %d TERMINA. Sale en el tiempo global: %d seg.\n",
               listaPacientes[cnt_paciente].id_proceso, reloj_global);

        cantidad_de_pacientes = cantidad_de_pacientes - 1;

      } else {

        reloj_global = reloj_global + QUANTUM;
        listaPacientes[cnt_paciente].tiempo_restante =
            listaPacientes[cnt_paciente].tiempo_restante - QUANTUM;

        printf("  -> Paciente %d atendido. Vuelve a la fila con %d seg "
               "restantes.\n",
               listaPacientes[cnt_paciente].id_proceso,
               listaPacientes[cnt_paciente].tiempo_restante);
      }
    }

    cnt_paciente++;

    if (cnt_paciente == PACIENTES) {
      cnt_paciente = 0;
    }
  }

  printf("\n=== SIMULACION TERMINADA ===\n");
  printf("Tiempo total de la clinica (Reloj Global): %d segundos\n",
         reloj_global);

  // --- REPORTE DE MÉTRICAS OSTEP ---
  printf("\n--- REPORTE DE TIEMPOS DE RETORNO (TURNAROUND TIME) ---\n");
  float suma_turnaround = 0;

  for (int i = 0; i < PACIENTES; i++) {
    printf(
        "Paciente %d | Llego: 0s | Rafaga: %d | Termino (Turnaround): %d seg\n",
        listaPacientes[i].id_proceso, listaPacientes[i].tiempo_rafaga,
        listaPacientes[i].tt_alta);
    suma_turnaround = suma_turnaround + listaPacientes[i].tt_alta;
  }

  printf("Tiempo de Retorno Promedio: %.2f segundos\n",
         suma_turnaround / PACIENTES);

  return 0;
}

void pedirPacientes(Proceso listaPacientes[]) {

  printf("Registrando pacientes en la clinica:\n");
  for (int i = 0; i < PACIENTES; i++) {
    int numero_aleatorio = (rand() % (10 - 1 + 1)) + 1;
    listaPacientes[i] = (Proceso){i + 1, numero_aleatorio, numero_aleatorio, 0};
    printf("Paciente %d - Cirugia requerida: %d seg\n", i + 1,
           numero_aleatorio);
  }
  printf("--------------------------------------\n");
}
