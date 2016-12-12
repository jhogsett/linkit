#ifndef COMMANDS_DEF_H
#define COMMANDS_DEF_H

// enable for full command set
#define FULL_COMMAND_SET

// enable for minimal friendly command set
// #define MINIMAL_COMMAND_SET

#if defined(FULL_COMMAND_SET)
#define NUM_COMMANDS 59

#define CMD_NONE       0
#define CMD_FIRST      1
#define CMD_PAUSE      1
#define CMD_CONTINUE   2
#define CMD_ERASE      3
#define CMD_BLINK      4
#define CMD_BLINK1     5
#define CMD_BLINK2     6
#define CMD_BLINK3     7
#define CMD_BLINK4     8
#define CMD_BLINK5     9
#define CMD_BLINK6    10
#define CMD_BLINKR    11
#define CMD_BLINKA    12
#define CMD_BLINKB    13
#define CMD_BREATHE   14
#define CMD_EFFECTR   15
#define CMD_FLUSH     16
#define CMD_BLEND     17
#define CMD_MAX       18
#define CMD_DIM       19
#define CMD_BRIGHT    20
#define CMD_FADE      21
#define CMD_WIPE      22
#define CMD_FLOOD     23  
#define CMD_RANDOM    24
#define CMD_MIRROR    25
#define CMD_ESHIFT    26
#define CMD_PSHIFT    27
#define CMD_PSHIFTO   28
#define CMD_WINDOW    29
#define CMD_RESET     30
#define CMD_DEMO      31
#define CMD_STATIC    32
#define CMD_RGBCOLOR  33
#define CMD_HSLCOLOR  34
#define CMD_RED       35
#define CMD_GREEN     36
#define CMD_BLUE      37
#define CMD_BLACK     38
#define CMD_YELLOW    39
#define CMD_ORANGE    40
#define CMD_PURPLE    41
#define CMD_CYAN      42
#define CMD_MAGENTA   43
#define CMD_PINK      44
#define CMD_WHITE     45
#define CMD_GRAY      46
#define CMD_LTGREEN   47
#define CMD_SEAFOAM   48
#define CMD_LTBLUE    49
#define CMD_DKGRAY    50
#define CMD_REPEAT    51
#define CMD_BLINKP    52
#define CMD_LOPOWER   53
#define CMD_HIPOWER   54
#define CMD_DISPLAY   55
#define CMD_PINON     56
#define CMD_PINOFF    57
#define CMD_LEVEL     58
#define CMD_CFADE     59

char* command_strings[] = {
  "pause",
  "continue",
  "erase",
  "blink",
  "blink1",
  "blink2",
  "blink3",
  "blink4",
  "blink5",
  "blink6",
  "blinkr",
  "blinka",
  "blinkb",
  "breathe",
  "effectr",
  "flush",
  "blend",
  "max",
  "dim",
  "bright",
  "fade",
  "wipe",
  "flood",
  "random",
  "mirror",
  "eshift",
  "pshift",
  "pshifto",
  "window",
  "reset",
  "demo",
  "static",
  "rgbcolor",
  "hslcolor",
  "red",
  "green",
  "blue",
  "black",
  "yellow",
  "orange",  
  "purple",
  "cyan",
  "magenta",
  "pink",
  "white",
  "gray",
  "ltgreen",
  "seafoam",
  "ltblue",
  "dkgray",
  "repeat",
  "blinkp",
  "lopower",
  "hipower",
  "display",
  "pinon",
  "pinoff",
  "level",
  "cfade"
};
#elif defined(MINIMAL_COMMAND_SET)

#ifdef WEARABLE
#define NUM_COMMANDS 39
#else
#define NUM_COMMANDS 37
#endif

#define CMD_NONE       0
#define CMD_FIRST      1

#define CMD_RESET      1
#define CMD_DISPLAY    2
#define CMD_ERASE      3
#define CMD_LEVEL      4
#define CMD_FLUSH      5
#define CMD_PAUSE      6
#define CMD_CONTINUE   7
#define CMD_REPEAT     8
#define CMD_DEMO       9
#define CMD_BLACK     10
#define CMD_RED       11
#define CMD_GREEN     12
#define CMD_BLUE      13
#define CMD_YELLOW    14
#define CMD_ORANGE    15
#define CMD_PURPLE    16
#define CMD_WHITE     17
#define CMD_RANDOM    18
#define CMD_RGBCOLOR  19
#define CMD_HSLCOLOR  20
#define CMD_BLINK     21
#define CMD_BLINK1    22
#define CMD_BLINK2    23
#define CMD_BLINK3    24
#define CMD_BLINK4    25
#define CMD_BLINK5    26
#define CMD_BLINK6    27
#define CMD_BLINKR    28
#define CMD_BLINKA    29
#define CMD_BLINKB    30
#define CMD_BREATHE   31
#define CMD_EFFECTR   32
#define CMD_FLOOD     33  
#define CMD_FADE      34
#define CMD_WIPE      35
#define CMD_PSHIFTO   36
#define CMD_WINDOW    37
#ifdef WEARABLE
#define CMD_LOPOWER   38
#define CMD_HIPOWER   39
#endif

char* command_strings[] = {
  "reset",
  "display",
  "erase",
  "level",
  "flush",
  "pause",
  "continue",
  "repeat",
  "demo",
  "black",
  "red",
  "green",
  "blue",
  "yellow",
  "orange",  
  "purple",
  "white",
  "random",
  "rgbcolor",
  "hslcolor",
  "blink",
  "blink1",
  "blink2",
  "blink3",
  "blink4",
  "blink5",
  "blink6",
  "blinkr",
  "blinka",
  "blinkb",
  "breathe",
  "effectr",
  "flood",
  "fade",
  "wipe",
  "pshifto",
  "window",
  "lopower",
  "hipower",
};
#endif

#endif

