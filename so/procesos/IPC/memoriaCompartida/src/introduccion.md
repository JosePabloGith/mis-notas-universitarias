# Mecanismos de proteccion de coherencia

En la clase vimos que al ejeutar los comandos
esperabamos que los 5 valores nos dieran valores iguales.
Pero al entrar al mismo tiemo ocurrio una condicion de carrera

buhguenado la salida

# Problemas comunes

- condicion de carrera :

- seccion critica
Es donde se indgresa al recurso compartido

- interbloqueo (deadlock):
Ocurre cuando 2 o más procesos se bloquean mutuamente
imagina que tenemos una sola via

si entre ambos se bloquean por querer recursos hay una pelea eterna.

- Inanicion :
Un proceso se queda esperando indefinidamente para entrar a su seccion critica
El sistema operativo nunca.

# Mecanismos de sincronizacion

basicamente son los de transito de la ciudad :D.

## SEMAFOROS

Esta es nuestra seccion principal

## MUTEX

objerto de bloqueo que permite a un proceso tomar el control

## Exclucion mutua

Garantiza que solo un proceso a la vez acceda a un recurso o seccion critica

## Monitores

Co

# Semáforos

Son uno de los Mecanismos de sincronizacion más
importantes en sistemas operativos

## ¿que es un semáforo?

basicamente una bandera, imagina que es una especie de baño
en verde pasas, en rojo te esperas.

se usan 2 señales

- wait() o P(): (Proberen = probar , intentar , disminuir)

- signal() o V() (Verhogen = incrementar):
Es una operacion que libera el recurso y notifica/desbloquea a otros
procesos en la cola "notifica a los otros a que entren"

## Tipos de semáforos

### binario

Son semáforos cuyo valor se restringe únicamente
a 0 o 1

-[ ]Controlan el acceso a un recurso compartido por un solo
  proceso a la vez (exclusion mutua)

- [ ] si el valor es 1, el recurso está libre y un proceso
     puede acceder a él cambiando el valor a 0

- [ ] si el valor es 0 , el recurso está ocupado y el ..

### Conteo o generales

- el semáforo por conteo es basicamente repartir
  n procesos entre n procesos por turnos (valores)

- [ ] ejemplo:
3 libre 3 recursos,
1 un recurso libre,
0 sin recursos,
3 3 procesos en cola (esperando),

#### ¿como operan?

# Como implementar los binarios

## ftok()

## semget() "llamada al sistema para crearlo"

```c

int segment(key_t key, int nsems , int semflg)
// 1. llave del ftok 
// 2. nsems numero de semaforos que 
// 3. semflg permisos

```

## semop() "llamada al sistema"(es basicamente el de las 2 señales)

```c

int semop(int semid, struct sembuf *sops, size_t nsops);

// 1. semid 
// 2. 

```

Esto es lo que hay dentro:

```c

struct sembuf {

  unsigned short sem_num;
  short sem 

};

struct sembuf operation; 
operation.sem_num = 0;

```

- semctl() "llamada al sistema para matar el semáforo".

## se presentaran

struct + union

- Union: similar al struct, pero este se carga uno a la vez.
        En cuestion de pesos, la union reserva el de mayor peso
        a comparacion al struct
