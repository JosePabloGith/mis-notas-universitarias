#include <math.h>    //
#include <stdbool.h> //
#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <unistd.h>
#define TAM 5 // cambiamos de 3 a 5
// BUG:Desconosco si se debe de cambiar o mantener ese numero 3

void determinar_si_es_primo(int);
void es_par_o_impar(int);
void mostrar_cantidad_divisores(int);
void mostrar_prioridad(int);

int main() {

  key_t clave;
  int id_sem, id_memoria, *buffer;
  struct sembuf operacion;

  clave = ftok("/tmp", 'P');

  if (clave == -1) {
    perror("Error al crear la llave");
    exit(1);
  }

  id_memoria = shmget(clave, TAM * sizeof(int), 0666);
  if (id_memoria == -1) {
    perror("Error al conectarse a la memoria compartida");
    exit(1);
  }

  buffer = (int *)shmat(id_memoria, NULL, 0);
  if (buffer == (int *)(-1)) {
    perror("Error al Adjuntar el buffer");
    exit(1);
  }

  // conectarse a los tres semaforos creados en el productor
  id_sem = semget(clave, 3, 0666);
  if (id_sem == -1) {
    perror("Error al conectarse a los semaforo");
    exit(1);
  }

  printf("CONSUMIDOR\n\nEsperando paquetes disponibles...\n\n");
  for (int i = 0; i < TAM; i++) {
    // WAIT (full) - Esperar paquete disponible
    operacion.sem_num = 1;
    operacion.sem_op = -1;
    operacion.sem_flg = 0;
    semop(id_sem, &operacion, 1);

    // WAIT (mutex) - Proteger buffer
    operacion.sem_num = 2;
    operacion.sem_op = -1;
    operacion.sem_flg = 0;
    semop(id_sem, &operacion, 1);

    // Region critica INICIO
    int paquete = buffer[i];
    buffer[i] = 0; // Marcar casilla como vacía (solo para visualización)
    printf("Paquete %d extraído de buffer[%d]\n", paquete, i);
    // Mostrar el estado actual del buffer
    printf("Estado del buffer: ");
    for (int j = 0; j < TAM; j++) {
      printf("%d ", buffer[j]);
    }
    printf("\n");

    // SIGNAL (mutex) Liberar buffer
    operacion.sem_num = 2;
    operacion.sem_op = 1;
    operacion.sem_flg = 0;
    semop(id_sem, &operacion, 1);

    // SIGNAL (empty) - Avisar espacio libre
    operacion.sem_num = 0;
    operacion.sem_op = 1;
    operacion.sem_flg = 0;
    semop(id_sem, &operacion, 1);

    // ya que capturamos la informacion de la zona critica, podemos jugar con la
    // misma

    printf("Procesando paquete...\n");
    es_par_o_impar(paquete);
    determinar_si_es_primo(paquete);
    mostrar_cantidad_divisores(paquete);
    mostrar_prioridad(paquete);
    sleep(3);
  }

  shmdt(buffer);
  printf("\nTodos los paquetes fueron consumidos\nConsumidor terminó\n");

  return 0;
}

// 1. Imprime si el número es par o impar
// usamos el operador %
// si dividimos  entre 2 y el residuo es 0
// entonce el numero es par
void es_par_o_impar(int numero) {
  if (numero % 2 == 0) {
    printf("El numero es par\n");
  } else {
    printf("El numero es impar\n");
  }
}

// 2. Teoria de numeros
//    teorema de la raiz cuadrada para la primalidad
//    * si un numero entero positivo n es compuesto "no primo", entonces
//      n debe tener un factor primo menor o igual que su raiz cuadrada.
void determinar_si_es_primo(int numero) {
  bool primo = true;

  if (numero <= 1) {
    // los numeros primos deben de ser mayores estrictos que 1
    // sino se cumple entonces el numero no es primo
    primo = false;
  } else if (numero == 2) {
    // caso especial, 2 es el unico numero par que es primo.
    primo = true;
  } else if (numero % 2 == 0) {
    // si el numero es par, no puede ser primo
    primo = false;
  } else {

    int limite = (int)sqrt(numero);
    // inicimos posterior del caso especial 2, [3]
    for (int i = 3; i <= limite; i += 2) {
      if (numero % i == 0) {
        primo = false;
        break;
      }
    }
  }

  if (primo) {
    printf("El numero es primo\n");
  } else {
    printf("El numero NO es primo\n");
  }
}

// Calculamos y mosttramos  la cantidad de divisores
void mostrar_cantidad_divisores(int numero) {
  if (numero <= 0) {
    printf("Cantidad de divisores: 0\n");
    return;
  }

  int contador = 0;
  int limite = (int)sqrt(numero);

  for (int i = 1; i <= limite; i++) {
    if (numero % i == 0) {
      contador++; // 'i' es divisor
      if (i != numero / i) {
        contador++; // El divisor compañero también cuenta
      }
    }
  }
  printf("Cantidad de divisores: %d\n", contador);
}

// 4. Imprime la prioridad del paquete de acuerdo a la tabla
void mostrar_prioridad(int numero) {
  if (numero >= 1 && numero <= 50) {
    printf("Prioridad: Alta\n");
  } else if (numero >= 51 && numero <= 100) {
    printf("Prioridad: Media\n");
  } else if (numero >= 101 && numero <= 200) {
    printf("Prioridad: Baja\n");
  } else {
    printf("Prioridad: Fuera de rango\n");
  }
}
