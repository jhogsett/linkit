#ifndef MACROS_H
#define MACROS_H

#include <avr/eeprom.h>
#include <command_processor.h>

// #define SUB_ARGS_FIX

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

#define DEFAULT_ERASE_BYTE 0xff

typedef bool (*DispatchFunction)(int cmd, byte *dispatch_data);

class Macros
{
  public:

  void begin(CommandProcessor *command_processor, DispatchFunction dispatch_function);
  bool is_macro_programmed(byte macro);
  byte set_macro(byte macro, char * commands);
  byte set_macro_from_serial(byte macro);
  void run_macro(byte macro, int times = 1, int delay_ = 0);
  void reset_macro(byte macro);
  void reset_all_macros();

  byte begin_dump_macro(byte macro, byte **position);
  byte continue_dump_macro(byte macro, byte **position);

  private:

  static byte macros[NUM_MEMORY_MACROS][NUM_MACRO_CHARS];
  CommandProcessor * command_processor;
  DispatchFunction dispatch_function;
//  static char static_buf[20];

  bool is_memory_macro(byte macro);
  bool is_eeprom_macro(byte macro);
  byte * get_memory_macro(byte macro);
  byte * get_eeprom_macro(byte macro);
  byte num_bytes_from_arg_marker(byte arg_marker);
  byte num_words_from_arg_marker(byte arg_marker);
  byte read_byte(byte * buffer, bool from_eeprom);
  void write_byte(byte * buffer, bool to_eeprom, byte data);
  word read_word(word * buffer, bool from_eeprom);
  void write_word(word * buffer, bool to_eeprom, word data);
  void determine_arg_marker(byte &arg_marker, byte &num_args);
  void set_macro_from_macro(byte macro, byte * buffer, bool from_eeprom);
};

//char Macros::static_buf[20];

// in-memory macros
byte Macros::macros[NUM_MEMORY_MACROS][NUM_MACRO_CHARS];

void Macros::begin(CommandProcessor *command_processor, DispatchFunction dispatch_function){
  this->command_processor = command_processor;
  this->dispatch_function = dispatch_function;
}

bool Macros::is_memory_macro(byte macro){
  return macro >= 0 && macro <= MAX_MEMORY_MACRO;
}

bool Macros::is_eeprom_macro(byte macro){
  return macro >= EEPROM_STARTING_MACRO && macro <= MAX_EEPROM_MACRO;
}

// get a read/write pointer to a memory macro slot
byte * Macros::get_memory_macro(byte macro){
  if(macro < 0 || macro > MAX_MEMORY_MACRO){
    return NULL;
  }
  return macros[macro];
}

// get an eeprom accessible pointer to an eeprom macro slot
byte * Macros::get_eeprom_macro(byte macro){
  byte effective_macro = macro - EEPROM_STARTING_MACRO;
  return (byte*)(effective_macro * NUM_MACRO_CHARS);
}

byte Macros::num_bytes_from_arg_marker(byte arg_marker){
  return (arg_marker - ARG_MARKER_FIRST) + 1;
}

byte Macros::num_words_from_arg_marker(byte arg_marker){
  return num_bytes_from_arg_marker(arg_marker) / 2;
}

byte Macros::read_byte(byte * buffer, bool from_eeprom){
  if(from_eeprom)
    return eeprom_read_byte(buffer);
  else
    return *buffer;
}

void Macros::write_byte(byte * buffer, bool to_eeprom, byte data){
  if(to_eeprom)
    eeprom_write_byte(buffer, data);
  else
    *buffer = data;
}

word Macros::read_word(word * buffer, bool from_eeprom){
  if(from_eeprom)
    return eeprom_read_word(buffer);
  else
    return *buffer;
}

void Macros::write_word(word * buffer, bool to_eeprom, word data){
  if(to_eeprom)
    eeprom_write_word(buffer, data);
  else
    *buffer = data;
}

