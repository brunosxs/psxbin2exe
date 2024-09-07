IDIR =../include
CC=gcc
CFLAGS="-I$(IDIR) -static"
TARGET = psxbin2exe
PREFIX="/usr/local"
ODIR=obj
LDIR =../lib

LIBS=-lm

_DEPS = 
DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))

_OBJS = bin2exe.o 
OBJS = $(patsubst %,$(ODIR)/%,$(_OBJS))

all: $(TARGET)

directory:
	mkdir -p $(ODIR)

$(TARGET): $(OBJS)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

$(ODIR)/%.o: %.c $(DEPS) | directory
	$(CC) -c -o $@ $< $(CFLAGS)


bin2exe: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

# Install the executable to the specified directory
install: $(TARGET)
	install -d $(PREFIX)/bin
	install $(TARGET) $(PREFIX)/bin

# Uninstall the executable
uninstall:
	rm -f $(PREFIX)/bin/$(TARGET)


.PHONY: clean

clean:
	rm -f $(ODIR)/*.o *~ core $(INCDIR)/*~ 
