TARGET = painel_admin.exe
CC = gcc
CFLAGS = -Wall -mwindows -fexec-charset=UTF-8 -finput-charset=UTF-8
SRCS = src/main.c src/usuario.c src/serial.c src/log.c
OBJS = $(SRCS:.c=.o)
LIBS = -lcomctl32

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	del $(OBJS) $(TARGET)
