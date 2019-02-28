#ifndef COMMANDS_DEF_H
#define COMMANDS_DEF_H

#define NUM_COMMANDS    103
#define CMD_NULL         -1
#define CMD_NONE          0
#define CMD_FIRST         1
#define CMD_FIRST_EFFECT 48

#define CMD_FLUSH         1
#define CMD_RESET         2 
#define CMD_ERASE         3
#define CMD_RUN_MACRO     4 

#define CMD_PALETTE       5 // move up

#define CMD_ROTATE        6 // move down
#define CMD_REPEAT        7 // move down

#define CMD_COPY          8

#define CMD_FLOOD         9
#define CMD_MIRROR       10
#define CMD_DISPLAY      11
#define CMD_ZONE         12
#define CMD_WINDOW       13
#define CMD_OFFSET       14
#define CMD_REVERSE      15
#define CMD_RGBCOLOR     16
#define CMD_HSLCOLOR     17
#define CMD_RED          18
#define CMD_ORANGE       19
#define CMD_YELLOW       20
#define CMD_GREEN        21
#define CMD_BLUE         22
#define CMD_PURPLE       23
#define CMD_CYAN         24
#define CMD_MAGENTA      25
#define CMD_LTBLUE       26
#define CMD_LTGREEN      27
#define CMD_SEAFOAM      28
#define CMD_PINK         29
#define CMD_AMBER        30
#define CMD_OLIVE        31
#define CMD_SKYBLUE      32
#define CMD_TURQUOISE    33
#define CMD_LAVENDER     34
#define CMD_ROSE         35
#define CMD_DKGRAY       36
#define CMD_GRAY         37
#define CMD_WHITE        38
#define CMD_TUNGSTEN     39
#define CMD_NEON         40
#define CMD_SODIUM       41
#define CMD_BLACK        42

#define CMD_RANDOM       43
#define CMD_DYN_COLOR    44
#define CMD_BLEND        45
#define CMD_DIM          46
#define CMD_BRIGHT       47
#define CMD_BLINK        48
#define CMD_BLINK1       49
#define CMD_BLINK2       50
#define CMD_BLINK3       51
#define CMD_BLINK4       52
#define CMD_BLINK5       53
#define CMD_BLINK6       54
#define CMD_BLINKA       55
#define CMD_BLINKB       56
#define CMD_BLINKD       57
#define CMD_BREATHE      58
#define CMD_BREATHE_D    59
#define CMD_SLOW_FADE    60
#define CMD_FAST_FADE    61
#define CMD_STORE        62 
#define CMD_RECALL       63
#define CMD_PUSH         64
#define CMD_PAUSE        65 // move down
#define CMD_CONTINUE     66
#define CMD_CLEAR        67 // move down
#define CMD_LEVEL        68
#define CMD_ANIM_ROTATE  69 // move down
#define CMD_CFADE        70
#define CMD_TEST         71 // move down
#define CMD_CONFIGURE    72
#define CMD_PIN          73 
#define CMD_SCHEDULE     74
#define CMD_CARRY        75 
#define CMD_SET_MACRO    76
#define CMD_DELAY        77
#define CMD_STOP         78
#define CMD_RANDOM_NUM   79 // move down

#define CMD_POSITION     80 // move up

#define CMD_RPOSITION    81
#define CMD_XYPOSITION   82

#define CMD_SHUFFLE      83 // move down
#define CMD_SETBLACK     84

#define CMD_SEQ_WHEEL    85 // move up

#define CMD_SEQ_SWING    86
#define CMD_SEQ_WHLCO    87
#define CMD_SEQ_WHLSN    88
#define CMD_SEQ_NEXTW    89
#define CMD_CLR_SEQ_HUE  90
#define CMD_CLR_SEQ_SAT  91
#define CMD_CLR_SEQ_LIT  92

#define CMD_FAN          93 // move down
#define CMD_APP          94

#define CMD_ADD          95 // move math ones up
#define CMD_SUB          96
#define CMD_MUL          97
#define CMD_DIV          98
#define CMD_MOD          99
#define CMD_DIF         100
#define CMD_AVG         101

#define CMD_DRAW_MODE   102 // move up a bit
#define CMD_POP         103 // MOVE up

const char cmd_001[] PROGMEM = "flu";
const char cmd_002[] PROGMEM = "rst";
const char cmd_003[] PROGMEM = "era";
const char cmd_004[] PROGMEM = "run";
const char cmd_005[] PROGMEM = "pal"; // palette color

