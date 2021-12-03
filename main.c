/**
 * climate.c
 *
 * Performs analysis on climate data provided by the
 * National Oceanic and Atmospheric Administration (NOAA).
 *
 * Input:    Tab-delimited file(s) to analyze.
 * Output:   Summary information about the data.
 *
 * Compile:  run make
 *
 * Example Run:      ./climate data_tn.tdv data_wa.tdv
 *
 *
 * Opening file: data_tn.tdv
 * Opening file: data_wa.tdv
 * States found: TN WA
 * -- State: TN --
 * Number of Records: 17097
 * Average Humidity: 49.4%
 * Average Temperature: 58.3F
 * Max Temperature: 110.4F 
 * Max Temperatuer on: Mon Aug  3 11:00:00 2015
 * Min Temperature: -11.1F
 * Min Temperature on: Fri Feb 20 04:00:00 2015
 * Lightning Strikes: 781
 * Records with Snow Cover: 107
 * Average Cloud Cover: 53.0%
 * -- State: WA --
 * Number of Records: 48357
 * Average Humidity: 61.3%
 * Average Temperature: 52.9F
 * Max Temperature: 125.7F
 * Max Temperature on: Sun Jun 28 17:00:00 2015
 * Min Temperature: -18.7F 
 * Min Temperature on: Wed Dec 30 04:00:00 2015
 * Lightning Strikes: 1190
 * Records with Snow Cover: 1383
 * Average Cloud Cover: 54.5%
 *
 * TDV format:
 * st  timestamp(unix)  geolocation   humidity   snow   cloud cover  lightning     pressure (Pa)  surface temp (k)
 * CA» 1428300000000»  9prcjqk3yc80»   93.0»      0.0»    100.0»        0.0»         95644.0»       277.58716
 * CA» 1430308800000»  9prc9sgwvw80»   4.0»       0.0»    100.0»        0.0»         99226.0»       282.63037
 * CA» 1428559200000»  9prrremmdqxb»   61.0»      0.0»    0.0»          0.0»         102112.0»      285.07513
 * CA» 1428192000000»  9prkzkcdypgz»   57.0»      0.0»    100.0»        0.0»         101765.0»      285.21332
 * CA» 1428170400000»  9prdd41tbzeb»   73.0»      0.0»    22.0»         0.0»         102074.0»      285.10425
 * CA» 1429768800000»  9pr60tz83r2p»   38.0»      0.0»    0.0»          0.0»         101679.0»      283.9342
 * CA» 1428127200000»  9prj93myxe80»   98.0»      0.0»    100.0»        0.0»         102343.0»      285.75
 * CA» 1428408000000»  9pr49b49zs7z»   93.0»      0.0»    100.0»        0.0»         100645.0»      285.82413
 *
 * Each field is separated by a tab character \t and ends with a newline \n.
 *
 * Fields:
 *      state code (e.g., CA, TX, etc),
 *      timestamp (time of observation as a UNIX timestamp),
 *      geolocation (geohash string),
 *      humidity (0 - 100%),
 *      snow (1 = snow present, 0 = no snow),
 *      cloud cover (0 - 100%),
 *      lightning strikes (1 = lightning strike, 0 = no lightning),
 *      pressure (Pa),
 *      surface temperature (Kelvin)
 */

#include <float.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define NUM_STATES 50

/* TODO: Add elements to the climate_info struct as necessary. */

struct climate_info {
    char code[3];
    unsigned long num_records;
    long sum_temp;
    double max_temp;
    double min_temp;
    char* min_temp_time;
    char* max_temp_time;
    unsigned long sum_humidity;
    unsigned long sum_strikes;
    unsigned long sum_snow;
    unsigned long sum_cloud;
};

void analyze_file(FILE *file, struct climate_info *states[], int num_states);
void print_report(struct climate_info *states[], int num_states);
char* timeToString(char* time);
double KtoF(double K);


int main() 
{

    char files[1][20] = {"data_multi.tdv"};
    /* TODO: fix this conditional. You should be able to read multiple files. */
    /*Program must read at least 1 file*/

    /* Let's create an array to store our state data in. As we know, there are
     * 50 US states. */
    struct climate_info *states[NUM_STATES] = { NULL };
    for (int i = 0; i < 1; ++i) {
        /* TODO: Open the file for reading */
        FILE* fileptr = fopen(files[i], "r");

        printf("Opening file: %s\n", files[i]);

        /* TODO: If the file doesn't exist, print an error message and move on
         * to the next file. */

        if (fileptr == NULL) {
            printf("File cannot be opened.\n");
        }
        else {

            /* TODO: Analyze the file */
            analyze_file(fileptr, states, NUM_STATES); 

            //closes file to free memory
            fclose(fileptr);
        }
    }

    /* Now that we have recorded data for each file, we'll summarize them: */
    print_report(states, NUM_STATES);

    return 0;
}

