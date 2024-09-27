#ifndef TEMP_FUNCTIONS_H_INCLUDED
#define TEMP_FUNCTIONS_H_INCLUDED

#include <inttypes.h>
#include <unistd.h>

//struct

struct data
{
    double day[31];        //РїРѕ С…РѕСЂРѕС€РµРјСѓ РЅСѓР¶РµРЅ РґР°Р±Р».
    double temp_mean;
    int temp_max;
    int temp_min;
    int cnt;
    int year;
};

//variable
int getopt(int argc, char * const argv[], const char *optstring);
extern char *optarg;
extern int optind, opterr, optopt;

//prototypes

int read_from_csv_file(char *filename, struct data *my_stat);  //РёРјСЏ С„Р°Р№Р»Р°?

void add_statistics(struct data *my_stat, int year, int month, int day, int temp);

void clear_stats(struct data *month);

void temp_year(int target_month, struct data *my_stat);

int my_getopt(int argc, char *argv[], char filename[], int *target_month);


#endif // TEMP_FUNCTIONS_H_INCLUDED
