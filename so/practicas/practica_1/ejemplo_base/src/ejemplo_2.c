#include <stdio.h>
#include <sys/wait.h>
#include <unistd.h>

int main() {
  int id = getpid();
  printf("Soy el proceso padre\n");
  pid_t idf = fork();
  if (idf == 0) {
    // dentro del hijo

    printf("Soy el proceso hijo\n");
    printf("Mi papa es el proceso con ID: %d\n",
           id); // en este punto el id es igual al de papá
    printf("estamos dentro del hijo, y este es id antes de cambia: %d\n", id);
    // podemos ver que id se sobreescribió en este punto :O
    id = getpid(); // aquí cambiamos el id clonado del padre, por ello
    printf("Mi ID es: %d\n\n", id);

    sleep(2);
    printf("EL hijo a terminado de dormir\n");

  } else {

    printf("Mi hijo es el proceso con ID: %d\n", idf);
    printf("Mi ID es: %d\n\n", id);

    sleep(60);

    wait(
        NULL); // <- importante, si esto no se ejecuta, no se limpia la tabla de
               // procesos es importante eliminarlos ya que la tabla es finita y
               // ocupa espacio y no se pueden usar los procesos reales
    printf("el padre ha terminado de dormir\n");
  }
  return 0;
}

// [*]
//      ¿idf == 0?
//                es para identificar al hijo
//
//      [quien es primero]
//                    El padre, pero si es mas lento entonces se lanzara al hijo
//                    el que se encarga de eso es el scheduler
//
//      Podemos ver que en estos casos se lanzo el else
//
//          - ¿porque?
//                    dependiendo de quien se esta ejecutando primero
//                    "cada quien tiene su propio idf"
//
//           -- ¿el padre se ejecuta siempre?
//                - no necesariamente,
//
//           -- NECECITAMOS MODIFICARLO PARA QUE LO PODAMOS VER
//
//           podemos crear un zombie
//
//           si le damos un monton de tiempo de espera dentro del padre
//           como lo dejamos y le damos poco tiempo dentro del hijo
//
//           podemos darnos cuenta de que lo matamos
//
//           es por ello que al momento de verlo dentro de la tabla de procesos
//           aparecera como estado muerto
//
//           ya que no se ha entrado al wait
