# Guía completa de tmux

> Referencia personal de atajos y conceptos — ir ampliando a medida que aprendo más.

## Índice

1. [Conceptos base](#conceptos-base)
2. [El prefix](#el-prefix)
3. [Sesiones](#sesiones)
4. [Ventanas (windows)](#ventanas-windows)
5. [Paneles (panes)](#paneles-panes)
6. [Modo copia / scroll](#modo-copia--scroll)
7. [Redimensionar paneles](#redimensionar-paneles)
8. [Comandos desde fuera de tmux](#comandos-desde-fuera-de-tmux)
9. [Configuración básica (`~/.tmux.conf`)](#configuración-básica-tmuxconf)
10. [Plugins recomendados](#plugins-recomendados)
11. [Integración con Neovim](#integración-con-neovim)
12. [Espacio para mis propios atajos](#espacio-para-mis-propios-atajos)

---

## Conceptos base

tmux organiza todo en una jerarquía de 3 niveles:

```
Sesión (session)
 └── Ventana (window)     ← como una "pestaña"
      └── Panel (pane)    ← división dentro de la ventana
```

- **Sesión**: un espacio de trabajo completo, persiste aunque cierres la terminal.
- **Ventana**: como una pestaña del navegador, dentro de una sesión.
- **Panel**: subdivisión de una ventana (splits horizontales/verticales).

---

## El prefix

Casi **todos** los atajos de tmux empiezan presionando una combinación llamada **prefix**, y luego sueltas y presionas la tecla del comando.

- **Prefix por defecto:** `Ctrl + b`
  Notación de esta guía: `prefix` significa presionar `Ctrl+b`, soltar, y luego presionar la siguiente tecla indicada.

> 💡 Mucha gente remapea el prefix a `Ctrl+a` (más cómodo, una sola mano) — lo vemos en la sección de configuración.

---

## Sesiones

Las sesiones son el nivel más alto. Puedes tener varias corriendo en paralelo aunque solo estés "viendo" una a la vez.

| Acción                            | Atajo / Comando                                         |
| --------------------------------- | ------------------------------------------------------- |
| Crear nueva sesión                | `tmux new -s nombre` (desde la terminal, fuera de tmux) |
| Listar sesiones                   | `tmux ls` (fuera de tmux) o `prefix s` (dentro)         |
| Adjuntarse a una sesión           | `tmux attach -t nombre`                                 |
| Adjuntarse a la última sesión     | `tmux attach`                                           |
| Desconectarse sin cerrar (detach) | `prefix d`                                              |
| Cambiar de sesión (menú visual)   | `prefix s`                                              |
| Renombrar sesión actual           | `prefix $`                                              |
| Matar/cerrar una sesión           | `tmux kill-session -t nombre` (fuera)                   |
| Matar el servidor tmux completo   | `tmux kill-server` (fuera, cierra TODO)                 |

**Flujo típico:**

```bash
tmux new -s proyecto      # crear y entrar a sesión "proyecto"
# ... trabajas ...
# prefix d                # te desconectas, todo sigue corriendo
tmux attach -t proyecto   # vuelves más tarde
```

---

## Ventanas (windows)

Dentro de una sesión, cada ventana es como una pestaña.

| Acción                                   | Atajo                          |
| ---------------------------------------- | ------------------------------ |
| Crear nueva ventana                      | `prefix c`                     |
| Cerrar ventana actual                    | `prefix &` (pide confirmación) |
| Ir a la siguiente ventana                | `prefix n`                     |
| Ir a la ventana anterior                 | `prefix p`                     |
| Ir a una ventana específica (por número) | `prefix [0-9]`                 |
| Listar todas las ventanas (menú visual)  | `prefix w`                     |
| Renombrar ventana actual                 | `prefix ,`                     |
| Moverse a la última ventana usada        | `prefix l`                     |

---

## Paneles (panes)

Los paneles son divisiones dentro de una ventana. Aquí es donde tmux brilla para trabajar con varias terminales a la vez.

### Crear y cerrar

| Acción                                                  | Atajo                              |
| ------------------------------------------------------- | ---------------------------------- |
| Dividir panel **verticalmente** (uno al lado del otro)  | `prefix %`                         |
| Dividir panel **horizontalmente** (uno arriba del otro) | `prefix "`                         |
| Cerrar panel actual                                     | `prefix x` (pide confirmación)     |
| Cerrar panel sin confirmar                              | `exit` o `Ctrl+d` dentro del panel |

> ⚠️ Nota mental: `%` (vertical) y `"` (horizontal) confunden al principio porque el símbolo no es intuitivo respecto al resultado visual. Con la práctica queda memorizado.

### Moverse entre paneles

| Acción                                         | Atajo                                  |
| ---------------------------------------------- | -------------------------------------- |
| Moverse al panel de la izquierda               | `prefix ←`                             |
| Moverse al panel de la derecha                 | `prefix →`                             |
| Moverse al panel de arriba                     | `prefix ↑`                             |
| Moverse al panel de abajo                      | `prefix ↓`                             |
| Ir al siguiente panel (en orden)               | `prefix o`                             |
| Ir al panel anterior usado                     | `prefix ;`                             |
| Mostrar números de panel (para saltar directo) | `prefix q` (luego presionas el número) |

### Layouts y disposición

| Acción                                             | Atajo            |
| -------------------------------------------------- | ---------------- |
| Alternar entre layouts predefinidos                | `prefix Espacio` |
| Convertir panel actual en pantalla completa (zoom) | `prefix z`       |
| Mover panel actual a su propia ventana             | `prefix !`       |
| Intercambiar panel actual con el siguiente         | `prefix Ctrl+o`  |

---

## Modo copia / scroll

Por defecto no puedes hacer scroll con el mouse/rueda como en una terminal normal — tmux tiene su propio modo.

| Acción                                              | Atajo                                       |
| --------------------------------------------------- | ------------------------------------------- |
| Entrar a modo copia (para hacer scroll/seleccionar) | `prefix [`                                  |
| Salir del modo copia                                | `q`                                         |
| Moverse (dentro del modo copia)                     | Flechas, o `h j k l` si usas keybindings vi |
| Iniciar selección (con keybindings vi)              | `Espacio`                                   |
| Copiar selección (con keybindings vi)               | `Enter`                                     |
| Pegar lo copiado                                    | `prefix ]`                                  |
| Buscar hacia atrás en el historial                  | `prefix [` luego `Ctrl+r`                   |

> 💡 Para que el modo copia use atajos estilo Vim (h/j/k/l, `v` para visual, `y` para yank) hay que activarlo en la config — lo verás en la sección de configuración más abajo.

---

## Redimensionar paneles

| Acción                            | Atajo           |
| --------------------------------- | --------------- |
| Agrandar panel hacia la izquierda | `prefix Ctrl+←` |
| Agrandar panel hacia la derecha   | `prefix Ctrl+→` |
| Agrandar panel hacia arriba       | `prefix Ctrl+↑` |
| Agrandar panel hacia abajo        | `prefix Ctrl+↓` |

> Mantener `Ctrl` después del prefix permite repetir el movimiento sin tener que presionar `prefix` cada vez, en algunas configuraciones.

---

## Comandos desde fuera de tmux

Estos los corres directo en tu terminal normal (bash), sin estar dentro de una sesión:

```bash
tmux                          # iniciar sesión sin nombre
tmux new -s nombre            # iniciar sesión con nombre
tmux ls                       # listar sesiones activas
tmux attach -t nombre         # adjuntarse a una sesión
tmux kill-session -t nombre   # matar una sesión específica
tmux kill-server              # matar TODAS las sesiones
```

---

## Configuración básica (`~/.tmux.conf`)

tmux lee su configuración de `~/.tmux.conf`. Si el archivo no existe, créalo:

```bash
nano ~/.tmux.conf
```

### Configuración de partida recomendada

```bash
# ============================================
# Prefix más cómodo: Ctrl+a en vez de Ctrl+b
# ============================================
unbind C-b
set -g prefix C-a
bind C-a send-prefix

# ============================================
# Habilitar mouse (click para cambiar de panel,
# rueda para scroll, arrastrar para redimensionar)
# ============================================
set -g mouse on

# ============================================
# Empezar numeración de ventanas/paneles en 1
# (más intuitivo que empezar en 0)
# ============================================
set -g base-index 1
setw -g pane-base-index 1

# ============================================
# Splits más intuitivos (mantienen el directorio actual)
# ============================================
bind % split-window -h -c "#{pane_current_path}"
bind '"' split-window -v -c "#{pane_current_path}"

# ============================================
# Modo copia con atajos estilo Vim
# ============================================
setw -g mode-keys vi

# ============================================
# Recargar configuración sin reiniciar tmux
# ============================================
bind r source-file ~/.tmux.conf \; display "Config recargada!"

# ============================================
# Más historial de scroll (líneas guardadas)
# ============================================
set -g history-limit 10000

# ============================================
# Colores correctos en terminales modernas
# ============================================
set -g default-terminal "tmux-256color"
set -ag terminal-overrides ",xterm-256color:RGB"
```

Después de editar, recarga con:

```bash
tmux source-file ~/.tmux.conf
```

(o usa el atajo `prefix r` si ya pusiste el bind de arriba)

---

## Plugins recomendados

tmux tiene un gestor de plugins llamado **TPM** (Tmux Plugin Manager). Para instalarlo:

```bash
git clone https://github.com/tmux-plugins/tpm ~/.tmux/plugins/tpm
```

Y agregar al final de `~/.tmux.conf`:

```bash
# Lista de plugins
set -g @plugin 'tmux-plugins/tpm'
set -g @plugin 'tmux-plugins/tmux-sensible'
set -g @plugin 'tmux-plugins/tmux-resurrect'   # guardar/restaurar sesiones
set -g @plugin 'tmux-plugins/tmux-continuum'   # auto-guardado de sesiones

# IMPORTANTE: esta línea va siempre al final del archivo
run '~/.tmux/plugins/tpm/tpm'
```

Luego, dentro de tmux: `prefix I` (mayúscula) para instalar los plugins listados.

| Plugin           | Para qué sirve                                                                                           |
| ---------------- | -------------------------------------------------------------------------------------------------------- |
| `tmux-sensible`  | Configuraciones por defecto razonables que casi todos quieren                                            |
| `tmux-resurrect` | Guarda manualmente el estado de tus sesiones (paneles, layout) para restaurar después de reiniciar el PC |
| `tmux-continuum` | Auto-guarda el estado periódicamente, sin que tengas que acordarte                                       |

---

## Integración con Neovim

Cuando uses tmux + Neovim juntos, vas a querer moverte entre paneles de tmux y splits de Neovim **con los mismos atajos** (`Ctrl+h/j/k/l`), sin tener que pensar en cuál de los dos estás usando.

Esto se logra con el plugin `vim-tmux-navigator`:

1. **En Neovim** (vía lazy.nvim, agregar a tu config de plugins):

```lua
{ "christoomey/vim-tmux-navigator" }
```

1. **En `~/.tmux.conf`**, agregar:

```bash
# Navegación fluida entre paneles de tmux y splits de nvim
is_vim="ps -o state= -o comm= -t '#{pane_tty}' \
    | grep -iqE '^[^TXZ ]+ +(\\S+\\/)?g?(view|n?vim?x?)(diff)?$'"
bind-key -n C-h if-shell "$is_vim" "send-keys C-h" "select-pane -L"
bind-key -n C-j if-shell "$is_vim" "send-keys C-j" "select-pane -D"
bind-key -n C-k if-shell "$is_vim" "send-keys C-k" "select-pane -U"
bind-key -n C-l if-shell "$is_vim" "send-keys C-l" "select-pane -R"
```

Con esto, `Ctrl+h/j/k/l` te mueve entre paneles de tmux Y entre splits de Neovim de forma transparente.

> 📝 Pendiente para mí: instalar esto y probarlo cuando retome el tema.

---

## Espacio para mis propios atajos

> Sección para ir agregando lo que vaya descubriendo o personalizando.

| Atajo | Acción | Notas |
| ----- | ------ | ----- |
|       |        |       |
|       |        |       |
|       |        |       |

---

## Cheatsheet ultra-rápido (los 10 que más voy a usar)

| Atajo         | Acción                          |
| ------------- | ------------------------------- |
| `prefix c`    | nueva ventana                   |
| `prefix %`    | split vertical                  |
| `prefix "`    | split horizontal                |
| `prefix ←↑↓→` | moverse entre paneles           |
| `prefix z`    | zoom al panel actual            |
| `prefix d`    | desconectarse (detach)          |
| `prefix [`    | entrar a modo scroll/copia      |
| `q`           | salir del modo copia            |
| `prefix x`    | cerrar panel                    |
| `tmux attach` | reconectarse a la última sesión |
