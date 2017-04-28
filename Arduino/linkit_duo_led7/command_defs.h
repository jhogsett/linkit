#ifndef COMMANDS_DEF_H
#define COMMANDS_DEF_H

#define NUM_COMMANDS     75
#define CMD_NULL         -1
#define CMD_NONE          0
#define CMD_FIRST         1
#define CMD_FLUSH         1
#define CMD_ERASE         2
#define CMD_ROTATE        3
#define CMD_REPEAT        4
#define CMD_COPY          5
#define CMD_FLOOD         6
#define CMD_MIRROR        7
#define CMD_DISPLAY       8
#define CMD_ZONE          9
#define CMD_WINDOW       10
#define CMD_OFFSET       11
#define CMD_REVERSE      12
#define CMD_RGBCOLOR     13
#define CMD_HSLCOLOR     14
#define CMD_RED          15
#define CMD_GREEN        16
#define CMD_BLUE         17
#define CMD_BLACK        18
#define CMD_YELLOW       19
#define CMD_ORANGE       20
#define CMD_PURPLE       21
#define CMD_CYAN         22
#define CMD_MAGENTA      23
#define CMD_PINK         24
#define CMD_WHITE        25
#define CMD_GRAY         26
#define CMD_LTGREEN      27
#define CMD_SEAFOAM      28
#define CMD_LTBLUE       29
#define CMD_DKGRAY       30
#define CMD_TUNGSTEN     31
#define CMD_RANDOM       32
#define CMD_BLEND        33
#define CMD_MAX          34
#define CMD_DIM          35
#define CMD_BRIGHT       36
#define CMD_BLINK        37
#define CMD_BLINK1       38
#define CMD_BLINK2       39
#define CMD_BLINK3       40
#define CMD_BLINK4       41
#define CMD_BLINK5       42
#define CMD_BLINK6       43
#define CMD_BLINKR       44
#define CMD_BLINKA       45
#define CMD_BLINKB       46
#define CMD_BLINKC       47
#define CMD_BREATHE      48
#define CMD_FADER        49
#define CMD_EFFECTR      50
#define CMD_STATIC       51
#define CMD_PAUSE        52
#define CMD_CONTINUE     53
#define CMD_RESET        54
#define CMD_CLEAR        55
#define CMD_LEVEL        56
#define CMD_FADE         57
#define CMD_WIPE         58
#define CMD_ESHIFT       59
#define CMD_PSHIFT       60
#define CMD_PSHIFTO      61
#define CMD_CFADE        62
#define CMD_LOPOWER      63
#define CMD_HIPOWER      64
#define CMD_PINON        65
#define CMD_PINOFF       66
#define CMD_DEMO         67
#define CMD_SETBLINKC    68
#define CMD_SETBLINKP    69
#define CMD_SCHEDULE     70
#define CMD_CARRY        71
#define CMD_SETBREATHET  72
#define CMD_SET_MACRO    73
#define CMD_RUN_MACRO    74
#define CMD_DELAY        75

