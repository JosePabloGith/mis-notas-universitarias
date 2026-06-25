# Sincronización entre procesos

## El problema: El aislamiento de los procesos

<mark>Cuando un programa se ejecuta, se convierte en un proceso.</mark> Por diseño y seguridad, <mark style="background-color: #FFA500; color: black;">el sistema operativo aísla estos procesos.</mark> Según nuestro documento base (ESTUDIO_Comunicación-entre-Procesos.pdf, pág. 4), <mark style="background-color: #00FF00; color: black;">los procesos poseen espacios de memoria independientes y no pueden acceder directamente a la información de otros procesos.</mark> Esto es excelente para evitar que un programa que falle (como un videojuego) corrompa la memoria de otro (como tu navegador), pero crea un problema: ¿Qué pasa si realmente necesitan trabajar en equipo y compartir datos?

## La Solucion a este problema

Para resolver esto, el Sistema Operativo provee una serie de **puentes** controlados. A este conjunto de mecanismos se le conoce como **IPC** (Comunicación entre Procesos). Su objetivo es permitir el intercambio de información y la coordinación sin romper la seguridad del sistema (ESTUDIO_Comunicación-entre-Procesos.pdf, pág. 7).

Algunos de los mecanismos de **IPC** más comunes en C para Linux/**UNIX** son:

- Pipes (Tuberías Anónimas y FIFOs): Son canales de comunicación de un solo sentido (como una manguera de agua). Uno escribe de un lado y el otro lee del otro.

- Colas de Mensajes: Permiten enviar bloques de información estructurada con prioridades.

- Memoria Compartida: El SO engaña a dos procesos para que un mismo bloque de RAM física aparezca en el espacio de memoria de ambos. ¡Es el método más rápido!

## 3. El reto principal: La Sincronización

Aquí es donde entra la parte crítica de tu consulta. Si usamos mecanismos rápidos como la Memoria Compartida, surge un caos inminente.

Imagina el siguiente escenario (ESTUDIO_Comunicación-entre-Procesos.pdf, pág. 4): ¿Cómo evitar que el proceso B intente leer los datos antes de que el proceso A termine de escribirlos? A este fenómeno de desorden se le conoce como Condición de Carrera (Race Condition), donde el resultado final depende de quién corrió más rápido, generando errores catastróficos e impredecibles (OSTEP_26_concurrencia.pdf).

Para poner orden, los Sistemas Operativos nos brindan herramientas de Sincronización, de las cuales destacan:

- Locks (Cerrojos/Mutex): Actúan como la llave del baño. Si un proceso entra a la zona crítica (donde están los datos compartidos), pone el seguro. Si otro quiere entrar, se queda esperando (bloqueado) hasta que el primero salga (OSTEP_28_locks.pdf).

- Semáforos: Inventados por el matemático Edsger Dijkstra, son contadores inteligentes que no solo bloquean, sino que pueden coordinar cuántos procesos acceden a un recurso al mismo tiempo o avisar cuando un evento ya ocurrió (OSTEP_31_semaphores.pdf).

# Memoria compartida: ¿Por qué es tan rápida y cómo funciona?

En los mecanismos de comunicación tradicionales (como los Pipes o Colas de Mensajes), cuando el Proceso A quiere enviar datos al Proceso B, ocurre esto:

El Proceso A copia los datos de su memoria al espacio del Kernel (Sistema Operativo).

El Proceso B copia los datos desde el espacio del Kernel a su propia memoria.
(Esto requiere tiempo de procesamiento y copias dobles en **RAM**).

En la Memoria Compartida, el Sistema Operativo hace un **truco de magia** con el hardware (Virtualización de Memoria). El **SO** toma un bloque de Memoria **RAM** física y lo **mapea** (proyecta) dentro del espacio de memoria virtual del Proceso A y, simultáneamente, en el del Proceso B.
Resultado: No hay copias a través del Kernel. Cuando el Proceso A escribe una variable, el Proceso B la ve instantáneamente porque físicamente están mirando el mismo chip de **RAM**.

## Cosas que debes de recordar para entender el este tema

### <mark style="background-color: #FFB790; color: black;">La Llave (Key) vs. El **ID** (shmid)</mark>

Para que dos procesos independientes encuentren el mismo bloque de memoria, necesitan un punto de encuentro.

- La Llave (key_t): Es el **acuerdo externo**. Imagina que es la dirección del edificio (ej. **Avenida Siempreviva 742**). Los procesos usan un archivo existente en el disco duro y un número para calcular una llave única.

- El **ID** (shmid): Es el **número de habitación interno** que el Sistema Operativo le asigna a esa memoria. Una vez que entregas la llave correcta, el **SO** te da este **ID** para que lo uses en el resto de operaciones.

### Las Funciones Clave y su Significado (En Lenguaje C)

`El manejo de memoria compartida sigue una estructura de 4 pasos fundamentales` <mark>(Crear/Obtener -> Anclar -> Desanclar -> Destruir).</mark>

### Generación de la llave

```c
// para generar la llave se necesita usar:
key_t ftok(const char *ruta, int id_proyecto);

//[ftok] literalmente significa llave al archivo
// su labor en convertir la ruta de un archivo
// existente y un numero id en una
// clave numerica.

// Manejo: Ambos procesos (A y B) deben de
// llamar ftok usando exactamente la misma ruta
// y el mismo múmero para obtener la misma
// llave.

```

### Reservar / obtener la memoria ("El locker")

```c

int shmget(key_t key, size_t size, int shmflg);

/*

[*] [shmget]: significa Obtener memoria compartida "Shared memory get".
  Esta funcion solicita al SO que asigne el bloque de memoria de tamaño "size"
  usando la "key" anterior.

[shmflag]: * si eres el creador, usas banderas como `IPC_CREAT` | 0666 "crear con permisos
de lectura/escritura".
    [-] Si eres el que se conecta después, solo le pasas 0666.

[**] La funcion retorna el famoso ID de la memoria(shmid)

*/

