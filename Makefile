# Variables
CC = gcc
CFLAGS = -Iinclude -I/usr/local/include -I/usr/include/cjson -Wall -Wextra
LDFLAGS = -L/usr/local/lib -L/lib/x86_64-linux-gnu -lpromhttp -lprom -lcjson

SRC_FOLDER = ./src
OBJ_FOLDER = ./obj
INCLUDE_FOLDER = ./include

# Archivos fuente
SOURCES = $(SRC_FOLDER)/main.c \
           $(SRC_FOLDER)/metrics.c \
		   $(SRC_FOLDER)/getinfojson.c \
           $(SRC_FOLDER)/expose_metrics.c

# Archivos objeto
OBJ_FILES = $(patsubst $(SRC_FOLDER)/%.c,$(OBJ_FOLDER)/%.o,$(SOURCES))

# Nombre del ejecutable
EXECUTABLE = metrics

# Regla por defecto
all: $(EXECUTABLE)

# Regla para crear el ejecutable
$(EXECUTABLE): $(OBJ_FILES)
	$(CC) $(OBJ_FILES) -o $@ $(LDFLAGS)

# Regla para compilar los archivos fuente en archivos objeto
$(OBJ_FOLDER)/%.o: $(SRC_FOLDER)/%.c
	@mkdir -p $(OBJ_FOLDER)  # Crea el directorio de objetos si no existe
	$(CC) $(CFLAGS) -c $< -o $@

# Limpiar archivos generados
clean:
	rm -rf $(OBJ_FOLDER) $(EXECUTABLE)

# Reglas especiales
.PHONY: all clean
