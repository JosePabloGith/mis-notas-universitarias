/*
 *  Objetivo:
 *      Observar de primera mano, la forma en la cual el scheduler es capas de
 * juzgar un proceso
 *
 *  Metodología:
 *      En la primera fase usando el comando sleep, pondremos el proceso
 * "encargado de procesar nuestras instrucciones aquí escritas"
 *
 *      [*] La idea aquí es ver como se juzga el proceso como nice, es decir
 *          el scheduler es engañado pensando que el proceso debe de ser uno que
 * interactúa con el usuario, y esta esperando una especie de entrada de datos.
 *          ¡implica que su valor de simpatia sera elevado es decir bajo!
 *          tendrá muy seguramente un estado de sleep [s]
 *
 *          -> esto implica que se le dará una alta prioridad, ya que es un buen
 * ciudadano no podemos asegurar cuanto, pero debe de ser un numero bajo
 *      [**]
 *
 *            Estas son las escalas :
 *
 *                      * escala de simpatia (NI | NICE)
 *                          [-20] : maxima prioridad , el proceso usa toda la
 * cpu [19] : minima prioridad, el proceso es muy amable [0] : valor por defecto
 *
 *                      * escala interna del nucleo (PRI)
 *
 *                          formula PRI = 20 + NI
 *                            minimo (0) : maxima prioridad para un proceso
 * normal ocurre cuando NI = -20 intermedio(20):  Prioridad normal o por defecto
 *                                             ocurre cuando NI es 0 y el
 * proceso no esta siendo ni premiado ni castigado maximo(39): Minima prioridad
 * para un proceso normal. Ocurre cuando el NI es 19 o cuando el sistema castiga
 * severamente a un proceso por acaparar la CPU.
 *
 *                           NOTA: htop aqui es mas amigable, da escalas de 0 al
 * 39
 *
 *                      * escala de tiempo real (RT)
 *                        Esta es una clase de planificación mas especial
 * reservada para tareas criticas del sistema como manejo de audio, video p
 * hardware elemental
 *
 *                            dentro del kernel va de [0 a 99]
 *
 *
 *                           [1] minima prioridad
 *                           [99] maxima prioridad
 *
 *             RECUERDA ESTA formula
 *
 *             Prioridad del kernel = 120 + Valor Nice
 *
 *
 * */

#include <stdio.h>
#include <unistd.h>

int main() {

  unsigned long long contador = 0;

  // [*] Esta area nos sirve para saber el PID del proceso que vamos a ejecutar
  //     y el PPID es decir, el PID del proceso padre que se encarga de llamar
  //     a este proceso
  printf("PID = %d\n", getpid());
  printf("PPID = %d\n", getppid());

  // [1.-] Esta es la fase de engaño al scheduler
  //       pensara que es un buen ciudadano, "numero NI entre 0 a 19"
  //       Con un estado de [S]
  printf("\nFase 1: el proceso dormira 120 segundos.\n");
  sleep(60);

  // Fase 2: Comportamiento acaparador (CPU Bound).
  //      Aquí el proceso exprime el procesador en estado Running [R].
  //      El Scheduler detectará el abuso y aumentará el valor numérico de su
  //      prioridad dinámica (PRI) para castigarlo.
  //      NOTA: El valor NI (Nice) se mantendrá intacto, ya que el SO
  //      no lo modifica automáticamente; solo el usuario puede hacerlo.
  printf("\nFase 2: el proceso utilizara CPU durante 60 segundos.\n");
  for (int segundos = 0; segundos < 120; segundos++) {
    for (unsigned long long i = 0; i < 500000000ULL; i++) {
      contador++;
    }
    printf("Segundo %d: contador = %llu\n", segundos + 1, contador);
  }

  printf("\nProceso terminado.\n");
  return 0;
}

// GLOSARIO:
//        Nos podemos topar con estos terminos
//
//        VIRT (virtual memory)
//        * Es la ilusion de memoria total que el SO le da a un proceso
//          "recuerda que cada proceso es engañado ", el SO le miente a cada
//          proceso haciendoles creer que tiene un espacio de memoria gigantesco
//          y exclusivo haciéndole creer que tiene un espacio de memoria
//          gigantesco y exclusivo solo para él.
//
//       RES (resident memory)
//       * ¿que es?
//                Es la memoria RAM física real que el proceso está usando y
//                ocupando en este exacto momento
//        * EL DETALLE TÉCNICO
//                mientras que VIRT es una promesa o una ilusion, RES es la
//                realidad. Es la cantidad de megabits de nuestros chips de RAM
//                que están secuestrados por el proceso.
//
//       SHR (shared memory)
//              * ¿que es?
//                        es la porción de memoria RES física que tu proceso
//                        esta compartiendo amigablemente con otros programas
//
//              * DETALLE TÉCNICO:
//                        podemos imaginar que tenemos 50 programas abieros
//                        dentro de nuestro SO y todos usan la fucion printf()
//                        de C. En lugar de cargar todo el codigo de printf() 50
//                        veces en la RAM, el SO lo carga una vez y hace que los
//                        50 procesos lo compartan. Esa memoria compartida se
//                        refleja en la columna SHR.
//
//
//      NEMOTECNIA:
//        Imagina un restaurante, con una mesa con comida y varios comensales
//      * VIRT es todo el menu del restaurante- Es todo lo que el cliente tiene
//      permitido pedir
//
//      * RES son los platillos de comida que el cliente tiene servidos
//      físicamente en su mesa
//         en ese momento.
//
//      * SHR es el salero y el servilletero puesto en el centro de la mesa de
//      los comensales
