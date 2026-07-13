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

- si todos los procesos nececitan la memoria "RAM"  entonces porque hay
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
