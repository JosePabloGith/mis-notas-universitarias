# Comandos basicos de manipulacion de ventanas de ZELLIJ

En esta sección describiremos a detalle la manera en la cual
podemos usar la famosa herramienta de multiplexación de terminales llamada
Zellij, para crear y manejar ventanas de terminales dentro de esta herramienta.

## Para crear una nueva ventana de terminal en Zellij, puedes usar el siguiente comando

| Accion | Atajo de teclado | Descripcion |
| :----: | :----: | :---- |
| **Divicion Inteligente** | `Alt` + `n` | Crea un nuevo panel de terminal dividiendo el espacio de manera inteligente. |
| **Dividir hacia la derecha** | `Ctrl` + `p` -> `r` | Divide el panel actual verticalmente (crea una columna a la derecha). |
| **Dividir hacia abajo** | `Ctrl` + `p` -> `d` | Divide el panel actual horizontalmente (crea una fila debajo). |
| **Crear Cuadricula (4 recuadros)** | `Alt` + `n` *(repetido)* | Usa division inteligente de forma sucesiva o combina divisiones derecha / abajo. |

---

## 2. Navegacion y Enfoque entre paneles

| Accion | Atajo de teclado | Descripcion |
| :----: | :----: | :---- |
| **Mover enfoque (flechas)** | `Alt` + `Arrow Keys` | Cambia el enfoque al panel adyacente en la dirección de la flecha. |
| **Mover enfoque (vim)** | `Alt` + `h/j/k/l` | Cambia el enfoque al panel adyacente usando las teclas de movimiento de Vim. |
| **Pantalla completa** | `Ctrl` + `p` -> `f` | Maximiza el panel actual, ocultando los demás. Presiona de nuevo para restaurar. |

## 3. Modos de vista : Flotantes y Apilados

| Accion | Atajo de teclado | Descripcion |
| :----: | :----: | :---- |
| **Alternar panel flotante** | `Ctrl` + `p` + `w` | Convierte el panel actual en una ventana flotante o lo devuelve a su posición original. |
| **Ocultar/Mostrar Flotantes** | `Ctrl` + `p` -> `e` | Oculta o muestra todos los paneles flotantes. |
| **Rotar Diseños (layouts)** | `Alt` + `[` o `Alt` + `]` | Cambia la forma en que se distribuyen los paneles en la pantalla, alternando entre diferentes layouts predefinidos. |

## 4. Modificación y gestión de tamaños

| Acción | Atajo de teclado | Descripción |
| :----: | :----: | :---- |
| **Agrandar / Achicar panel** | `Alt` + `+` o `Alt` + `-` | Ajusta de forma rápida el tamaño general del panel actualmente activo. |
| **Modo Ajustar Tamaño** | `Ctrl` + `n` | Entra al modo dedicado para cambiar dimensiones detalladas de la terminal. |
| **Ajustar con Flechas** | `Ctrl` + `n` -> `Flechas` | Expande o contrae el panel exactamente hacia el borde de la flecha presionada. |
| **Ajustar con Vim** | `Ctrl` + `n` -> `h/j/k/l` | Modifica el tamaño de los bordes del panel usando las teclas tradicionales de Vim. |

---

## 5. Gestión de Pestañas (Tabs)

| Acción | Atajo de teclado | Descripción |
| :----: | :----: | :---- |
| **Moverse entre pestañas** | `Alt` + `[` o `Alt` + `]` | Salta directamente a la pestaña de trabajo anterior o siguiente de tu barra superior. |
| **Modo Pestaña** | `Ctrl` + `t` | Activa el modo para administrar de forma masiva los escritorios virtuales de Zellij. |
| **Nueva pestaña limpia** | `Ctrl` + `t` -> `n` | Abre un espacio de trabajo completamente vacío con una única terminal. |
| **Renombrar pestaña** | `Ctrl` + `t` -> `r` | Permite escribir un nombre personalizado para organizar mejor tus tareas de desarrollo. |
| **Sincronizar paneles** | `Ctrl` + `t` -> `s` | Todo lo que escribas en una terminal se replicará en tiempo real en todos los paneles de esa pestaña. |

---

## 6. Control de Sesiones y Segundo Plano

| Acción | Atajo de teclado | Descripción |
| :----: | :----: | :---- |
| **Modo Sesión** | `Ctrl` + `o` | Entra a la suite de control para desvincular o guardar el estado del sistema. |
| **Enviar a segundo plano** | `Ctrl` + `o` -> `d` | Desacopla (*Detach*) Zellij regresando a tu shell original, dejando tus procesos corriendo. |
| **Administrador visual** | `Ctrl` + `o` -> `w` | Despliega una interfaz gráfica interactiva para buscar y revivir otras sesiones activas. |
| **Guardar Layout actual** | `Ctrl` + `o` -> `l` | *(Novedad v0.44)* Almacena la distribución exacta de tus paneles para replicarla a futuro. |
| **Cerrar todo por completo** | `Ctrl` + `q` | Mata inmediatamente todas las pestañas, paneles y procesos activos de la sesión actual. |
