CC = cc
CFLAGS = -g -Wall -fopenmp -lm
TARGET = img-steg

all : $(TARGET)

$(TARGET) : main.o functions.o
	$(CC) $(CFLAGS) -o $(TARGET) main.o functions.o

main.o : main.c steg.h
	$(CC) $(CFLAGS) -c main.c

functions.o : functions.c steg.h
	$(CC) $(CFLAGS) -c functions.c

clean :
	$(RM) $(TARGET) main.o functions.o