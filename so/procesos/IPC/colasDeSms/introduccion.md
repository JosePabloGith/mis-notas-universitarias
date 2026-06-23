# Colas de Mensajes (Message Queues) en Sistemas Operativos

**Repositorio:** `mis-notas-universitarias/so`  
**Referencias Clave:** * `ESTUDIO_Comunicación-entre-Procesos.pdf`
* *The Linux Programming Interface* (Michael Kerrisk)

---

## 1. El Concepto Base: El Aislamiento de los Procesos

En los Sistemas Operativos modernos (como UNIX/Linux), hay una regla de oro: **Los procesos están aislados**. 

Imagina que cada programa en ejecución (proceso) vive en una habitación insonorizada y sin ventanas. El Sistema Operativo le da a cada uno su propio espacio de memoria por seguridad. Esto significa que **el Proceso A no puede leer ni modificar las variables (como un *string*) que viven en la memoria del Proceso B.**

## 2. ¿Qué es una Cola de Mensajes?

Si el Proceso A necesita enviarle un *string* al Proceso B, no puede simplemente "pasárselo" en una variable compartida. Necesitan un intermediario de confianza. Aquí es donde entra el Sistema Operativo ofreciendo mecanismos de **Comunicación entre Procesos (IPC - Inter-Process Communication)**.

Una **Cola de Mensajes** es esencialmente un **buzón de correos** administrado por el kernel del Sistema Operativo:
1. El **Proceso A** le dice al Sistema Operativo: *"Toma este string y guárdalo en el buzón número 5"*.
2. El **Proceso B** le dice al Sistema Operativo: *"Oye, revisa si hay mensajes para mí en el buzón número 5 y entrégamelos"*.

## 3. El set de funciones en C (System V IPC)

Las funciones en C (`ftok`, `msgget`, etc.) son simplemente las herramientas estándar en sistemas UNIX para interactuar con ese "buzón". En nuestra próxima lección veremos cómo se mapean:

* `ftok()`: Fabrica la llave única para encontrar el buzón.
* `msgget()`: Obtiene el acceso (abre o crea) el buzón.
* `msgsnd()`: Envía (mete) un mensaje en el buzón.
* `msgrcv()`: Recibe (saca) un mensaje del buzón.
* `msgctl()`: Controla el buzón (por ejemplo, para destruirlo cuando ya no se usa).

## 4. Que son esas funciones y como funcionan

## 1. El Problema: ¿Cómo se encuentran dos procesos ciegos?

Imagina que la **Cola de Mensajes** es una caja fuerte invisible que vive dentro del Kernel (memoria del Sistema Operativo). 
* El Proceso A quiere crear la caja fuerte y meter un mensaje.
* El Proceso B quiere abrir esa misma caja fuerte y sacar el mensaje.

Como los procesos están aislados, no pueden pasarse la dirección de memoria de la caja fuerte. Necesitan un "Punto de Encuentro" o un **Número de Identificación Único (Key)** que ambos puedan calcular por su cuenta y que les dé el mismo resultado.

## 1. ¿Qué es `key_t ftok(const char *pathname, int proj_id);`?

La palabra **ftok** significa literalmente **"File TO Key"** (De Archivo a Clave). 
Es una función matemática (un algoritmo de *hashing*) que toma dos ingredientes y los mezcla para generar un número único (una clave de tipo `key_t`). Esta clave es el "número de serie" de tu oficina de correos (la cola de mensajes) dentro de la memoria del kernel.

### El porqué de la llave (Un último repaso arquitectónico)
Imagínate que el casillero físico en el Kernel solo tiene un número interno: el `ID` (por ejemplo, el casillero `5`). Tu programa `emisor.c` lo crea y obtiene el `5`. Pero el programa `receptor.c` es un binario completamente independiente. Cuando se ejecuta, ¿cómo sabe que tiene que pedirle al Kernel el casillero `5` y no el `3` o el `10`? 
No puede adivinarlo. Por eso necesitan la **Llave** (`key_t`). La llave es un punto de acuerdo público y externo (matemático) que ambos procesos calculan por separado usando el mismo archivo (`/tmp`) y el mismo ID (`'M'`). Al pasarle esa misma llave a `msgget()`, el Kernel dice: *"Ah, ambos me están trayendo la misma combinación, por lo tanto, a ambos les daré acceso al casillero interno 5"*. La llave es para **encontrarse**; el ID es para **operar**.

