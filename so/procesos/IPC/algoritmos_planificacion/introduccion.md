# Algoritmos de planificacion de recursos

- Buscamos adueñarnos del CPU

Objetivo 2 algoritmos

# ¿por que es necesaria la planificacion ?

- hay muchos procesos en listo y solo una CPU disponible

## ¿quien decide?

El scheduling es el componente del kernel

[roung robing]

[algoritmos de prioridad]

## ¿por cuanto tiempo?

## ¿cuando toma las decisiciones el scheduler?

1. Cuando un proceso es creado
2. un proceso termina
3. adueñarnos

# obkjetivos

- maximizar el uso de la CPU
- maximizar el rendimiento
- maximizar el tiempo

- minimizar el tiempo de espera
- minimizar el tiempo de respuesta: que no se queden pasmados
- deben de ser justos con todos "fairness"

# Clasificacion de los algoritmos

- no apropiativos:
  - no quitan el CPU a procesos que lo usen o hasta bloquearse
  - muy pocos cambios de contexto

- FCFS & SJF shortest Job First (rafagas de carga)

## apropiativos

- Preemtive. El so piede retirar del CPU a un proceso

- Los algoritomos Round robing RR , Prioridad y multiples colas.

# simularas ROUND ROBIN (RR)(100ms)

- 1ms es de cambio de contexto

# -----------------------------------------------------------------

# Teoria: Sincronizacion vs planificacion de procesos

Para entender adecuadamente como interactúan los procesos en un
sistema operativo, debemos separar dos problemas fundamentales:

- Cómo comparten informacion sin destruirla (sincronización de procesos)
- Cóo se turnan para usar le procesador (planificación de procesos)

# Planificacion de procesos y Round Robin

Mientras que la sincronizacion de procesos busca evitar que mas de dos procesos
choquen entre si, al momento de acceder a areas criticas, el
`Sheduler` tabien llamado <mark>planificador del SO</mark> el cual decide
a que proceso le toca ejecutarse en la CPU y por cuanto tiempo.

# Algoritmo Round Robin (RR)

Tambien es conocido por ser el algoritmo justo, famoso y utilizado en sistemas de
tiempo compartido, ya que permite que todos los procesos tengan la misma oportunidad de ejecutarse en la CPU. El algoritmo funciona asignando un tiempo fijo (quantum) a cada proceso en la cola de listos. Cuando un proceso agota su quantum, se coloca al final de la cola y el siguiente proceso en la lista obtiene su turno.

## ¿como funciona?

1. Time Slice / Quantum: A diferencia de algoritmos más antiguos donde un proceso
   corre hasta que termina, Round Robin le asigna a cada proceso un
   pequeño intervalo de tiempo llamado Quantum o Time Slice.
   por ejemplo: 10 o 20 milisegundos.

2. El turno: El proceso se ejecuta en la CPU durante ese Quantum.

3. La interrupcion: Cuando el Quantum se agota, un temporizador de
   Hardware (timer interrupt) dispara una interrupción. El SO
   toma el control, pausa el proceso (guardando su estado "COUNTER SWITCH") y lo coloca al
   final de la cola de listos.

4. Ciclo: esto se repite circularmente.

## Ventajas

- Excekente Tiempo de respuesta (Responce time): Esto es ideal para
  sistemas interactivos "como la computadora personal". Al turnar
  a los procesos tán rapido, da la ilusion de que todos
  (navegador, reproductor de musica, editor de texto, etc) se ejecutan al mismo tiempo.

- Equidad(Fairness): ningun proceso puede acaparar la CPU por siempre.
  Todos reciben una porción justa "proporcional" de tiempo
  de procesamiento.

- evita la inanicion (starvation): Ningun proceso puede ser bloqueado indefinidamente, ya
  que todos reciben su turno.

  ## Desventajas

  - Este algoritmo tiene un pesimo tiempo de retorno (Turnaround Time): si tienes
    3 procesos gigantes que duran exactamente lo mismo y empiezan a la vez,
    Round Robin los va a ir avanzando de a poquito a todos. Esto significa que los
    3 terminaran casi al mismo tiempo, mucho más tarde que si dejaras correr
    a uno por completo, luego al otro, y finalmente al último. Esto es un problema en sistemas donde
    los procesos tienen tiempos de ejecución muy diferentes.

  - Sobrecarga por cambio de contexto: Si el Quantum es demasiado
    pequeño, el SO gastará más tiempo guardanfo y cargando estados
    (context switching) que realmente ejecutando los programas de los
    usuarios. Encontrar el tamaño ideal del Quantum es un desafío. Un Quantum
    demasiado grande se acerca a FCFS, mientras que uno demasiado pequeño genera demasiados
    cambios de contexto.
