# Nombre del compilador
CXX = g++

# Opciones de compilación
CXXFLAGS =

# Archivo fuente y ejecutable
SRC = main.cpp
EXE = main

# Regla para compilar el programa
$(EXE): $(SRC)
	$(CXX) $(CXXFLAGS) $(SRC) -o $(EXE)

# Regla para limpiar archivos generados
clean:
	rm -f $(EXE)

# Regla para ejecutar el programa
run: $(EXE)
	./$(EXE)

# Regla por defecto
.PHONY: clean run