### El misterio de `/tmp`: Si vive en la memoria, ¿por qué usamos una ruta?
Aquí está el truco maestro de los diseñadores de UNIX:
Como los procesos (tu Emisor y tu Receptor) viven aislados y no comparten memoria, necesitan **un punto de referencia en común** para poder encontrarse. ¿Y qué es lo único que todos los procesos comparten y pueden ver? **El sistema de archivos (el disco duro).**

Cuando tú haces `ftok("/tmp", 'M')`:
1. El sistema operativo **NO** crea ningún archivo llamado 'M' ni guarda tus mensajes en `/tmp`. Todo sigue viviendo exclusivamente en la memoria RAM (en el kernel).
2. Lo que hace `ftok` es ir al sistema de archivos, buscar la carpeta `/tmp` y preguntarle al disco duro: *"Oye, ¿cuál es tu número de identificación interno (conocido como inodo)?"*.
3. Toma ese número de identificación del directorio `/tmp` y lo usa como **semilla matemática** para generar tu clave única.

Se usa `/tmp` porque es un directorio que siempre existe en los sistemas Linux/UNIX. Podrías usar `/home/pablo/mi_archivo.txt` si quisieras, **siempre y cuando el archivo exista** antes de correr el programa.

### ¿Qué es la `'M'`? (El `proj_id`)
La `'M'` no es el nombre de un archivo. Es un identificador de proyecto (`proj_id`), que es simplemente un número entero de 8 bits (un solo carácter).

**¿Para qué sirve?**
Imagina que usas `/tmp` para generar la clave de tu cola de mensajes. Pero tu amigo también está programando en la misma computadora y también usa `/tmp` para su cola. ¡Las claves chocarían! 
Para evitarlo, tú le sumas la letra `'M'` (de Mensajes) y tu amigo le suma la letra `'X'`. Así, aunque ambos usen el mismo archivo base (`/tmp`), `ftok` generará dos claves matemáticas completamente distintas.

### ¿Por qué si la clave es `-1` significa error?
En el lenguaje C y en la programación de Sistemas Operativos UNIX/Linux, el número **-1** es el estándar universal para decir *"Algo salió mal"*.

Si `ftok` te devuelve `-1`, significa que falló al crear la clave. ¿Por qué fallaría?
Generalmente ocurre porque el archivo o directorio que le pasaste (en este caso `"/tmp"`) **no existe** o tu programa **no tiene permisos** para leerlo. Como no pudo leer el número de identificación del archivo, entra en pánico, devuelve `-1` y te dice: "No puedo generar la clave". Por eso en tu código haces:
```c
if (clave == -1) {
    perror("no se pudo crear la clave");
    exit(1);
}
```

## 2. La función msgget() y sus Secretos

Hemos llegado a la función central del mecanismo: `msgget()`. Si `ftok()` era conseguir la dirección de la oficina de correos, `msgget()` es la ventanilla donde se gestiona el buzón físico.

Vamos a destripar la firma de la función:

```c

int msgget(key_t key, int msgflg);

```

---

### 1. La Alegoría de la Vida Real: El Lockers del Gimnasio

Imagina que entras a un gimnasio (el Kernel del Sistema Operativo) que tiene una sección de casilleros/lockers para que la gente se deje notas de forma segura. 

Llegas a la recepción y le dices al encargado: *"Quiero interactuar con un casillero"*. El encargado te pide dos cosas:
1. **La Clave (`key`):** Un número de identificación (el que generaste con `ftok`).
2. **Las Intenciones y Reglas (`msgflg`):** Qué quieres hacer si el casillero no existe y quién tiene permiso de abrirlo.

El encargado revisa sus registros y, si todo está en orden, te entrega una **tarjeta con un número de casillero asignado** (este es el ID retornado, por ejemplo, el casillero `5`). A partir de ese momento, ya no usas la clave larga matemática; en tus próximas instrucciones (`msgsnd` o `msgrcv`) solo dices: *"Pon esto en el casillero 5"*.

---

### 2. ¿Qué es eso de los permisos y opciones (`msgflg`)?

El segundo parámetro, `msgflg`, es una máscara de bits. Combina dos cosas usando el operador OR lógico (`|`): **Opciones de Creación** + **Permisos de Acceso**.

