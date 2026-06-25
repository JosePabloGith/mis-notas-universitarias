# Guia de uso de git

En este documento se redactarán las practicas mas usuales
en el manejo de git.

## Triangulo dorado

Es recomendado realizar siempre el siguiente combo
de comandos en caso de que se manejen multiples
ramas de un mismo proyecto.

Con el fin de evitar cambios abruptos:

1. `git fetch`: Descarga los metadatos de GitHub
   (se entera de qué hizo tu otra máquina).

2. `git status`: Te avisa si estás detrás, adelante o si todo está limpio.

3. `git pull`: Trae e integra físicamente los archivos a tu carpeta.

# Casos de errores potenciales

Si dentro de tu proyecto principal `protegido por git` cometiste
algún error grabe desde el ultimo avance, podrás involucionar
a al commit anterior, eliminando todas las fallas
que se hayan cometido "es por ello que se deben de hacer
commits por cada cambio minucioso" de modo
que se pueda involucionar de forma segura:

- git reset --soft HEAD~1

# git branch ?

Una branch (rama) es, en esencia, una línea de trabajo separada
dentro del mismo proyecto.

Piensa en tu proyecto como si fuera un cuaderno.
La rama principal (main o a veces master) sería el cuaderno
“oficial”.
Una branch sería como sacar una copia de ese cuaderno en el
estado actual para hacer cambios sin tocar el original.

---

### <font color="orange">Idea intuitiva </font>

Imagina que tienes un proyecto llamado tienda-web.

En este momento, tu rama principal main tiene esto:

- página de inicio
- formulario de login
- menú principal

### <font color = "orange">Ahora quieres agregar una nueva función: modo oscuro.</font>

> :warning: Si haces esos cambios directamente en main y te equivocas, puedes romper la versión principal del proyecto.

Entonces haces una rama aparte:

- git branch modo-oscuro

Esa rama sería como decir:

- “Voy a abrir una línea de trabajo separada para probar el modo
  oscuro sin ensuciar o arriesgar la rama principal”.

- Entonces, ¿a qué se refiere exactamente “rama”?

Se refiere a una versión paralela del historial del proyecto.
No es otra carpeta completamente distinta ni otro proyecto separado.
Es el mismo repositorio, pero con una línea de desarrollo distinta.

En otras palabras:

> :memo: main = la línea principal

> :memo: modo-oscuro = una línea alternativa donde haces cambios nuevos

> :memo: luego, si todo sale bien, unes esa rama con main

**¿Para qué sirve usar ramas?**

`Sirve para trabajar sin dañar
la versión principal.`

### Casos típicos

1. `Agregar una nueva función`

Ejemplo:

- agregar modo oscuro
- agregar un formulario nuevo
- agregar un buscador

1. `Arreglar un error`

Ejemplo:

- corregir un bug en el login
- corregir un cálculo mal hecho
- arreglar un botón que no funciona

1. `Probar algo sin comprometer el proyecto principal`

- Ejemplo:
  Cambiar el diseño completo de
  una página
- probar otra librería
- reorganizar archivos

1. `Trabajar varias cosas al mismo tiempo`

Por ejemplo:

- rama login-nuevo
- rama modo-oscuro
- rama correccion-menu

Cada una tiene su trabajo separado.

Ejemplo concreto para visualizarlo

Supón que tu proyecto está así en main:

```none

main
 └── Proyecto funcionando con:
      - inicio
      - login
      - menú

```

Ahora quieres hacer una calculadora dentro de la página.

1. Creas una rama:

2. git branch calculadora

3. Ahora el proyecto conceptualmente queda así:

```none

main ---------> versión estable del proyecto
 \
  \---- calculadora ---------> aquí haces cambios nuevos


```

En la rama calculadora puedes:

- crear archivos nuevos
- modificar HTML, CSS, JS
- cometer errores
- borrar cosas
- rehacer partes

> :bulb: **Tip:** todo esto sin tocar la rama principal.

### ¿Qué pasa si no usas ramas?

Si trabajas siempre en main:

mezclas código estable con código en pruebas
si algo sale mal, ensucias la rama principal
si trabajas con otras personas, todos chocan entre sí más fácilmente
es más difícil saber qué cambio pertenece a qué tarea.

> :warning: Algo muy importante: crear una rama no te cambia automáticamente a ella

Si haces:

- git branch calculadora

- [x] solo la crea.

`Para moverte a esa rama necesitas cambiarte a ella`.

Por ejemplo:

- git switch calculadora

o con el comando tradicional:

- git checkout calculadora

Ejemplo real paso a paso

Supón que tienes un proyecto llamado agenda.

1. Estás en la rama principal
   git branch

Podría mostrarte algo así:

- `*main`

El asterisco indica en qué rama estás.

1. Quieres agregar un módulo de contactos favoritos

- Entonces creas una rama:

- git branch favoritos

1. Te cambias a esa rama

