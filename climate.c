/* Author: Sarah Ling
 * ID: 20668079 
 * Due Date: 12/6/21
 *
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
 * CA 1428300000000  9prcjqk3yc80   93.0      0.0    100.0        0.0         95644.0       277.58716
 * CA 1430308800000  9prc9sgwvw80   4.0       0.0    100.0        0.0         99226.0       282.63037
 * CA 1428559200000  9prrremmdqxb   61.0      0.0    0.0          0.0         102112.0      285.07513
 * CA 1428192000000  9prkzkcdypgz   57.0      0.0    100.0        0.0         101765.0      285.21332
 * CA 1428170400000  9prdd41tbzeb   73.0      0.0    22.0         0.0         102074.0      285.10425
 * CA 1429768800000  9pr60tz83r2p   38.0      0.0    0.0          0.0         101679.0      283.9342
 * CA 1428127200000  9prj93myxe80   98.0      0.0    100.0        0.0         102343.0      285.75
 * CA 1428408000000  9pr49b49zs7z   93.0      0.0    100.0        0.0         100645.0      285.82413
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

struct climate_info {
    char code[3];
    unsigned long num_records;
    double sum_temp;
    double max_temp;
    double min_temp;
    char* min_temp_time;
    char* max_temp_time;
    unsigned long sum_humidity;
    unsigned long sum_strikes;
    unsigned long sum_snow;
    unsigned long sum_cloud;
};

// Function Prototypes
void analyze_file(FILE *file, struct climate_info *states[], int num_states);
void print_report(struct climate_info *states[], int num_states);
char* timeToString(char* time);
double KtoF(double K);


int main(int argc, char *argv[]) 
{
  //Program must read at least 1 file to be able to run  
  if (argc < 1){
    printf("At least 1 file must be opened!\n");
    return EXIT_FAILURE;
  }

  /* Let's create an array to store our state data in. As we know, there are
    * 50 US states. */
  struct climate_info *states[NUM_STATES] = { NULL };

  for (int i = 1; i < argc; ++i) {
    /* Opens the file for reading */
    FILE* fileptr = fopen(argv[i], "r");

    printf("Opening file: %s\n", argv[i]);

    /* If the file doesn't exist, an error message is printed and the
    program moves on to the next file. */

    if (fileptr == NULL) {
      printf("File cannot be opened.\n");
    }
    else {

      /* Analyzes the file */
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
  char* token;
  while (fgets(line, line_sz, file) != NULL) {

    token = strtok(line, " \t\n"); //getting code
    char* temp_time = NULL;
    int found_index = -1; //set to -1, meaning not yet found

    for (int i = 0; i < num_states; i++){

      //state code is found in states array
      if (*(states + i) != NULL && !strcmp((*states + i)->code, token)){
        found_index = i;
        break;
      }
    }

    //if state code is not found in states array, create new one
    if (found_index < 0){
      for (int i = 0; i < num_states && found_index < 0; i++){
        if (*(states + i) == NULL){
          *(states + i) = calloc(num_states, sizeof(struct climate_info));
          found_index = i;
          (*states + found_index)->num_records = 0;
          (*states + found_index)->sum_temp = 0;

          /* Not an elegant way to set default values for temperatures, but
          the temperatures set to values that are are out of this world so 
          that the first temperature processed from the file will act as default */ 
          (*states + found_index)->max_temp = -1000;
          (*states + found_index)->min_temp = 1000;

          (*states + found_index)->sum_humidity = 0;
          (*states + found_index)->sum_snow = 0;
          (*states + found_index)->sum_cloud = 0;
          (*states + found_index)->sum_strikes = 0;
          (*states + found_index)->max_temp_time = calloc(30, sizeof(char));
          (*states + found_index)->min_temp_time = calloc(30, sizeof(char));
          strcpy((*states + found_index)->code, token);
        }
      }
    }

    (*states + found_index)->num_records+= 1;

    token = strtok(NULL, " \t\n");
    //time is extracted from string
    temp_time = token;

    token = strtok(NULL, " \t\n");
    //geolocation is extracted, but will be unused

    token = strtok(NULL, " \t\n");
    //humidity is extracted
    (*states + found_index)->sum_humidity += atol(token);

    token = strtok(NULL, " \t\n");
    //snow is extracted
    if (atol(token)) {
      (*states + found_index)->sum_snow+= 1;
    }

    token = strtok(NULL, " \t\n");
    //cloud is extracted
    (*states + found_index)->sum_cloud += atol(token);

    token = strtok(NULL, " \t\n");
    //lightning is extracted
    if (atol(token)) {
      (*states + found_index)->sum_strikes+= 1;
    }

    token = strtok(NULL, " \t\n");
    //pressure is extracted, but will be unused

    token = strtok(NULL, " \t\n");
    //surface temp is extracted as Kelvin
    double temp_F = KtoF(atof(token)); //converted to Fahrenheit
    char* string_time = timeToString(temp_time);// convert UNIX time to ctime format
    (*states + found_index)->sum_temp += temp_F;
    if (temp_F > (*states + found_index)->max_temp){ //set max temp
      (*states + found_index)-> max_temp = temp_F;
      strcpy((*states + found_index)-> max_temp_time, string_time);
    }
    if (temp_F < (*states + found_index)->min_temp){ //set min temp
      (*states + found_index)-> min_temp = temp_F;
      strcpy((*states + found_index)-> min_temp_time, string_time);
    }
  }
}

//prints out the summary for each state. See format above
void print_report(struct climate_info *states[], int num_states) {
  printf("States found: ");
  for (int i = 0; i < num_states; i++) {
      if (states[i] != NULL) {
          struct climate_info *info = (*states + i);
          printf("%s ", info->code);
      }
  }
  printf("\n");

  for (int i = 0; i < num_states; i++) {
    if (states[i] != NULL){
      printf("-- State: %s --\n", (*states + i)->code);
      printf("Number of Records: %ld\n", (*states + i)->num_records);
      printf("Average Humidity: %.1f%%\n", (double) (*states + i)->sum_humidity/(*states + i)->num_records);
      double avg_temp = (*states + i)->sum_temp/(*states + i)->num_records;
      printf("Average Temperature: %.1fF\n", avg_temp);
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

//Converts Kelvin to Fahrenheit
double KtoF(double K) {
    return K * 1.8 - 459.67;
}

//Converts UNIX time to ctime
char* timeToString(char* time) {
    time_t timestamp = atol(time) / 1000;
    char* timestamp_string = calloc(40, sizeof(char));
    strcpy(timestamp_string, ctime(&timestamp));
    //printf("here3\n");
    return strtok(timestamp_string, "\n");
    //strips trailing newline that is added by ctime
}
