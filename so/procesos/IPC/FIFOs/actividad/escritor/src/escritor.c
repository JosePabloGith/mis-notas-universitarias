//
// MOLDE_PROCESO A
//

#include <fcntl.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

// ############### Definimos rutas ###########################

// FIFO_A aquí se pide números al usuario
// e inyecta el numero obtenido, la idea es que
// este proceso al momento de hacer open, se asegure
// que el proceso B se comunique con el FIFO_A para poder
// inyectar el numero, [aquí el proceso A no debe de Morir]
// Mientras el proceso B lee el numero usando el FIFO_A, para
// leerlo y jugar con el.
#define FIFO_A "/tmp/fifo_A"

// Aquí el proceso B usara el fifo_B
// para inyectar el numero calculado con los datos leidos
// es decir el proceso B escribira en el FIFO_B
//
// posteriormente el proceso A leera el FIFO_B
// para imprimir en consola que es lo que calculo
// el proceso B.
//
// Para posteriormente eliminar los fifos
#define FIFO_B "/tmp/fifo_B"

// ###########################################################

//====== declaración de funciones =====
int pedirNumeros();
void esPerfecto(int, int);
//=====================================

int main() {

  // para evitar temas de permisos usare esto///
  mode_t umask_vieja = umask(0);
  int permiso = 0666;
  /////////////////////////////////////////////

  int z = pedirNumeros();

  // creamos LOS FIFOS A USAR, POR CONVENCIÓN AL INICIO
  mkfifo(FIFO_A, permiso);
  mkfifo(FIFO_B, permiso);

  umask(umask_vieja);

  // ############################################################

  // aquí se quedara hasta que se establezca la comunicación
  // con el segundo proceso que abra el FIFO_A para leerlo
  int fd_write_A = open(FIFO_A, O_WRONLY);
  // estado del fifo
  if (fd_write_A == -1) {
    perror("Error al abrir el FIFO_A");
    return 1;
  }
  // aquí implica que se logro abrir el archivo
  // como tenemos O_WRONLY podemos escribir en el :D
  write(fd_write_A, &z, sizeof(z));
  // cerramos el canal
  close(fd_write_A);

  // ############################################################

  // aquí suponemos que el proceso B ha escrito dentro de FIFO_B
  // este proceso A leera ese resultado para decirnos que es lo que
  // calculo el proceso_B

  // esto se quedara en espera y continuara si y solo si
  // el proceso A esta del otro lado ha inyectado su valor calculado
  int fd_read_B = open(FIFO_B, O_RDONLY);
  // estado del archivo
  if (fd_read_B == -1) {
    perror("\nError al abrir el FIFO_B :O");
    return 1;
  }
  // si llegamos aqui, entonces se pudo abrir el archivo
  // si es 0 no es perfecto, si es 1 es perfecto
  int numeroLeido;

  read(fd_read_B, &numeroLeido, sizeof(numeroLeido));
  close(fd_read_B);

  // en este punto tenemos leído
  // el numeroLeido xD.

  esPerfecto(numeroLeido, z);

  //%%%%%%%%% LIMPIAMOS %%%%%%%%%%%%
  unlink(FIFO_A);
  unlink(FIFO_B);
  printf("\n\n\n\n\nLimpiamos con exito los fifos :D\n");
  //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

  return 0;
}

/*
 * @brief Pedimos números al usuario
 * @return regresa la suma de los 2 números ingresados por el usuario
 * */
int pedirNumeros() {
  printf("\ndame un numero: ");
  int a;
  scanf("%d", &a);
  printf("\nDame otro numero:");
  int b;
  scanf("%d", &b);

  return a + b;
}

void esPerfecto(int numeroLeido, int z) {

  if (numeroLeido == 0) {
    printf("\nEl numero sumado entre los 2 numeros leidos :%d NO es PERFECTO :O",
           z);
  } else {
    printf(
        "\nEl numero sumado entre los 2 numeros leidos :%d ES PERFECTO :P",
        z);
  }
}
