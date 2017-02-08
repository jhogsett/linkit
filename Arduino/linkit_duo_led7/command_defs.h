#ifndef COMMANDS_DEF_H
#define COMMANDS_DEF_H

#define CMD_NONE       0
#define CMD_FIRST      1

#define NUM_COMMANDS  134
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
#define CMD_RANDOM       31
#define CMD_BLEND        32
#define CMD_MAX          33
#define CMD_DIM          34
#define CMD_BRIGHT       35
#define CMD_BLINK        36
#define CMD_BLINK1       37
#define CMD_BLINK2       38
#define CMD_BLINK3       39
#define CMD_BLINK4       40
#define CMD_BLINK5       41
#define CMD_BLINK6       42
#define CMD_BLINKR       43
#define CMD_BLINKA       44
#define CMD_BLINKB       45
#define CMD_BLINKC       46
#define CMD_BREATHE      47
#define CMD_EFFECTR      48
#define CMD_STATIC       49
#define CMD_PAUSE        50
#define CMD_CONTINUE     51
#define CMD_RESET        52
#define CMD_CLEAR        53
#define CMD_LEVEL        54
#define CMD_FADE         55
#define CMD_WIPE         56
#define CMD_ESHIFT       57
#define CMD_PSHIFT       58
#define CMD_PSHIFTO      59
#define CMD_CFADE        60
#define CMD_LOPOWER      61
#define CMD_HIPOWER      62
#define CMD_PINON        63
#define CMD_PINOFF       64
#define CMD_DEMO         65
#define CMD_FLUSHOLD     66
#define CMD_ERASEOLD     67
#define CMD_ROTATEOLD    68
#define CMD_REPEATOLD    69
#define CMD_COPYOLD      70
#define CMD_FLOODOLD     71
#define CMD_MIRROROLD    72
#define CMD_DISPLAYOLD   73
#define CMD_ZONEOLD      74
#define CMD_WINDOWOLD    75
#define CMD_OFFSETOLD    76
#define CMD_REVERSEOLD   77
#define CMD_RGBCOLOROLD  78
#define CMD_HSLCOLOROLD  79
#define CMD_REDOLD       80
#define CMD_GREENOLD     81
#define CMD_BLUEOLD      82
#define CMD_BLACKOLD     83
#define CMD_YELLOWOLD    84
#define CMD_ORANGEOLD    85
#define CMD_PURPLEOLD    86
#define CMD_CYANOLD      87
#define CMD_MAGENTAOLD   88
#define CMD_PINKOLD      89
#define CMD_WHITEOLD     90
#define CMD_GRAYOLD      91
#define CMD_LTGREENOLD   92
#define CMD_SEAFOAMOLD   93
#define CMD_LTBLUEOLD    94
#define CMD_DKGRAYOLD    95
#define CMD_RANDOMOLD    96
#define CMD_BLENDOLD     97
#define CMD_MAXOLD       98
#define CMD_DIMOLD       99
#define CMD_BRIGHTOLD   100
#define CMD_BLINKOLD    101
#define CMD_BLINK1OLD   102
#define CMD_BLINK2OLD   103
#define CMD_BLINK3OLD   104
#define CMD_BLINK4OLD   105
#define CMD_BLINK5OLD   106
#define CMD_BLINK6OLD   107
#define CMD_BLINKROLD   108
#define CMD_BLINKAOLD   109
#define CMD_BLINKBOLD   110
#define CMD_BLINKCOLD   111
#define CMD_BREATHEOLD  112
#define CMD_EFFECTROLD  113
#define CMD_STATICOLD   114
#define CMD_PAUSEOLD    115
#define CMD_CONTINUEOLD 116
#define CMD_RESETOLD    117
#define CMD_CLEAROLD    118
#define CMD_LEVELOLD    119
#define CMD_FADEOLD     120
#define CMD_WIPEOLD     121
#define CMD_ESHIFTOLD   122
#define CMD_PSHIFTOLD   123
#define CMD_PSHIFTOOLD  124
#define CMD_CFADEOLD    125
#define CMD_LOPOWEROLD  126
#define CMD_HIPOWEROLD  127
#define CMD_PINONOLD    128
#define CMD_PINOFFOLD   129
#define CMD_DEMOOLD     130
#define CMD_SETBLINKC   131
#define CMD_SETBLINKP   132
#define CMD_SCHEDULE    133
#define CMD_CARRY       134

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
const char cmd_031[] PROGMEM = "rnd";
const char cmd_032[] PROGMEM = "ble";
const char cmd_033[] PROGMEM = "max";
const char cmd_034[] PROGMEM = "dim";
const char cmd_035[] PROGMEM = "brt";
const char cmd_036[] PROGMEM = "bli";
const char cmd_037[] PROGMEM = "bl1";
const char cmd_038[] PROGMEM = "bl2";
const char cmd_039[] PROGMEM = "bl3";
const char cmd_040[] PROGMEM = "bl4";
const char cmd_041[] PROGMEM = "bl5";
const char cmd_042[] PROGMEM = "bl6";
const char cmd_043[] PROGMEM = "blr";
const char cmd_044[] PROGMEM = "bla";
const char cmd_045[] PROGMEM = "blb";
const char cmd_046[] PROGMEM = "blc";
const char cmd_047[] PROGMEM = "bre";
const char cmd_048[] PROGMEM = "efr";
const char cmd_049[] PROGMEM = "sta";
const char cmd_050[] PROGMEM = "pau";
const char cmd_051[] PROGMEM = "cnt";
const char cmd_052[] PROGMEM = "rst";
const char cmd_053[] PROGMEM = "clr";
const char cmd_054[] PROGMEM = "lev";
const char cmd_055[] PROGMEM = "fad";
const char cmd_056[] PROGMEM = "wip";
const char cmd_057[] PROGMEM = "esh";
const char cmd_058[] PROGMEM = "psh";
const char cmd_059[] PROGMEM = "pso";
const char cmd_060[] PROGMEM = "cfa";
const char cmd_061[] PROGMEM = "lop";
const char cmd_062[] PROGMEM = "hip";
const char cmd_063[] PROGMEM = "pon";
const char cmd_064[] PROGMEM = "pof";
const char cmd_065[] PROGMEM = "dem";
const char cmd_066[] PROGMEM = "flush";
const char cmd_067[] PROGMEM = "erase";
const char cmd_068[] PROGMEM = "rotate";
const char cmd_069[] PROGMEM = "repeat";
const char cmd_070[] PROGMEM = "copy";
const char cmd_071[] PROGMEM = "flood";
const char cmd_072[] PROGMEM = "mirror";
const char cmd_073[] PROGMEM = "display";
const char cmd_074[] PROGMEM = "zone";
const char cmd_075[] PROGMEM = "window";
const char cmd_076[] PROGMEM = "offset";
const char cmd_077[] PROGMEM = "reverse";
const char cmd_078[] PROGMEM = "rgbcolor";
const char cmd_079[] PROGMEM = "hslcolor";
const char cmd_080[] PROGMEM = "red";
const char cmd_081[] PROGMEM = "green";
const char cmd_082[] PROGMEM = "blue";
const char cmd_083[] PROGMEM = "black";
const char cmd_084[] PROGMEM = "yellow";
const char cmd_085[] PROGMEM = "orange";
const char cmd_086[] PROGMEM = "purple";
const char cmd_087[] PROGMEM = "cyan";
const char cmd_088[] PROGMEM = "magenta";
const char cmd_089[] PROGMEM = "pink";
const char cmd_090[] PROGMEM = "white";
const char cmd_091[] PROGMEM = "gray";
const char cmd_092[] PROGMEM = "ltgreen";
const char cmd_093[] PROGMEM = "seafoam";
const char cmd_094[] PROGMEM = "ltblue";
const char cmd_095[] PROGMEM = "dkgray";
const char cmd_096[] PROGMEM = "random";
const char cmd_097[] PROGMEM = "blend";
const char cmd_098[] PROGMEM = "max";
const char cmd_099[] PROGMEM = "dim";
const char cmd_100[] PROGMEM = "bright";
const char cmd_101[] PROGMEM = "blink";
const char cmd_102[] PROGMEM = "blink1";
const char cmd_103[] PROGMEM = "blink2";
const char cmd_104[] PROGMEM = "blink3";
const char cmd_105[] PROGMEM = "blink4";
const char cmd_106[] PROGMEM = "blink5";
const char cmd_107[] PROGMEM = "blink6";
const char cmd_108[] PROGMEM = "blinkr";
const char cmd_109[] PROGMEM = "blinka";
const char cmd_110[] PROGMEM = "blinkb";
const char cmd_111[] PROGMEM = "blinkc";
const char cmd_112[] PROGMEM = "breathe";
const char cmd_113[] PROGMEM = "effectr";
const char cmd_114[] PROGMEM = "static";
const char cmd_115[] PROGMEM = "pause";
const char cmd_116[] PROGMEM = "continue";
const char cmd_117[] PROGMEM = "reset";
const char cmd_118[] PROGMEM = "clear";
const char cmd_119[] PROGMEM = "level";
const char cmd_120[] PROGMEM = "fade";
const char cmd_121[] PROGMEM = "wipe";
const char cmd_122[] PROGMEM = "eshift";
const char cmd_123[] PROGMEM = "pshift";
const char cmd_124[] PROGMEM = "pshifto";
const char cmd_125[] PROGMEM = "cfade";
const char cmd_126[] PROGMEM = "lopower";
const char cmd_127[] PROGMEM = "hipower";
const char cmd_128[] PROGMEM = "pinon";
const char cmd_129[] PROGMEM = "pinoff";
const char cmd_130[] PROGMEM = "demo";
const char cmd_131[] PROGMEM = "sbc";
const char cmd_132[] PROGMEM = "sbp";
const char cmd_133[] PROGMEM = "sch";
const char cmd_134[] PROGMEM = "car";

