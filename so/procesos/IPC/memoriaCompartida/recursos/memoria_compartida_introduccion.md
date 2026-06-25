# Indice

1. [Introduccion](#memoria-compartida)
2. [Sentido](#¿porque-se-usa?)
3. [contras](#problemas)
4. Solución

# Memoria Compartida

En un sistema operativo (como tu Debian o Ubuntu), recuerda que cada proceso vive aislado en su propio "espacio de direcciones virtuales". Esto es una regla de oro de la protección de memoria del SO para evitar que un proceso modifique la memoria de otro por error.

Sin embargo, a veces necesitamos que dos procesos colaboren y compartan datos. La Memoria Compartida es un mecanismo de IPC (Inter-Process Communication) que le dice al Kernel: "Por favor, toma una sección de la memoria RAM física y haz que sea visible (mapeada) en el espacio de memoria de mis dos procesos simultáneamente".

# ¿porque se usa?

La gran ventaja (El porqué se usa):
Es el mecanismo de comunicación más rápido que existe en UNIX/Linux.
En otros mecanismos que probablemente ya viste (como los Pipes anónimos o FIFOs), cuando un proceso envía un dato, este tiene que viajar de la memoria del proceso hacia la memoria del Kernel (el SO), y luego el SO lo copia hacia la memoria del otro proceso receptor. ¡Eso implica tiempo y copiado de datos!.

Con la memoria compartida, se elimina el intermediario para el envío de datos. Una vez que el Kernel establece la región compartida, los procesos leen y escriben directamente en esa memoria a la velocidad del hardware (la RAM), sin tener que hacer copias extra a través del Kernel.

# Problemas

Condición de carrera, esto se da cuando dos o más
procesos intentan acceder y modificar variables
compartidas "a la vez", de modo que se dan casos de
solapamiento de informacion / perdida de integridad

# Solución

### Exclusión Mutua y Sincronización

Memoria-Compartidaa evitar que esto sea un caos, los sistemas operativos nos brindan herramientas de sincronización. Necesitamos garantizar algo llamado Exclusión Mutua (Mutual Exclusion): asegurar que si un proceso está escribiendo o leyendo en la memoria compartida, los demás procesos se queden esperando afuera de esa "zona crítica" hasta que el primero termine.

Para lograr esto en C, utilizamos mecanismos como los Semáforos o los Locks (Candados). Un semáforo actúa literalmente como el semáforo de una calle o como un letrero de "Ocupado" en el baño: el proceso bloquea la puerta, hace sus cambios en la memoria y luego la desbloquea para que el siguiente pueda pasar.
