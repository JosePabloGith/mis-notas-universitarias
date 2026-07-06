/*
 * En este ejemplo, vamos a usar un pequeño semaforo binario
 * para simular una transaccion
 * */

/*
 * NOTE:Para crear un semáforo en System V, debemos de crear un "tablero"
 *      o conjunto de semáforos:
 *      Las librerias a usar son:
 *      #include<sys/types.h>
 *      #include<sys/ipc.h>
 *      #include<sys/sem.h>
 *
 * NOTE: estas son las funciones clave
 *      1.- key_t ftok; para generar la direccion en donde se puede encontrar
 *          la configuracion del semaforo
 *
 *      2.- creamos el tablero con segmet()
 *          int segmet(key_t llave, int num_semaforos, int sema_flags);
 *
 *      3.- el semaforo nace vacio, debemos darle vida
 *          lo inicializamos usando semctl(id_semaforo, numero_del_semaforo,
 * SETVAL, valor_inicial); nota: el numero_del_semaforo es un arreglo, por eso
 * empieza en cero.
 *
 * NOTE: 4.- Operar el semaforo con semop() el wait y el signal
 *
 *
 * NOTE: La estructura de sembuf tiene 3 campos
 *      struct sembuf operacion;
 *        operacion.sem_num = 0; // ¿a que semaforo del arreglo le aplicamos la
 * operación? operacion.sem_op = ?; // que quieres hacer "sumar o restar"
 *        operacion.sem_flg = 0; //bamderas adicionales "normalmente 0"
 *
 *      Cuando se llena el formulario, se lo pasamos a la funcion:
 *      semop(id_semaforo,&operacion,1); //ese uno es para notificar que solo
 * mandamos uno
 *
 *
 * */

#include <pthread.h>
#include <sem>