### A) Opciones de Creación (Las banderas IPC)
* **`IPC_CREAT`:** Significa *"Si el casillero con esta clave no existe en el gimnasio, créalo en este momento"*. Si ya existía, simplemente dame acceso a él.
* **`IPC_EXCL`:** Se usa junto con `IPC_CREAT` (`IPC_CREAT | IPC_EXCL`). Significa *"Quiero crear un casillero nuevo, pero si ya existe uno con esa misma clave, lanza un error inmediatamente"*. Esto garantiza que tú seas el único dueño original de esa cola y nadie más la estaba usando antes.

### B) Los Permisos (El número octal `0666`)
En tu código tienes `IPC_CREAT | 0666`. ¿Qué significa ese `0666`? Son los clásicos permisos al estilo UNIX/Linux para proteger quién puede meter o sacar strings de tu cola:
* El primer `6` (dueño): Tiene permiso de **Lectura (4) + Escritura (2) = 6**. Puede enviar y recibir mensajes.
* El segundo `6` (grupo): El grupo del usuario también puede leer y escribir.
* El tercer `6` (otros): Cualquier otro usuario del sistema puede leer y escribir en la cola.

Si quisieras que **solo tu usuario** pudiera usar la cola y nadie más pudiera espiarte o meter texto, usarías `0600` (Lectura/Escritura para ti, Cero para el resto).

---

### 3. ¿Qué retorna `msgget()`?

Retorna un **identificador de cola de mensajes** (un entero positivo o cero). Es el número de casillero interno que asignó el Kernel. Es único para tu programa en ese momento y es mucho más rápido de procesar para la CPU que estar buscando la clave original a cada rato.

---

### 4. Si el ID de la cola es `-1`, ¿qué implica?

Implica un **Fallo Crítico**. El Kernel te está diciendo: *"No pude darte un casillero"*. Al igual que con `ftok()`, el `-1` activa el bloque de seguridad de tu código:

```c
if (id_cola == -1) {
    perror("Error al crear la cola de mensajes");
    exit(1);
}
```

### ¿Por qué razones fallaría `msgget()` devolviendo `-1`?

Cuando esto pasa, el sistema operativo guarda un código de error específico en una variable global llamada `errno`. Los motivos más comunes son:
1. **`EEXIST`:** Usaste `IPC_CREAT | IPC_EXCL` pero la cola ya existía en la memoria del kernel. Es decir, el casillero ya estaba ocupado.
2. **`EACCES`:** La cola ya existe, pero tú no tienes permisos de lectura/escritura (por ejemplo, fue creada con `0600` por el usuario `root` y tú estás intentando entrar como el usuario estándar `alumnado`). El gimnasio te prohíbe tocarlo.
3. **`ENOENT`:** Intentaste acceder a una cola sin usar la bandera `IPC_CREAT`, y resulta que nadie la ha creado todavía. Llegas a buscar un casillero que no existe.
4. **`ENOSPC`:** El Sistema Operativo se quedó sin memoria en el Kernel para administrar más colas de mensajes. Has alcanzado el límite máximo permitido por el sistema.

## El Envío del Mensaje: `msgsnd()`

Una vez que ya abriste tu casillero (`msqid`), es hora de meter un sobre con información usando la llamada al sistema:

```c
int msgsnd(int msqid, const void *msgp, size_t msgsz, int msgflg);
```

Vamos a destripar cada uno de sus cuatro parámetros basándonos en la documentación estándar de IPC y el código que tienes en tu archivo `SistemasOperativos_ejerciciosDinamicos`.

### - `int msqid` (El Casillero Destino)
Es el identificador entero que te devolvió `msgget()`. Le dice al Kernel exactamente en cuál cola de mensajes de la memoria RAM se va a depositar el paquete.

### -  `const void *msgp` (El Puntero al Sobre)
Es la dirección de memoria de la estructura (`struct`) que quieres enviar. En tu código le pasas `&msg`.
* **Regla estricta del Kernel:** Esta <u>estructura obligatoriamente debe empezar con un campo de tipo `long` que representa el "tipo" o "prioridad" del mensaje, seguido por los datos (el texto). </u>

```c
struct mensaje {
    long tipo;       // OBLIGATORIO: Debe ser > 0
    char texto[200]; // Tus datos reales
};
```

