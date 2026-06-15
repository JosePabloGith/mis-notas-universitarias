/*
 *
 * En este ejemplo, creare un proceso 
 * El cual tendra 3 hijos en el mismo nivel.
 *
 * Cada hijo imprimira su PID y el PID de su padre.
 *
 * INDICACIONES:
 *         -- Cada proceso debe imprimir su PID y el PID de su padre.
 *         -- cada proceso debe de esperar 30 segundos antes de terminar, para que se pueda ver en consola.
 *         
 * */

int main() {
   
   //Primero crearemos las variables necesarias 
   //para almacenar los 3 PID de los hijos.
   
   pit_t primer_hijo, segundo_hijo, tercer_hijo;

   // remarcaremos el PID del proceso padre
   // para que se vea en consola.
   printf("Soy el proceso padre, este es mi PID: %d\n", getpid());

   // ahora crearemos a cada uno de los hijos utilizando la función fork().
   // por ceparado para que quede mas claro. Que es lo que esta pasando.
   
   primer_hijo = fork();
   if (primer_hijo < 0 ){
	   perror("Error al crear el primer hijo, falta memoria");
	   exit(1);
   }else if (primer_hijo == 0){
	   // Estamos en el bloque de memoria del primer hijo.
	   // podemos imprimir su PID y EL PPID de su padre.
	   printf("HOLA, soy el primer hijo, mi papa es %d y mi PID es %d\n",getpid(), getppid());
	   exit(0); // Salimos del proceso hijo
   }else {

	   //Estamos dentro del bloque de memoria del proceso padre
	   // Supongo que aqui se puede crear al segundo hijo.
	   
	   segundo_hijo = fork();
	   if (segundo_hijo < 0){
		   perror("Error al crear el segundo hijo, falta memoria");
		   exit(1);
	   }else if (segundo_hijo == 0){
		   // Estamos en el bloque de memoria del segundo hijo.
		   // podemos imprimir su PID y EL PPID de su padre.
		   printf("HOLA, soy el segundo hijo, mi papa es %d y mi PID es %d\n",getpid(), getppid());
		   exit(0); // Salimos del proceso hijo
	   }else {
		   //estamos dentro del bloque de memoria del proceso padre, o eso creo
		   // Supongo que aqui se puede crear al tercer hijo.
		   //
		   tercer_hijo = fork();
		   if (tercer_hijo < 0){
			   perror("Error al crear el tercer hijo, falta memoria");
			   exit(1);
		   }else if(tercer_hijo == 0){
			   // Estamos en el bloque de memoria del tercer hijo.
			   // podemos imprimir su PID y EL PPID de su padre.
			   printf("HOLA, soy el tercer hijo, mi papa es %d y mi PID es %d\n",getpid(), getppid());
			   exit(0); // Salimos del proceso hijo
		   }else{
			   // Estamos en el bloque de memoria del proceso padre, o eso creo.
			   // No se que mas hacer aqui, creo que ya se han creado los 3 hijos.
			   // Solo queda esperar a que terminen los hijos para que el proceso padre termine.
			   wait(NULL); // Esperamos a que el primer hijo termine
			   wait(NULL); // Esperamos a que el segundo hijo termine
			   wait(NULL); // Esperamos a que el tercer hijo termine
			   printf("Todos mis hijos han terminado, adios!\n");
			   exit(0); // Salimos del proceso padre
		   }

	   }

   }

}
