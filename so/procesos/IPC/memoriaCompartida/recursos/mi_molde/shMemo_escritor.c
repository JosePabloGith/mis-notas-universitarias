/*
 * La idea de tras de este molde es entender
 * adecuadamente el uso de los structs
 *
 * ya que en memoria compartida se utilizan para poder
 * entender de mejor manera que es lo que se esta haciendo
 *
 *
 * */

// en nuestro caso usaremos structs
// Con esto ganamos lo siguiente:
//   [*] Cuando el kernel construya la puerta con shmat
//       en lugar de de decirle que trate la tierra
//       vacia como una simple fila de números
//       (int *), le diremos que ponga nuestro molde
//       de CajaCompartida.

#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>

typedef struct {

  int id_del_escritor; // ¿quien escribio esto?
  int datos[5];        // - el arreglo original de 5 numeritos

} CajaCompartida;

int main() {

  key_t llave;
  CajaCompartida *cajaDeNumeros;

  llave = ftok("/tmp", 'K');

  if (llave == -1) {
    perror("no es posible generar la llave");
    exit(-1);
  }

  // pedimos la memoria
  // mandamos llave
  //  establecemos que queremos espacio para una sola variable de tipo
  //  CajaCompartida se no esta el segmento de memoria lo crea IPC_CREAT Permiso
  //  a a todos para escribir y leer
  int id_ticket_memory =
      shmget(llave, sizeof(CajaCompartida), IPC_CREAT | 0666);
  if (id_ticket_memory == -1) {
    perror("no es posible crear la memoria compartida");
    exit(-1);
  }
  // conectamos lo solicitado:
  cajaDeNumeros = (CajaCompartida *)shmat(id_ticket_memory, NULL, 0);

  // la comprobacion puede ser unversal, shmat retorna -1 convertido en
  // puntero generico (void *) en caso de error
  if (cajaDeNumeros == (void *)-1) {
    perror("No se pudo adjuntar el segmento de memoria RAM");
    exit(1);
  }

  // en este punto ya deberiamos de tener comunicacion con el segmento de la
  // RAM, especificamente cajaDeNumeros

  //[!] Este es elpunto critico, es donde muy posiblemente se debe de colocar un
  //    semaforo. Esto se debe principalemente a que si no controlamos esta
  //    seccion critica

  // TODO: pedir_llave_del_semaforo(); -> esto bloquea a los demás

  int cantidadNumeros =
      sizeof(cajaDeNumeros->datos) / sizeof(cajaDeNumeros->datos[0]);
  printf("Ingresa los %d numeros usuario comun: \n", cantidadNumeros);

  for (int i = 0; i < cantidadNumeros; i++) {
    // aquí cajaDeNumeros ya tiene accedo a la RAM
    scanf("%d", &cajaDeNumeros->datos[i]); // ZONA CRITICA
  }

  // TODO:devolver_llave_del_semaforo(); -> para dejar pasar al otro proceso

  // en este punto ya deberia de haber 5 numeros dentro de
  // la memoria compartida. se deberian de ver con ipcs -m

  // cerramos la puerta, de momento esta parte del codigo
  // cumplio su proposito
  shmdt(cajaDeNumeros);

  return 0;
}

/*
 Glosario:

    [*]¿que es CajaCompartida = *mi_caja?
        El * significa "soy un apuntador", básicamente
        le decimos al compilador: "oye bro, la variable
         mi_caja NO va a guardar una caja de datos real"
         - Lo que va a guardar es una direccion de memoria
         que dice en dónde está esa caja allá afuera. "RAM"

      [!] Bajo este escenario, al hacer:

          CajaCompartida *mi_caja; //creamos la variable
                                  //que guardara la direccion
                                  //que nos dara shmat
        [!] esa variable *mi_caja solo guarda una direccion de
            memoria. Pero en C, un apuntador no sabe qué tan
            grande es la memoria que hay detras. Simplemente
            te dice: "El parque empieza en tal punto de la RAM"

       [!] La función shmat() "el albañil":
            te conecta al parque y te regresa la direccion
            de esa primera baldosa. El tamaño real del parque
            se define en shmget()

      [!] considerando esto, es por eso mismo la practica
          que vayas a realizar debe de apartar exactmane
          la cantidad de numeros que se vayan a usar.

          Si observas dentro del ejemplo del profesor
          se declararon exactmane 5 espacios para guardar
          5 datos de numeros int.
          Pero en nuestro caso estamos definiendo
          int datos[5], es por ello que para este caso
          solo pediremos espacio para una variable de tipo
          CajaCompartida.


          // recordando que debemos de castear
          // al tipo de dato que se usara
          mi_caja = (CajaCompartida *) shmat(id_memoria,NULL,0);






 * */
