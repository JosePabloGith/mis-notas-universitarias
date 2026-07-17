# introduccion

- Administracion de memoria
- Memoria virtual
- DMA
- integrar todos los componentes del sistema operativo.

# ¿por que?

el corazon del curso son los procesos cprriendo al mismo
tiempo. Los procesos apare de CPU nececitan la memoria para
que el proceso pueda guardar

- codigo del programa
- variables
- funciones
- biblioteca
- pila
- Datos dinamicos

"en la ram basicamente tenemos estos bloques"

- So
- proceso a
- proceso b
- proceso c
- espacio libre

# ¿quien chamucos controla el espacio libre?

Lo hace el administrador de memoria

- reserva espacio para nuevos procesos
- protege la memoria
- compartir la memoria, para no desperdiciarla
- liberar la memoria

# ¿que ocurriria si un programa nunca libera la memoria que uso?

- si todos los procesos nececitan la memoria "RAM" entonces porque hay
  diferentes tipos de memoria

- No hay una sola memoria que pueda hacer todo, eso no existe.
  de existir costaria un riñon.

# jerarquia de memoria

- de velocidad
- capacidad
- costo
- proximidad

# ¿porque es necesario pasar por la ram?

- basicamente por el tipo de tecnologia que hay dentro, la velocidad es primordial
  el programa que esta en memoria secundaria al ejecutarlo lo pasamos a la RAM

- Basicamente no se usa por esto:
- tiempos de acceso
- la ram tiene un tiempo de acceso a 80 nano segundos
- mientras que un ssd tarda 20 micro segundos :D

## Razón

la velocidad de tiempo de resopueta de la RAM es absolutamente mas rapido

- Recuerda que el procesador nunca ejecuta instrucciones desde otra fuente diferente a la ram

# organizacion de la RAM

-
- cada proceso ocupa una region de memoria diferente: Esto ess basicamente por seguridad y las condiciones de carrera.

- cuando la ram se acaba, se usa la memoria externa

# MEMORIA VIRTUAL

- existen varios procesos, cada uno tiene su propia memoria

imagina esto:

```c
int arreglo[10];

arreglo[100] = 5; // a la madre
```

- sucedera lo siguiente:
  -[ ] corrupcion de Datos
  -[ ] fallos de aplicaciones
  -[ ] bloqueos del sistema

# espacios de direcciones

Con esto cada proceso

- direccion logica VS direccion fisica

direccion logica basicamente es el molde

# Direccion logica vs direccion fisico

El espacio de direcciones es la vista que un programa en ejecución tiene de la memoria del sistema. Cuando un programa se compila, se diseña bajo la ilusión de que posee su propia memoria privada, comenzando desde la dirección 0 y extendiéndose de forma contigua en un espacio potencialmente enorme (de 32 o 64 bits). A las direcciones generadas por el programa se les conoce como direcciones lógicas o virtuales.

Por otro lado, la dirección física es la ubicación real y tangible dentro de la memoria RAM donde residen los datos.

Tu analogía con GTA V es excelente y muy acertada: mediante la virtualización de memoria, el sistema operativo (SO) permite que el direccionamiento lógico sea mucho más grande que la RAM física disponible. El programa opera en su propia "burbuja" o capullo aislado sin saber (ni necesitar saber) en qué parte física de la RAM está realmente guardado.

# La MMU (Memory Management Unit) y la Traducción

Como bien anotaste, el programa no puede acceder directamente a la RAM con sus direcciones virtuales; requiere un intermediario de hardware en el procesador llamado MMU (Unidad de Gestión de Memoria). Cada vez que el programa intenta hacer una lectura (load), escritura (store) o buscar una instrucción (fetch), la MMU interviene dinámicamente.

## La MMU se encarga de tres tareas críticas

1. Traducción (Address Translation): Transforma la dirección virtual provista por la instrucción en la dirección física real de la RAM. En esquemas simples (como base y límite), esto se hace sumando el valor de un registro base a la dirección lógica.

