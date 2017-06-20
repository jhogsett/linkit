#ifndef COMMANDS_DEF_H
#define COMMANDS_DEF_H

#define NUM_COMMANDS     99
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
#define CMD_AMBER        32
#define CMD_OLIVE        33
#define CMD_SKYBLUE      34
#define CMD_TURQUOISE    35
#define CMD_LAVENDER     36
#define CMD_ROSE         37
#define CMD_RANDOM       38
#define CMD_BLEND        39
#define CMD_MAX          40
#define CMD_DIM          41
#define CMD_BRIGHT       42
#define CMD_BLINK        43
#define CMD_BLINK1       44
#define CMD_BLINK2       45
#define CMD_BLINK3       46
#define CMD_BLINK4       47
#define CMD_BLINK5       48
#define CMD_BLINK6       49
#define CMD_BLINKA       50
#define CMD_BLINKB       51
#define CMD_BLINKC       52
#define CMD_BREATHE      53
#define CMD_SLOW_FADE    54
#define CMD_FAST_FADE    55
#define CMD_TWINKLE      56
#define CMD_RAW          57 
#define CMD_STATIC       58
#define CMD_BLINKR       59
#define CMD_EFFECTR      60
#define CMD_PAUSE        61
#define CMD_CONTINUE     62
#define CMD_RESET        63
#define CMD_CLEAR        64
#define CMD_LEVEL        65
#define CMD_FADE         66
#define CMD_WIPE         67
#define CMD_ANIM_ROTATE  68
#define CMD_PSHIFT       69
#define CMD_PSHIFTO      70
#define CMD_CFADE        71
#define CMD_TEST         72
#define CMD_HIPOWER_OPEN      73
#define CMD_PINON        74
#define CMD_PINOFF       75
#define CMD_SETFADERATE  76
#define CMD_SETBLINKC    77
#define CMD_SETBLINKP    78
#define CMD_SCHEDULE     79
#define CMD_CARRY        80
#define CMD_SETBREATHET  81
#define CMD_SET_MACRO    82
#define CMD_RUN_MACRO    83
#define CMD_DELAY        84
#define CMD_STOP         85
#define CMD_RANDOM_NUM   86
#define CMD_POSITION     87
#define CMD_RPOSITION    88
#define CMD_PALETTE      89
#define CMD_SHUFFLE      90
#define CMD_SETBLACK     91
#define CMD_SEQ_WHEEL    92
#define CMD_SEQ_SWING    93
#define CMD_SEQ_DRIVE    94
#define CMD_SEQ_BOUND    95
#define CMD_SEQ_WHLCO    96
#define CMD_SEQ_SWGCO    97
#define CMD_SEQ_WHLSN    98
#define CMD_SEQ_SWGSN    99

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
const char cmd_032[] PROGMEM = "amb";
const char cmd_033[] PROGMEM = "olv";
const char cmd_034[] PROGMEM = "sky";
const char cmd_035[] PROGMEM = "tur";
const char cmd_036[] PROGMEM = "lav";
const char cmd_037[] PROGMEM = "ros";
const char cmd_038[] PROGMEM = "rnd";
const char cmd_039[] PROGMEM = "ble";
const char cmd_040[] PROGMEM = "max";
const char cmd_041[] PROGMEM = "dim";
const char cmd_042[] PROGMEM = "brt";
const char cmd_043[] PROGMEM = "bli";
const char cmd_044[] PROGMEM = "bl1";
const char cmd_045[] PROGMEM = "bl2";
const char cmd_046[] PROGMEM = "bl3";
const char cmd_047[] PROGMEM = "bl4";
const char cmd_048[] PROGMEM = "bl5";
const char cmd_049[] PROGMEM = "bl6";
const char cmd_050[] PROGMEM = "bla";
const char cmd_051[] PROGMEM = "blb";
const char cmd_052[] PROGMEM = "blc";
const char cmd_053[] PROGMEM = "bre";
const char cmd_054[] PROGMEM = "sfd";
const char cmd_055[] PROGMEM = "ffd";
const char cmd_056[] PROGMEM = "twi";
const char cmd_057[] PROGMEM = "raw";
const char cmd_058[] PROGMEM = "sta";
const char cmd_059[] PROGMEM = "blr";
const char cmd_060[] PROGMEM = "efr";
const char cmd_061[] PROGMEM = "pau";
const char cmd_062[] PROGMEM = "cnt";
const char cmd_063[] PROGMEM = "rst";
const char cmd_064[] PROGMEM = "clr";
const char cmd_065[] PROGMEM = "lev";
const char cmd_066[] PROGMEM = "fad";
const char cmd_067[] PROGMEM = "wip";
const char cmd_068[] PROGMEM = "art";
const char cmd_069[] PROGMEM = "psh";
const char cmd_070[] PROGMEM = "pso";
const char cmd_071[] PROGMEM = "cfa";
const char cmd_072[] PROGMEM = "tst";
const char cmd_073[] PROGMEM = "hip";
const char cmd_074[] PROGMEM = "pon";
const char cmd_075[] PROGMEM = "pof";
const char cmd_076[] PROGMEM = "sfr";
const char cmd_077[] PROGMEM = "sbc";
const char cmd_078[] PROGMEM = "sbp";
const char cmd_079[] PROGMEM = "sch";
const char cmd_080[] PROGMEM = "car";
const char cmd_081[] PROGMEM = "sbt";
const char cmd_082[] PROGMEM = "set";
const char cmd_083[] PROGMEM = "run";
const char cmd_084[] PROGMEM = "del";
const char cmd_085[] PROGMEM = "stp";
const char cmd_086[] PROGMEM = "rng";
const char cmd_087[] PROGMEM = "pos";
const char cmd_088[] PROGMEM = "rps";
const char cmd_089[] PROGMEM = "pal";
const char cmd_090[] PROGMEM = "shf";
const char cmd_091[] PROGMEM = "sbl";
const char cmd_092[] PROGMEM = "seq";
const char cmd_093[] PROGMEM = "sqs";
const char cmd_094[] PROGMEM = "sqd";
const char cmd_095[] PROGMEM = "sqb";
const char cmd_096[] PROGMEM = "swc";
const char cmd_097[] PROGMEM = "ssc";
const char cmd_098[] PROGMEM = "sws";
const char cmd_099[] PROGMEM = "sss";

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
   cmd_091, cmd_092, cmd_093, cmd_094, cmd_095, cmd_096, cmd_097, cmd_098, cmd_099
};

#endif

