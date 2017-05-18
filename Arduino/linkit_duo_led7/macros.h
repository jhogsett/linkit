//! not able to set a memory or eeprom macros from within an eeprom macro

#ifndef MACROS_H
#define MACROS_H

// add an instant macro function
// takes the dispatch data and calls process_command() N times

#include "dependencies.h"
#include "config.h"
#include "eeprom.h"

extern Dependencies dependencies;
extern bool dispatch_command(int cmd, char *dispatch_data);

#define MACRO_END_MARKER 0xff

#define MACRO_ARG1_MARKER 0xf9 
#define MACRO_ARG2_MARKER 0xfa 
#define MACRO_ARG3_MARKER 0xfb 
#define MACRO_ARG4_MARKER 0xfc 
#define MACRO_ARG5_MARKER 0xfd 
#define MACRO_ARG6_MARKER 0xfe 
#define ARG_MARKER_FIRST  MACRO_ARG1_MARKER
#define ARG_MARKER_LAST   MACRO_ARG6_MARKER

// this one doesn't need to be stored in macros, but it could be useful if supporting receiving binary commands
#define MACRO_CMD_MARKER 0xf0

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

int num_bytes_from_arg_marker(byte arg_marker){
  return (arg_marker - ARG_MARKER_FIRST) + 1;
}

int num_words_from_arg_marker(byte arg_marker){
  return num_bytes_from_arg_marker(arg_marker) / 2;
}

// copy bytes until the end of macro marker
void set_packed_memory_macro_from_memory(int macro, char * buffer){
  char * str = ::get_memory_macro(macro);

  byte b;
  while((b = *buffer++) != MACRO_END_MARKER){
    *str++ = b;

    if(b >= ARG_MARKER_FIRST && b <= ARG_MARKER_LAST){
      // copy the packed arguments, which can be any value 0-255
      // including the end of macro marker
      int num_bytes = num_bytes_from_arg_marker(b);
      
      for(int i = 0; i < num_bytes; i++){
        *str++ = *buffer++;  
      }
    }
  }
  *str = MACRO_END_MARKER;
}

// copy bytes until the end of macro marker
void set_packed_eeprom_macro_from_memory(int macro, char * buffer){
  char * str = ::get_eeprom_macro(macro);

  byte b;
  while((b = *buffer++) != MACRO_END_MARKER){
    eeprom_write_byte((byte*)str++, b);

    if(b >= ARG_MARKER_FIRST && b <= ARG_MARKER_LAST){
      // copy the packed arguments, which can be any value 0-255
      // including the end of macro marker
      int num_bytes = num_bytes_from_arg_marker(b);

      for(int i = 0; i < num_bytes; i++){
        eeprom_write_byte((byte*)str++, *buffer++);  
      }
    }
  }

  eeprom_write_byte((byte*)str, MACRO_END_MARKER);
}

// copy bytes until the end of macro marker
void set_packed_memory_macro_from_eeprom(int macro, char * buffer){
  char * str = ::get_memory_macro(macro);

  byte b;
  while((b = eeprom_read_byte((byte*)buffer++)) != MACRO_END_MARKER){
    *str++ = b;

    if(b >= ARG_MARKER_FIRST && b <= ARG_MARKER_LAST){
      // copy the packed arguments, which can be any value 0-255
      // including the end of macro marker
      int num_bytes = num_bytes_from_arg_marker(b);

      for(int i = 0; i < num_bytes; i++){
        *str++ = eeprom_read_byte((byte*)buffer++);  
      }
    }
  }
  *str = MACRO_END_MARKER;
}

