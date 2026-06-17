/**
 * PROCESO_B
 */
// El PROCESO_B debe de leer del FIFO_A
// EL PROCESO_B debe de escribir en FIFO_B

#include <fcntl.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include <stdbool.h>
#include <stdio.h>

// """""""" Direcciones """"""""""""""""
#define DIR_FIFO_A "/tmp/fifo_A"
#define DIR_FIFO_B "/tmp/fifo_B"
//""""""""""""""""""""""""""""""""""""""

// $$$$$$$ declaraciones $$$$$$$$$$
int esPerfecto(int);
// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

int main() {

  //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  int auxZ;
  int  flagPerfect;

  // abrimos el archivo FIFO_A
  int fd_read_A = open(DIR_FIFO_A, O_RDONLY);
  // verificamos
  if (fd_read_A == -1) {
    perror("error al abrir FIFO_A\n");
    return 1;
  }
  // significa que aquí se abrió el FIFO_A
  read(fd_read_A, &auxZ, sizeof(auxZ));
  close(fd_read_A);
  //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

  // ya podemos jugar con el numero leido
  flagPerfect = esPerfecto(auxZ);

  // ahora debemos de mandar el resultado
  // el proceso_A ya creo los FIFOs
  // de hecho esta esperando que lo abramos
  // y metamos el calculo
  int fd_write_B = open(DIR_FIFO_B, O_WRONLY);
  if (fd_write_B == -1) {
    perror("error al abrir FIFO_B\n");
    return 1;
  }
  write(fd_write_B, &flagPerfect, sizeof(flagPerfect));
  close(fd_write_B);

  printf("\n\n $$$$$$$$$$$$$$$$$$$$$$$\n");
  printf("Mision cumplica por PROCESO_B, leí, jugué, y mande resultado\n");
  printf("\n\n $$$$$$$$$$$$$$$$$$$$$$$\n");

  return 0;
}

int esPerfecto(int z) {

  // contador necesario
  int cont_aux = 1;

  for (int i = 2; i < z; i++) {
    //
    // exploraremos
    // comprobando esta condición
    if (z % i == 0) {
      // el numero i divide perfectamente a z
      cont_aux = cont_aux + i;
    }
  }

  printf("esto es lo que vale z: %d\n", cont_aux);

 if (z == cont_aux) {
      return 1; // Retornamos 1 indicando que SI es perfecto
  } else {
      return 0; // Retornamos 0 indicando que NO es perfecto
  }
}
