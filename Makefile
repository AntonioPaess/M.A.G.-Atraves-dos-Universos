


CC = gcc 


EXECUTABLE = mag_game


SRCDIR = src


SOURCES = $(wildcard $(SRCDIR)/*.c)


OBJECTS = $(patsubst $(SRCDIR)/%.c,%.o,$(SOURCES))


$(info SRCDIR is [$(SRCDIR)])
$(info SOURCES is [$(SOURCES)])
$(info OBJECTS is [$(OBJECTS)])



HOMEBREW_PREFIX := $(shell brew --prefix)


CFLAGS = -Wall -std=c11 -O2 -g -I$(SRCDIR) -DPLATFORM_DESKTOP


LDFLAGS = 


UNAME_S := $(shell uname -s)

ifeq ($(UNAME_S),Linux)
    
    CFLAGS += 
    LDFLAGS += -lraylib -lGL -lm -lpthread -ldl -lrt -lX11
else ifeq ($(UNAME_S),Darwin)
    
    ifeq ($(strip $(HOMEBREW_PREFIX)),)
        $(error "Homebrew prefix not found. Please ensure Homebrew is installed and 'brew --prefix' works.")
    endif
    CFLAGS += -I$(HOMEBREW_PREFIX)/include
    LDFLAGS += -L$(HOMEBREW_PREFIX)/lib -lraylib -framework OpenGL -framework Cocoa -framework IOKit -framework CoreAudio -framework CoreVideo
    
    
else
    
    
    
endif


all: $(EXECUTABLE)


$(EXECUTABLE): $(OBJECTS)
	$(CC) $(OBJECTS) -o $@ $(LDFLAGS)


%.o: $(SRCDIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@


clean:
	rm -f $(OBJECTS) $(EXECUTABLE) ranking.txt


rebuild: clean all

.PHONY: all clean rebuild