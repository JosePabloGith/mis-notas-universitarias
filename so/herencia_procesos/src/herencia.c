#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>

int main() {
  int x = 10;

  FILE *archivo;

  archivo = fopen("salida.txt", "a");

  if (archivo == NULL) {
    printf("Error al abrir archivo\n");
    return 1;
  }

  pid_t pid = fork(); // aqui se inicia el crea el proceso hijo

  if (pid == 0) {
    /*esta es el área del hijo*/
    x = 100;

    printf("\n_____Hijo_____\n");
    printf("PID  : %d\n", getpid());
    printf("PPID : %d\n", getppid());
    printf("x    : %d\n", x);

    fprintf(archivo, "Hijo: PID=%d PPID=%d x=%d\n", getpid(), getppid(), x);
  } else {

    // este es el el area del PADRE

    x = 50;
    printf("\n_____Padre_____\n");
    printf("PID  : %d\n", getpid());
    printf("PPID : %d\n", getppid());
    printf("x    : %d\n", x);

    fprintf(archivo, "PADRE: PID=%d PPID=%d x=%d\n", getpid(), getppid(), x);
  }

  fclose(archivo); // area de cerrado del archivo abierto

  return 0;
}
