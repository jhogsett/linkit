#include "command_defs.h"

#define DELIMITER_CHAR ':'
#define MAX_STRING_LENGTH 16
#define NUM_SUB_ARGS 3

class CommandProcessor 
{
  public:
  static char str[];
  static char arg[];
  static int sub_args[];

  bool received_command();
  void send_ack();
  int get_command();
  void dispatch_command();

  private:
  void save_args();
  bool str_equal(char *str1, char *str2);
  bool is_command(char *str, char *command);
  int get_sub_args();
  void reset_args();
};

char CommandProcessor::str[MAX_STRING_LENGTH];
char CommandProcessor::arg[MAX_STRING_LENGTH];
int CommandProcessor::sub_args[NUM_SUB_ARGS] = { 0, 0, 0 };

bool CommandProcessor::received_command(){
  if(Serial1.available() > 0){
    int c = Serial1.readBytesUntil(DELIMITER_CHAR, str, MAX_STRING_LENGTH);
    str[c] = 0;
    return true;
  } else {
    return false;
  }
}

void CommandProcessor::send_ack(){
  Serial1.write("k");
}

void CommandProcessor::save_args(){
  strcpy(arg, str); 
  get_sub_args();
}

bool CommandProcessor::str_equal(char *str1, char *str2){
  int l = strlen(str1);
  if(l != strlen(str2))
    return false;

  for(int i = 0; i < l; i++){
    if(str1[i] != str2[i])
      return false;
  }
  return true;
}

bool CommandProcessor::is_command(char *str, char *command){
  return str_equal(str, command);  
}

int CommandProcessor::get_sub_args(){
  char *token = strtok(arg, ",");
  sub_args[0] = atoi(token);
  token = strtok(NULL, ",");
  sub_args[1] = atoi(token);
  token = strtok(NULL, ",");
  sub_args[2] = atoi(token);
}

void CommandProcessor::reset_args(){
  strcpy(arg, "");
}

int CommandProcessor::get_command(){
  for(int i = 0; i < NUM_COMMANDS; i++){
    if(is_command(str, commands[i])){
      return CMD_FIRST + i;
    }
  }
  return CMD_NONE;
}

void CommandProcessor::dispatch_command(){
  int cmd = get_command();
  
  switch(cmd){
    case CMD_NONE:      save_args();                                                          break;
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

  if(Serial1.available() == 0) 
    // only acknowledge when there are no more commands available
    // so that multiple additional commands if any are processed first
    send_ack();
}

