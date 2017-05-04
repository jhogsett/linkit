#ifndef MACROS_H
#define MACROS_H

#include "dependencies.h"

extern Dependencies dependencies;

extern bool dispatch_command(int cmd, char *dispatch_data);

#if defined(RADIUS8)
#define NUM_MACROS 4
#else
#define NUM_MACROS 10
#endif

#define MAX_MACRO (NUM_MACROS - 1)
#define MACRO_CHARS 50

char macros[NUM_MACROS][MACRO_CHARS];

char * get_macro(int macro){
  if(macro < 0 || macro > MAX_MACRO){
    return NULL;
  }
  return macros[macro];
}

void set_macro_from_buffer(int macro, char * buffer){
  char * str = ::get_macro(macro);
  if(str != NULL){
    strcpy(str, buffer);   
  }
}

void process_commands(char * commands){
  char *saveptr;
  char buffer[MACRO_CHARS];
  char *rest_of_buffer;
 
  int command = dependencies.command_processor.begin_get_commands(commands, &saveptr, buffer, &rest_of_buffer);
  if(command == CMD_NULL){
    return;
  }

  do{
    if(command != CMD_NONE){
      if(!::dispatch_command(command, rest_of_buffer)){
        // false means the rest of the buffer has been copied and not 
        return;
      }
    }
    command = dependencies.command_processor.get_next_command(&saveptr, &rest_of_buffer);
    
  }while(command != CMD_NULL);
}

void set_macro_from_macro(int macro, char *rest_of_buffer){
  set_macro_from_buffer(macro, rest_of_buffer);
}

void set_macro(int macro){
  set_macro_from_buffer(macro, dependencies.command_processor.get_input_buffer());
}

void run_macro(int macro, int times){
  // don't pass in this macro running's arguments
  dependencies.command_processor.reset_args();
  
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

