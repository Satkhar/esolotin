#include <stdio.h>
#include <math.h>

#include "my_function.h"

//формула 1         𝑓1 = 0. 6𝑥 + 3
float f1(float x)
{
    return 0.6*x + 3;
}

//формула 2         𝑓2 = (𝑥 − 2)^3 – 1          ==  (x - 2)*( x*x - 4*x + 4 ) - 1 == x*x*x - 4*x*x + 4*x - 2*x*x + 8*x - 8 + 4 == x*x*x - 6*x*x + 12*x - 5
float f2(float x)
{
    return x*x*x - 6*x*x + 12*x - 5;
}

//формула 3         𝑓3 = 3 / 𝑥
float f3(float x)
{
    return 3.0/x;
}

//формула 4         𝑓4 = x // прямая
float f4(float x)
{
    return x;
}

float f_test(float x)
{
    return x*x - 5.01;
}

//функция проверки знака
int signF(float val)
{
    return val > 0 ? 1 : -1;
}

//функция сравнения f(x)
int signFunc(float val, my_function f1, my_function f2)
{
    return f1(val) > f2(val) ? 1 : -1;
}

//функия нахождения корня
float findChordRoot(float xl, float xr, float eps, my_function f)
{
    int stepcount=0;

    while(fabs(xr - xl) > eps)
        {

        xl = xr - (xr - xl) * f(xr) / (f(xr) - f(xl));
        xr = xl - (xl - xr) * f(xl) / (f(xl) - f(xr));
        stepcount++;
        }
 //   printf("\nxl  %f  xr  %f\n",xl , xr);
    printf("\nFind root for %d steps\n", stepcount);

    return xr;
}


//ещё одна функция нахождения корня (не используется)
float rootFindDiv2(float xl, float xr, float eps, my_function f)
{
    int stepcount=0; //число шагов
    float xm;
    while(fabs(xr-xl)>eps)
        { //вещественный модуль разницы
        stepcount++;
        xm=(xl+xr)/2; // середина отрезка
        if(f(xr)==0)
        { // нашли решение на правой границе
            printf("Find root for %d steps\n",stepcount);
            return xr;
        }
        if(f(xl)==0)
        { // нашли решение на левой границе
            printf("Find root for %d steps\n",stepcount);
            return xl;
        }
        if(signF(xl) != signF(xm)) //если знак отличается
            xr=xm;
        else
            xl=xm;
        }
    printf("\nFind root for %d steps\n",stepcount); //статистика
    return (xl+xr)/2;
}


//функция нахождения точки пересечения двух фунций
float findCross(float xl, float xr, float eps, my_function func1, my_function func2)
{
    int stepcount=0; //число шагов
    float xm;
    while(fabs(xr-xl)>eps)
        { //вещественный модуль разницы
        stepcount++;
        xm=(xl+xr)/2; // середина отрезка
        if(func1(xr)==func2(xr))
        { // нашли решение на правой границе
            printf("Find for %d steps\n",stepcount);
            return xr;
        }
        if(func1(xl)==func2(xl))
        { // нашли решение на левой границе
            printf("Find for %d steps\n",stepcount);
            return xl;
        }

        if(signFunc(xl, func1, func2) != signFunc(xm, func1, func2)) //смотрим пересечение
            xr=xm;
        else
            xl=xm;
        }
    printf("Find for %d steps\n",stepcount); // за сколько шагов нашли
    return (xl+xr)/2;
}

//функция вычисления интеграла методом трапеций
float calcIntegralTrap(float xl, float xr, float eps, my_function f)
{
    //если вдруг диапазоны перепутаны
    if(xl > xr)
    {
        float temp = xl;
        xl = xr;
        xr = temp;
    }

    int64_t count=0;

    float sum = 0;
//    float h = (xr-xl)/n;
    float h = (xr-xl)*eps;

//    printf("\n step is %f \n",h); //статистика

    for(float x=xl+h; x<xr-h; x+=h)
    {
        sum += 0.5*h*(f(x)+f(x+h));
        count++;
    }
    printf("\n Find for %d steps \n", count); //за сколько нашли площадь

    return sum;
}

//функция нахождения большего числа
float my_max(float *a, float *b)
{
    return *a > *b ? *a : *b;
}

//int my_getopt(int argc, char *argv[], char filename[], int *target_month)



//обработчик флагов
int my_getopt(int argc, char *argv[], float *in_eps, float *test_square, int *root_test)
{
    int rez=0;
    while ( (rez = getopt(argc,argv,"hs:re:")) != -1){
        switch (rez){
            case 'h':
                printf("This function need to search square from f1, f2, f3\n");

                printf("Where f1, f2, f3:\n\
                       f1 = 0.6x + 3;\n\
                       f2 = (x - 2)^3 - 1;\n\
                       f3 = 3 / x\n");

                printf("Usage: [options]\n\
                -h This help text\n\
                -s <len> Test function find square for square with <len> length. Float positive values. \n\
                -r Start test function find root by Chord method.\n\
                -e <eps> Input float positive value of precision. Standart usage precision is 0.001\n\
                \n");
                printf("Example: %s -t 2.0 0.001 \n",argv[0]);
                return 0;

            case 'e':
                if((optarg) > 0)
                {
                    printf("Input precision is %s\n",optarg);
                    *in_eps = atof(optarg);
                    break;
                }
                else
                {
                    printf("Bad precision, need more 0: %s Try -h for help\n", argv[optind-1]);
                    return 1;
                }

                break;

            case 's': //
                // printf("optind is %s\n",argv[optind-1]);

                /*  //кусок для ввода нескольких параметров. если в
                optind--;       //или так, или optind-1//
                for( ;optind < argc && *argv[optind] != '-'; optind++){
                    //  DoSomething( argv[optind] );
                    printf("next optind is %s\n",argv[optind]);
                }
                */

                if(atof(optarg) > 0)
                {
                    printf("Test find square is choosen.\n");
                    printf("Test length is %s\n",argv[optind-1]);
                    *test_square = atof(optarg);
                    break;
                }
                else
                {
                    printf("Bad length, need more 0: %s Try -h for help\n", argv[optind-1]);
                    return 1;
                }
                break;
            case 'r':

                printf("Test find root by Chord method is choosen.\n");
                *root_test = 1;
                break;
            case '?':
                printf("Unknown argument: %s Try -h for help\n", argv[optind-1]);
                return 1;
            default:
                printf("Unknown argument: %s Try -h for help\n", argv[optind-1]);
                return 1;
        };
    };
    return 2;
}
