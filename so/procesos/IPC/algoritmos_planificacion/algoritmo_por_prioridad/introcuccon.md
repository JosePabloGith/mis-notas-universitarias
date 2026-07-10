# Algoritmo de planificacion por prioridad

Es un algoritmo que puede ser apropiativo y no apropiativo, surgue
de la idea de que no todos los procesos son iguales
aquí hay favoritos.

1. selecciona para ejecutar el proceso con la prioridad
   más alta de la cola de procesos

2. La prioridad es un entero que representa un entero que representa
   la importancia de un proceso.
   [sera el numero mas bajo]

- Debes de considerar que al momento de empatar faboriismos se aplica FCFS

1.

2. Se evita la inanicion por envejecimiento
   Esto es vital para que un proceso no entre en la situacion de ser siempre
   excluido.

# Consideraciones

- Mientras menor sea el numero, sera la prioridad
- no importa quien llega primero

# ¿Qué es la Prioridad?

A cada proceso se le asigna un número que representa su nivel de urgencia.
Ojo con la "trampa" clásica de los libros (como OSTEP): Dependiendo del sistema, a veces el número más bajo significa la prioridad más alta. (Ejemplo: Prioridad 1 es urgencia máxima, Prioridad 10 es tarea de fondo).

# 2. ¿Qué significa "Apropiativo" (Preemptive)?

Los algoritmos por prioridad se dividen en dos tipos: No Apropiativos y Apropiativos.

La Analogía: La Sala de Urgencias (ER)
Imagina que la CPU es el único doctor de guardia en urgencias.

- No Apropiativo (Non-Preemptive): El doctor está curando a un paciente con un esguince en el tobillo (Prioridad 5). De repente, entra por la puerta un paciente con un infarto (Prioridad 1). En este modelo, el doctor le dice al del infarto: "Espere su turno, tengo que terminar de vendar este tobillo primero". El doctor no suelta el procesador hasta que termina voluntariamente.

- Apropiativo (Preemptive): El doctor está vendando el tobillo (Prioridad 5). Entra el paciente con el infarto (Prioridad 1). El doctor inmediatamente deja de vendar el tobillo, manda al paciente del esguince de regreso a la sala de espera, y atiende al paciente del infarto. El proceso de mayor prioridad le "arrebata" (apropia) la CPU al proceso de menor prioridad.

# El gran problema: La Inanición (Starvation)

Este algoritmo tiene un defecto trágico. Si a la clínica no dejan de llegar pacientes con infartos, fracturas y sangrados (Prioridades 1, 2 y 3)... ¡el pobre paciente del esguince (Prioridad 5) podría quedarse sentado en la sala de espera para siempre y morir de viejo ahí! A esto en Sistemas Operativos se le llama Inanición (Starvation).

- La Solución - Envejecimiento (Aging): Para evitar la inanición, los Sistemas Operativos usan una técnica donde, si un proceso lleva mucho tiempo esperando, el SO le "mejora" su prioridad gradualmente. (Ej. El del esguince lleva 10 horas esperando, el SO lo sube a Prioridad 2 para que por fin lo atiendan).

# 4. ¿Cómo cambia nuestro código en C?

Para transformar tu excelente simulador anterior a este nuevo algoritmo, necesitaremos hacer tres cambios vitales:

- Regresar la variable prioridad: Tu intuición anterior era correcta, ¡ahora sí la vamos a usar en el struct Proceso!

- Adiós al Quantum: El algoritmo de Prioridad Pura no usa Quantum. Un proceso corre hasta que termina, A MENOS QUE llegue alguien con mayor prioridad.

- Revolución en la Cola: Nuestra fila FIFO ya no sirve por sí sola. Si llega alguien con prioridad 1, no se va al final de la fila; ¡se salta a todos y se mete al principio! Necesitaremos convertir nuestra cola en una Cola de Prioridad (ordenada por el nivel de urgencia).

```c

// Nuestro struct volverá a cambiar ligeramente:
typedef struct {
    int id;
    int prioridad;        // ¡ESTRELLA DEL ALGORITMO! (1 = Mayor prioridad)

    int t_llegada;
    int rafaga_total;
    int rafaga_restante;

    int t_inicio;
    int t_fin;
    int t_retorno;
    int t_espera;
    int t_respuesta;
} Proceso;



```