- git switch favoritos

Ahora ya estás trabajando en favoritos.

1. Haces cambios

Por ejemplo:

- agregas un botón “Agregar a favoritos”

modificas el archivo contactos.js
agregas estilos nuevos 5. Guardas esos cambios con commits

Más adelante harás:

- git add .
- git commit -m "Agrega
  funcionalidad de contactos
  favoritos"

Eso queda guardado en la rama favoritos, no en main.

1. Cuando todo funciona, unes esa rama con main

Después, vuelves a main y fusionas la rama.

Eso ya es el paso de merge, pero por ahora quédate con la idea:

La rama te sirve para trabajar aparte; luego, si sale bien, incorporas esos cambios al proyecto principal.

Regla mental sencilla
Una branch es:

un espacio de trabajo separado dentro del mismo proyecto para desarrollar algo sin tocar directamente la rama principal.

Cuándo deberías usar una branch

Usa una rama casi siempre que vayas a hacer un cambio con intención propia, por ejemplo:

una nueva función
una corrección importante
una tarea separada
una prueba experimental
una refactorización

Una forma útil de pensarlo es:

¿Voy a cambiar algo importante o específico? → crea una branch.
¿Quiero mantener main limpia y estable? → crea una branch.
¿Voy a trabajar algo que podría fallar o quedar a medias? → crea una branch.
Cuándo no sería tan necesario

Si estás tú solo, en un repositorio muy pequeño, y vas a corregir una tontería mínima, podrías trabajar directo en main.
Aun así, acostumbrarte a usar ramas es una muy buena práctica.

Resumen corto

Una branch en Git es una rama de trabajo independiente dentro del mismo repositorio.

Sirve para:

desarrollar funciones nuevas
corregir errores
probar cambios
trabajar sin arriesgar la rama principal

- La idea principal es esta:

`main conserva la versión estable
tú haces cambios en otra rama
cuando todo funciona, unes esa rama con la principal`

### Comandos de brach

1. Ver en qué rama estoy y qué ramas existen

```bash

git branch

```

```none
# como se podra ver :
* main
  login-nuevo
  modo-oscuro

# recueda que * es el que indica en donde estaas
```

1. Mostrar las ramas locales de forma explicita

```none
git brach --list

```

1. mostrar ramas remotas

```none
git brach -a

# esto sirve para listar todas
# las ramas [locales , remotas]
```

```none
# se vera algo como:
* main
  dev
  remotes/origin/main
  remotes/origin/dev
```

1. Crear ramas

Para crear ramas debemos de usar lo siguiente :

```sh
git branch nombre-rama

# recueda que crearla no implica
#ser cambiado a ella, debes de
#manualmente cambiar de rama
#a la rama que creaste

usando git switch nombre-rama

# tambien se puede crear y cambiar
# a la vez

git switch -c nombre-rama


```

1. Renombrar ramas

```bash
# debes de estar parado en
# la rama vieja a renombrar
git branch -m nuevo-nombre

## si no quiere estar parado
# en el lugar, puedes renombrar
# a distancia

git branch -m nombre-rama-vieja nombre-nuevo
```

1. Eliminar ramas

```bash

# Eliminar una rama de manera segura
git branch -d nombre-rama

# ¿que hace exactamente?
# - Borra la rama local `solo si git` considera que ya fue funcionada o que no
# perderas trabajo importante

# ¿como forzar el borrado?
# considera que esto borra la rama
# aunque no se haya fuscionado
# DEBES DE TENER CUIDADO
git branch -D nombre-rama

```

1. ver la información extra de
   las ramas

```bash

git branch -v

# podrá mostrar cosas como :
* main         a1b2c3d Corrige error en login
  modo-oscuro  e4f5g6h Agrega estilos oscuros
```

- ver ramas ya fusionadas con la
  rama actual

```bash
# Esto sirve para mostrar las
# ramas que ya fueron integradas
# a la rama donde estas parado

# sirve para detectar ramas
# que ya se han integrado
# y podrias eliminar con seguridad
git branch --merged

# VER RAMAS NO fusionadas
git branch --no-merged

```

### Cómo fusionar ramas en Git

> **Idea clave:** las ramas se **crean, listan, renombran o eliminan** con `git branch`,  
> pero se **fusionan** con `git merge`.

---

### 1) ¿Qué significa "fusionar una rama"?

Fusionar una rama significa **traer los cambios de una rama hacia otra**.

Por ejemplo, si trabajaste en una rama llamada `modo-oscuro` y ya quedó bien, puedes llevar esos cambios a `main`.

---

### 2) Regla mental importante

#### La rama en la que **estás parado** es la que **recibe** los cambios

Eso significa que si quieres meter los cambios de `modo-oscuro` dentro de `main`, el proceso es:

1. cambiarte a `main`
2. ejecutar `git merge modo-oscuro`

---

### 3) Sintaxis básica

```bash
git merge nombre-rama
```
