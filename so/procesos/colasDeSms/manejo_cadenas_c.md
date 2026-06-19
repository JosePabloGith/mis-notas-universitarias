# Asesoría en Sistemas Operativos: Guía de Supervivencia de Cadenas en C

¡Tranquilo! Es súper común llegar a las prácticas de Sistemas Operativos peleando un poco con el manejo de memoria y cadenas en C. El tiempo apremia, así que vamos directo al grano para que tengas un "acordeón" o "cheat sheet" perfecto para tu práctica.

## 1. Mi predicción: ¿Arreglos o Punteros?

**Mi predicción es que usarás AMBOS, pero con roles muy definidos:**

En tu código de colas de mensajes ya definiste la estructura así:
```c
struct mensaje {
  long tipo;
  char texto[200]; // Arreglo normal (memoria estática)
};
```
* **Para almacenar (El Buzón):** Usarás **cadenas normales (arreglos estáticos)** como `texto[200]`. Esto es obligatorio porque el Kernel necesita saber *exactamente* cuántos bytes contiguos de memoria vas a enviar en el `msgsnd`. Si usaras un puntero (`char *texto`), solo enviarías la dirección de memoria (8 bytes), y el proceso receptor intentaría leer una dirección de memoria a la que no tiene acceso, causando un fallo de segmentación (Segmentation Fault).
* **Para manipular (Las Herramientas):** Usarás **punteros**. Todas las funciones internas de C (`strcpy`, `strlen`) reciben punteros (`char *`) bajo el capó. Cuando le pasas tu arreglo `msg.texto` a una función, C automáticamente lo convierte en un puntero al primer elemento.

---

## 2. La Regla de Oro del Kernel: El Terminador Nulo `\0`
En C, un string no es un tipo de dato real, es solo una fila de caracteres en la memoria. ¿Cómo sabe C dónde termina tu palabra si el arreglo mide 200 bytes y tu palabra solo 4 ("Hola")?
Lo sabe gracias al carácter nulo: `\0`. 
**Si olvidas poner el `\0` o lo sobrescribes por accidente, las funciones leerán memoria basura infinita hasta chocar y colapsar tu programa.**

---

## 3. Tu Navaja Suiza: La librería `<string.h>`

Para usar estas funciones, siempre debes incluir `#include <string.h>` al inicio de tu archivo.

### A) Copiar cadenas (`strcpy` y `strncpy`)
Nunca puedes hacer `msg.texto = "Hola";`. Debes copiar la memoria.
**Recomendación de SO:** Usa `strncpy` siempre. Es más seguro para evitar el desbordamiento de búfer (Buffer Overflow).

```c
#include <string.h>

// strcpy: Copia hasta encontrar el \0 (Peligroso si el origen es muy grande)
strcpy(msg.texto, "Alerta de sistema"); 

// strncpy: Copia con límite de seguridad (El tamaño de tu arreglo - 1)
strncpy(msg.texto, "Alerta de sistema", sizeof(msg.texto) - 1);
msg.texto[sizeof(msg.texto) - 1] = '\0'; // Aseguramos el terminador por seguridad
```

### B) Medir la longitud (`strlen`)
Devuelve la cantidad de caracteres *antes* del `\0`. No te da el tamaño del arreglo total, solo del texto útil.

```c
int longitud = strlen(msg.texto);
printf("El mensaje mide %d caracteres\n", longitud);
```

### C) Comparar cadenas (`strcmp`)
No puedes usar `if (texto1 == texto2)`. Eso compara direcciones de memoria, no el texto.
`strcmp` devuelve `0` si son EXACTAMENTE iguales.

```c
if (strcmp(msg.texto, "SALIR") == 0) {
    printf("El usuario solicitó apagar el proceso.\n");
}
```

### D) Concatenar / Unir cadenas (`strcat`)
Pega una cadena al final de otra.

```c
char saludo[200] = "Hola ";
strcat(saludo, "Proceso Receptor"); 
// saludo ahora contiene "Hola Proceso Receptor"
```

### E) Extraer palabras o separar por tokens (`strtok`)
**¡Muy útil para IPC!** Si mandas un mensaje como `"ALERTA:50:SOBRECALENTAMIENTO"`, puedes extraer las partes separándolas por los dos puntos (`:`).

```c
char datos[] = "ALERTA:50:SOBRECALENTAMIENTO";
char *token = strtok(datos, ":");

while (token != NULL) {
    printf("Dato extraído: %s\n", token);
    token = strtok(NULL, ":"); // Continuar con la misma cadena
}
```

---

## 4. Función Personalizada: Revertir una Cadena (Reverse)
C no tiene una función nativa para voltear un string en `<string.h>`. Como me lo pediste, aquí tienes una implementación clásica y optimizada usando punteros que puedes copiar y pegar en tu práctica:

