// for some reason scheduling #21 runs but then stops as if it got canceled

// when a macro sets multiple schedules, they don't get set right


// not able to set a memory or eeprom macros from within an eeprom macro

// when copying macros, have to ignore end of macro marker inside arguments, which can have a 0xff

#ifndef MACROS_H
#define MACROS_H

// add an instant macro function
// takes the dispatch data and calls process_command() N times

#include "dependencies.h"
#include "config.h"
#include "eeprom.h"

extern Dependencies dependencies;
extern bool dispatch_command(int cmd, char *dispatch_data);

#define USE_PACKED_MACROS

#ifdef USE_PACKED_MACROS
#define MACRO_END_MARKER 0xff
#define MACRO_ARG_MARKER 0xfe
// this one doesn't need to be stored in macros, but it could be useful if supporting receiving binary commands
#define MACRO_CMD_MARKER 0xfd
#endif

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

#ifdef USE_PACKED_MACROS
// copy bytes until the end of macro marker
void set_packed_memory_macro_from_memory(int macro, char * buffer){
  char * str = ::get_memory_macro(macro);

  byte b;
  while((b = *buffer) != MACRO_END_MARKER){
    *str = b;
    buffer++;
    str++;
  }
  *str = MACRO_END_MARKER;
}

// copy bytes until the end of macro marker
void set_packed_eeprom_macro_from_memory(int macro, char * buffer){
  char * str = ::get_eeprom_macro(macro);

  byte b;
  while((b = *buffer) != MACRO_END_MARKER){
    eeprom_write_byte((byte*)str, b);
    buffer++;
    str++;
  }
  eeprom_write_byte((byte*)str, MACRO_END_MARKER);
  
//  char c;
//  while((c = *buffer++) != MACRO_END_MARKER)
//    eeprom_write_byte((byte*)str++, c);
//
//  // write the end of macro marker
//  eeprom_write_byte((byte*)str, MACRO_END_MARKER);
}

// copy bytes until the end of macro marker
void set_packed_memory_macro_from_eeprom(int macro, char * buffer){
  char * str = ::get_memory_macro(macro);

  str[0] = 15;
  str[1] = 6;
  str[2] = 1;
  str[3] = 255;
 
//  byte b;
//  while((b = eeprom_read_byte((byte*)buffer)) != MACRO_END_MARKER){
//    *str = b;
//    buffer++;
//    str++;
//  }
//  *str = MACRO_END_MARKER;

//
//  if(str != NULL){
//    char c;
//    while((c = eeprom_read_byte((byte*)buffer++)) != MACRO_END_MARKER){
//      *str = c;
//      str++;
//    }
//
//    // write the end of macro marker
//    *str = MACRO_END_MARKER;
//  }
}

// copy bytes until the end of macro marker
void set_packed_eeprom_macro_from_eeprom(int macro, char * buffer){
  char * str = ::get_eeprom_macro(macro);

// this doesn't work
//  eeprom_write_byte((byte*)(str+0), 15);
//  eeprom_write_byte((byte*)(str+1), 6);
//  eeprom_write_byte((byte*)(str+2), 1);
//  eeprom_write_byte((byte*)(str+3), 255);

// this doesn't work
  byte b;
  while((b = eeprom_read_byte((byte*)buffer)) != MACRO_END_MARKER){
    eeprom_write_byte((byte*)str, b);
    buffer++;
    str++;
  }
  eeprom_write_byte((byte*)str, MACRO_END_MARKER);
  
//  char c;
//  while((c = eeprom_read_byte((byte*)buffer++)) != MACRO_END_MARKER)
//    eeprom_write_byte((byte*)str++, c);
//
//  // write the end of macro marker
//  eeprom_write_byte((byte*)str, MACRO_END_MARKER);
}

#else

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
#endif

// TODO @@@
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

// TODO @@@
// process commands stored in PROGMEM 
//void process_commands_P(const char * commands){
void process_commands_P(const __FlashStringHelper * commands){
  // copy the macro into the tokenizing buffer
  char buffer[NUM_MACRO_CHARS]; 
  strcpy_P(buffer, (const char *)commands);
  process_commands(buffer);
}

#if !defined(USE_PACKED_MACROS)
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
#endif

void reset_macro(int macro){
  if(is_memory_macro(macro))
    ::get_memory_macro(macro)[0] = '\0';
  else if(is_eeprom_macro(macro))
#ifdef USE_PACKED_MACROS
    eeprom_write_byte((byte*)::get_eeprom_macro(macro), MACRO_END_MARKER);
#else  
    strcpy_eeprom_from_memory(::get_eeprom_macro(macro), "\0");
#endif
}

void reset_all_macros(){
  for(int i = 0; i < NUM_MACROS; i++)
    reset_macro(i);
}

bool is_programmed(byte macro){
  return ::eeprom_is_programmed(::get_eeprom_macro(macro));
}

#ifdef USE_PACKED_MACROS
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

      // write arguments marker
      *macro_buffer++ = MACRO_ARG_MARKER;

      // pack the arguments 
      for(int i = 0; i < 3; i++){
        *((int *)macro_buffer) = dependencies.command_processor.sub_args[i];
        macro_buffer += 2;
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

      // write arguments marker
      eeprom_write_byte((byte*)macro_buffer++, MACRO_ARG_MARKER);

      // pack the arguments 
      for(int i = 0; i < 3; i++){
        eeprom_write_word((word*)macro_buffer, (word)dependencies.command_processor.sub_args[i]);
        macro_buffer += 2;
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

void run_packed_memory_macro(int macro, int times, int delay_ = 0){
  // don't pass in this macro running's arguments
  dependencies.command_processor.reset_args();

  times = max(1, times);

  char * macro_buffer = ::get_memory_macro(macro);
  if(macro_buffer == NULL)
    // not a valid macro location
    return;

  for(int i = 0; i < times; i++){     
    macro_buffer = ::get_memory_macro(macro);

    byte cmd;
    while((cmd = *macro_buffer++) != MACRO_END_MARKER){
      if(cmd == MACRO_ARG_MARKER){
        // unpack the arguments
        for(int i = 0; i < 3; i++){
          dependencies.command_processor.sub_args[i] = *((int *)macro_buffer);
          macro_buffer += 2;
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

void run_packed_eeprom_macro(int macro, int times, int delay_ = 0){
  // don't pass in this macro running's arguments
  dependencies.command_processor.reset_args();
  
  times = max(1, times);
  
  for(int i = 0; i < times; i++){     
    char * macro_buffer = ::get_eeprom_macro(macro);
    
    byte cmd;
    while((cmd = eeprom_read_byte((byte*)macro_buffer)) != MACRO_END_MARKER){
      macro_buffer++;
      if(cmd == MACRO_ARG_MARKER){
        // unpack the arguments
        for(int i = 0; i < 3; i++){
          dependencies.command_processor.sub_args[i] = (int)eeprom_read_word((word*)macro_buffer);
          macro_buffer += 2;
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

void run_packed_macro(int macro, int times, int delay_ = 0)
{
  if(is_memory_macro(macro))
  {
    run_packed_memory_macro(macro, times, delay_);
  } 
  else if(is_eeprom_macro(macro))
  {
    run_packed_eeprom_macro(macro, times, delay_);
  }
}
#endif
#endif

