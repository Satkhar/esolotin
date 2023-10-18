#ifndef MY_FUNCTION_H_INCLUDED
#define MY_FUNCTION_H_INCLUDED

#include <inttypes.h>
#include <unistd.h>

//синоним для упрощения передачи функции формул
typedef float (*my_function)(float);


//variable
int getopt(int argc, char * const argv[],
const char *optstring);
extern char *optarg;
extern int optind, opterr, optopt;

//prototypes

float f1(float x);
float f2(float x);
float f3(float x);
float f4(float x);
float f_test(float x);

int my_getopt(int argc, char *argv[], float *in_eps, float *test_square, int *root_test);

float findChordRoot(float xl, float xr, float eps, my_function f);

float rootFindDiv2(float xl, float xr, float eps, my_function f);

int signF(float val);

float findCross(float xl, float xr, float eps, my_function func1, my_function func2);

float calcIntegralTrap(float xl, float xr, float n, my_function f);

float my_max(float *a, float *b);


#endif // MY_FUNCTION_H_INCLUDED
