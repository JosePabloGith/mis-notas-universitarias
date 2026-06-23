# avisos

Estudiar algoritmos de ordenamiento

## IPC memoria compartida

- anteriormente los procesos con pipe, se tenia una memoria
  aislada por proceso, esto lo hace el kernel por seguridad

- para que estos puedan compartir memoria, el kernel
  permite la intercomunicación rápidamente en la misma memoria.

- [x] Puede existir la condición de carrera
- [x]

# ventajas

- Facilidad de comunicación
- velocidad
- bajo overhead
- bidireccionalidad:
  solo abriendo la memoria compartida es suficiente
  para hacer lectura y escritura

- auqui no se copian los datos

# desventajas

- al compartir memoria se pueden sobreescribir
  datos usados por otros procesos. `condicion de carrera`

- para evitar la sobreescritura se necesitan usar
  semáforos, etc.

- NECESITAS APUNTADORES

- Problemas de seguridad, se pueden corromper los datos.

# como trabajamos con esto ?

- Un proceso debe de pedir la apertura del ordenamiento de memoria al kernel.

- otros procesos se adjuntan (attach) a ese segmento
  mapeándolo en su propio espacio de direcciones
  virtuales.
  `solo lo hace 1`

  `Hay 4 procesos queriendo entrar en memoria compartida`

- CUANDO UN PROCESO ESCRIBE, LOS CAMBIOS SON INMEDIATAMENTE VISIBLES PARA LOS DEMÁS.

# Elementos necesarops

- ftok() -> pide una llave "recuerda"
- shmget() -> llamada al sistema: crea la ,memoria compartida `se crea una vez`
- shmat() -> Esta basicamente adjunta
- shmdt() -> Esta basicamente dice que ya no nececnita la memoria, para que se la quiten
- shmctl() -> [CUIDADO] este borra la memoria compartida