const char* const command_strings[] PROGMEM = {
   cmd_001, cmd_002, cmd_003, cmd_004, cmd_005, cmd_006, cmd_007, cmd_008, cmd_009, cmd_010,
   cmd_011, cmd_012, cmd_013, cmd_014, cmd_015, cmd_016, cmd_017, cmd_018, cmd_019, cmd_020,
   cmd_021, cmd_022, cmd_023, cmd_024, cmd_025, cmd_026, cmd_027, cmd_028, cmd_029, cmd_030,
   cmd_031, cmd_032, cmd_033, cmd_034, cmd_035, cmd_036, cmd_037, cmd_038, cmd_039, cmd_040,
   cmd_041, cmd_042, cmd_043, cmd_044, cmd_045, cmd_046, cmd_047, cmd_048, cmd_049, cmd_050,
   cmd_051, cmd_052, cmd_053, cmd_054, cmd_055, cmd_056, cmd_057, cmd_058, cmd_059, cmd_060,
   cmd_061, cmd_062, cmd_063, cmd_064, cmd_065, cmd_066, cmd_067, cmd_068, cmd_069, cmd_070,
   cmd_071, cmd_072, cmd_073, cmd_074, cmd_075, cmd_076, cmd_077, cmd_078, cmd_079, cmd_080,
   cmd_081, cmd_082, cmd_083, cmd_084, cmd_085, cmd_086, cmd_087, cmd_088, cmd_089, cmd_090,
   cmd_091, cmd_092, cmd_093, cmd_094, cmd_095, cmd_096, cmd_097, cmd_098, cmd_099, cmd_100,
   cmd_101, cmd_102, cmd_103, cmd_104, cmd_105, cmd_106, cmd_107, cmd_108, cmd_109, cmd_110,
   cmd_111, cmd_112, cmd_113, cmd_114, cmd_115, cmd_116, cmd_117, cmd_118, cmd_119, cmd_120,
   cmd_121, cmd_122, cmd_123, cmd_124, cmd_125, cmd_126, cmd_127, cmd_128, cmd_129, cmd_130,
   cmd_131, cmd_132, cmd_133, cmd_134
};

#endif

