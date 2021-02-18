#define _GNU_SOURCE
#include <dirent.h>
#include <fcntl.h>
#include <linux/input.h>
#include <unistd.h>
#include <regex.h>
#include <string.h>
#include <ctype.h>

const char* INPUT_DEVICE_DIR = "/dev/input/";
const char* EVENT_DEVICE_NAME = "event";
const char* day[] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};
const char* month[] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};

const char* keys[] = { //All N-U are buttons that are not relevant to this application
    "", //N-U
    "", //N-U

    //Keypad top numbers
    "1", "2", "3", "4", "5", "6", "7", "8", "9", "0",

    "-", "=", "<bs>", "\t",

    "q", "w", "e", "r", "t", "y", "u", "i", "o", "p",

    "[", "]", "\n",

    "", //N-U


    "a", "s", "d", "f", "g", "h", "j", "k", "l",

    ";", "'", "`", "shft", "#",

    "z", "x", "c", "v", "b", "n", "m",

    ",", ".", "/", "shft", "*",

    "", //N-U
    " ", "cap",

    " F1 ", " F2 ", " F3 ", " F4 ", " F5 ", " F6 ", " F7 ", " F8 ", " F9 ", " F10 ",

    "", "", //N-U

    //Numeric Keypads
    "7", "8", "9",
    "-",
    "4", "5", "6",
    "+",
    "1", "2",
    "3", "0",
    ".", "", "",

    "\\", " F11 ", " F12 ",

    "", "", "", "", "", "", "", //N-U

    "\n",
    "", //N-U
    "/",
};

const char* shift_keys[] = {
  "", //N-U
  "", //N-U

  //Keypad top numbers
  "!", "\"", "£", "$", "%", "^", "&", "*", "(", ")",

  "_", "+", "<bs>", "\t",

  "q", "w", "e", "r", "t", "y", "u", "i", "o", "p",

  "{}", "}", "\n",

  "", //N-U


  "a", "s", "d", "f", "g", "h", "j", "k", "l",

  ":", "@", "¬", "shft", "~",

  "z", "x", "c", "v", "b", "n", "m",

  "<", ">", "?", "shft", "*",

  "", //N-U
  " ", "cap",

  " F1 ", " F2 ", " F3 ", " F4 ", " F5 ", " F6 ", " F7 ", " F8 ", " F9 ", " F10 ",

  "", "", //N-U

  //Numeric Keypads
  "7", "8", "9",
  "-",
  "4", "5", "6",
  "+",
  "1", "2",
  "3", "0",
  ".", "", "",

  "|", " F11 ", " F12 ",

  "", "", "", "", "", "", "", //N-U

  "\n",
  "", //N-U
  "/",
};
