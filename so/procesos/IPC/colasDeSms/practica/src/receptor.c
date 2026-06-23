/*
 *    Sistemas Operativos
 *
 *    * MARIA RENEE BARRIENTOS ARROYO
 *
 *    * PEREZ DE ANDA JOSE PABLO
 *
 * */

//
// vamos a realizar una modificación al codigo
// con la idea de colocar dos procesos receptores
//  [*] UN RECEPTOR QUE IDENTIFIQUE MENSAJES DE TIPO 1
//
//  [*] UN RECEPTOR QUE IDENTIFIQUE MENSAJES DE TIPO 2
//
//  Profesor, perdone la demora. Quice hacer una segunda version
//  a la de mi compañera. Porfavor no le reste puntos a ella.
//
//  En mi defensa escuche que queria que le pasaramos el codigo
//  corregido y funcional.
//  :D

// si no era así :O , perdoneme T-T

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/types.h>

// estructura del mensaje
struct mensaje {
  long tipo;
  char texto[200];
};

int main() {

  int id_cola;
  // conectamos a la cola de menasajes
  key_t clave = ftok("/tmp", 'M');

  // la variable que nos servira para capturar el sms
  struct mensaje msg;

  // llenamos el numero de boleto del casillero, esto lo menciono en clase
  // sobre un compañero que va al locker con el ticket
  id_cola = msgget(clave, 0666);

  // activamos el filtrador, esto lo mostro en su diapositiva de demonios
  while (1) {

    if (id_cola == -1) {
      perror("error: no existe la cola de mensajes");
      exit(1);
    }

    printf("funciono con cualquier tipo de mensaje...\n\n");

    // si queremos que el receptor acepte cualquier mensaje, independientemente
    // de su tipo, podemos usar 0 en lugar de 1 SE QUEDA como con while para que
    // este a la escucha de los eventos vamos a colocar 1 para que acepte
    // cualquier mensaje de tipo 1, y el otro

    // aqui tratamos con el sms
    // se bloquea el sistema
    // se extrae de la cola , lo copia dentro de msg , y lo elimina del buffer
    // del sistema
    if (msgrcv(id_cola, &msg, sizeof(struct mensaje) - sizeof(long), 0, 0) ==
        -1) {
      perror("error al recibir el mensaje :O");
      exit(1);
    }

    // descartamos el tipo de sms
    if (msg.tipo == 1) {
      // mensajes de tipo 1

      printf("Detectado mensaje de prioridad 1\n");
      printf("mensaje Leido: %s\n", msg.texto);
      // printf("Mi proposito es hacer esto : determinar y mostrar la cantidad
      // de " "vocales y espacios en blanco contenidos en el mensaje.\n");

      // calculamos el tamaño del mensaje usando strlen, debemos de considerar
      //  que este strlen no cuenta /0, por lo que no se incluye en el conteo de
      //  caracteres
      int longitud = strlen(msg.texto);
      int contador_vocales = 0;
      int contador_consonantes = 0;
      int contador_espacios = 0;

      char copiaAuxiliar[longitud + 1];
      strcpy(copiaAuxiliar, msg.texto);

      // pasamos a minusculas
      for (int i = 0; i < longitud; i++) {
        // esto nos asegura contar las vocales mayusculas
        // simplemete pasandolas a minusculas
        // sin afectar el contenido de msg.texto original
        copiaAuxiliar[i] = tolower(copiaAuxiliar[i]);
      }

      for (int i = 0; i < longitud; i++) {

        // con el ciclo recorremos cada espacio de la cadena
        // para cpmprobar si es vocal o consonante

        if (copiaAuxiliar[i] == 'a' || copiaAuxiliar[i] == 'e' ||
            copiaAuxiliar[i] == 'i' || copiaAuxiliar[i] == 'o' ||
            copiaAuxiliar[i] == 'u') {
          // si entramos aqui entonces se detectaron las vocales
          contador_vocales++;
        } else {
          // puede ser consonante, basura o espacio
          if (copiaAuxiliar[i] != ' ' && isalpha(copiaAuxiliar[i])) {
            contador_consonantes++;
          } else {
            // puede ser
            contador_espacios++;
          }
        }
      }

      printf("la cantidad de vocales es: %d\n", contador_vocales);
      printf("la cantidad de consonantes es: %d\n", contador_consonantes);
      printf("La cantidad de espacios es: %d\n", contador_espacios);

    } else if (msg.tipo == 2) {

      // en este tipo de mensajes se pide que se debe de determinar la cantidad
      // de consonantes y cantidad de letras mayusculas
      printf("mensaje de tipo 2 detectado\n");
      printf("%s\n", msg.texto);
      printf("procesando cantidad de consonantes y cantidad de letras "
             "mayuscilas\n");

      int tamanioMensaje = strlen(msg.texto);
      int contadorMayusculas = 0;
      int contadorConsonantes = 0;
      char auxCadena[tamanioMensaje + 1];
      // copiamos la cadena
      strcpy(auxCadena, msg.texto);

      char vocales[] = "aAeEiIoOuU";

      for (int i = 0; i < tamanioMensaje; i++) {
        // preguntaremos si cada uno de los caracteres dentro de
        // msg.texto es una mayuscula
        // podemos usar strchr para pasarle el arreglo vocales
        //  si regresa NULL entonces no encontro ninguna coincidendia
        //  con vocales[] lo que implica que es una consonante
        //  o cualquier otra cosa diferente a una vocal

        // le meto tambien la comprobacion de isupper
        // para descartar ruido
        if (isupper(auxCadena[i]) && isalpha(auxCadena[i])) {
          // se detecto una mayuscula
          contadorMayusculas++;
          // tenemos que determinar si esta mayuscula es
          // consonantes
          if (strchr(vocales, auxCadena[i]) == NULL) {
            // es consonante
            contadorConsonantes++;
          }
        } else {
          // es una minuscula o otra cosa
          // debemos de contabilizar las posibles inusculas
          // que son consonantes
          // aqui debemos cuidar algo y es que no estamos considerando
          // que el usuario pueda ingresar cualquier otra cosa diferente de
          // las letras, y si por ejemplo pone ':' como no esta en la
          // lista de vocales[] entrara dentro del if si no lo conectamos
          // investigando llenamos a una posible solucion usando
          // isalpha el cual puede detectar si lo que esta comparando es
          // o no una letra
          if (strchr(vocales, auxCadena[i]) == NULL && isalpha(auxCadena[i])) {
            contadorConsonantes++;
          }
        }
      }
      printf("cantidad de consonantes sms2 :%d \n", contadorConsonantes);
      printf("cantidad de mayusculas sms2: %d\n", contadorMayusculas);
    }
  }
  return 0;
}