const char cmd_006[] PROGMEM = "rot";
const char cmd_007[] PROGMEM = "rep";
const char cmd_008[] PROGMEM = "cpy";
const char cmd_009[] PROGMEM = "flo";
const char cmd_010[] PROGMEM = "mir";
const char cmd_011[] PROGMEM = "dis";
const char cmd_012[] PROGMEM = "zon";
const char cmd_013[] PROGMEM = "win";
const char cmd_014[] PROGMEM = "off";
const char cmd_015[] PROGMEM = "rev";
const char cmd_016[] PROGMEM = "rgb";
const char cmd_017[] PROGMEM = "hsl";
const char cmd_018[] PROGMEM = "red";
const char cmd_019[] PROGMEM = "org";
const char cmd_020[] PROGMEM = "yel";
const char cmd_021[] PROGMEM = "grn";
const char cmd_022[] PROGMEM = "blu";
const char cmd_023[] PROGMEM = "pur";
const char cmd_024[] PROGMEM = "cyn";
const char cmd_025[] PROGMEM = "mag";
const char cmd_026[] PROGMEM = "lbl";
const char cmd_027[] PROGMEM = "lgr";
const char cmd_028[] PROGMEM = "sea";
const char cmd_029[] PROGMEM = "pnk";
const char cmd_030[] PROGMEM = "amb";
const char cmd_031[] PROGMEM = "olv";
const char cmd_032[] PROGMEM = "sky";
const char cmd_033[] PROGMEM = "tur";
const char cmd_034[] PROGMEM = "lav";
const char cmd_035[] PROGMEM = "ros";
const char cmd_036[] PROGMEM = "dgr";
const char cmd_037[] PROGMEM = "gry";
const char cmd_038[] PROGMEM = "wht";
const char cmd_039[] PROGMEM = "tun";
const char cmd_040[] PROGMEM = "neo";
const char cmd_041[] PROGMEM = "sod";
const char cmd_042[] PROGMEM = "blk";
const char cmd_043[] PROGMEM = "rnd";
const char cmd_044[] PROGMEM = "dyn"; // dynamic color rendering
const char cmd_045[] PROGMEM = "ble";
const char cmd_046[] PROGMEM = "dim";
const char cmd_047[] PROGMEM = "brt";
const char cmd_048[] PROGMEM = "bli";
const char cmd_049[] PROGMEM = "bl1";
const char cmd_050[] PROGMEM = "bl2";
const char cmd_051[] PROGMEM = "bl3";
const char cmd_052[] PROGMEM = "bl4";
const char cmd_053[] PROGMEM = "bl5";
const char cmd_054[] PROGMEM = "bl6";
const char cmd_055[] PROGMEM = "bla";
const char cmd_056[] PROGMEM = "blb";
const char cmd_057[] PROGMEM = "bld";
const char cmd_058[] PROGMEM = "bre";
const char cmd_059[] PROGMEM = "brd";
const char cmd_060[] PROGMEM = "sfd";
const char cmd_061[] PROGMEM = "ffd";
const char cmd_062[] PROGMEM = "sto";
const char cmd_063[] PROGMEM = "rcl";
const char cmd_064[] PROGMEM = "psh"; // push arg0 to accumulators
const char cmd_065[] PROGMEM = "pau";
const char cmd_066[] PROGMEM = "cnt";
const char cmd_067[] PROGMEM = "clr";
const char cmd_068[] PROGMEM = "lev";
const char cmd_069[] PROGMEM = "art";
const char cmd_070[] PROGMEM = "cfa";
const char cmd_071[] PROGMEM = "tst";
const char cmd_072[] PROGMEM = "cfg";
const char cmd_073[] PROGMEM = "pin";
const char cmd_074[] PROGMEM = "sch";
const char cmd_075[] PROGMEM = "car";
const char cmd_076[] PROGMEM = "set";
const char cmd_077[] PROGMEM = "del";
const char cmd_078[] PROGMEM = "stp";
const char cmd_079[] PROGMEM = "rng";
const char cmd_080[] PROGMEM = "pos"; // set position (, width)
const char cmd_081[] PROGMEM = "rps"; // random position within current window
const char cmd_082[] PROGMEM = "xyp"; // x,y position
const char cmd_083[] PROGMEM = "shf"; // shuffle/reset palette
const char cmd_084[] PROGMEM = "sbl"; // set black level
const char cmd_085[] PROGMEM = "seq"; // wheel
const char cmd_086[] PROGMEM = "sqs"; // swing
const char cmd_087[] PROGMEM = "swc"; // wheel cosine
const char cmd_088[] PROGMEM = "sws"; // wheel sine
const char cmd_089[] PROGMEM = "snw"; // sequence next window
const char cmd_090[] PROGMEM = "csh"; // color sequence by hue
const char cmd_091[] PROGMEM = "css"; // color sequence by saturation
const char cmd_092[] PROGMEM = "csl"; // color sequence by lightness
const char cmd_093[] PROGMEM = "fan";
const char cmd_094[] PROGMEM = "app";
const char cmd_095[] PROGMEM = "add"; // accumulator manipulation
const char cmd_096[] PROGMEM = "sub";
const char cmd_097[] PROGMEM = "mul";
const char cmd_098[] PROGMEM = "div";
const char cmd_099[] PROGMEM = "mod";
const char cmd_100[] PROGMEM = "dif";
const char cmd_101[] PROGMEM = "avg";
const char cmd_102[] PROGMEM = "drw";
const char cmd_103[] PROGMEM = "pop";

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
   cmd_101, cmd_102, cmd_103
};

#endif
