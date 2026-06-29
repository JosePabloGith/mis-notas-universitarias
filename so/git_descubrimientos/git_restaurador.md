# protocolo ante fallos de sentido dentro de git

Si estamos leyendo esto, implcia que estamos ante
algun problema relacionado con ramas divergentes
del proyecto.

Si dentro de github se tiene la realidad
original y dentro de algun directorio local
tienes discrepancias garrafales.

puedes ejecutar los siguientes comandos
para restablecer la linea de sentido acorde
a github.

```sh

# Para lograr eso debemos de ejecutar lo sigueinte

git fetch --all
# Esto descarga todo lo que se tenga dentro de
# github

git reset --hard origin/main

#¿Qué hace?: Agarra tu rama local actual y la obliga a apuntar exactamente al mismo punto donde está la rama main de GitHub (origin/main). Borra tus commits locales desviados y sobreescribe los archivos modificados.

 git clean -ffxd
# ¿Qué hace?: Es el lanzallamas definitivo. El parámetro -ff fuerza el borrado, la x borra incluso los archivos ignorados (como configuraciones locales o carpetas node_modules si las hubiera), y la d borra directorios enteros que no pertenezcan al GitHub real.



```

## Paginas de consulta

<https://ohshitgit.com/>