### - `size_t msgsz` (El Tamaño del Contenido)

<mark>¡Ojo aquí!</mark> Este es uno de los errores más comunes en los exámenes de laboratorio. <span style="background-color: lightgreen; color: black;">El Kernel te pide el tamaño de los **datos reales**, **NO** el tamaño de toda la estructura</span>. No le interesa el tamaño de la etiqueta de prioridad (`long`).
Por eso en tu código fuente tienes exactamente esto:

```c

sizeof(struct mensaje) - sizeof(long);

```
Le restas el tamaño del `long` para decirle al Kernel: *"Oye, mi sobre completo mide 208 bytes (8 bytes del long + 200 del char), pero de datos netos solo te estoy mandando 200 bytes"*.

### - `int msgflg` (Las Reglas de Espera ante una Cola Llena)
Las colas de mensajes en el Kernel tienen un límite físico de almacenamiento (un espacio máximo en bytes, comúnmente 65,536 bytes por defecto en Linux). ¿Qué pasa si el emisor envía mensajes tan rápido que llena la cola antes de que el receptor los lea? Aquí es donde entra este parámetro:

* <span style="background-color: #FF69B4; color: black;">** Opción `0` (Bloqueante - "Me espero")** </span> 
  Si pones `0` como bandera, le dices al Kernel: *"Si la oficina de correos está llena y no cabe mi sobre, suspende mi proceso (ponlo en estado **Bloqueado**). No me despiertes hasta que el receptor saque un mensaje, se libere espacio y yo pueda meter el mío"*. Tu programa se congelará temporalmente en esa línea de forma segura sin consumir CPU (espera pasiva).
  
* **Opción `IPC_NOWAIT` (No Bloqueante - "Lanza error si está llena"):**
  Le dices al Kernel: *"Tengo prisa. Si la cola está llena, NO me bloquees. Devuélveme un error de inmediato"*. La función fallará, retornará `-1` y configurará la variable `errno` con el valor `EAGAIN` (Inténtalo de nuevo). Esto sirve para programas de tiempo real que no pueden darse el lujo de quedarse congelados esperando hardware o memoria.

---

### - ¿Qué retorna `msgsnd()`?

* **`0` en caso de Éxito:** Significa que el mensaje ya fue copiado con éxito desde el espacio de usuario (tu programa) al espacio del Kernel (la RAM protegida) y ya está haciendo fila.
* **`-1` en caso de Error:** Significa que el mensaje se rechazó. El bloque `if (msgsnd(...) == -1)` se activará.

### - ¿Por qué fallaría `msgsnd` devolviendo `-1`?
1. **`EAGAIN`:** La cola estaba llena y especificaste `IPC_NOWAIT`.
2. **`EIDRM`:** Mientras tu proceso estaba dormido esperando espacio (con bandera `0`), otro usuario o proceso destruyó la cola de mensajes usando `msgctl()` o el comando `ipcrm` desde la terminal. Te quedaste esperando un casillero que ya no existe.
3. **`EINVAL`:** El ID de la cola no es válido, o el `msg.tipo` es menor que 1 (el tipo de mensaje obligatoriamente debe ser un número positivo).

---

### Pregunta de validación para tu Proceso de Pensamiento:

Imagina que estás diseñando un sistema de alertas para el tablero de un automóvil conectado a sensores de hardware. Si el motor se está sobrecalentando, el proceso "Sensor" debe mandar un mensaje urgente a la cola IPC para que el proceso "Pantalla" lo muestre.

**Considerando la vida humana y la seguridad del hardware, ¿cuál de las dos políticas de `msgflg` usarías para el mensaje de sobrecalentamiento del motor: `0` (esperar si la cola está llena) o `IPC_NOWAIT` (abortar/notificar inmediatamente por otra vía si está llena)? ¿Por qué?**

---

## 1. La Recepción: `msgrcv()`

El programa receptor utiliza esta función para ir al casillero en el Kernel y extraer un "sobre". Su firma es:

```c
ssize_t msgrcv(int msqid, void *msgp, size_t msgsz, long msgtyp, int msgflg);
```

