all : pseudo-shell
	
	
pseudo-shell: main.o command.o string_parser.o
	gcc -g -o pseudo-shell main.o command.o string_parser.o

main.o: main.c
	gcc -g -c main.c

command.o: command.c
	gcc -g -c command.c

string_parser.o: string_parser.c string_parser.h
	gcc -g -c string_parser.c

clean:
	rm -f core *.o pseudo-shell output.txt
