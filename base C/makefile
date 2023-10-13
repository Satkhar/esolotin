all: temp

temp: main.o temp_functions.o
	gcc -Ltemp_functions -o temp main.o temp_functions.o
	
main.o: main.c temp_functions.h
	gcc -c -o main.o main.c 

temp_functions.o: temp_functions.c
	gcc -c -o temp_functions.o temp_functions.c
