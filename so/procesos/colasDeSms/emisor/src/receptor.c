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
  key_t clave = ftok("/tmp", 'M');
  struct mensaje msg;

  id_cola = msgget(clave, 0666);

  if (id_cola == -1) {
    perror("error: no existe la cola de mensajes");
    exit(1);
  }

  printf("solo fuciono con mensajes de Tipo 1...\n\n");

  while (1) {
    if (msgrcv(id_cola, &msg, sizeof(struct mensaje) - sizeof(long), 1, 0) ==
        -1) {
      perror("error al recibir los mensajes");
      exit(1);
    }

    printf("Detectado un mensaje de prioridad 1\n");
    printf("mensaje Leido: %s\n", msg.texto);
    printf("-----------------------\n");
  }

  return 0;
}
