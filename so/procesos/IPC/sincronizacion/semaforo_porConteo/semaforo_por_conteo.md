# semáforo por conteo

Mientras que un semáforo binario sirve para exclusión mutua (asegurar que solo un proceso toque un recurso a la vez), un semáforo contador sirve para gestionar un conjunto de recursos idénticos.

Su valor interno (el contador) ya no se restringe a 0 o 1, sino que se inicializa con un número entero **N** que representa la cantidad total de recursos disponibles en el sistema. Esto permite que múltiples hilos de ejecución (hasta **N**) entren a la sección crítica de forma simultánea.

## Alegoria : "El Estacionamiento Privado"

Imagina un estacionamiento que tiene exactamente 5 lugares disponibles.

- El semáforo contador es el letrero electrónico en la entrada que marca "5 espacios libres", junto con la pluma automática.

- sem_wait() (Entrar): Cuando llega un coche (un hilo), mira el letrero. Como dice 5, la pluma se levanta, el coche entra, y el letrero baja a 4. Si llegan 4 coches más, el letrero baja a 0.

- La Fila de Espera (Kernel): Si llega un sexto coche y el letrero dice 0, la pluma no se abre. El conductor tiene que apagar el motor y esperar en la fila de la calle (el hilo se bloquea y se va a dormir).

- sem_post() (Salir): Cuando uno de los 5 coches que estaba adentro termina sus compras y sale del estacionamiento, le avisa a la caseta. El letrero sube de 0 a 1, la pluma se abre, y el Kernel despierta al coche que estaba esperando en la calle para que pueda entrar.

# estructura y sintaxis en POSIX

La belleza de esto es que en C (usando el estándar POSIX), las funciones son exactamente las mismas que en el semáforo binario. La única diferencia radica en la inicialización.

```c
#include <semaphore.h>
#include <pthread.h>

sem_t estacionamiento; // Declaración global

int main() {
    // Parámetro 1: Dirección del semáforo
    // Parámetro 2: 0 (compartido entre hilos del mismo proceso)
    // Parámetro 3: VALOR INICIAL. Aquí está la magia, lo iniciamos en 5.
    sem_init(&estacionamiento, 0, 5);

    // ... creación de tus hilos ...

    sem_destroy(&estacionamiento);
    return 0;
}

void* coche_hilo(void* arg) {
    // El hilo solicita un espacio. Si el valor es > 0, lo decrementa y pasa.
    // Si es 0, se bloquea.
    sem_wait(&estacionamiento);

    /* --- SECCIÓN LIMITADA --- */
    // Aquí el hilo usa uno de los 5 recursos disponibles.
    /* ------------------------ */

    // El hilo libera el recurso. Incrementa el contador y despierta a otro si hay fila.
    sem_post(&estacionamiento);

    return NULL;
}

```

# Recomendaciones de Oro y Detalles Criticos

- La regla de inicialización (Regla de Kivolowitz): ¿No sabes en qué valor inicializar tu semáforo? Piensa en "la cantidad de recursos que estás dispuesto a regalar inmediatamente después de la inicialización". Si tienes 5 conexiones a base de datos, inicializa en 5. Si estás esperando a que un hilo hijo termine una tarea para leer su resultado, inicializa en 0 (porque al inicio no tienes nada que "regalar", debes esperar a que el hijo haga un sem_post).

- Cuidado con la ilusión de seguridad (El error más común): ¡Un semáforo contador NO protege la memoria compartida contra condiciones de carrera! Si 5 hilos entran al estacionamiento y los 5 intentan estacionarse en el mismo cajón exacto (modificar la misma variable al mismo tiempo), habrá un choque.

- La solución híbrida: Por lo anterior, en problemas reales complejos, casi siempre verás que los programadores usan ambos: un semáforo contador (para limitar cuántos entran) y un semáforo binario o mutex adentro (para proteger qué modifican cuando ya están adentro).

- Throttling (Aceleración controlada): Los semáforos por conteo son la herramienta perfecta para evitar que un servidor colapse. Si creas 1000 hilos pero solo tienes memoria para que 10 trabajen a la vez, pones un semáforo contador en 10. A esto se le llama admisión de control o throttling.
