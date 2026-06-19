#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/types.h>

struct mensaje {

  long tipo;
  char texto[200];
};

int main() {

  int id_cola;
  key_t clave;
  struct mensaje msg;

  clave = ftok("/tmp", 'M');

  if (clave == -1) {

    perror("no se pudo crear la clave");
    exit(1);
  }

  id_cola = msgget(clave, IPC_CREAT | 0666);

  if (id_cola == -1) {
    perror("Error al crear la cola de mensajes");
    exit(1);
  }

  //%%%%%%%%%%%%%%%% 2 %%%%%%%%%%%%%%%%%%%%%%%%
  printf("ingresa la importancia del sms (de momento puedes usar 1 o 2): ");
  if (scanf("%ld", &msg.tipo) != 1 || msg.tipo <= 0) {
    fprintf(
        stderr,
        "Error: La importancia del sms debe ser un número entero positivo.\n");
    exit(1);
  }
  // consumimos el salto de linea pendiente después de scanf
  getchar();

  printf("ingresa el Contenido del sms a enviar: ");
  scanf(" %[^\n]",
        msg.texto); // Lee hasta el salto de línea o hasta 199 caracteres

  // comprobamos si hubo un error al mandar el mensaje
  if (msgsnd(id_cola, &msg, sizeof(struct mensaje) - sizeof(long), 0) == -1) {
    perror("Error al enviar el mensaje");
    exit(1);
  }

  printf("Mensaje guardado correctamente\n");
  printf("clave generada (hex):0x%x\n", clave);
  printf("prioridad de mensaje: %ld\n", msg.tipo);

  return 0;
}