```c
#include <stdio.h>
#include <string.h>

// Función para invertir una cadena en su propio espacio de memoria
void invertir_cadena(char *cadena) {
    int longitud = strlen(cadena);
    int inicio = 0;
    int fin = longitud - 1;
    char temporal;

    while (inicio < fin) {
        // Intercambiamos el carácter de la izquierda con el de la derecha
        temporal = cadena[inicio];
        cadena[inicio] = cadena[fin];
        cadena[fin] = temporal;

        // Movemos los índices hacia el centro
        inicio++;
        fin--;
    }
}

// Ejemplo de uso:
/*
int main() {
    struct mensaje msg;
    strcpy(msg.texto, "Sistemas Operativos");
    
    printf("Original: %s\n", msg.texto);
    invertir_cadena(msg.texto);
    printf("Invertido: %s\n", msg.texto);
    
    return 0;
}
*/
```


## 1. Construcción Segura de Cadenas (`<stdio.h>`)

Cuando necesites enviar un mensaje IPC que contenga texto y variables (como números), no puedes usar `strcat` fácilmente. Aquí entra tu mejor amigo: `snprintf`.

### `snprintf` (String Print Formatted - Versión Segura)
A diferencia de `printf` que imprime en la pantalla, `snprintf` "imprime" dentro de tu arreglo de caracteres, uniendo texto y variables, y asegurándose de **no desbordar** la memoria.

```c
#include <stdio.h>

struct mensaje msg;
int sensor_id = 42;
float temperatura = 36.5;

// Empaquetamos todo en un solo string de forma segura
snprintf(msg.texto, sizeof(msg.texto), "SENSOR:%d:TEMP:%.2f", sensor_id, temperatura);

// msg.texto ahora contiene: "SENSOR:42:TEMP:36.50"
```

---

## 2. Conversión: De Texto a Números (`<stdlib.h>`)

Si tu proceso receptor recibe el string `"150"` a través de `msgrcv()`, la computadora lo ve como texto, no como un número para hacer matemáticas. Necesitas convertirlo.

### `atoi` (ASCII to Integer) y `atof` (ASCII to Float)
Convierten una cadena a un tipo de dato numérico.

```c
#include <stdlib.h>

char edad_str[] = "25";
int edad = atoi(edad_str); 
// edad ahora es el entero 25. Ya puedes hacer: edad + 5;
```

*Nota de SO:* Si necesitas una conversión a prueba de balas que detecte errores, investiga `strtol` (String to Long), que es el estándar recomendado en el Kernel de Linux para evitar fallos si el usuario ingresa letras en lugar de números.

---

## 3. Búsqueda y Navegación (`<string.h>`)

A veces necesitas buscar palabras clave dentro de los mensajes que te llegan.

### `strstr` (String sub-String)
Busca una palabra (subcadena) dentro de un texto grande. Devuelve un puntero a la posición donde comienza la palabra, o `NULL` si no la encuentra.

```c
#include <string.h>

char log_msj[] = "2026-04-10 [ERROR] Falla de segmentacion en CPU 1";

// Buscamos si el string contiene la palabra "[ERROR]"
if (strstr(log_msj, "[ERROR]") != NULL) {
    printf("¡Alerta! Se detectó un error crítico en los logs.\n");
}
```

### `strchr` (String Character)
Busca la primera aparición de un **carácter** específico en una cadena.

```c
char correo[] = "pablo@universidad.edu";
char *arroba = strchr(correo, '@');

if (arroba != NULL) {
    printf("El dominio es: %s\n", arroba + 1); // Imprime: universidad.edu
}
```

---

## 4. Manipulación de Mayúsculas/Minúsculas (`<ctype.h>`)

Muy útil para estandarizar comandos. Si tu proceso espera el comando `"SALIR"`, pero el usuario escribe `"salir"`, fallará a menos que lo conviertas.

### `toupper` y `tolower`
Estas funciones operan letra por letra, por lo que necesitas un algoritmo (bucle) para convertir toda la cadena.

```c
#include <ctype.h>
#include <string.h>

void convertir_a_mayusculas(char *cadena) {
    for (int i = 0; cadena[i] != '\0'; i++) {
        cadena[i] = toupper(cadena[i]);
    }
}
```

---

## 5. Algoritmos de Supervivencia (Snippets)

Estos son pedazos de código que te salvarán la vida en tus prácticas:

### A) Eliminar el molesto salto de línea (`\n`)
Cuando usas `fgets` o lees datos por teclado, C suele capturar el salto de línea al dar "Enter" (guardando `"Hola\n"`). Esto rompe las comparaciones (`strcmp`).
**El truco de `strcspn`:**

```c
#include <string.h>

char entrada[50] = "Comando123\n";
// Busca el \n y lo reemplaza por \0 (fin de cadena)
entrada[strcspn(entrada, "\n")] = '\0'; 
// entrada ahora es "Comando123" puro.
```

### B) Validar cómo "comienza" una cadena (Prefijos)
A veces en una cola de mensajes te llegan datos variados y quieres saber si el mensaje empieza con una palabra clave, sin importar qué siga después. Usamos `strncmp` (comparación con límite de caracteres).

