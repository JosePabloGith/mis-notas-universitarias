#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>

typedef struct {
	int id_del_escritor;
	int numeros[5];
} CajaCompartida;

void sumarElementos (CajaCompartida *unaCaja){
    int elementos = sizeof(unaCaja->numeros);
    int sumatoria = 0;
    
    for (int i = 0; i < elementos; i++) {
        sumatoria += unaCaja->numeros[i];
    }
    
    printf("La sumatoria de todos los elementos es %d.\n", sumatoria);
}

void conseguirMayor (CajaCompartida *unaCaja){
    int elementos = sizeof(unaCaja->numeros);
    int mayor = 0;
    
    for (int i = 0; i < elementos; i++) {
        if(unaCaja->numeros[i] >= mayor)
            mayor = unaCaja->numeros[i];
    }
    
    printf("El mayor es el elemento=%d.\n", mayor);
}


int main()
{
	CajaCompartida *caja;

	//crear llave
	key_t llave = ftok("/tmp", 'K');
	if (llave == -1) {
		perror("Error al crear la clave para la memoria compartida");
		exit(1);
	}

	//acceder a memoria compartida
	int ticket_id_memory = shmget(llave, sizeof(CajaCompartida), 0666);
	if (ticket_id_memory == -1) {
		perror("Error al acceder a la memoria compartida");
		exit(1);
	}

	//conectar a memoria compartida
	caja = (CajaCompartida *)shmat(ticket_id_memory, NULL, 0);
	if (caja == (void *)-1) {
		perror("Error al conectar a la memoria compartida");
		exit(1);
	}

    //sumar elementos:
    sumarElementos(caja);
    
    //mostrar elemento mayor:
    conseguirMayor(caja);
    
    //desadjuntar arreglo:
    printf("\n");
    if (shmdt(caja) == -1) {
        perror("Error al desadjuntar el segmento de memoria");
    }
    
    //eliminar el segmento de memoria
    //shmctl(id_memoria, IPC_RMID, NULL);
    
    return 0;
}
