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
  printf("Importancia del sms: \n");
  scanf(" %[^\n]", msg.texto);

  if (msgsnd(id_cola, &msg, sizeof(struct mensaje) - sizeof(long), 0) == -1) {
    perror("error al enviar el sms");
    exit(1);
  }

  printf("Mensaje guardado correctamente\n");
  printf("clave generada (hex):0x%x\n", clave);
  printf("prioridad de mensaje: %ld\n", msg.tipo);

  return 0;
}
