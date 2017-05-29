//#ifndef MACROS_H
//#define MACROS_H
//
//// add an instant macro function
//// takes the dispatch data and calls process_command() N times
//
//#include <avr/eeprom.h>
//
//#include "config.h"
////#include "dispatcher.h"
//#include <command_processor.h>
//
////extern Dependencies dependencies;
////extern bool dispatch_command(int cmd, char *dispatch_data);
//
////extern Dispatcher dispatcher;
//
////#ifndef DISPATCHER_H
//class Dispatcher
//{
//  public:
//  bool dispatch_command(int cmd, byte *dispatch_data = NULL);
//};
////#endif
//
//
//
//
///// PICKUP: move macros and scheduler to command
//
//
//
//
//
//
//#define MACRO_END_MARKER 0xff
//
//#define MACRO_ARG1_MARKER 0xf9 
//#define MACRO_ARG2_MARKER 0xfa 
//#define MACRO_ARG3_MARKER 0xfb 
//#define MACRO_ARG4_MARKER 0xfc 
//#define MACRO_ARG5_MARKER 0xfd 
//#define MACRO_ARG6_MARKER 0xfe 
//#define ARG_MARKER_FIRST  MACRO_ARG1_MARKER
//#define ARG_MARKER_LAST   MACRO_ARG6_MARKER
//
//// this one doesn't need to be stored in macros, but it could be useful if supporting receiving binary commands
//#define MACRO_CMD_MARKER 0xf0
//
//#define MAX_MEMORY_MACRO (NUM_MEMORY_MACROS - 1)
//#define MAX_EEPROM_MACRO (EEPROM_STARTING_MACRO + NUM_EEPROM_MACROS - 1)
//#define MAX_MACRO (NUM_MACROS - 1)
//
//#define DEFAULT_ERASE_BYTE 0xff
//
////class Dispatcher;
//
//class Macros
//{
//  public:  
//
//  void begin(Dispatcher * dispatcher, CommandProcessor * command_processor);
//
//  byte set_macro(byte macro, char * commands);
//  byte set_macro_from_serial(byte macro);
//
//  void run_macro(byte macro, int times = 1, int delay_ = 0);
//
//  static byte macros[NUM_MEMORY_MACROS][NUM_MACRO_CHARS];
//
//  Dispatcher * dispatcher;
//  CommandProcessor * command_processor;
//
//  bool is_memory_macro(byte macro);
//  bool is_eeprom_macro(byte macro);
//
//  byte * get_memory_macro(byte macro);
//  byte * get_eeprom_macro(byte macro);
//  
//  byte num_bytes_from_arg_marker(byte arg_marker);
//  byte num_words_from_arg_marker(byte arg_marker);
//
//  void set_memory_macro_from_memory(byte macro, byte * buffer);
//  void set_eeprom_macro_from_memory(byte macro, byte * buffer);
//  void set_memory_macro_from_eeprom(byte macro, byte * buffer);
//  void set_eeprom_macro_from_eeprom(byte macro, byte * buffer);
//
//  void process_commands(char * buffer);
//  void process_commands_P(const __FlashStringHelper * commands);
//
//  void reset_macro(byte macro);
//  void reset_all_macros();
//
//  bool is_programmed(byte macro);
//  
//  void determine_arg_marker(byte &arg_marker, byte &num_args);
//
//  byte set_memory_macro(byte macro, char * commands);
//  byte set_eeprom_macro(byte macro, char * commands);
//
//  void run_memory_macro(byte macro, int times);
//  void run_eeprom_macro(byte macro, int times);
//
//};
//
//// in-memory macros
//byte Commands::macros[NUM_MEMORY_MACROS][NUM_MACRO_CHARS];
//
//void Commands::begin(Dispatcher * dispatcher, CommandProcessor * command_processor){
//  this->dispatcher = dispatcher;
//  this->command_processor = command_processor;
//}
//
//bool Commands::is_memory_macro(byte macro){
//  return macro >= 0 && macro <= MAX_MEMORY_MACRO;
//}
//
//bool Commands::is_eeprom_macro(byte macro){
//  return macro >= EEPROM_STARTING_MACRO && macro <= MAX_EEPROM_MACRO;
//}
//
//// get a read/write pointer to a memory macro slot
//byte * Commands::get_memory_macro(byte macro){
//  if(macro < 0 || macro > MAX_MEMORY_MACRO){
//    return NULL;
//  }
//  return macros[macro];
//}
//
//// get an eeprom accessible pointer to an eeprom macro slot
//byte * Commands::get_eeprom_macro(byte macro){
//  byte effective_macro = macro - EEPROM_STARTING_MACRO;
//  return (byte*)(effective_macro * NUM_MACRO_CHARS);
//}
//
//byte Commands::num_bytes_from_arg_marker(byte arg_marker){
//  return (arg_marker - ARG_MARKER_FIRST) + 1;
//}
//
//byte Commands::num_words_from_arg_marker(byte arg_marker){
//  return num_bytes_from_arg_marker(arg_marker) / 2;
//}
//
//// copy bytes until the end of macro marker
//void Commands::set_memory_macro_from_memory(byte macro, byte * buffer){
//  byte * str = get_memory_macro(macro);
//
//  byte b;
//  while((b = *buffer++) != MACRO_END_MARKER){
//    *str++ = b;
//
//    if(b >= ARG_MARKER_FIRST && b <= ARG_MARKER_LAST){
//      // copy the packed arguments, which can be any value 0-255
//      // including the end of macro marker
//      byte num_bytes = num_bytes_from_arg_marker(b);
//      
//      for(byte i = 0; i < num_bytes; i++){
//        *str++ = *buffer++;  
//      }
//    }
//  }
//  *str = MACRO_END_MARKER;
//}
//
//// copy bytes until the end of macro marker
//void Commands::set_eeprom_macro_from_memory(byte macro, byte * buffer){
//  byte * str = get_eeprom_macro(macro);
//
//  byte b;
//  while((b = *buffer++) != MACRO_END_MARKER){
//    eeprom_write_byte(str++, b);
//
//    if(b >= ARG_MARKER_FIRST && b <= ARG_MARKER_LAST){
//      // copy the packed arguments, which can be any value 0-255
//      // including the end of macro marker
//      byte num_bytes = num_bytes_from_arg_marker(b);
//
//      for(byte i = 0; i < num_bytes; i++){
//        eeprom_write_byte(str++, *buffer++);  
//      }
//    }
//  }
//
//  eeprom_write_byte(str, MACRO_END_MARKER);
//}
//
//// copy bytes until the end of macro marker
//void Commands::set_memory_macro_from_eeprom(byte macro, byte * buffer){
//  byte * str = get_memory_macro(macro);
//
//  byte b;
//  while((b = eeprom_read_byte(buffer++)) != MACRO_END_MARKER){
//    *str++ = b;
//
//    if(b >= ARG_MARKER_FIRST && b <= ARG_MARKER_LAST){
//      // copy the packed arguments, which can be any value 0-255
//      // including the end of macro marker
//      byte num_bytes = num_bytes_from_arg_marker(b);
//
//      for(byte i = 0; i < num_bytes; i++){
//        *str++ = eeprom_read_byte(buffer++);  
//      }
//    }
//  }
//  *str = MACRO_END_MARKER;
//}
//
//// copy bytes until the end of macro marker
//void Commands::set_eeprom_macro_from_eeprom(byte macro, byte * buffer){
//  byte * str = get_eeprom_macro(macro);
//
//  byte b;
//  while((b = eeprom_read_byte(buffer++)) != MACRO_END_MARKER){
//    eeprom_write_byte(str++, b);
//
//    if(b >= ARG_MARKER_FIRST && b <= ARG_MARKER_LAST){
//      // copy the packed arguments, which can be any value 0-255
//      // including the end of macro marker
//      byte num_bytes = num_bytes_from_arg_marker(b);
//
//      for(byte i = 0; i < num_bytes; i++){
//        eeprom_write_byte(str++, eeprom_read_byte(buffer++));  
//      }
//    }
//  }
//  
//  eeprom_write_byte(str, MACRO_END_MARKER);
//}
//
//// process the series of commands and arguments in the passed memory buffer
//// the string must be tokenizable by strtok (get's corrupted)
//void Commands::process_commands(char * buffer){
//  if(buffer == NULL || *buffer == '\0'){
//    return;
//  }
//
//  // point to the end of the buffer for overrun protection
//  // this points at the terminator
//  char *last_char = buffer + strlen(buffer);
//
//  // begin_get_commands() and get_next_command() need an external pointer
//  // to hold onto the strtok_r state
//  char *saveptr;
//
//  // get the first command or set of arguments
//  char *command = command_processor->begin_get_commands(buffer, &saveptr);
//
//  // point to the remaining string after this command + terminator
//  // this is needed for copying strings when setting macros
//  // if this is the last command in the string, the location 
//  //   1 past the string + terminator overruns the end of the buffer
//  byte * rest_of_buffer = (byte*)min(command + strlen(command) + 1, last_char);
//
//  // process the command or arguments
//  int cmd = command_processor->process_command(command);
//
//  if(cmd == CMD_NULL){
//    // there was no command or arguments
//    return;
//  }
//
//  do{
//    // CMD_NONE is returned when there are arguments instead of a command
//    // arguments are not dispatched they're captured in CommandProcessor::process_command()
//    if(cmd != CMD_NONE){
//      // send the command to the dispatcher to be run
//      // pass the pointer to the rest of the buffer 
//      //   in case it's needed to set a macro
//      if(!dispatcher->dispatch_command(cmd, rest_of_buffer)){
//        // false means the rest of the buffer has been copied 
//        // so there are no more commands to process 
//        return;
//      }
//    }
//
//    // get the next set command or argumemts
//    command = command_processor->get_next_command(&saveptr);
//    rest_of_buffer = (byte*)min(command + strlen(command) + 1, last_char);
//    cmd = command_processor->process_command(command);
//    
//  }while(cmd != CMD_NULL);
//}
//
//// process commands stored in PROGMEM 
//void Commands::process_commands_P(const __FlashStringHelper * commands){
//  char * buffer = command_processor->borrow_char_buffer();
//  strcpy_P(buffer, (const char *)commands);
//  process_commands(buffer);
//}
//
//void Commands::reset_macro(byte macro){
//  if(is_memory_macro(macro))
//    get_memory_macro(macro)[0] = '\0';
//  else if(is_eeprom_macro(macro))
//    eeprom_write_byte(get_eeprom_macro(macro), MACRO_END_MARKER);
//}
//
//void Commands::reset_all_macros(){
//  for(byte i = 0; i < NUM_MACROS; i++)
//    reset_macro(i);
//}
//
//bool Commands::is_programmed(byte macro){
//  return eeprom_read_byte(get_eeprom_macro(macro)) != DEFAULT_ERASE_BYTE;
//}
//
//void Commands::determine_arg_marker(byte &arg_marker, byte &num_args){
//  int * sub_args = command_processor->sub_args;
//
//  if(sub_args[2] != 0){
//    arg_marker = MACRO_ARG6_MARKER;
//    num_args = 3;
//  } else if(sub_args[1] != 0){
//    arg_marker = MACRO_ARG4_MARKER;
//    num_args = 2;
//  } else {
//    int arg0 = sub_args[0];
//    if(arg0 >= 1 && arg0 <= 255 ) {
//      arg_marker = MACRO_ARG1_MARKER;
//    } else if(arg0 != 0){
//      arg_marker = MACRO_ARG2_MARKER;
//      num_args = 1;
//    }
//  }
//}
//
//byte Commands::set_memory_macro(byte macro, char * commands){
//  byte * macro_buffer = get_memory_macro(macro);
//  if(macro_buffer == NULL)
//    // not a valid memory macro location
//    return 0;
//
//  if(commands == NULL || *commands == '\0'){
//    // no commands; empty the macro
//    *macro_buffer = MACRO_END_MARKER;
//    return 0;
//  }
//
//  // begin_get_commands() and get_next_command() need an external pointer
//  // to hold onto the strtok_r state
//  char *saveptr;
//
//  // get the first command or set of arguments
//  char *command = command_processor->begin_get_commands(commands, &saveptr);
//  int cmd = command_processor->lookup_command(command);
//
//  if(cmd == CMD_NULL){
//    // no commands; empty buffer
//    *macro_buffer = MACRO_END_MARKER;
//    return 0;
//  }
//    
//  byte byte_count = 0;
//  do{
//    if(cmd == CMD_NONE){
//      // this is a set of arguments
//      command_processor->get_sub_args(command);
//
//      // pack the arguments 
//      byte arg_marker;
//      byte num_args = 0;
//      determine_arg_marker(arg_marker, num_args);
//
//      // write arguments marker
//      *macro_buffer++ = arg_marker;
//      byte_count++;
//      
//      if(arg_marker == MACRO_ARG1_MARKER){
//        *macro_buffer++ = (byte)command_processor->sub_args[0];
//        byte_count++;
//      } else {
//        for(byte i = 0; i < num_args; i++){
//          *((int *)macro_buffer) = command_processor->sub_args[i];
//          macro_buffer += 2;
//          byte_count += 2;
//        }
//      }
//    } else {
//      // write the command byte to the macro buffer
//      *macro_buffer++ = (byte)cmd;
//      byte_count++;
//    }
//
//    // get the next command or argumemts
//    command = command_processor->get_next_command(&saveptr);
//    cmd = command_processor->lookup_command(command);
//    
//  }while(cmd != CMD_NULL);
//
//  // write end of macro marker
//  *macro_buffer = MACRO_END_MARKER;
//
//  return byte_count;
//}
//
//byte Commands::set_eeprom_macro(byte macro, char * commands){
//  byte * macro_buffer = get_eeprom_macro(macro);
//
//  if(commands == NULL || *commands == '\0'){
//    // no commands; empty the macro
//    eeprom_write_byte(macro_buffer, MACRO_END_MARKER);
//    return 0;
//  }
//
//  // begin_get_commands() and get_next_command() need an external pointer
//  // to hold onto the strtok_r state
//  char *saveptr;
//
//  // get the first command or set of arguments
//  char *command = command_processor->begin_get_commands(commands, &saveptr);
//  int cmd = command_processor->lookup_command(command);
//
//  if(cmd == CMD_NULL){
//    // no commands; empty buffer
//    eeprom_write_byte(macro_buffer, MACRO_END_MARKER);
//    return 0;
//  }
//
//  byte byte_count = 0;
//  do{
//    if(cmd == CMD_NONE){
//      // this is a set of arguments
//      command_processor->get_sub_args(command);
//
//      // pack the arguments 
//      byte arg_marker;
//      byte num_args = 0;
//      determine_arg_marker(arg_marker, num_args);
//
//      // write arguments marker
//      eeprom_write_byte(macro_buffer++, arg_marker);
//      byte_count++;
//      
//      // pack the arguments 
//      int * sub_args = command_processor->sub_args;
//      if(arg_marker == MACRO_ARG1_MARKER){
//        eeprom_write_byte(macro_buffer++, sub_args[0] & 0xff);
//        byte_count++;
//      } else {
//        for(byte i = 0; i < num_args; i++){
//          eeprom_write_word((word*)macro_buffer, (word)sub_args[i]);
//          macro_buffer += 2;
//          byte_count += 2;
//        }
//      }
//    } else {
//      // write the command byte to the macro buffer
//      eeprom_write_byte(macro_buffer++, cmd);
//      byte_count++;
//    }
//
//    // get the next command or argumemts
//    command = command_processor->get_next_command(&saveptr);
//    cmd = command_processor->lookup_command(command);
//    
//  }while(cmd != CMD_NULL);
//
//  // write end of macro marker
//  eeprom_write_byte(macro_buffer, MACRO_END_MARKER);
//
//  return byte_count;
//}
//
//byte Commands::set_macro(byte macro, char * commands){
//  if(is_memory_macro(macro))
//    return set_memory_macro(macro, commands);
//  else if(is_eeprom_macro(macro))
//    return set_eeprom_macro(macro, commands);
//}
//
//// used with the "set" command to set a macro from the serial input buffer
//byte Commands::set_macro_from_serial(byte macro){
//  return set_macro(macro, command_processor->get_input_buffer());
//}
//
//void Commands::run_memory_macro(byte macro, int times){
//  // don't pass in this macro running's arguments
//  command_processor->reset_args();
//
//  times = max(1, times);
//  byte * cached_macro_buffer = get_memory_macro(macro);
//
//  byte * macro_buffer = cached_macro_buffer;
//  if(macro_buffer == NULL || *macro_buffer == '\0')
//    // not a valid macro location or macro is empty
//    return;
//
//  int * sub_args = command_processor->sub_args;
//  for(int i = 0; i < times; i++){     
//    macro_buffer = cached_macro_buffer;
//
//    byte cmd;
//    while((cmd = *macro_buffer++) != MACRO_END_MARKER){
//      if(cmd >= ARG_MARKER_FIRST && cmd <= ARG_MARKER_LAST){
//        // unpack the arguments
//        if(cmd == MACRO_ARG1_MARKER){
//          // the most common case a value 1-255
//          sub_args[0] = *macro_buffer++;
//        } else {
//          byte num_args = num_words_from_arg_marker(cmd);
//          for(byte i = 0; i < num_args; i++){
//            sub_args[i] = *((int *)macro_buffer);
//            macro_buffer += 2;
//          }
//        }
//      } else {
//        if(cmd == CMD_SET_MACRO){
//          byte new_macro = sub_args[0];
//          if(is_memory_macro(new_macro))
//            set_memory_macro_from_memory(new_macro, macro_buffer);
//          else if(is_eeprom_macro(new_macro))
//            set_eeprom_macro_from_memory(new_macro, macro_buffer);
//
//          // remaining macro has been consumed
//          return;
//        } else {
//          if(!dispatcher->dispatch_command(cmd, macro_buffer)){
//            // the remaining macro buffer has been copied to set a macro
//            // so there are no more commands to dispatch
//            return;
//          }
//        }
//      }
//    }
//  }
//}
//
//void Commands::run_eeprom_macro(byte macro, int times){
//  // don't pass in this macro running's arguments
//  command_processor->reset_args();
//  
//  times = max(1, times);
//  byte * cached_macro_buffer = get_eeprom_macro(macro);
//
//  int * sub_args = command_processor->sub_args;
//  for(int i = 0; i < times; i++){     
//    byte * macro_buffer = cached_macro_buffer;
//    
//    byte cmd;
//    while((cmd = eeprom_read_byte(macro_buffer)) != MACRO_END_MARKER){
//      macro_buffer++;
//
//      if(cmd >= ARG_MARKER_FIRST && cmd <= ARG_MARKER_LAST){
//        // unpack the arguments
//        if(cmd == MACRO_ARG1_MARKER){
//          // the most common case a value 1-255
//          sub_args[0] = eeprom_read_byte(macro_buffer++);
//        } else {
//          byte num_args = num_words_from_arg_marker(cmd);
//          for(byte i = 0; i < num_args; i++){
//            sub_args[i] = (int)eeprom_read_word((word*)macro_buffer);
//            macro_buffer += 2;
//          }
//        }
//      } else {
//        if(cmd == CMD_SET_MACRO){
//          byte new_macro = sub_args[0];
//          if(is_memory_macro(new_macro)){
//            set_memory_macro_from_eeprom(new_macro, macro_buffer);
//          } else if(is_eeprom_macro(macro)){
//            set_eeprom_macro_from_eeprom(new_macro, macro_buffer);
//          }
//        
//        // remaining macro has been consumed
//        return;
//        } else {
//        if(!dispatcher->dispatch_command(cmd, macro_buffer)){
//          // the remaining macro buffer has been copied to set a macro
//          // so there are no more commands to dispatch
//          return;
//          }
//        }
//      }
//    }
//  }
//}
//
//void Commands::run_macro(byte macro, int times, int delay_)
//{
//  if(is_memory_macro(macro))
//    run_memory_macro(macro, times);
//  else if(is_eeprom_macro(macro))
//    run_eeprom_macro(macro, times);
//
//  if(delay_ > 0)
//    delay(delay_);
//}
//
//#endif

