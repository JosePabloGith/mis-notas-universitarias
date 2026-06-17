# Colas de mensajes 

Nececitamos algo para llenar y para extraer informacion 

## cosas mencionadas 
- clave  = ftok("/tmp", 'M'); 
/tmp solo es un "archivo en memoria " y el caracter  para calcular el hash 

- int msgget(key_t key, int msgflg);

Donde los parámetros son 

- [ ] puede regresar el id de la cola y los permisos 

Imagina que tienes un casillero "la clave de acceso al valor te lo da 
msgget 
"

- int msgsnd(int msqiq , const void *msgp, size_t msgsz , int msgflg); 

#### El struckt a mandar 


##### recibe el sms desde una cola de mensajes


