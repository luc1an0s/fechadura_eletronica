# Nome do executável
TARGET = painel_admin.exe

# Compilador
CC = gcc

# Flags de compilação
CFLAGS = -Wall

# Arquivos fonte
SRCS = main.c usuario.c serial.c log.c

# Arquivos objeto
OBJS = $(SRCS:.c=.o)

# Regra principal
all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	del $(OBJS) $(TARGET)