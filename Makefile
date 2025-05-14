# Makefile para o projeto Raylib M.A.G.

# Compilador
CC = gcc # ou clang, que é comum no macOS

# Nome do executável
EXECUTABLE = mag_game

# Diretório dos fontes
SRCDIR = src

# Arquivos fonte .c (todos os .c em SRCDIR)
SOURCES = $(wildcard $(SRCDIR)/*.c)

# Arquivos objeto .o (gerados automaticamente a partir de SOURCES)
OBJECTS = $(patsubst $(SRCDIR)/%.c,%.o,$(SOURCES))

# Linhas de depuração para verificar os valores das variáveis
$(info SRCDIR is [$(SRCDIR)])
$(info SOURCES is [$(SOURCES)])
$(info OBJECTS is [$(OBJECTS)])

# Detectar Homebrew prefixo no macOS
# Usamos := para que o shell command seja executado apenas uma vez
HOMEBREW_PREFIX := $(shell brew --prefix)

# Flags de compilação
CFLAGS = -Wall -std=c11 -O2 -g -I$(SRCDIR) -DPLATFORM_DESKTOP

# Flags de linkagem
LDFLAGS = # Default empty

# Configurações específicas do sistema operacional
UNAME_S := $(shell uname -s)

ifeq ($(UNAME_S),Linux)
    # Flags para Linux
    CFLAGS += # Adicionar caminhos de include específicos do Linux se necessário
    LDFLAGS += -lraylib -lGL -lm -lpthread -ldl -lrt -lX11
else ifeq ($(UNAME_S),Darwin)
    # Flags para macOS
    ifeq ($(strip $(HOMEBREW_PREFIX)),)
        $(error "Homebrew prefix not found. Please ensure Homebrew is installed and 'brew --prefix' works.")
    endif
    CFLAGS += -I$(HOMEBREW_PREFIX)/include
    LDFLAGS += -L$(HOMEBREW_PREFIX)/lib -lraylib -framework OpenGL -framework Cocoa -framework IOKit -framework CoreAudio -framework CoreVideo
    # Se estiver usando gcc de Homebrew, pode ser necessário adicionar $(HOMEBREW_PREFIX)/bin ao PATH ou especificar o compilador
    # Ex: CC = $(HOMEBREW_PREFIX)/bin/gcc-13 (verifique sua versão do gcc)
else
    # Flags para outros sistemas (ex: Windows com MinGW) - pode precisar de ajuste manual
    # CFLAGS += -I C:/raylib/raylib/src 
    # LDFLAGS += -L C:/raylib/raylib/src -lraylib -lopengl32 -lgdi32 -lwinmm
endif

# Alvo padrão: compila tudo
all: $(EXECUTABLE)

# Regra para linkar o executável
$(EXECUTABLE): $(OBJECTS)
	$(CC) $(OBJECTS) -o $@ $(LDFLAGS)

# Regra para compilar arquivos .c para .o
%.o: $(SRCDIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

# Alvo para limpar arquivos compilados
clean:
	rm -f $(OBJECTS) $(EXECUTABLE) ranking.txt

# Alvo para recompilar tudo (limpa e compila)
rebuild: clean all

.PHONY: all clean rebuild