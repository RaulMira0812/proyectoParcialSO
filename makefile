FILES = server.c libreria/*.c libreria_h/*.h
INPUT = server.c libreria/*.c
FLAGS = -pthread
OUTPUT = server

server:$(FILES)
	gcc -o $(OUTPUT) $(FLAGS) $(INPUT)

clean:
	rm -f server