```

### Anclaje <mark>(Attach)</mark>

```c
void *shmat(int shmid, const void *shmaddr, int shmflag);

/*

  [*] [shmat] significa shared memory attach.
      Le dice al -SO "toma el locker con el ID
      y conéctalo a mi espacio de memoria"

    [retorna]: Te regresara un puntero (void * ).
    Con el cual indicarias (A partir de este momento, usas
    ese puntero exactamente igual que si hubieras hecho
    un malloc().

    Si casteas el puntero a una estructura
    (struct mi_dato *) , podrías acceder a los datos
    directamente.

*/

```

### Desanclaje y destrucción (Dead & control)

```c
int shmdt (const void *shmaddr);

int shmctl (int shmid, int cmd , struct shmid_ds *buf);

/*
  [*] shmdt(detach) : Cuando un proceso ya no va a usar la memoria, se desconecta. OJO
    Esto NO borra la memoria, solo desconecta al proceso de ella.

  [**] shmctl (control): Usando el comando IPC_RMID, le indicamos
      al SO que destruya físicamente el bloque de RAM compartida.

*/

```

### Recomendaciones Críticas (Best Practices)

Manejo de la Destrucción (IPC_RMID): A diferencia de la memoria asignada con malloc (que se libera sola cuando tu programa muere), la memoria compartida de System V es persistente en el Kernel. Si tu programa falla o lo cierras con Ctrl+C sin llamar a shmctl(..., IPC_RMID, ...), ese bloque de **RAM** se quedará ocupado **para siempre** (hasta que reinicies la **PC** o lo borres manualmente desde la terminal con el comando ipcrm).

Casteo Inteligente: Normalmente, la memoria compartida no se usa para guardar **textos sueltos**. Se definen structs en un archivo de cabecera (.h) compartido. Así, ambos procesos castean el puntero void \* devuelto por shmat a ese struct y acceden limpiamente a variables organizadas (ej. datos->temperatura = 25;).

El Elefante en la Habitación (Sincronización): Retomando la clase anterior (OSTEP_26_concurrencia.pdf), la memoria compartida no tiene mecanismos de bloqueo automáticos. Si el Proceso A está escribiendo una cadena de texto en la memoria y el Proceso B lee al mismo tiempo, B leerá basura o texto cortado. Obligatoriamente debe combinarse con Semáforos (sem_wait, sem_post) para organizar los turnos.

```c

// Este código esta pensado para ejecutarse
// en conjunto con los códigos de ejemplo
// vistos en clase.
//aumenta.c

#include<stdio.h>
#include<stdlib.h>
#include<sys/ipc.h>
#include<sys/shm.h>

#define TAM 5
#define REP 100000

int main(){
  key_t clave;
  int id_memoria;
  int *arreglo;

  clave = ftok("/tmp",'A');
  if(clave == -1){

    perror("error al crear la llave");
    exit(1);
  }

  id_memoria = shmget(clave, TAM*sizeof(int) , 0666);

  if(id_memoria == -1){
    perror("error al conectarse a la memoria compartida");
    exit(1);
  }

  arreglo = (int *) shmat (id_memoria, NULL, 0);
  if(arreglo == (int *)(-1)){
    perror("Error al adjuntar el segmento");
    exit(1);
  }

  for(int r = 0 ; r < REP; r++){

    for(int i = 0; i < TAM; i++){
      arreglo[i] = arreglo[i] + 1;
    }
  }

  shmdt(arreglo);
  return 0;

}

```

- ejecutar 2 veces
  `/incrementa & ./incrementa &`

- despues ejecutaras al lector

- que explico
