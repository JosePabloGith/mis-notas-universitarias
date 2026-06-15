// En este ejemplo se remarcara la estructura del profesor

#include <stdio.h>
#include <sys/wait.h>
#include <unistd.h>

int factorial(int n) {

  int f = 1;
  for (int i = 1; i <= n; i++) {
    f *= i;
  }
  return f;
}

int main() {

  int fd[2];
  pipe(fd); // al la tuberia le mandamos un arreglo de 2 posiciones
            // f0 lectura
            // f1 escritura

  /*
   * AREA DE EJERCIC
   *
   * */

  // podemos ver que se creo la tuberia
  // antes de los fork  para poder comunicar los procesos
  pid_t pid = fork();

  if (pid > 0) {

    // DENTRO DE LA MORIA PRINCIPAL DEL PADRE

    close(
        fd[0]); // cerramos la lectura (vamos a escribir)
                // si no se hace esto puede haber choques entre los descriptores
    int num = 5; // declaramos una variable
    write(fd[1], &num,
          sizeof(num)); // aquí mandamos escribir fd[1],
                        // mandamos la direccion del 5 y el tamaño en bites
    close(fd[1]); // cerramos la escritura, ya terminamos de usar ese recurso
    wait(NULL);   // ESPERAMOS A QUE TERMINEN LOS PROCESOS HIJOS

  } else if (pid == 0) {

    close(fd[1]); // cerramos la escritura (buscamos leer)
    int n; // creamos la variable auxiliar que vamos a obtener de la tuberia
    read(fd[0], &n, sizeof(n)); // leemos fd[0],
                                // la direccion donde vamos a guardar lo que
                                // leeamos de la tuberia
    printf("el hijo lee del pipe el dato: %d\n",
           n); // indicamos que leyo el hijo
    printf("El factorial de %d es %d\n", n,
           factorial(n)); // procesamos lo leido
    close(fd[0]);         // cerramos la lectura
  }

  return 0;
}

/*
 * ahora el dato se debe de imprimir dentro del padre
 * */
