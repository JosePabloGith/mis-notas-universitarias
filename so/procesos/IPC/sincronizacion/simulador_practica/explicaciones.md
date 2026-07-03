# observaciones

- aumentador_simulador.c

```c
// Lógica para el trabajador (Pseudocódigo mental)

// 1. PRIMERO: ¿Hay lugar en el procesador? (Semáforo Contador)
// Si no hay lugar (valor 0), el SO dormirá a este proceso aquí.
operacion.sem_op = -1;
semop(id_sem_contador, &operacion, 1);

// 2. SEGUNDO: ¿Está libre el acceso a la memoria? (Semáforo Binario)
// Aquí es donde garantizas exclusión mutua.
operacion.sem_op = -1;
semop(id_sem_binario, &operacion, 1);

// --- ZONA CRÍTICA: Aquí ocurre el trabajo ---
for(int i=0; i<TAM; i++) arreglo[i]++;
// -------------------------------------------

// 3. TERCERO: Libero el acceso a la memoria (Binario)
operacion.sem_op = 1;
semop(id_sem_binario, &operacion, 1);

// 4. CUARTO: Libero mi lugar en el procesador (Contador)
operacion.sem_op = 1;
semop(id_sem_contador, &operacion, 1);

```

# ejecucion del contador

para ejecutar 5 veces el aumentador.c puedes usar esto:

```bash
gcc aumentador_simulacion.c -o aumentador_simulacion
for i in $(seq 1 5); do ./aumentador_simulacion & done
wait

```
