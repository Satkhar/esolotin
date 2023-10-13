#include <stdio.h>
#include <stdlib.h>

#include "my_function.h"
//#include "my_function.c"



//курсовая работа advancedd C.
//  С заданной точностью ε вычислить площадь плоской фигуры, ограниченной тремя кривыми,
//  𝑓1 = 0. 6𝑥 + 3
//  𝑓2 = (𝑥 − 2)^3 – 1
//  𝑓3 = 3 / 𝑥
//  Метод хорд (секущих)   для поиска корней                точность eps1
//  Формула Симпсона       для поиска площади(интеграла)    точность eps2


/* структура программы:
формулы задаются как функция (потом передача/вызов через функцию)

функция вычисления корня

функция вычисления интеграла(площади)

функция поиска нужной площади - из бОльшей площади вычитаем меньшие площади. (чтобы правльно было - нужно смещение, поднять ф-ии от нуля?)
*/


/*
для флагов:
-h - help, описание что умеет
-r - test, ф-ия тестирования площади на квадрате. надо задать сторону и точность
*/

int main(int argc, char *argv[])
{
    printf("Hello!\n");

    //задание переменных
    int ret=0;                  //для обработки getopt
    int root_test = 0;          //не 0 когда нужен тест ф-ии корня
    float test_square = 0.0;    //переменная для теста нахождения площади
    float eps = 0.001;          //точность вычисления по умолчанию

    float xl = -10.0;           //начало отрезка оси Х по умолчанию
    float xr = 10.0;            //конец отрезка оси Х по умолчанию



    float answerX12 = 0.0, answerX13 = 0.0, answerX23 = 0.0;    //переменные для поиска пересечения ф-ий
    float square1 = 0.0, square2 = 0.0, square3 = 0.0;          //переменные для поиска площади под функцией
    float squareBig = 0.0;                                      //переменная для наибольшей площади
    float squareAnswer = 0.0;                                   //перенная найденной площади пересечения трех ф-ий

    if ((ret = my_getopt(argc, argv, &eps, &test_square, &root_test)) != 2)
    {
        return ret;
    }

    printf("Current precision (eps) is %f\n", eps);

    //тестовые функции
    if(test_square > 0.0)       //если была введена длина для проверки площади, проверяем
    {
        printf("------ Test find square is started\n");

            //поиск площади под каждой кривой
        answerX12 = 0.0;            // первая точка - 0
        answerX13 = test_square;    // вторая - заданая длинна

        square1 = 2.0*calcIntegralTrap(answerX12,  answerX13, eps, f4); //умножаем на 2 т.к. ф-ия линейная, а площадь - квадрата

        printf("Calculated test square is = %f\n", square1);
    }

    if(root_test)       //если был флаг на проверку поиска корня - проверяем
    {
        float answer = 0.0;
        float xl_test = -10.0;           //начало отрезка оси Х по умолчанию
        float xr_test = 10.0;            //конец отрезка оси Х по умолчанию

        printf("------ Test find root is started\n");

        printf("Test function is x*x - 5.01 \n");

        printf("For test input the left and right points of the x-axis segment (xl), (xr)\n");
        scanf("%f, %f", &xl_test, &xr_test);
        if(root_test == 1)
        {
            answer = findChordRoot( xl_test,  xr_test,  eps,  f_test);
        }
        printf("Calculated test root is = %f\n", answer);
    }


    printf("------\n");
    printf("Start calculation\n\n");


        /* //кусок где искались корни функций
    answer = findChordRoot( xl,  xr,  eps,  f1);
    printf("findChordRoot 1 = %f\n", answer);

    answer = findChordRoot( xl,  xr,  eps,  f2);
    printf("findChordRoot 2 = %f\n", answer);

    answer = findChordRoot( xl,  xr,  eps,  f3);
    printf("findChordRoot 3 = %f\n", answer);

    answer = rootFindDiv2( xl,  xr,  eps,  f3);
    printf("rootFindDiv2 4 = %f\n", answer);
*/

    //поиск точек пересечения
    answerX12 = findCross( xl,  xr,  eps,  f1, f2);
    printf("function f1 and f2 cross at = %f\n\n", answerX12);

    answerX13 = findCross( xl,  xr,  eps,  f1, f3);
    printf("function f1 and f3 cross at = %f\n\n", answerX13);

    answerX23 = findCross( xl,  xr,  eps,  f2, f3);
    printf("function f2 and f3 cross at = %f\n\n", answerX23);

 //   printf("(answerX12 - answerX13)/eps = %f\n", (answerX12 - answerX13)/eps); //тут можно посмотреть количество дискретов для нахождения площади с заданной точностью

    printf("Start calculating integrals for f1, f2, f3 by Trapeze method \n");

    //поиск площади под каждой кривой
    square1 = calcIntegralTrap(answerX12,  answerX13, eps, f1);

    printf("Integral f1 = %f\n", square1);

    square2 = calcIntegralTrap(answerX12,  answerX23, eps, f2);
    printf("Integral f2 = %f\n", square2);

    square3 = calcIntegralTrap(answerX23,  answerX13, eps, f1);
    printf("Integral f3 = %f\n\n", square3);

    //поиск наибольшей площади, из которой будем вычитать меньшие.
    squareBig = my_max(&square2, &square3);
    squareBig = my_max(&squareBig, &square1);

    //printf("squareBig f3 = %f\n", squareBig);       // можно посмотреть наибольшую площадь под функцией

    //берем двойную найденную площадь и из неё вычитаем все площади (чтобы меньше сравнивать и искать какая ф-ия дала бОльшую площадь)
    squareAnswer = squareBig*2 - square1 - square2 - square3;

    printf("squareAnswer = %f\n", squareAnswer);

    return 0;
}
