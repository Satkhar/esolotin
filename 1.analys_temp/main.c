
//-------------------------------------------------------------------------------------------------
/// @file main.c
/// @author Евгений Солотин
/// @brief Основной файл.
/// Читает .csv файл, при некорректных данных выводит сообщение, выводит в консоль статистику.
/// Флаги: -h показывает справку. Тестовый файл для проверки - temperature_small.csv
/// @copyright Copyright (c) 2024
//-------------------------------------------------------------------------------------------------

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "temp_function.h"

//-------------------------------------------------------------------------------------------------

/// @brief главная функция 
int main(int argc, char *argv[])
{
    int ret = 0;
    int target_month = -1;
    char filename[100] = "temperature_small.csv";   //
    struct data month[12];

    //    printf("target_month is %d\n", target_month);
    //    printf("csv file %s\n", filename);

    if ((ret = my_getopt(argc, argv, filename, &target_month)) != 2)
    {
        return ret;
    }

    printf("Hello world!\n");
    printf("Now i will read csv file %s\n", filename);
    if(target_month != -1)
    {
        printf("You want statistic at %d month\n", target_month);
    }

    //чистим структуру.
    // clear_stats(month);  //  старый вариант очистки
    memset(&month, 0, sizeof(struct data)); //  вариант с memset. но предупреждение из-за double формата

    if(read_from_csv_file(filename, month) != 0)
    {
        return 1;
    }

    temp_year(target_month, month);
    return 0;
}