const char cmd_001[] PROGMEM = "flu";
const char cmd_002[] PROGMEM = "era";
const char cmd_003[] PROGMEM = "rot";
const char cmd_004[] PROGMEM = "rep";
const char cmd_005[] PROGMEM = "cpy";
const char cmd_006[] PROGMEM = "flo";
const char cmd_007[] PROGMEM = "mir";
const char cmd_008[] PROGMEM = "dis";
const char cmd_009[] PROGMEM = "zon";
const char cmd_010[] PROGMEM = "win";
const char cmd_011[] PROGMEM = "off";
const char cmd_012[] PROGMEM = "rev";
const char cmd_013[] PROGMEM = "rgb";
const char cmd_014[] PROGMEM = "hsl";
const char cmd_015[] PROGMEM = "red";
const char cmd_016[] PROGMEM = "grn";
const char cmd_017[] PROGMEM = "blu";
const char cmd_018[] PROGMEM = "blk";
const char cmd_019[] PROGMEM = "yel";
const char cmd_020[] PROGMEM = "org";
const char cmd_021[] PROGMEM = "pur";
const char cmd_022[] PROGMEM = "cyn";
const char cmd_023[] PROGMEM = "mag";
const char cmd_024[] PROGMEM = "pnk";
const char cmd_025[] PROGMEM = "wht";
const char cmd_026[] PROGMEM = "gry";
const char cmd_027[] PROGMEM = "lgr";
const char cmd_028[] PROGMEM = "sea";
const char cmd_029[] PROGMEM = "lbl";
const char cmd_030[] PROGMEM = "dgr";
const char cmd_031[] PROGMEM = "tun";
const char cmd_032[] PROGMEM = "rnd";
const char cmd_033[] PROGMEM = "ble";
const char cmd_034[] PROGMEM = "max";
const char cmd_035[] PROGMEM = "dim";
const char cmd_036[] PROGMEM = "brt";
const char cmd_037[] PROGMEM = "bli";
const char cmd_038[] PROGMEM = "bl1";
const char cmd_039[] PROGMEM = "bl2";
const char cmd_040[] PROGMEM = "bl3";
const char cmd_041[] PROGMEM = "bl4";
const char cmd_042[] PROGMEM = "bl5";
const char cmd_043[] PROGMEM = "bl6";
const char cmd_044[] PROGMEM = "blr";
const char cmd_045[] PROGMEM = "bla";
const char cmd_046[] PROGMEM = "blb";
const char cmd_047[] PROGMEM = "blc";
const char cmd_048[] PROGMEM = "bre";
const char cmd_049[] PROGMEM = "fdr";
const char cmd_050[] PROGMEM = "efr";
const char cmd_051[] PROGMEM = "sta";
const char cmd_052[] PROGMEM = "pau";
const char cmd_053[] PROGMEM = "cnt";
const char cmd_054[] PROGMEM = "rst";
const char cmd_055[] PROGMEM = "clr";
const char cmd_056[] PROGMEM = "lev";
const char cmd_057[] PROGMEM = "fad";
const char cmd_058[] PROGMEM = "wip";
const char cmd_059[] PROGMEM = "esh";
const char cmd_060[] PROGMEM = "psh";
const char cmd_061[] PROGMEM = "pso";
const char cmd_062[] PROGMEM = "cfa";
const char cmd_063[] PROGMEM = "lop";
const char cmd_064[] PROGMEM = "hip";
const char cmd_065[] PROGMEM = "pon";
const char cmd_066[] PROGMEM = "pof";
const char cmd_067[] PROGMEM = "dem";
const char cmd_068[] PROGMEM = "sbc";
const char cmd_069[] PROGMEM = "sbp";
const char cmd_070[] PROGMEM = "sch";
const char cmd_071[] PROGMEM = "car";
const char cmd_072[] PROGMEM = "sbt";
const char cmd_073[] PROGMEM = "set";
const char cmd_074[] PROGMEM = "run";
const char cmd_075[] PROGMEM = "del";

const char* const command_strings[] PROGMEM = {
   cmd_001, cmd_002, cmd_003, cmd_004, cmd_005, cmd_006, cmd_007, cmd_008, cmd_009, cmd_010,
   cmd_011, cmd_012, cmd_013, cmd_014, cmd_015, cmd_016, cmd_017, cmd_018, cmd_019, cmd_020,
   cmd_021, cmd_022, cmd_023, cmd_024, cmd_025, cmd_026, cmd_027, cmd_028, cmd_029, cmd_030,
   cmd_031, cmd_032, cmd_033, cmd_034, cmd_035, cmd_036, cmd_037, cmd_038, cmd_039, cmd_040,
   cmd_041, cmd_042, cmd_043, cmd_044, cmd_045, cmd_046, cmd_047, cmd_048, cmd_049, cmd_050,
   cmd_051, cmd_052, cmd_053, cmd_054, cmd_055, cmd_056, cmd_057, cmd_058, cmd_059, cmd_060,
   cmd_061, cmd_062, cmd_063, cmd_064, cmd_065, cmd_066, cmd_067, cmd_068, cmd_069, cmd_070,
   cmd_071, cmd_072, cmd_073, cmd_074, cmd_075
};

#endif