2. Protección y Aislamiento (Isolation): La MMU verifica que la dirección lógica sea legal y no se exceda de los límites permitidos para ese proceso. Si un proceso intenta acceder a una dirección fuera de sus límites o modificar el espacio de otro proceso o del kernel, la MMU eleva una excepción (falla) y el SO destruye el proceso invasor para proteger el sistema.

3. Soporte de Memoria Virtual: Permite la paginación, coordinándose con el sistema operativo para gestionar qué partes del programa están en la RAM y cuáles no.

# Paginas y Marcos de Memoria "paginación"

Para administrar la memoria de manera eficiente y evitar que los programas tengan que guardarse de forma estrictamente contigua en la RAM (lo cual causaría enormes huecos inutilizables), el sistema divide la memoria en bloques de tamaño fijo:

- Páginas: Son los bloques en los que se divide el espacio de direcciones virtual del proceso.

- Marcos de página (Frames): Son los bloques de igual tamaño en los que se divide la memoria física (RAM).

Tanto las páginas como los marcos deben tener exactamente el mismo tamaño (por convención general y eficiencia en sistemas Linux, suelen ser de 4 KB). La gran ventaja de esto es que las páginas de un mismo programa pueden distribuirse de forma totalmente dispersa y no contigua por toda la RAM física; la MMU y las tablas de páginas se encargan de mantener el orden de manera transparente para el usuario.
7

# Organización de un Proceso en Memoria

El espacio de direcciones virtuales de un proceso clásico no es una masa amorfa; está estructurado de una forma lógica muy clara que divide los diferentes tipos de datos:

- Código (Text): Aquí residen las instrucciones estáticas del programa. Como su tamaño no cambia durante la ejecución, se coloca en un extremo fijo del espacio de direcciones.

- Datos (Data/BSS): Segmento donde se almacenan las variables globales e inicialmente declaradas en el código.

- Heap (Memoria Dinámica): Es el espacio reservado para los datos solicitados explícitamente por el programador durante el tiempo de ejecución (en C, mediante llamadas a la función malloc()). Comienza justo después del código y crece en una dirección determinada.

- Stack (Pila): Se encarga de llevar el control de las llamadas a funciones, almacenar variables locales, pasar parámetros y manejar los valores de retorno.

Regla de crecimiento: Para permitir que tanto el Heap como el Stack puedan crecer y encogerse dinámicamente a lo largo de la vida del programa, se colocan tradicionalmente en extremos opuestos del espacio de direcciones, creciendo en direcciones contrarias para aprovechar al máximo el espacio vacío intermedio.

# Memoria Secundaria, Intercambio (Swapping) y Fallo de Página

¿Qué pasa si abres demasiadas aplicaciones (o una pestaña de Chrome hambrienta de recursos) y la memoria RAM física se llena? Aquí es donde la magia de la memoria virtual brilla a través de la siguiente regla de oro: lo que se está usando activamente debe estar en la RAM; lo que no se usa, puede esperar en el almacenamiento secundario.

- Swapping e Intercambio: El sistema operativo traslada automáticamente las páginas de memoria de los programas inactivos hacia el disco duro o SSD, específicamente a un área reservada llamada espacio de intercambio (swap space), liberando marcos en la RAM física para los programas que los necesitan en ese instante.

- Fallo de Página (Page Fault): Si el programa en ejecución necesita ejecutar una instrucción o leer un dato que está guardado en una página que fue enviada al swap space (es decir, no está presente en la RAM), la MMU no podrá resolver la dirección física. En ese milisegundo ocurre un Fallo de Página.

Cuando ocurre un fallo de página, el hardware detiene momentáneamente el programa y salta al kernel del sistema operativo mediante una trampa (trap). El SO hace lo siguiente:

1. Localiza la página solicitada dentro del espacio de intercambio en el almacenamiento secundario.

2. Busca un marco libre en la memoria RAM física para traer la página de vuelta.

3. Si todos los marcos de la RAM están llenos, el SO se ve obligado a elegir una página víctima en la RAM, mandarla al disco (swap out) y colocar la nueva página en su lugar (swap in). Una vez hecho esto, actualiza la tabla de páginas y le dice a la CPU que reintente la instrucción cohibida de forma totalmente transparente.

