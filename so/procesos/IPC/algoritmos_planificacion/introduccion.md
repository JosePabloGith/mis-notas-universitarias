# Algoritmos de planificacion de recursos

- Buscamos adueñarnos del CPU

Objetivo 2 algoritmos

# ¿por que es necesaria  la planificacion ?

- hay muchos procesos en listo y solo una CPU disponible

## ¿quien decide?

El scheduling  es el componente del kernel

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

- FCFS & SJF shortest Job First  (rafagas de carga)

## apropiativos

- Preemtive. El so piede retirar del CPU a un proceso

- Los algoritomos Round robing RR , Prioridad y multiples colas.

# simularas ROUND ROBIN (RR)(100ms)

- 1ms es de cambio de contexto