// copy bytes until the end of macro marker
void set_packed_eeprom_macro_from_eeprom(int macro, char * buffer){
  char * str = ::get_eeprom_macro(macro);

  byte b;
  while((b = eeprom_read_byte((byte*)buffer++)) != MACRO_END_MARKER){
    eeprom_write_byte((byte*)str++, b);

    if(b >= ARG_MARKER_FIRST && b <= ARG_MARKER_LAST){
      // copy the packed arguments, which can be any value 0-255
      // including the end of macro marker
      int num_bytes = num_bytes_from_arg_marker(b);

      for(int i = 0; i < num_bytes; i++){
        eeprom_write_byte((byte*)str++, eeprom_read_byte((byte*)buffer++));  
      }
    }
  }
  
  eeprom_write_byte((byte*)str, MACRO_END_MARKER);
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

#define USE_COMMAND_BUFFER
// process commands stored in PROGMEM 
//void process_commands_P(const char * commands){
void process_commands_P(const __FlashStringHelper * commands){
#ifdef USE_COMMAND_BUFFER
  char * buffer = dependencies.command_processor.borrow_char_buffer();
#else
  // copy the macro into the tokenizing buffer
  char buffer[NUM_MACRO_CHARS]; // todo: can an existing buffer be used here?
#endif
  strcpy_P(buffer, (const char *)commands);
  process_commands(buffer);
}

void reset_macro(int macro){
  if(is_memory_macro(macro))
    ::get_memory_macro(macro)[0] = '\0';
  else if(is_eeprom_macro(macro))
    eeprom_write_byte((byte*)::get_eeprom_macro(macro), MACRO_END_MARKER);
}

void reset_all_macros(){
  for(int i = 0; i < NUM_MACROS; i++)
    reset_macro(i);
}

bool is_programmed(byte macro){
  return ::eeprom_is_programmed(::get_eeprom_macro(macro));
}

void determine_arg_marker(byte &arg_marker, byte &num_args){
  if(dependencies.command_processor.sub_args[2] != 0){
    arg_marker = MACRO_ARG6_MARKER;
    num_args = 3;
  } else if(dependencies.command_processor.sub_args[1] != 0){
    arg_marker = MACRO_ARG4_MARKER;
    num_args = 2;
  } else if(dependencies.command_processor.sub_args[0] >= 1 && dependencies.command_processor.sub_args[0] <= 255 ) {
    arg_marker = MACRO_ARG1_MARKER;
  } else if(dependencies.command_processor.sub_args[0] != 0){
    arg_marker = MACRO_ARG2_MARKER;
    num_args = 1;
  }
}

void set_packed_memory_macro(int macro, char * commands){
  char * macro_buffer = ::get_memory_macro(macro);
  if(macro_buffer == NULL)
    // not a valid memory macro location
    return;

  if(commands == NULL || *commands == '\0'){
    // no commands; empty the macro
    *macro_buffer = MACRO_END_MARKER;
    return;
  }

  // begin_get_commands() and get_next_command() need an external pointer
  // to hold onto the strtok_r state
  char *saveptr;

  // get the first command or set of arguments
  char *command = dependencies.command_processor.begin_get_commands(commands, &saveptr);

  int cmd = dependencies.command_processor.lookup_command(command);

  if(cmd == CMD_NULL){
    // no commands; empty buffer
    *macro_buffer = MACRO_END_MARKER;
    return;
  }
    
  do{
    if(cmd == CMD_NONE){
      // this is a set of arguments
      dependencies.command_processor.get_sub_args(command);

      // pack the arguments 
      byte arg_marker;
      byte num_args = 0;
      determine_arg_marker(arg_marker, num_args);

      // write arguments marker
      *macro_buffer++ = arg_marker;

      if(arg_marker == MACRO_ARG1_MARKER){
        *macro_buffer++ = (byte)dependencies.command_processor.sub_args[0];
      } else {
        for(int i = 0; i < num_args; i++){
          *((int *)macro_buffer) = dependencies.command_processor.sub_args[i];
          macro_buffer += 2;
        }
      }
    } else {
      // write the command byte to the macro buffer
      *macro_buffer++ = (byte)cmd;
    }

    // get the next command or argumemts
    command = dependencies.command_processor.get_next_command(&saveptr);
    cmd = dependencies.command_processor.lookup_command(command);
    
  }while(cmd != CMD_NULL);

  // write end of macro marker
  *macro_buffer = MACRO_END_MARKER;
}

void set_packed_eeprom_macro(int macro, char * commands){
  char * macro_buffer = ::get_eeprom_macro(macro);

  if(commands == NULL || *commands == '\0'){
    // no commands; empty the macro
    eeprom_write_byte((byte*)macro_buffer, MACRO_END_MARKER);
    return;
  }

  // begin_get_commands() and get_next_command() need an external pointer
  // to hold onto the strtok_r state
  char *saveptr;

  // get the first command or set of arguments
  char *command = dependencies.command_processor.begin_get_commands(commands, &saveptr);

  int cmd = dependencies.command_processor.lookup_command(command);

  if(cmd == CMD_NULL){
    // no commands; empty buffer
    eeprom_write_byte((byte*)macro_buffer, MACRO_END_MARKER);
    return;
  }
    
  do{
    if(cmd == CMD_NONE){
      // this is a set of arguments
      dependencies.command_processor.get_sub_args(command);

      // pack the arguments 
      byte arg_marker;
      byte num_args = 0;
      determine_arg_marker(arg_marker, num_args);

      // write arguments marker
      eeprom_write_byte((byte*)macro_buffer++, arg_marker);

      // pack the arguments 
      if(arg_marker == MACRO_ARG1_MARKER){
        eeprom_write_byte((byte*)macro_buffer++, dependencies.command_processor.sub_args[0] & 0xff);
      } else {
        for(int i = 0; i < num_args; i++){
          eeprom_write_word((word*)macro_buffer, (word)dependencies.command_processor.sub_args[i]);
          macro_buffer += 2;
        }
      }
    } else {
      // write the command byte to the macro buffer
      eeprom_write_byte((byte*)macro_buffer++, cmd);
    }

    // get the next command or argumemts
    command = dependencies.command_processor.get_next_command(&saveptr);
    cmd = dependencies.command_processor.lookup_command(command);
    
  }while(cmd != CMD_NULL);

  // write end of macro marker
  eeprom_write_byte((byte*)macro_buffer, MACRO_END_MARKER);
}

void set_packed_macro(int macro, char * commands){
  if(is_memory_macro(macro))
    set_packed_memory_macro(macro, commands);
  else if(is_eeprom_macro(macro))
    set_packed_eeprom_macro(macro, commands);
}

// used with the "set" command to set a macro from the serial input buffer
void set_packed_macro_from_serial(int macro){
  set_packed_macro(macro, dependencies.command_processor.get_input_buffer());
}

void run_packed_memory_macro(int macro, int times){
  // don't pass in this macro running's arguments
  dependencies.command_processor.reset_args();

  times = max(1, times);

  char * macro_buffer = ::get_memory_macro(macro);
  if(macro_buffer == NULL || *macro_buffer == '\0')
    // not a valid macro location or macro is empty
    return;

  for(int i = 0; i < times; i++){     
    macro_buffer = ::get_memory_macro(macro);

    byte cmd;
    while((cmd = *macro_buffer++) != MACRO_END_MARKER){
      if(cmd >= ARG_MARKER_FIRST && cmd <= ARG_MARKER_LAST){
        // unpack the arguments
        if(cmd == MACRO_ARG1_MARKER){
          // the most common case a value 1-255
          dependencies.command_processor.sub_args[0] = *(int *)macro_buffer++ & 0xff;
        } else {
          int num_args = num_words_from_arg_marker(cmd);
          for(int i = 0; i < num_args; i++){
            dependencies.command_processor.sub_args[i] = *((int *)macro_buffer);
            macro_buffer += 2;
          }
        }
      } else {
        // if setting a packed memory macro from a packed memory macro, 
        //   use a special version of set memory macro from memory 
        //   that copies until the end of macro marked, instead of \0
        if(cmd == CMD_SET_MACRO_F){
          int new_macro = dependencies.command_processor.sub_args[0];
          if(is_memory_macro(new_macro))
            set_packed_memory_macro_from_memory(new_macro, macro_buffer);
          else if(is_eeprom_macro(new_macro))
            set_packed_eeprom_macro_from_memory(new_macro, macro_buffer);

          // remaining macro has been consumed
          return;
        } else {
          if(!::dispatch_command(cmd, macro_buffer)){
            // the remaining macro buffer has been copied to set a macro
            // so there are no more commands to dispatch
            return;
          }
        }
      }
    }
  }
}

void run_packed_eeprom_macro(int macro, int times){
  // don't pass in this macro running's arguments
  dependencies.command_processor.reset_args();
  
  times = max(1, times);
  
  for(int i = 0; i < times; i++){     
    char * macro_buffer = ::get_eeprom_macro(macro);
    
    byte cmd;
    while((cmd = eeprom_read_byte((byte*)macro_buffer)) != MACRO_END_MARKER){
      macro_buffer++;

      if(cmd >= ARG_MARKER_FIRST && cmd <= ARG_MARKER_LAST){
        // unpack the arguments
        if(cmd == MACRO_ARG1_MARKER){
          // the most common case a value 1-255
          dependencies.command_processor.sub_args[0] = eeprom_read_byte((byte*)macro_buffer++);
        } else {
          int num_args = num_words_from_arg_marker(cmd);
          for(int i = 0; i < num_args; i++){
            dependencies.command_processor.sub_args[i] = (int)eeprom_read_word((word*)macro_buffer);
            macro_buffer += 2;
          }
        }
      } else {
        // if setting a packed memory macro from a packed memory macro, 
        //   use a special version of set memory macro from memory 
        //   that copies until the end of macro marked, instead of \0
        if(cmd == CMD_SET_MACRO_F){
          if(is_memory_macro(macro)){
            set_packed_memory_macro_from_eeprom(macro, macro_buffer);
          } else if(is_eeprom_macro(macro)){
            set_packed_eeprom_macro_from_eeprom(macro, macro_buffer);
          }
        
        // remaining macro has been consumed
        return;
        } else {
        if(!::dispatch_command(cmd, macro_buffer)){
          // the remaining macro buffer has been copied to set a macro
          // so there are no more commands to dispatch
          return;
          }
        }
      }
    }
  }
}

void run_packed_macro(int macro, int times = 1, int delay_ = 0)
{
  if(is_memory_macro(macro))
  {
    run_packed_memory_macro(macro, times);
  } 
  else if(is_eeprom_macro(macro))
  {
    run_packed_eeprom_macro(macro, times);
  }
  if(delay_ > 0)
    delay(delay_);
}
#endif

