/*
 * NOTE: Para poder simular adecuadamente un algoritmo
 *       round robin, es necesario contemplar los siguientes asoectos:
 *
 *NOTE: * se nececita una cola FIFO, para enlistar los procesos en orden

        * un reloj que avanza solo cuando hay trabajo, o salta
          al siguiente evento de llegada si la cola esta vacia.

        * manejar 3 momentos: llegada de procesos, ejecucion de quantum
          y fin de proceso.
 * */

#include <stdio.h>
#include <stdlib.h>

#define MAX_PROCESOS 100

/*
 * Definimos la estructura del proceso que se va a simular
 * id_procesp: identificador del proceso
 * int llegada: indica si el proceso esta llegando a ejecutarse
 * int rafaga: indica cuanto tiempo de CPU nececita el proceso
 * int restante: indica que tanto le hace falta al procesos para terminar
 * int inicio: indica el tiempo en el que el proceso inicia su ejecucion
 *             en caso de que no le ponfremos -1
 * int fin: indica el tiempo en el que un proceso termino su ejecucion
 * */

typedef struct {
  int id;
  int llegada;
  int rafaga;
  int restante;
  int inicio;
  int fin;
} Proceso;

Como se modela un fifo

    struct nodo {
  int dato;
  struct nodo *sig;

}

        ** /
