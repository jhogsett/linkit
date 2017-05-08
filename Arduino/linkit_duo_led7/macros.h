#ifndef MACROS_H
#define MACROS_H

#include "dependencies.h"
#include "config.h"
#include "eeprom.h"

extern Dependencies dependencies;

extern bool dispatch_command(int cmd, char *dispatch_data);

#define MAX_MEMORY_MACRO (NUM_MEMORY_MACROS - 1)
#define MAX_EEPROM_MACRO (EEPROM_STARTING_MACRO + NUM_EEPROM_MACROS - 1)
#define MAX_MACRO (NUM_MACROS - 1)

// in-memory macros
char macros[NUM_MEMORY_MACROS][NUM_MACRO_CHARS];

bool is_memory_macro(int macro){
  return macro >= 0 && macro <= MAX_MEMORY_MACRO;
}

bool is_eeprom_macro(int macro){
  return macro >= EEPROM_STARTING_MACRO && macro <= MAX_EEPROM_MACRO;
}

// get a read/write pointer to a memory macro slot
char * get_memory_macro(int macro){
  if(macro < 0 || macro > MAX_MEMORY_MACRO){
    return NULL;
  }
  return macros[macro];
}

// get an eeprom accessible pointer to an eeprom macro slot
char * get_eeprom_macro(int macro){
  int effective_macro = macro - EEPROM_STARTING_MACRO;
  return (char *)(effective_macro * NUM_MACRO_CHARS);
}

void set_memory_macro_from_memory(int macro, char * buffer){
  char * str = ::get_memory_macro(macro);
  if(str != NULL)
    strcpy(str, buffer);   
}

void set_eeprom_macro_from_memory(int macro, char * buffer){
  char * str = ::get_eeprom_macro(macro);
  strcpy_eeprom_from_memory(str, buffer);   
}

// used with the "stm" command to set a macro from memory
// buffer is the "rest of buffer" pointer to either memory or eeprom
void set_macro_from_memory(int macro, char * buffer){
  if(is_memory_macro(macro))
    set_memory_macro_from_memory(macro, buffer);
  else if(is_eeprom_macro(macro))
    set_eeprom_macro_from_memory(macro, buffer);
}

// used with the "set" command to set a macro from the serial input buffer
void set_macro_from_serial(int macro){
  set_macro_from_memory(macro, dependencies.command_processor.get_input_buffer());
}

// copies the macro from memory or eeprom to the tokenizing buffer
void get_macro_for_processing(int macro, char * buffer){
  if(is_memory_macro(macro))
    strcpy(buffer, get_memory_macro(macro));
  else if(is_eeprom_macro(macro))
    strcpy_memory_from_eeprom(buffer, get_eeprom_macro(macro));   
}

// process the series of commands and arguments in the passed memory buffer
// the string must be tokenizable by strtok (get's corrupted)
void process_commands(char * buffer){
  if(buffer == NULL || *buffer == '\0'){
    return;
  }

  // point to the end of the buffer for overrun protection
  // this points at the terminator
  char *last_char = buffer + strlen(buffer);

  // begin_get_commands() and get_next_command() need an external pointer
  // to hold onto the strtok_r state
  char *saveptr;

  // get the first command or set of arguments
  char *command = dependencies.command_processor.begin_get_commands(buffer, &saveptr);

  // point to the remaining string after this command + terminator
  // this is needed for copying strings when setting macros
  // if this is the last command in the string, the location 
  //   1 past the string + terminator overruns the end of the buffer
  char * rest_of_buffer = min(command + strlen(command) + 1, last_char);

  // process the command or arguments
  int cmd = dependencies.command_processor.process_command(command);

  if(cmd == CMD_NULL){
    // there was no command or arguments
    return;
  }

  do{
    // CMD_NONE is returned when there are arguments instead of a command
    // arguments are not dispatched they're captured in CommandProcessor::process_command()
    if(cmd != CMD_NONE){
      // send the command to the dispatcher to be run
      // pass the pointer to the rest of the buffer 
      //   in case it's needed to set a macro
      if(!::dispatch_command(cmd, rest_of_buffer)){
        // false means the rest of the buffer has been copied 
        // so there are no more commands to process 
        return;
      }
    }

    // get the next set command or argumemts
    command = dependencies.command_processor.get_next_command(&saveptr);
    rest_of_buffer = min(command + strlen(command) + 1, last_char);
    cmd = dependencies.command_processor.process_command(command);
    
  }while(cmd != CMD_NULL);
}

void run_macro(int macro, int times, int delay_ = 0){
  // don't pass in this macro running's arguments
  dependencies.command_processor.reset_args();
  
  if(times < 1){
    times = 1;
  }

  // copy the macro into the tokenizing buffer
  // either from memory or eeprom
  char buffer[NUM_MACRO_CHARS]; 

  for(int i = 0; i < times; i++){     
    get_macro_for_processing(macro, buffer);

    if(buffer != '\0')
      process_commands(buffer);

    if(delay_ != 0)
      delay(delay_);

    if(buffer == '\0')
      break;
  }
}

void reset_macro(int macro){
  if(is_memory_macro(macro))
    ::get_memory_macro(macro)[0] = '\0';
  else if(is_eeprom_macro(macro))
    strcpy_eeprom_from_memory(::get_eeprom_macro(macro), "\0");   
}

void reset_all_macros(){
  for(int i = 0; i < NUM_MACROS; i++)
    reset_macro(i);
}

bool is_programmed(byte macro){
  return ::eeprom_is_programmed(::get_eeprom_macro(macro));
}

#endif

