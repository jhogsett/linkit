#ifndef MACROS_H
#define MACROS_H

#include "dependencies.h"

extern Dependencies dependencies;

extern void dispatch_command(int cmd);

#define NUM_MACROS 10
#define MAX_MACRO (NUM_MACROS - 1)
#define MACRO_CHARS 50

char macros[NUM_MACROS][MACRO_CHARS];

// to have macros run macros, need the buffer being tokenized to be allocated each entrance

void process_commands(char * commands){
  char *saveptr;
  char buffer[MACRO_CHARS];
 
  int command = dependencies.command_processor.begin_get_commands(commands, &saveptr, buffer);
  if(command == CMD_NULL){
    return;
  }

  do{
    if(command != CMD_NONE){
      ::dispatch_command(command);
    }
    command = dependencies.command_processor.get_next_command(&saveptr);
    
  }while(command != CMD_NULL);
}

char * get_macro(int macro){
  if(macro < 0 || macro > MAX_MACRO){
    return NULL;
  }
  return macros[macro];
}

void set_macro(int macro){
  char * str = ::get_macro(macro);
  if(str != NULL){
    strcpy(str, dependencies.command_processor.get_input_buffer());   
  }
}

void run_macro(int macro, int times){
  if(times < 1){
    times = 1;
  }
  char * str = ::get_macro(macro);
  if(str != NULL){
    for(int i = 0; i < times; i++){
      ::process_commands(str);
    }
  }
}

#endif