// copy bytes until the end of macro marker
void Macros::set_macro_from_macro(byte macro, byte * buffer, bool from_eeprom){
  bool to_eeprom;
  byte * macro_data;
  if(is_memory_macro(macro)){
    macro_data = get_memory_macro(macro);
    to_eeprom = false;
  } else {
    macro_data = get_eeprom_macro(macro);
    to_eeprom = true;
  }

  byte b;
  while((b = read_byte(buffer, from_eeprom)) != MACRO_END_MARKER){
    write_byte(macro_data, to_eeprom, b);
    macro_data++;
    buffer++;

    if(b >= ARG_MARKER_FIRST && b <= ARG_MARKER_LAST){
      // copy the packed arguments, which can be any value 0-255
      // including the end of macro marker
      byte num_bytes = num_bytes_from_arg_marker(b);

      for(byte i = 0; i < num_bytes; i++){
        write_byte(macro_data, to_eeprom, read_byte(buffer, from_eeprom));
        macro_data++;
        buffer++;
      }
    }
  }
  write_byte(macro_data, to_eeprom, MACRO_END_MARKER);
}

void Macros::reset_macro(byte macro){
  if(is_memory_macro(macro))
    get_memory_macro(macro)[0] = '\0';   // TODO should this be the macro end marker
  else if(is_eeprom_macro(macro))
    eeprom_write_byte(get_eeprom_macro(macro), MACRO_END_MARKER);
}

void Macros::reset_all_macros(){
  for(byte i = 0; i < NUM_MACROS; i++)
    reset_macro(i);
}

bool Macros::is_macro_programmed(byte macro){
  return eeprom_read_byte(get_eeprom_macro(macro)) != DEFAULT_ERASE_BYTE;
}

void Macros::determine_arg_marker(byte &arg_marker, byte &num_args){
  int * sub_args = command_processor->sub_args;

  if(sub_args[2] != 0){
    arg_marker = MACRO_ARG6_MARKER;
    num_args = 3;
  } else if(sub_args[1] != 0){
    arg_marker = MACRO_ARG4_MARKER;
    num_args = 2;
  } else {
    int arg0 = sub_args[0];
    if(arg0 >= 0 && arg0 <= 255 ) {
      arg_marker = MACRO_ARG1_MARKER;
    } else if(arg0 != 0){
      arg_marker = MACRO_ARG2_MARKER;
      num_args = 1;
    } else {
      //arg_marker = MACRO_ARG2_MARKER;
      num_args = 0;
    }
  }
}

byte Macros::set_macro(byte macro, char * commands){
  byte * macro_buffer;
  bool to_eeprom;

  if(is_eeprom_macro(macro)){
    macro_buffer = get_eeprom_macro(macro);
    to_eeprom = true;
  } else {
    macro_buffer = get_memory_macro(macro);
    to_eeprom = false;
  }

  if(!to_eeprom && macro_buffer == NULL)
    // not a valid memory macro location
    return 0;

  if(commands == NULL || *commands == '\0'){
    // no commands; empty the macro
    write_byte(macro_buffer, to_eeprom, MACRO_END_MARKER);
    return 0;
  }

  // begin_get_commands() and get_next_command() need an external pointer
  // to hold onto the strtok_r state
  char *saveptr;

  // get the first command or set of arguments
  char *command = command_processor->begin_get_commands(commands, &saveptr);
  int cmd = command_processor->lookup_command(command);

  if(cmd == CMD_NULL){
    // no commands; empty buffer
    write_byte(macro_buffer, to_eeprom, MACRO_END_MARKER);
    return 0;
  }

  byte byte_count = 0;
  do{
    if(cmd == CMD_NONE){
      // this is a set of arguments

      // doesn't work for "50" through "59"
      // command_processor->get_sub_args(command);

      // no cause was identified for the odd strtok_r behavior
      // however adding a trailing delimiter prevents the problem
      char buf[20];
      strcpy(buf, command);
      strcat(buf, ",");
      command_processor->get_sub_args(buf);

      // pack the arguments
      byte arg_marker;
      byte num_args = 0;
      determine_arg_marker(arg_marker, num_args);

      // write arguments marker
      write_byte(macro_buffer, to_eeprom, arg_marker);
      macro_buffer++;
      byte_count++;

      if(arg_marker == MACRO_ARG1_MARKER){
        write_byte(macro_buffer, to_eeprom, (byte)command_processor->sub_args[0]);
        macro_buffer++;
        byte_count++;
      } else {
        for(byte i = 0; i < num_args; i++){
          write_word((word*)macro_buffer, to_eeprom, (word)command_processor->sub_args[i]);
          macro_buffer += 2;
          byte_count += 2;
        }
      }
    } else {
      // write the command byte to the macro buffer
      write_byte(macro_buffer, to_eeprom, (byte)cmd);
      macro_buffer++;
      byte_count++;
    }

    // get the next command or argumemts
    command = command_processor->get_next_command(&saveptr);
    cmd = command_processor->lookup_command(command);

  }while(cmd != CMD_NULL);

  // write end of macro marker
  write_byte(macro_buffer, to_eeprom, MACRO_END_MARKER);

  return byte_count;
}