# Algoritmos de Reemplazo de Páginas

Para decidir a quién expulsar de la RAM cuando no hay marcos libres, el sistema operativo implementa algoritmos de reemplazo:

- FIFO (First In, First Out): El primero en llegar a la RAM es el primero en ser expulsado cuando se necesita espacio. Es muy simple de programar, pero no siempre es eficiente, ya que podría expulsar una página que se usa constantemente solo porque llegó hace mucho tiempo.

- OPT / MIN (Algoritmo Óptimo): Expulsa la página que tardará más tiempo en volver a ser utilizada en el futuro. Como bien señalan tus notas, este algoritmo es puramente teórico porque el SO no tiene "un zafiro que vea el futuro"; sin embargo, sirve como el estándar de oro perfecto para medir qué tan buenos son los demás algoritmos en simulaciones.

1. LRU (Least Frequently Used - Menos Frecuentemente Utilizada)

- ¿Cómo piensa?: Se basa en la popularidad o intensidad de uso.

Mecanismo: Cada página tiene un contador interno que incrementa cada vez que el proceso la lee o escribe. Cuando la RAM se llena, el sistema operativo busca y expulsa la página que tenga el menor número de accesos (la menos popular).

1. Algoritmo de Segunda Oportunidad o del Reloj (Clock Algorithm)

- ¿Cómo piensa?: Es una mejora directa y brillante del algoritmo FIFO.

- Mecanismo: Imagina que las páginas en la RAM forman un círculo (como las horas de un reloj) y hay una manecilla apuntando a una página víctima. Cada página tiene un bit de referencia (que el hardware pone en 1 cuando se usa). Cuando se busca espacio, la manecilla avanza: si ve un 1, le da una "segunda oportunidad", cambia el bit a 0 y pasa a la siguiente. Si encuentra una página con el bit en 0, esa es la elegida para ser expulsada.

# Vigencia en la actualidad en SO modernos

Los conceptos siguen vigentes, pero las implementaciones en los Sistemas Operativos modernos (como Linux o Windows) han evolucionado hacia variantes mucho más sofisticadas. Aquí está la realidad de los 5 clásicos en la actualidad:

- FIFO: Ya NO se usa en producción. Es sumamente ineficiente y sufre de un problema llamado la Anomalía de Belady (un fenómeno raro donde, si le das más memoria RAM al sistema, ¡curiosamente genera más fallos de página!).

- Óptimo (OPT): Sigue siendo imposible de implementar en la vida real porque requeriría predecir el futuro exacto del código. Solo se usa en laboratorios para medir qué tan buenos son otros algoritmos en simulaciones.

- LRU Puro: Es un algoritmo excelente en teoría, pero en la práctica es demasiado costoso para el hardware. Para mantener un LRU perfecto, la CPU tendría que actualizar una marca de tiempo (timestamp) o mover nodos en una lista enlazada en cada mismísima referencia de memoria, lo que destruiría el rendimiento del procesador.

## ¿Qué se usa en los sistemas modernos hoy en día?

Los sistemas operativos actuales utilizan Aproximaciones de LRU muy optimizadas. Por ejemplo, el kernel de Linux implementa un esquema llamado LRU de Dos Listas (o Algoritmo 2Q) combinado con conceptos del Algoritmo del Reloj.

En lugar de una sola lista, Linux divide las páginas de la memoria en dos categorías principales:

1. Lista Activa: Contiene las páginas que han sido accedidas muy recientemente de manera constante.

2. Lista Inactiva: Contiene páginas candidatas a ser expulsadas al espacio de intercambio (swap space) si ocurre un fallo de página (page fault).

Cuando una página inactiva es accedida de nuevo, el kernel la promueve a la lista activa. Si la lista activa se satura, las páginas menos usadas "caen" hacia la lista inactiva. De esta forma, el sistema operativo logra un comportamiento muy cercano al LRU perfecto, pero con un consumo de hardware mínimo y veloz.
