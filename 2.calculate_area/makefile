.PHONY: all clean 

all: finaly_solotin

# without object files
#finaly_solotin: main.c my_function.c my_function.h
#	gcc -o finaly_solotin main.c my_function.c


finaly_solotin: main.o my_function.o
	gcc -Lmy_function -o finaly_solotin main.o my_function.o
	
main.o: main.c my_function.h
	gcc -c -o main.o main.c 
	
my_function.o: my_function.c
	gcc -c -o my_function.o my_function.c

clean:
	rm *.o 


