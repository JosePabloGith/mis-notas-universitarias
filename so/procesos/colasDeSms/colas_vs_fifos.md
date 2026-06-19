# Colas de Mensajes (Message Queues) vs FIFOs (Tuberías Nombradas)

**Repositorio:** `mis-notas-universitarias/so`  
**Referencias Clave:** * `ESTUDIO_Comunicación-entre-Procesos.pdf`
* *The Linux Programming Interface* (Michael Kerrisk)
* *Advanced programming in the UNIX environment* (W. Richard Stevens)

---

## 1. Diferencias Fundamentales

Aunque tanto las FIFOs como las Colas de Mensajes sirven para la **Comunicación entre Procesos (IPC)** y permiten intercambiar información entre procesos que *no* tienen un ancestro común (a diferencia de las tuberías anónimas o *pipes* simples), su naturaleza y propósito son distintos:

### FIFOs (Tuberías Nombradas)
* **Naturaleza:** Aparecen como un archivo especial en el sistema de archivos (ej. `/tmp/mi_fifo`).
* **Flujo de Datos (Stream):** Transmiten datos como un **flujo continuo de bytes (byte stream)**, sin límites de mensaje definidos. El lector simplemente lee bytes, sin saber si esos bytes forman uno o varios "mensajes".
* **Sincronización:** Son estrictamente FIFO (First-In, First-Out). El primer byte escrito es el primero en leerse. No se pueden "saltar" datos.
* **Acoplamiento Temporal (Bloqueo):** Por defecto, abrir una FIFO para lectura bloquea el proceso hasta que otro la abra para escritura (y viceversa). Requieren que ambos procesos estén activos e interactuando casi al mismo tiempo.
* **Persistencia:** El *archivo* (el nodo en el disco) persiste hasta que se borra, pero los *datos* no. Una vez leídos, los datos desaparecen de la tubería.

### Colas de Mensajes (Message Queues - System V IPC)
* **Naturaleza:** Viven enteramente dentro del Kernel (en la memoria del Sistema Operativo). No las ves haciendo un simple `ls`.
* **Flujo de Datos (Paquetes/Datagramas):** Transmiten datos en **bloques discretos llamados "mensajes"**. El Kernel sabe exactamente dónde empieza y dónde termina cada mensaje.
* **Sincronización:** ¡No están obligadas a ser FIFO! Cada mensaje tiene un **tipo** asociado (un número entero). El receptor puede pedir: *"Dame el primer mensaje de tipo 3"*, ignorando todos los demás.
* **Desacoplamiento Temporal:** Un proceso puede abrir la cola, dejar 5 mensajes y terminar (morir). Horas después, otro proceso puede arrancar, abrir la cola y leer los mensajes.
* **Persistencia:** La cola y los mensajes persisten en la memoria del Kernel hasta que un proceso los elimina explícitamente (con `msgctl()`) o hasta que el sistema se reinicia.

---

## 2. El Propósito y Sentido de las Colas de Mensajes

Si una FIFO puede comunicar procesos, ¿para qué usar una Cola de Mensajes? Las Colas de Mensajes se eligen cuando se necesita:

### A. Estructura y "Paquetización" de Datos Segura
En tu mega práctica, si usas strings (ej. enviar registros de usuarios):
* **Con FIFO:** Si el proceso A envía "Pablo" y luego "Perez", y el proceso B hace un `read()` grande, podría leer "PabloPerez" todo junto. Tienes que programar lógica extra (ej. agregar delimitadores `\n`) para separar los strings.
* **Con Colas:** Envías el paquete "Pablo" y el paquete "Perez" usando `msgsnd()`. El proceso B hace un `msgrcv()` y sacará "Pablo" en una sola pieza. El siguiente `msgrcv()` sacará "Perez". El Kernel preserva la estructura.

### B. Multiplexación por Tipos (El "Súper Poder" de las Colas)
Imagina que varios clientes envían peticiones a un servidor:
* Los clientes pueden enviar mensajes etiquetados por tipo: `Tipo 1` (Urgente), `Tipo 2` (Normal).
* El servidor puede usar `msgrcv()` pidiendo primero los mensajes de `Tipo 1`. Esto es imposible con una FIFO estándar (tendrías que leer todo y clasificarlo en tu propio programa).
* También permite que múltiples procesos lean de la misma cola sin "robarse" los mensajes equivocados: el Proceso B solo saca mensajes de Tipo B, el Proceso C solo saca mensajes de Tipo C.

### C. Asincronía Total
Las colas de mensajes son ideales para arquitecturas estilo "Buzón de Sugerencias" o sistemas de *spooling* (como una cola de impresión). Los productores pueden encolar trabajo a su propio ritmo, y los consumidores pueden extraerlo a su propio ritmo, sin necesidad de estar sincronizados o ejecutándose simultáneamente.

**Resumen para tu práctica:** Usarás Colas de Mensajes porque te permitirán enviar tus *strings* como paquetes cerrados y organizados (posiblemente por tipos), sin preocuparte de que los textos se mezclen o se "peguen", y sin requerir que el proceso lector esté bloqueado esperando al escritor en ese mismo instante.
