# Indice

1. [Introduccion](#memoria-compartida)
2. [Sentido](#¿porque-se-usa?)
3. [contras](#problemas)
4. [Solución de problemas](#problemas)

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

Para lograr esto en C, utilizamos mecanismos como

- los Semáforos
- los Locks (Candados).

Un semáforo actúa literalmente como el semáforo de una calle o como un letrero de "Ocupado" en el baño: el proceso bloquea la puerta, hace sus cambios en la memoria y luego la desbloquea para que el siguiente pueda pasar.

# Codigos profesor

El profesor a compartido sus codigos molde para poder
hacer su practica de manera rapida, es por ello que primero
debemos de analizar como funciona.

```c
// Dentro del codigo del profesor se ve algo como:
// debemos de recordar que el sistema necesita una forma
// única de identificar el "canal" de memoria que vamos a
// crear . ftok toma la ruta de archivo "en este caso /tmp"
// y un caracter 'A' para generar la clave numérica única.

// [*] AMBOS PROCESOS "LECTOR ESCRITOR deben de usar
// exactamente los mismos parametros en ftok para generar
// la misma llave"
clave = ftok("/tmp", 'A');

/*
  Posteriormente el profesor solicita un segmento

  + ¿que hace?
    Aquí es donde le decimos al kernerl: "Dame un pedazo"
    de memoria RAM de tamaño TAM * sizeof(int)

  $ Un detalle es que la bandera IPC_CREAT le dice al
    SO que si no existe, lo cree. El 0666 son los
    permisos de lectura/escritura para todos, como si
    fuera una especie de archivo.

  $ Dato curioso
    shm significa shared memory (Memoria Compartida)
    get significa obtener

    shmget literalmente significa (obtener memoria compartida)

  + ¿que es lo que guardará id_memoria?
    NO GUARDA EL ARREGLO
    NO GUARDA DATOS
    NO ES LA MEMORIA EN SÍ

    * Basicamente es un ticket

    -- piensallo así : imagina que compras una chamarra
                      de piel a la tintoreria
                      1. Entregas tu chamarra al encargado (kernel)
                      2. El encargado toma la chamarra, lo guarda
                        en el pasillo reservado "donde no tienes
                        acceso, y a cambio te da un ticket"

    [] recuerda que el kernel de linux mantiene una tabla interna
       que registra todos los pedazos de memoria compartida que
       están activos en la computadora. La función shmget()
       simplemente crear el bloque en la RAM y retorna el ticket

    [*] Con esto básicamente logramos que cualquier proceso
        que quiera interactuar con esa memoria especifica
        simplemente deberá presentarle ese ticket id_memoria

        * si el kernel regresa -1 significa que ya no hay espacio.


*/
id_memoria = shmget(clave, TAM * sizeof(int) , 0666 | IPC_CREAT );

// dentro del codigo del profesor, una vez
// que se genera el ticket y se comprueba de que el
// kernel no haya respondido con -1 "memoria llena o algo así":
// se hace lo siguiente:

arreglo = (int *)shmat(id_memoria , NULL , 0);

/*
  [*] ¿que es shmat()?

      >> sh 'shared Memory'
      >> at 'Attach' (adjuntar, unir , conectar).

      >> shmat literalmente significa "conectar a la memoria"

  [?] ¿cual es su función?
      Para entenderlo podemos usar esta alegoria:
      Imagina que tu programa C vive en su propia casa.
      Esta casa esta aislada por el kernel, entonces no se puede
      ver lo que hay afuera.

      anteriormente usamos shmget. Lo único que hizo esa función
      es pedirle al SO que construyera un parque publico
      (memoria compartida) afuera de la casa, de modo que el SO
      nos dio un papelito ticket.

    [!] Pero si tu programita esta dentro de su casa aislada
        con un ticket en sus manos, el programita no puede
        ir a jugar al parque. EL programa en ese punto no tiene
        una forma física de poder meter datos allá.

    [★] Aquí es donde entra shmat, basicamente es como
        un albañil del SO, tu programita le da el ticket
        contenido en id_memoria y él construye una puerta
        directa desde tu casa hacia ese parque.

        En el momento de que shmat termina su chamba,
        tu programita ya puede cruzar la puerta y jugar en el
        parque. ("ya puede dejar sus datos en la memoria
        compartida")

    [!] Explicamos a detalle:

        arreglo = (int *)shmat(id_memoria, NULL, 0);

        shmat(...) : oye SO quiero que me conectes a una memoria
                   compartida
        -(id_memoria) : Aquí tienes el ticket. Este es el
                      espacio exacto al que quiero que me conectes

        -(..., NULL): Construye la puerta en el lugar de mi
                      casa que tu quieras. No me importa
                      la dirección exacta, confío en ti.
                      "Dejamos que el kernel decida qué
                      dirección de memoria virtual asignar"
                      Sin meternos en tantos problemas.

        -(...,..., 0): Estamos indicando que regla de apertura
                      de la puerta. Cuando es 0 decimos :
                      "abre la puerta con todos los permisos
                      normales (lectura, escritura)"

        (int *)...(...,...,...): Piensa que el parque de
              afuera solo es tierra muerta "vacia"
              con (int *) basicamente le decimos a C:
              "a partir de ahora, todo lo que pongamos aquí
              tratalo como numeros enteros int"


      arreglo = (...)...(...,...,...):
              "Finalmente, a esa puerta que acabas de construir
              llámala arreglo"

      [★] ¿que logramos con eso?
          Una vez que la linea:
           arreglo = (int *)shmat(id_memoria , NULL , 0);
          se ejecuta correctamente, la variable arreglo
          ya no es una variable normal de tu programa. Se
          ha convertido en un "puente magico";

        Si en la siguiente instrucción colocaras arreglo[0] = 5;
        ese numero 5 no se guarda en la memoria normal
        del programa, viaja inmediatamente a través de la puerta
        y la guarda en la memoria RAM compartida, donde
        cualquier otro programa que tenga otra puerta al parque
        podrá verlo.

    [$] hay una seccion del codigo del profesor que hace esto:

        shmdt(arreglo);

        -- 'shared memory deteach' quita la puerta de la casa
            memoria virtual del programita
            basicamente le dice al SO ""


    [☠] CUANDO YA SE HAYA TERMINADO EL PROPOSITO DE
        LA MEMORIA COMPARTIDA ES FUNDAMENTAL ELIMINARLA
        O DE LO CONTRARIO SE QUEDARA ALLÍ HASTA EL REINICIO
        DE LA COMPUTADORA.

        Es por ello que si estas por salir
        se hace uso de

        shmctl(id_memoria, IPC_RMID, NULL);

        literalmente se destruye el parque y se devuelven los
        recursos al SO.

*/


```

# Conceptos necesarios

Para entender a detalle que es lo que se hará dentro del
tema de memoria compartida, considera lo siguiente:

Si tienes 4 procesos intentando acceder a ese mismo arreglo (tu pizarra compartida) al mismo tiempo, el problema de la "Condición de Carrera" que vimos hace rato se multiplica por cuatro.

Imagina a 4 estudiantes con plumones corriendo hacia la misma pizarra para escribir en la misma casilla (arreglo[0]) exactamente en el mismo milisegundo. Va a ser un desastre y los datos van a quedar corruptos o sobreescritos.

Para solucionar esto, en Sistemas Operativos usamos Semáforos.

<mark>Piensa en un semáforo como la llave del baño de una cafetería:</mark>

- Solo hay una llave.

- El Proceso 1 llega, toma la llave, entra al baño (accede al
  arreglo) y le pone seguro a la puerta.

- Llegan los Procesos 2, 3 y 4. Como la puerta tiene seguro y no hay llave, se quedan dormidos esperando afuera (el Sistema Operativo los pausa para que no consuman CPU).

- El Proceso 1 termina de escribir en el arreglo, sale, y devuelve la llave (desbloquea el semáforo).

- El Sistema Operativo despierta a uno de los procesos que estaba esperando (ej. Proceso 2) y le da la llave para que sea su turno.

- <mark>Esta zona donde los procesos escriben en el arreglo se llama "Zona Crítica"</mark>. La regla de oro en Sistemas Operativos es: <mark>Nunca dejes que más de un proceso entre a la Zona Crítica al mismo tiempo.</mark>

```c

//esto se hace dentro del codigo del profesor
pedir_llave_del_semaforo(); //<- bloquea a los demás

printf("ingresa los datos del arrglo");
for(int i = 0 ; i < TAM ; i++){
  //considera que aqui arreglo, ya tiene conexión con la memoria
  //compartida
  scanf("%d",&arreglo[i]); //-> zona critica
}

devolver_llave_del_semaforo(); //<- dejamos pasar al siguiente proceso

```

- -> La RAM Física (El mundo real): Allá afuera en la tarjeta RAM real de tu computadora está el "parque público" (el pedazo de memoria física que creó shmget).

- -> El Espacio Virtual del Proceso (Tu casa): Recuerda que tu programa en C no ve la RAM real directamente por motivos de seguridad; el Sistema Operativo le da a tu programa una ilusión llamada Espacio de Direcciones Virtuales (las paredes de tu casa). Tu programa cree que toda la memoria es suya.

# Introduccion a los semáforos

La implementación de Semáforos en System V (UNIX) requiere usar 3 funciones que siguen una lógica muy parecida a la memoria compartida:

- semget (Crear/Obtener la llave).

- semctl (Darle un valor inicial a la llave, como "Baño desocupado").

- semop (Hacer la operación de bloquear o desbloquear).
