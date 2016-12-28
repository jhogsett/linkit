#ifndef COMMANDS_DEF_H
#define COMMANDS_DEF_H

#define NUM_COMMANDS 63
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
#define CMD_COPY      60
#define CMD_OFFSET    61
#define CMD_CLEAR     62
#define CMD_ZONE      63

const char cmd_1[] PROGMEM = "pause";
const char cmd_2[] PROGMEM = "continue";
const char cmd_3[] PROGMEM = "erase";
const char cmd_4[] PROGMEM = "blink";
const char cmd_5[] PROGMEM = "blink1";
const char cmd_6[] PROGMEM = "blink2";
const char cmd_7[] PROGMEM = "blink3";
const char cmd_8[] PROGMEM = "blink4";
const char cmd_9[] PROGMEM = "blink5";
const char cmd_10[] PROGMEM = "blink6";
const char cmd_11[] PROGMEM = "blinkr";
const char cmd_12[] PROGMEM = "blinka";
const char cmd_13[] PROGMEM = "blinkb";
const char cmd_14[] PROGMEM = "breathe";
const char cmd_15[] PROGMEM = "effectr";
const char cmd_16[] PROGMEM = "flush";
const char cmd_17[] PROGMEM = "blend";
const char cmd_18[] PROGMEM = "max";
const char cmd_19[] PROGMEM = "dim";
const char cmd_20[] PROGMEM = "bright";
const char cmd_21[] PROGMEM = "fade";
const char cmd_22[] PROGMEM = "wipe";
const char cmd_23[] PROGMEM = "flood";
const char cmd_24[] PROGMEM = "random";
const char cmd_25[] PROGMEM = "mirror";
const char cmd_26[] PROGMEM = "eshift";
const char cmd_27[] PROGMEM = "pshift";
const char cmd_28[] PROGMEM = "pshifto";
const char cmd_29[] PROGMEM = "window";
const char cmd_30[] PROGMEM = "reset";
const char cmd_31[] PROGMEM = "demo";
const char cmd_32[] PROGMEM = "static";
const char cmd_33[] PROGMEM = "rgbcolor";
const char cmd_34[] PROGMEM = "hslcolor";
const char cmd_35[] PROGMEM = "red";
const char cmd_36[] PROGMEM = "green";
const char cmd_37[] PROGMEM = "blue";
const char cmd_38[] PROGMEM = "black";
const char cmd_39[] PROGMEM = "yellow";
const char cmd_40[] PROGMEM = "orange";
const char cmd_41[] PROGMEM = "purple";
const char cmd_42[] PROGMEM = "cyan";
const char cmd_43[] PROGMEM = "magenta";
const char cmd_44[] PROGMEM = "pink";
const char cmd_45[] PROGMEM = "white";
const char cmd_46[] PROGMEM = "gray";
const char cmd_47[] PROGMEM = "ltgreen";
const char cmd_48[] PROGMEM = "seafoam";
const char cmd_49[] PROGMEM = "ltblue";
const char cmd_50[] PROGMEM = "dkgray";
const char cmd_51[] PROGMEM = "repeat";
const char cmd_52[] PROGMEM = "blinkp";
const char cmd_53[] PROGMEM = "lopower";
const char cmd_54[] PROGMEM = "hipower";
const char cmd_55[] PROGMEM = "display";
const char cmd_56[] PROGMEM = "pinon";
const char cmd_57[] PROGMEM = "pinoff";
const char cmd_58[] PROGMEM = "level";
const char cmd_59[] PROGMEM = "cfade";
const char cmd_60[] PROGMEM = "copy";
const char cmd_61[] PROGMEM = "offset";
const char cmd_62[] PROGMEM = "clear";
const char cmd_63[] PROGMEM = "zone";

const char* const command_strings[] PROGMEM = {
   cmd_1,  cmd_2,  cmd_3,  cmd_4,  cmd_5,  cmd_6,  cmd_7,  cmd_8,  cmd_9, cmd_10,
  cmd_11, cmd_12, cmd_13, cmd_14, cmd_15, cmd_16, cmd_17, cmd_18, cmd_19, cmd_20,
  cmd_21, cmd_22, cmd_23, cmd_24, cmd_25, cmd_26, cmd_27, cmd_28, cmd_29, cmd_30,
  cmd_31, cmd_32, cmd_33, cmd_34, cmd_35, cmd_36, cmd_37, cmd_38, cmd_39, cmd_40,
  cmd_41, cmd_42, cmd_43, cmd_44, cmd_45, cmd_46, cmd_47, cmd_48, cmd_49, cmd_50,
  cmd_51, cmd_52, cmd_53, cmd_54, cmd_55, cmd_56, cmd_57, cmd_58, cmd_59, cmd_60,
  cmd_61, cmd_62, cmd_63
};

#endif

