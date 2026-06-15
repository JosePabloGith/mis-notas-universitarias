# --Compilador generalizado By Jose Pablo Perez De Anda

# ----Compilador y Banderas ----
CC = gcc
# -Wall y -Wextra activan advertencias (detectan fallos)
#  -Iinclude le dice al Compilador donde estan las cabeceras personalizadas .h
#  -g permite usar herramientas como gdb o valgrid en el futuro
CFLAGS = -Wall -Wextra -g -Iinclude -pthread

# estructura de los directorios 
SRC_DIR = src
INC_DIR = include
BIN_DIR = bin
# añadimos obj/ para guardar los binarios intermedios de forma  limpia
OBJ_DIR = obj

# deteccion dinamica de archivos a compilar
# Encontrara de manera dinamica la direccion de los archivos
SOURCES = $(wildcard $(SRC_DIR)/*.c)
# traduce los nombres .c a .o para saber que tiene que compilar

OBJECTS = $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SOURCES))

# nombre del binario final (DEBEMOS DE COLOCAR SIEMPRE EL NOMBRE DESEADO)
TARGET = $(BIN_DIR)/ejemplo_2

# --- REGLAS Principales

# 1.- make solito, con la idea de solo construir el proyecto
all: $(TARGET)

# regla para enlazar el programa final
$(TARGET): $(OBJECTS) | $(BIN_DIR)
	$(CC) $(CFLAGS) $^ -o $@
	@echo "Compilacion terminada. Ejecutable listo"

# regla de conversion  en caso de que no haya carpetas 
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Regla para crear las carpetas dinamicamente sino existen
$(BIN_DIR) $(OBJ_DIR):
	mkdir -p $@
run: all
	@echo "______Iniciando ejecucion______\n"
	@./$(TARGET)
	@echo "\n _______ ejecucion finalizada_____\n"

# make clean
clean:
	@echo "Limpiando binarios y objetos ..."
	rm -rf $(OBJ_DIR) $(TARGET)

# le indicamos a make que estas palabras son comandos
.PHONY: all run clean