### Los parámetros desglosados:
* **`int msqid`:** El ID del casillero (el que te dio `msgget`).
* **`void *msgp`:** La dirección de memoria (el puntero `&msg`) donde el Kernel va a "desempacar" o guardar el mensaje que extraiga. Tu programa debe tener un `struct` vacío listo para recibir los datos.
* **`size_t msgsz`:** El tamaño máximo del texto que estás dispuesto a leer (recuerda, usando `sizeof(struct mensaje) - sizeof(long)`).

**Aquí vienen los dos parámetros más interesantes:**

* **`long msgtyp` (Filtro de lectura):** ¡Esta es la magia de las colas de mensajes! A diferencia de los *Pipes* donde lees lo que caiga, aquí puedes elegir qué leer:
    * Si `msgtyp == 0`: Lee el primer mensaje de la cola, sin importar su tipo (como una fila normal, FIFO).
    * Si `msgtyp > 0`: Lee el primer mensaje que tenga **exactamente** ese tipo. (Ej. Si le pones `1`, ignorará todos los mensajes tipo `2` o `3` y sacará solo el `1`).
    * Si `msgtyp < 0`: Lee el mensaje con el tipo más bajo que sea menor o igual al valor absoluto de `msgtyp`. (Útil para manejar prioridades).

* **`int msgflg` (Las banderas de acción):** Como bien dedujiste en tu nota, esto le dice al Kernel qué hacer en casos límite:
    * **`0` (Bloqueante):** Si la cola está vacía o no hay mensajes del tipo que pediste, el Kernel suspende tu proceso (lo pone a dormir) hasta que llegue uno.
    * **`IPC_NOWAIT`:** Si no hay el mensaje que buscas, el Kernel no te duerme; retorna `-1` inmediatamente y configura el error `ENOMSG`.
    * **`MSG_NOERROR`:** ¿Qué pasa si te mandan un sobre con 500 bytes de texto, pero tu `msgsz` dice que solo puedes recibir 200? Por defecto, el Kernel aborta y te da error. Si usas esta bandera, le dices al Kernel: *"Trunca el mensaje. Dame los primeros 200 bytes y destruye el resto en silencio"*.

### ¿Qué retorna `msgrcv()`?
Retorna la **cantidad de bytes reales** que copió a tu variable (el tamaño del texto). Si falla, retorna `-1`. Al extraer el mensaje, **este se borra permanentemente** de la cola en el Kernel.

---

## 2. El Administrador: `msgctl()`

Me comentas que no tienes mucha información sobre esta función. `msgctl` (Message Control) es vital porque **las colas de mensajes en System V IPC son persistentes en el Kernel**. 

Si tus programas `emisor.c` y `receptor.c` terminan su ejecución, **¡la cola de mensajes NO desaparece!** Se queda ocupando memoria RAM hasta que reinicies la computadora o la destruyas manualmente. 

Para eso sirve `msgctl()`, para operar sobre la configuración de la cola:

```c
int msgctl(int msqid, int cmd, struct msqid_ds *buf);
```

### ¿Cómo se usa para destruir la cola?
El uso principal en las prácticas de la universidad es para limpiar la memoria al final del programa.
Para destruir el casillero, usamos el comando `IPC_RMID` (Remove ID):

```c
// Destruye la cola de mensajes inmediatamente
msgctl(id_cola, IPC_RMID, NULL);
```

* **`cmd = IPC_RMID`:** Le ordena al Kernel destruir la cola de mensajes de la memoria, despertando (con un error) a cualquier proceso que estuviera dormido esperando leer o escribir en ella.
* **`buf = NULL`:** Como solo queremos destruirla, no necesitamos pasarle ninguna estructura de datos para leer sus estadísticas. (Si usaramos `IPC_STAT` nos devolvería quién la creó, a qué hora, cuántos bytes tiene, etc.).

---

### Pregunta de validación para tu Proceso de Pensamiento:

Imagina que programas el Emisor para que mande tres mensajes seguidos a la misma cola, pero con diferentes prioridades (tipos):
1. Primero manda un mensaje de `tipo 3`.
2. Luego manda un mensaje de `tipo 1` (Urgente).
3. Por último manda un mensaje de `tipo 2`.

**Si tu programa Receptor ejecuta `msgrcv(id_cola, &msg, tamano, 0, 0);` (Nota que el `msgtyp` es `0`), ¿cuál de los tres mensajes leerá primero y por qué?**

¡Analiza cómo funciona el parámetro `msgtyp` y dime tu respuesta!
