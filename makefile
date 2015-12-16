FILES = server.c libreria/*.c
INPUT = server.c libreria/*.c
FLAGS = -pthread
OUTPUT = server

#gcc -o server -pthread server.c libreria/*.c

server:
	gcc -o $(OUTPUT) $(FLAGS) $(INPUT)

clean:
	rm -f server