CC = gcc	# компилятор

CFLAGS = -Wall -I.		# все варнинги; ищем все .h в текущем каталоге.   . - обязательна
LDFLAGS = 

SRC = main.c temp_function.c

OBJ = $(SRC:.c=.o)		# делаем .о из .с

#TEMP_PATH = ./build

all: temp

#-----------------------------
# компилятор	ко всем целевый файлам (это $(OBJ)) 
temp: $(OBJ)
	$(CC) $(LDFLAGS) $^ -o $@		
	
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

#-----------------------------

#temp: main.o temp_function.o
#	$(CC) -Ltemp_functions -o temp main.o temp_function.o

#main.o: main.c temp_function.h
#	$(CC) -c -o main.o main.c 

#temp_function.o: temp_function.c
#	$(CC) -c -o temp_function.o temp_function.c

#-----------------------------

.PHONY: all temp clean

clean:
	 del -fr $(OBJ) temp
#	rm -fr *.o temp