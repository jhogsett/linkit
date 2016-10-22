/*******************************************************************/

#define DELIMITER_CHAR ':'
#define MAX_STRING_LENGTH 100
#define NUM_SUB_ARGS 3

char str[MAX_STRING_LENGTH];
char arg[MAX_STRING_LENGTH];
int sub_args[NUM_SUB_ARGS] = { 0, 0, 0 };

bool received_command(){
  if(Serial1.available() > 0){
    int c = Serial1.readBytesUntil(DELIMITER_CHAR, str, MAX_STRING_LENGTH);
    str[c] = 0;
    return true;
  } else {
    return false;
  }
}

bool str_equal(char *str1, char *str2){
  int l = strlen(str1);
  if(l != strlen(str2))
    return false;

  for(int i = 0; i < l; i++){
    if(str1[i] != str2[i])
      return false;
  }
  return true;
}

bool is_command(char *str, char *command){
//  return strcmp(str, command) == 0;  
  return str_equal(str, command);  
}

int get_sub_args(char * args){
  char *arg = strtok(args, ",");
  sub_args[0] = atoi(arg);
  arg = strtok(NULL, ",");
  sub_args[1] = atoi(arg);
  arg = strtok(NULL, ",");
  sub_args[2] = atoi(arg);
}

void save_args(char *str){
  strcpy(arg, str); 
  get_sub_args(arg);
}

void reset_args(){
  strcpy(arg, "");
}

void send_ack(){
  Serial1.write("k");
}

#define NUM_COMMANDS 51
char *commands[] = {
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
  "efade",
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
  "repeat"
};

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
#define CMD_EFADE     22
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

int get_command(char *cmd){
  for(int i = 0; i < NUM_COMMANDS; i++){
    if(is_command(cmd, commands[i])){       //    if(strcmp(cmd, commands[i]) == 0){
      return CMD_FIRST + i;
    }
  }
  return CMD_NONE;
}

void dispatch_command(int cmd){
  switch(cmd){
    case CMD_NONE:      save_args(str);                                                       break;
    case CMD_PAUSE:     paused = true;                                                        break;
    case CMD_CONTINUE:  paused = false;                                                       break;
    case CMD_ERASE:     erase(true);                                                          break;
    case CMD_BLINK:     start_effect(BLINK_ON);                                               break;
    case CMD_BLINK1:    start_effect(BLINK_ON_1);                                             break;
    case CMD_BLINK2:    start_effect(BLINK_ON_2);                                             break;
    case CMD_BLINK3:    start_effect(BLINK_ON_3);                                             break;
    case CMD_BLINK4:    start_effect(BLINK_ON_4);                                             break;
    case CMD_BLINK5:    start_effect(BLINK_ON_5);                                             break;
    case CMD_BLINK6:    start_effect(BLINK_ON_6);                                             break;
    case CMD_BLINKR:    start_blinking_r();                                                   break;
    case CMD_BLINKA:    start_effect(BLINK_ON_A);                                             break;
    case CMD_BLINKB:    start_effect(BLINK_ON_B);                                             break;
    case CMD_BREATHE:   start_effect(BREATHE_ON);                                             break;
    case CMD_EFFECTR:   start_effect_r();                                                     break;
    case CMD_FLUSH:     flush();                                                              break;
    case CMD_BLEND:     do_blend();                                                           break;
    case CMD_MAX:       do_max();                                                             break;
    case CMD_DIM:       do_dim();                                                             break;
    case CMD_BRIGHT:    do_bright();                                                          break;
    case CMD_FADE:      do_fade();                                                            break;
    case CMD_EFADE:     do_exhale_fade();                                                     break;
    case CMD_FLOOD:     do_flood();                                                           break;
    case CMD_RANDOM:    do_random();                                                          break; 
    case CMD_MIRROR:    do_mirror();                                                          break;
    case CMD_ESHIFT:    do_elastic_shift(sub_args[0]); reset_args();                          break;
    case CMD_PSHIFT:    do_power_shift(sub_args[0]); reset_args();                            break;
    case CMD_PSHIFTO:   power_shift_object(sub_args[0], sub_args[1]); reset_args();           break;
    case CMD_WINDOW:    set_window(sub_args[0]); reset_args();                                break;
    case CMD_RESET:     reset();                                                              break;
    case CMD_DEMO:      do_demo();                                                            break;
    case CMD_STATIC:    start_effect(STATIC_ON);                                              break;
    case CMD_RGBCOLOR:  push_rgb_color(sub_args[0], sub_args[1], sub_args[2]); reset_args();  break;
    case CMD_HSLCOLOR:  push_hsl_color(sub_args[0], sub_args[1], sub_args[2]); reset_args();  break;
    case CMD_RED:       push_color(red);                                                      break;
    case CMD_GREEN:     push_color(green);                                                    break;
    case CMD_BLUE:      push_color(blue);                                                     break;
    case CMD_BLACK:     push_color(black);                                                    break;
    case CMD_YELLOW:    push_color(yellow);                                                   break;
    case CMD_ORANGE:    push_color(orange);                                                   break;
    case CMD_PURPLE:    push_color(purple);                                                   break;
    case CMD_CYAN:      push_color(cyan);                                                     break;
    case CMD_MAGENTA:   push_color(magenta);                                                  break; 
    case CMD_PINK:      push_color(pink);                                                     break; 
    case CMD_WHITE:     push_color(white);                                                    break; 
    case CMD_GRAY:      push_color(gray);                                                     break;
    case CMD_LTGREEN:   push_color(ltgreen);                                                  break;
    case CMD_SEAFOAM:   push_color(seafoam);                                                  break;
    case CMD_LTBLUE:    push_color(ltblue);                                                   break;
    case CMD_DKGRAY:    push_color(dkgray);                                                   break;
    case CMD_REPEAT:    do_repeat(sub_args[0]); reset_args();                                 break;
  }
}

