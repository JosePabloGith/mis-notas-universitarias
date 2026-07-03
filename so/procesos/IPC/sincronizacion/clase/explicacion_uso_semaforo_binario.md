# Semaforo binario SYSTEM V

A diferencia del estándar moderno donde usas funciones simples, System V te obliga a configurar el semáforo "a mano" usando estructuras de C. El ciclo se divide en 4 etapas: Obtener, Llenar, Usar y Borrar.

Ciclo de Vida de un Semáforo System V

A diferencia del estándar moderno donde usas funciones simples, System V te obliga a configurar el semáforo "a mano" usando estructuras de C. El ciclo se divide en 4 etapas: Obtener, Llenar, Usar y Borrar.

## 1. ¿Dónde se INVOCA (se crea o se pide acceso al Kernel)?

Se hace mediante la función semget().
Todos tus programas (creador, lector, escritor) deben tener esta línea. Le dice al Kernel: "Dame el ID del semáforo asociado a esta clave".

```c
int id_sem;
// Parámetros:
// 1. La clave (para que todos hablen del mismo)
// 2. Cuántos semáforos quieres (1)
// 3. Permisos y banderas (IPC_CREAT lo crea si no existe)
id_sem = semget(clave, 1, 0666 | IPC_CREAT);

```

## 2. ¿Desde dónde se LLENA (inicializa el valor a 1 disponible o 0 ocupado)?

Se hace mediante la función semctl() y la famosa union semun.
Generalmente, esto solo se hace una vez, por lo que vive en tu código `creador.c.`

Aquí es donde entra la union. Una union es un tipo de dato en C que agrupa variables, y el Kernel nos obliga a usarla para pasarle el valor inicial.

```c

// 1. Declaramos la union (A veces hay que definirla manualmente si la librería de C no la trae)

union semun {
int val; // <- Esta es la que nos importa para inicializar
struct semid_ds *buf;
unsigned short*array;
};

// 2. Creamos una variable de ese tipo y la "llenamos" con el valor 1 (Semáforo libre)
union semun opciones;
opciones.val = 1;

// 3. Le mandamos el valor al Kernel usando semctl (Control de Semáforo)
// Parámetros: ID del semáforo, Número de semáforo (0 es el primero), Comando SETVAL, el valor
semctl(id_sem, 0, SETVAL, opciones);


```

## 3. ¿Dónde se LLAMA (cómo pide y devuelve la llave)?

Se hace mediante la función `semop()` y el `struct sembuf`.
Esto vive en el escritor.c y el lector.c. Aquí es donde evitas que los procesos choquen. Para hacerlo, <mark>debes llenar un pequeño formulario llamado struct sembuf y mandárselo al Kernel.</mark>

```c
// 1. Declaras el "formulario" de operación
struct sembuf operacion;
operacion.sem_num = 0; // ¿A qué semáforo le hablamos? Al 0 (el único que hay)
operacion.sem_flg = 0; // Banderas adicionales (0 = comportamiento normal)

/*--- PARA ENTRAR A LA SECCIÓN CRÍTICA (WAIT / P) ---*/
operacion.sem_op = -1; // -1 significa "Quiero restar 1 para tomar la llave"
semop(id_sem, &operacion, 1); // El Kernel ejecuta la resta. ¡Si estaba en 0, te duerme aquí!

/*... AQUÍ MODIFICAS TUS DATOS SIN QUE NADIE TE MOLESTE ...*/
//
//
//
/*--- PARA SALIR DE LA SECCIÓN CRÍTICA (SIGNAL / V) ---*/
operacion.sem_op = 1; // 1 significa "Quiero sumar 1 para devolver la llave"
semop(id_sem, &operacion, 1); // El Kernel suma 1 y despierta al que sigue


```

## 4. ¿Dónde se BORRA?

Se hace mediante semctl() usando el comando IPC_RMID.
Esto suele ir en lector.c al final, o en un programa independiente de limpieza. Si no lo borras, el semáforo se queda a vivir en la RAM de Linux hasta que reinicies la computadora.

// Comando IPC_RMID significa "Remove ID" (Borrar)
semctl(id_sem, 0, IPC_RMID);