```c
char comando[] = "KILL:1045"; // Queremos matar el proceso 1045

// Comparamos solo los primeros 5 caracteres
if (strncmp(comando, "KILL:", 5) == 0) {
    printf("Ejecutando rutina de terminación de proceso...\n");
}
```

# Asesoría en Sistemas Operativos: Predicciones de Laboratorio (Colas de Mensajes)

¡Qué excelente visión estratégica! Tratar de anticipar lo que el profesor te va a pedir basándote en la bibliografía oficial es una de las mejores tácticas para sobrevivir a Sistemas Operativos.

He revisado los libros base que mencionas en tu entorno de estudio, específicamente **"The Linux Programming Interface"** (Michael Kerrisk), **"Advanced Programming in the UNIX Environment"** (Stevens & Rago) y **"Beginning Linux Programming"** (Matthew & Stones). 

En el capítulo de *System V Message Queues* de estos libros, los autores siempre proponen 3 arquitecturas clásicas de ejercicios. Casi te puedo garantizar que tu profesor elegirá una variante de alguna de estas tres:

---

## Predicción 1: El Sistema de Chat Bidireccional
*(Basado en "Beginning Linux Programming" - Capítulo de IPC)*

**¿De qué trata?**
Se te pide crear dos programas (`usuarioA.c` y `usuarioB.c`) que simulen un chat como WhatsApp por la terminal, pero usando **una sola cola de mensajes** compartida.

**El truco a resolver (Lo que evalúa el profesor):**
Si ambos usan la misma cola, ¿cómo evitas que `usuarioA` lea por accidente el mensaje que él mismo acaba de enviar? 
* **La solución clásica:** Jugar con el `msgtyp`. Se acuerda que `usuarioA` siempre enviará mensajes con `tipo = 1` y recibirá mensajes con `tipo = 2`. Por su parte, `usuarioB` enviará mensajes con `tipo = 2` y solo leerá los de `tipo = 1`. 

---

## Predicción 2: Arquitectura Cliente-Servidor Multiusuario
*(Basado en "The Linux Programming Interface" - Cap. 46)*

**¿De qué trata?**
Construir un programa `servidor.c` que corra en segundo plano y varios programas `cliente.c`. Los clientes envían "solicitudes" al servidor (por ejemplo, "conviérteme este texto a mayúsculas" o "haz esta suma matemática"), y el servidor les devuelve el resultado a cada uno.

**El truco a resolver (Lo que evalúa el profesor):**
Si hay 5 clientes enviando datos a la misma cola, cuando el servidor responda... ¿cómo sabe el Cliente 3 cuál es su respuesta exacta y no agarrar la del Cliente 1?
* **La solución clásica:** El cliente empaqueta su mensaje poniendo como `tipo` de envío un número general (ej. `1` para el servidor). Dentro de los datos (`msg.texto`), el cliente manda su número de proceso (PID). Cuando el servidor procesa la respuesta, la mete a la cola usando como `tipo` el PID del cliente. Así, el Cliente 3 hace un `msgrcv(..., PID_Cliente3, ...)`.

---

## Predicción 3: Prioridad de Tareas (El "Hospital" o "Planificador")
*(Basado en "Advanced Programming in the UNIX Environment" - Cap. 15)*

**¿De qué trata?**
Se simula un sistema de procesamiento de alertas donde llegan mensajes de distintas urgencias. 
* Tipo 1: Alerta Crítica (Fuego en el servidor).
* Tipo 2: Alerta Media (Memoria casi llena).
* Tipo 3: Registro Normal (Usuario hizo login).

**El truco a resolver (Lo que evalúa el profesor):**
El receptor debe procesar **siempre** las alertas críticas primero, incluso si llegaron después de las normales.
* **La solución clásica:** Aprovechar la función especial del parámetro `msgtyp` en valores negativos. Si el receptor hace `msgrcv(id_cola, &msg, tamaño, -3, 0);`, el Kernel automáticamente buscará y extraerá el mensaje con el tipo más bajo (urgente) que sea menor o igual a 3.

---

### Preparándote para el reto (Pregunta de Validación)

Dado que te preocupa el manejo de cadenas, imagina que tu profesor te asigna la **Predicción 2 (Cliente-Servidor)** para hacer una "Calculadora Remota". 

El cliente necesita enviarle al servidor un mensaje que contenga el tipo de operación ("SUMA") y los dos números (ej. `5` y `8`). 
Considerando lo que aprendiste en nuestro glosario de cadenas sobre la función `snprintf`... 

**¿Cómo escribirías la línea de código con `snprintf` para empaquetar la palabra "SUMA", el número `5` y el número `8` en un solo `msg.texto` de forma que el servidor pueda separarlo fácilmente después?** (Piensa en un formato que use separadores, como dos puntos `:`).

¡Escribe tu idea de formato y te diré si es a prueba de balas para el laboratorio!