// used with the "set" command to set a macro from the serial input buffer
byte Macros::set_macro_from_serial(byte macro){
  return set_macro(macro, command_processor->get_input_buffer());
}

// arg[0] macro number to run, default = 0
// arg[1] number of times to run, default = 1
// arg[3] milliseconds delay between runs, default = no delay
void Macros::run_macro(byte macro, int times, int delay_){
  times = max(1, times);

  byte * cached_macro_buffer;
  bool from_eeprom;
  if(is_eeprom_macro(macro)){
    cached_macro_buffer = get_eeprom_macro(macro);
    from_eeprom = true;
  } else {
    cached_macro_buffer = get_memory_macro(macro);
    from_eeprom = false;
 }

  // don't pass in this macro running's arguments
  // todo: restore previous arguments to allow passing values into macros
  command_processor->reset_args();

  byte * macro_buffer = cached_macro_buffer;
  if(!from_eeprom && (macro_buffer == NULL || *macro_buffer == '\0'))
    // not a valid macro location or macro is empty
    return;

  int * sub_args = command_processor->sub_args;

  for(int i = 0; i < times; i++){
    macro_buffer = cached_macro_buffer;

    byte cmd;
    while((cmd = read_byte(macro_buffer, from_eeprom)) != MACRO_END_MARKER){
      macro_buffer++;
      if(cmd >= ARG_MARKER_FIRST && cmd <= ARG_MARKER_LAST){
        // unpack the arguments
        if(cmd == MACRO_ARG1_MARKER){
          // the most common case a value 1-255
          sub_args[0] = read_byte(macro_buffer, from_eeprom);
          macro_buffer++;
        } else {
          byte num_args = num_words_from_arg_marker(cmd);
          for(byte i = 0; i < num_args; i++){
            sub_args[i] = (int)read_word((word*)macro_buffer, from_eeprom);
            macro_buffer += 2;
          }
        }
      } else {
        if(cmd == CMD_SET_MACRO){
          byte new_macro = sub_args[0];
          set_macro_from_macro(new_macro, macro_buffer, false);

          // remaining macro has been consumed
          return;
        } else {
            if(!(this->dispatch_function)(cmd, macro_buffer)){
            // the remaining macro buffer has been copied to set a macro
            // so there are no more commands to dispatch
            return;
          }
        }
      }
    }
    if(delay_ > 0)
      delay(delay_);
  }
}

byte Macros::begin_dump_macro(byte macro, byte **position){
  bool from_eeprom;
  if(is_eeprom_macro(macro)){
    *position = get_eeprom_macro(macro);
    from_eeprom = true;
  } else {
    *position = get_memory_macro(macro);
    from_eeprom = false;
  }

  return read_byte(*position, from_eeprom);
}

byte Macros::continue_dump_macro(byte macro, byte **position){
  *position = *position + 1;
  return read_byte(*position, is_eeprom_macro(macro));
}
#endif

