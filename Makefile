CC = gcc
CFLAGS = -Wall

OBJ = main.o usuario.o log.o serial.o

fechadura: $(OBJ)
    $(CC) $(CFLAGS) -o fechadura $(OBJ)

clean:
    rm -f *.o fechadura