void analyze_file(FILE *file, struct climate_info **states, int num_states){
    const int line_sz = 100;
    char line[line_sz];
    char* rest;
    char* token;

    while (fgets(line, line_sz, file) != NULL) {

      rest = line;
      token = strtok_r(NULL, " \t\n", &rest); //getting code
      char* temp_time = NULL;
      int found_index = -1;

      for (int i = 0; i < num_states; i++){
        if (*(states + i) != NULL && !strcmp((*states + i)->code, token)){
          found_index = i;
          break;
        }
      }

      //if not found, create new one from empty element in states
      if (found_index < 0){
        for (int i = 0; i < num_states && found_index < 0; i++){
          if (*(states + i) == NULL){
                  printf("making new\n");

            //printf("Making new %s %d\n", token, i);//~~~~~~~~~~~~~~~~~~~~~~~~~
            *(states + i) = calloc(1, sizeof(struct climate_info));
            found_index = i;
            (*states + found_index)->num_records = 0;
            (*states + found_index)->sum_temp = 0;
            (*states + found_index)->max_temp = 0;
            (*states + found_index)->min_temp = 0;
            (*states + found_index)->sum_humidity = 0;
            (*states + found_index)->sum_snow = 0;
            (*states + found_index)->sum_cloud = 0;
            (*states + found_index)->sum_strikes = 0;
            //(*states + found_index)->num_records = (unsigned long) calloc(1, sizeof(unsigned long));
            //(*states + found_index)->sum_temp = (long) calloc(1, sizeof(long));
            //(*states + found_index)->max_temp = (long) calloc(1, sizeof(long));
            (*states + found_index)->max_temp_time = calloc(30, sizeof(char));
            //(*states + found_index)->min_temp = (long) calloc(1, sizeof(double));
            (*states + found_index)->min_temp_time = calloc(30, sizeof(char));
            //(*states + found_index)->sum_humidity = (long) calloc(1, sizeof(long));
            //(*states + found_index)->sum_strikes = (long) calloc(1, sizeof(unsigned long));
            //(*states + found_index)->sum_snow = (long) calloc(1, sizeof(long));
            //(*states + found_index)->sum_cloud = (long) calloc(1, sizeof(long));
            strcpy((*states + found_index)->code, token);
                  printf("success\n");

            //printf("Success\n");
          }
        }
      }

      (*states + found_index)->num_records+= 1;
      token = strtok_r(NULL, " \t\n", &rest);
      //time
      temp_time = token;
      token = strtok_r(NULL, " \t\n", &rest);
      //geolocation will be skipped
      token = strtok_r(NULL, " \t\n", &rest);
      //humidity
      (*states + found_index)->sum_humidity += atol(token);

      token = strtok_r(NULL, " \t\n", &rest);
      //snow
      if (atol(token)) {
        (*states + found_index)->sum_snow+= 1;

      }
      token = strtok_r(NULL, " \t\n", &rest);
      //cloud
      (*states + found_index)->sum_cloud += atol(token);
      

      token = strtok_r(NULL, " \t\n", &rest);
      //lightning
      if (atol(token)) {
        (*states + found_index)->sum_strikes+= 1;
      }
      token = strtok_r(NULL, " \t\n", &rest);
      //pressure will be skipped
      token = strtok_r(NULL, " \t\n", &rest);
      //surface temp
      double temp_kelvin = KtoF(atol(token));
      //printf("success1\n");
      char* string_time = timeToString(temp_time);
      //printf("success2\n");
      (*states + found_index)->sum_temp += temp_kelvin;
      if (temp_kelvin > (*states + found_index)->max_temp){
        (*states + found_index)-> max_temp = temp_kelvin;
        
        strcpy((*states + found_index)-> max_temp_time, string_time);
        //printf("BIG %lf\n", temp_kelvin);
        //printf("%s", timeToString(temp_time));
      }
      if (temp_kelvin < (*states + found_index)->min_temp){
        (*states + found_index)-> min_temp = temp_kelvin;
        strcpy((*states + found_index)-> min_temp_time, string_time);
        //printf("SMALL %lf\n", temp_kelvin);
        //printf("%s", timeToString(temp_time));
      }
      /* TODO: We need to do a few things here:
        *
        *       * Tokenize the line.
        *       * Determine what state the line is for. This will be the state
        *         code, stored as our first token.
        *       * If our states array doesn't have a climate_info entry for
        *         this state, then we need to allocate memory for it and put it
        *         in the next open place in the array. Otherwise, we reuse the
        *         existing entry.
        *       * Update the climate_info structure as necessary.
        */
    }
    printf("END\n");
}

void print_report(struct climate_info *states[], int num_states) {
    printf("States found: ");
    for (int i = 0; i < num_states; i++) {
        if (states[i] != NULL) {
            struct climate_info *info = (*states + i);
            printf("%s ", info->code);
        }
    }
    printf("\n");

    /* TODO: Print out the summary for each state. See format above. */

    for (int i = 0; i < num_states; i++) {
      if (states[i] != NULL){
        printf("--State: %s--\n", (*states + i)->code);
        printf("Number of Records: %ld\n", (*states + i)->num_records);
        printf("Average Humidity: %.1f%%\n", (double) (*states + i)->sum_humidity/(*states + i)->num_records);
        printf("Average Temperature: %.1fF\n", (double) (*states + i)->sum_temp/(*states + i)->num_records);
        printf("Max Temperature: %.1fF\n", (double) (*states + i)->max_temp);
        printf("Max Temperature on: %s\n", (*states + i)->max_temp_time);
        printf("Min Temperature: %.1fF\n", (double) (*states + i)->min_temp);
        printf("Min Temperature on: %s\n", (*states + i)->min_temp_time);
        printf("Lightning Strikes: %.lu\n", (*states + i)->sum_strikes);
        printf("Records with Snow Cover: %.lu\n", (*states + i)->sum_snow);
        printf("Average Cloud Cover: %.1f%%\n", (double) (*states + i) ->sum_cloud/(*states + i)->num_records);
      }
    }
}

double KtoF(double K) {
    return K * 1.8 - 459.67;
}

char* timeToString(char* time) {
    time_t timestamp = atol(time) / 1000;
    char* timestamp_string = calloc(40, sizeof(char));
    strcpy(timestamp_string, ctime(&timestamp));
    //printf("here3\n");
    return strtok(timestamp_string, "\n");
    //strips trailing newline that is added by ctime
}
