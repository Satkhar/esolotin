#include <stdio.h>
#include <stdlib.h>

#include "temp_functions.h"

int main(int argc, char *argv[])
{
    int ret=0;
    int target_month = -1;
    char filename[100] = "temperature_small.csv";   //Р·РЅР°С‡РµРЅРёРµ РїРѕ СѓРјРѕР»С‡Р°РЅРёСЋ
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

    clear_stats(month);

    if(read_from_csv_file(filename, month))
    {
        return 1;
    }

    temp_year(target_month, month);
    return 0;
}