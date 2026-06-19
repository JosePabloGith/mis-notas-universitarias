
// intención por el día de hoy:
//
// vamos a realizar una modificación al codigo
// con la idea de colocar dos procesos receptores
//  [*] UN RECEPTOR QUE IDENTIFIQUE MENSAJES DE TIPO 1
//
//  [*] UN RECEPTOR QUE IDENTIFIQUE MENSAJES DE TIPO 2

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/types.h>

struct mensaje {
  long tipo;
  char texto[200];
};

int main() {
  int id_cola;
  key_t clave = ftok("/tmp", 'M');
  struct mensaje msg;

  id_cola = msgget(clave, 0666);

  if (id_cola == -1) {
    perror("error: no existe la cola de mensajes");
    exit(1);
  }

  printf("funciono con cualquier tipo de mensaje...\n\n");

  // si queremos que el receptor acepte cualquier mensaje, independientemente de
  // su tipo, podemos usar 0 en lugar de 1
  // SE QUEDA como con while para que este a la escucha de los eventos
  // vamos a colocar 1 para que acepte cualquier mensaje de tipo 1, y el otro
  // receptor va a colocar 2 para que acepte cualquier mensaje de tipo 2
  while (1) {

    if (msgrcv(id_cola, &msg, sizeof(struct mensaje) - sizeof(long), 1, 0) ==
        -1) {
      perror("error al recibir los mensajes de tipo 1 :O");
      exit(1);
    }

    printf("Detectado un mensaje de prioridad 1\n");
    printf("mensaje Leido: %s\n", msg.texto);
    printf("Mi proposito es hacer esto : determinar y mostrar la cantidad de "
           "vocales y espacios en blanco contenidos en el mensaje.\n");

    // declaramos un auxiliar para guardar el mensaje
    //  vamos a contar la cantidad de vocales que tiene la cadena

    // char vocales[];

    int longitud = strlen(msg.texto);
    int contador_vocales = 0;
    int contador_consonantes = 0;
    for (int i = 0; i < longitud; i++) {
      if (msg.texto[i] == 'a' || msg.texto[i] == 'e' || msg.texto[i] == 'i' ||
          msg.texto[i] == 'o' || msg.texto[i] == 'u') {
        // printf("la cantidad de vocales es: %d\n", 1);
        contador_vocales++;
      } else {
        // printf("la cantidad de consonantes es: %d\n", 0);
        contador_consonantes++;
      }

      // printf("-----------------------\n");
    }

    printf("la cantidad de vocales es: %d\n", contador_vocales);
    printf("la cantidad de consonantes es: %d\n", contador_consonantes);
  }

  return 0;
}
