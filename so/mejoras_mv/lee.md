# sección de mejora de terminal

Dentro de nuestra terminal virtual llamada
Alacrititty, se presenta un bugg visual, el cual debemos de solucionar
para que sea mas comodo escribir.

## Solución colores feos

```bash

# debemos de dirigirnos a esta siguiente ubicacion
cd ~/.config/alacritty/

# debemos de buscar el archivo llamado alacritty.yml y abrirlo

# debemos de agregar al inicio de ese archivo
# la siguiente instruccion

# OBLIGACION DE COLORES A TERMINAL GENERICA
env:
  TERM: xterm-256color

# debemos de guardar , cerrar y matar el proceso de alacritty
# posteriormente reiniciar la termina
# DEBE DE VERSE EL NUEVO COLOR.
```
