
//-------------------------------------------------------------------------------------------------
/// @file temp_function.c
/// @author Евгений Солотин
/// @brief Файл с используемыми функциями.
/// @copyright Copyright (c) 2024
//-------------------------------------------------------------------------------------------------

#include <stdio.h>
#include "temp_function.h"
#include <stdlib.h>
#include <string.h>

//-------------------------------------------------------------------------------------------------

/// @brief функция чтения csv файла.
/// @param filename - указатель на файл
/// @param my_stat - структура, что заполняется из считываемого файла
/// @return возвращает 0 если нет ошибок
int read_from_csv_file(char *filename, struct data *my_stat)
{
    FILE *temp_file;
    int year, month, day, hour, minute, temp;
    char ch='.';
    char my_buff[23] = {0};

    if(filename == NULL)
        return 1;

    temp_file = fopen(filename, "r");      // если возвращает 0 не видит файл

    // читаем максимум 21 символ до символа новой строки. s - ожидаем пробел/табуляцию после считывания строки
    for(int k = 1; -1 != fscanf(temp_file, "%21[^\n]s", my_buff); k++)
    {
        do
        {
            ch = fgetc(temp_file);
 //           printf("%d ch is - %c", k, ch);
        }
        while((EOF != ch) && ('\n' != ch));     // возвращает char, сравнивает с EOF или с новой строкой

        if(my_buff[0] < '0' || my_buff[0] > '9')
        {
            printf("Error! Bad file. Must contain numeral values\n");
            return 1;
        }

        //  смотрим, что считалось нужное количество параметров иначе выводим предупреждение
        if(sscanf(my_buff, "%d;%d;%d;%d;%d;%d", &year, &month, &day, &hour, &minute, &temp) == 6)
        {
            add_statistics(my_stat, year, month, day, temp);
        }
        else
        {
            printf("    Warning!\n--------"
                   "Bad data in %d string at file: %s\n", k, filename);
        }
    }
    printf("\n");
    fclose(temp_file);
    return 0;
}

//-------------------------------------------------------------------------------------------------

/// @brief функция добавления считанного значения в целевую структуру
/// @param my_stat - указатель на целевую структуру
/// @param year - записываемый параметр "год"
/// @param month - записываемый параметр "месяц"
/// @param day - записываемый параметр "день"
/// @param temp  - записываемый параметр "температура"
void add_statistics(struct data *my_stat, int year, int month, int day, int temp)
{
    double temp_for_mean = 0.0;
    month = month - 1;  //для правильной записи
    day = day - 1;

    //если данные этого месяца пустые - вписываем год и начальные значения температуры
    //нужно, чтобы не было косяков, если год не янв-янв
    if(my_stat[month].cnt == 0)
    {
        my_stat[month].year = year;
        my_stat[month].temp_max = temp;
        my_stat[month].temp_min = temp;
        my_stat[month].temp_mean = temp;
    }
    else if(my_stat[month].year != year)
    {
        return; //тут добавить ошибку др года
    }

    my_stat[month].day[day] += temp;        //сумма двевной температуры

    if(my_stat[month].temp_max < temp)
    {
        my_stat[month].temp_max = temp;
    }
    if(my_stat[month].temp_min > temp)
    {
        my_stat[month].temp_min = temp;
    }

    my_stat[month].cnt++;

    for(int i = 0; i <= day; i++)
    {
        temp_for_mean += my_stat[month].day[i]*1.0; //сумма температуры за месяц
    }

    my_stat[month].temp_mean = (double)(temp_for_mean/my_stat[month].cnt);    //средняя температура за месяц. пересчитываем с каждым новым значением
}

//-------------------------------------------------------------------------------------------------

/// @brief старый код по очистке структуры. оставлю как память.
/// @param target 
void clear_stats(struct data *target)
{
    for(int i = 0; i < 12; i++)
    {
        for(int k = 0 ; k < 31; k++)
        {
            target[i].day[k] = 0;
        }
        target[i].temp_mean = 0;
        target[i].temp_max = 0;
        target[i].temp_min = 0;
        target[i].cnt = 0;
        target[i].year = 0;
    }
    return;
}

//-------------------------------------------------------------------------------------------------

/// @brief функция вывода статистики, считанной из структуры
/// @param target_month - значение интересующего месяца. при -1 водится статистика за 12 месяцев
/// @param my_stat - указатель на структуру с данными
void temp_year(int target_month, struct data *my_stat)
{
    double temp_mean;
    int min_year = 0;
    int max_year = 0;
    int cnt = 0;
    
    (void)min_year;
    (void)max_year;
    (void)cnt;

    if(target_month == -1)
    {
        for(int i = 0; i < 12; i++)
        {
            if(my_stat[i].cnt == 0)
            {
                printf("At %d month have not value\n", i+1);
            }
            else
            {
                printf("At %d month mean temp: %.1f, min temp: %d, max temp: %d\n",
                       i+1, my_stat[i].temp_mean, my_stat[i].temp_min, my_stat[i].temp_max);
                cnt++;

                if(my_stat[i].temp_min < min_year)
                {
                    min_year = my_stat[i].temp_min;
                }

                if(my_stat[i].temp_max > max_year)
                {
                    max_year = my_stat[i].temp_max;
                }

            }
            temp_mean = my_stat[i].temp_mean;

        }
        printf("\nIn %d year mean temp: %.1f, min temp: %d, max temp %d\n",
               my_stat[0].year, temp_mean/cnt*1.0, min_year, max_year);
    }
    else
    {
        target_month -= 1;
        if(my_stat[target_month].cnt == 0)
        {
            printf("At %d month have not value\n", target_month+1);
        }
        else
        {
            printf("Month %d, year %d, mean temp: %.1f, min temp: %d, max temp %d\n",
                   target_month+1, my_stat[target_month].year, my_stat[target_month].temp_mean, my_stat[target_month].temp_min, my_stat[target_month].temp_max);

        }
        // temp_mean = my_stat[target_month].temp_mean; 
    }
}

int my_getopt(int argc, char *argv[], char filename[], int *target_month)
{
    int rez=0;
    while ( (rez = getopt(argc,argv,"hm:f:")) != -1){
        switch (rez){
            case 'h':
                printf("This function need to search temperature from .csv file\n");
                printf("Usage: [options]\n\
                       -h This help text\n\
                       -m <mm> Statistic from 'mm' month. 1...12 available\n\
                       -f <filename.csv> File with temperature data. Must have numeric values\n");
                       printf("Example: %s -f temperature_example.csv\n",argv[0]);
                return 0;
            case 'm': //
                //printf("Input month is %s\n",argv[optind-1]);
                if(atoi(optarg) > 0 && atoi(optarg) < 13)
                {
                    *target_month = atoi(optarg);
                    break;
                }
                else
                {
                    printf("Bad month argument: %s Try -h for help\n", argv[optind-1]);
                    return 1;
                }
                break;
            case 'f':
                printf("Input filename is %s\n",optarg);
                strcpy(filename, optarg);
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
