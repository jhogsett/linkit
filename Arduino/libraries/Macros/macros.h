#ifndef MACROS_H
#define MACROS_H

#include <avr/eeprom.h>
#include <command_processor.h>
#include <common.h>

#define MACRO_END_MARKER 0xff

#define NEW_ARG_ENCODING

#ifndef NEW_ARG_ENCODING
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
#else
#define ARG_MARKER (0x80)
#endif

#define MAX_MEMORY_MACRO (NUM_MEMORY_MACROS - 1)
#define MAX_EEPROM_MACRO (EEPROM_STARTING_MACRO + NUM_EEPROM_MACROS - 1)
#define MAX_MACRO (NUM_MACROS - 1)

#define DEFAULT_ERASE_BYTE 0xff

//#define NEW_ARG_ENCODING

//#define STRTOK_50_59_FIX

typedef bool (*DispatchFunction)(int cmd, byte *dispatch_data);

class Macros
{
  public:

  void begin(CommandProcessor *command_processor, DispatchFunction dispatch_function);
  byte set_macro(byte macro, char * commands);
  byte set_macro_from_serial(byte macro);
  void run_macro(byte macro, int times = 1, int delay_ = 0);

  byte begin_dump_macro(byte macro, byte **position);
  byte continue_dump_macro(byte macro, byte **position);

  private:

  static byte macros[NUM_MEMORY_MACROS][NUM_MACRO_CHARS];
  CommandProcessor * command_processor;
  DispatchFunction dispatch_function;

  bool is_memory_macro(byte macro);
  bool is_eeprom_macro(byte macro);
  byte * get_memory_macro(byte macro);
  byte * get_eeprom_macro(byte macro);

#ifndef NEW_ARG_ENCODING
  byte num_bytes_from_arg_marker(byte arg_marker);
  byte num_words_from_arg_marker(byte arg_marker);
#endif

  byte read_byte(byte * buffer, bool from_eeprom);
  void write_byte(byte * buffer, bool to_eeprom, byte data);
  word read_word(word * buffer, bool from_eeprom);
  void write_word(word * buffer, bool to_eeprom, word data);

#ifndef NEW_ARG_ENCODING
  void determine_arg_marker(byte &arg_marker, byte &num_args);
#endif

  void set_macro_from_macro(byte macro, byte * buffer, bool from_eeprom);
  bool get_macro_ptr(byte macro, byte **pptr);

#ifdef NEW_ARG_ENCODING
  bool fits_in_byte(int value);
  int encode_args(byte * buffer, bool to_eeprom);
  int encoded_args_bytes(byte marker);
  int decode_args(byte marker, byte * buffer, bool from_eeprom);
#endif
};

// in-memory macros
byte Macros::macros[NUM_MEMORY_MACROS][NUM_MACRO_CHARS];

void Macros::begin(CommandProcessor *command_processor, DispatchFunction dispatch_function)
{
  this->command_processor = command_processor;
  this->dispatch_function = dispatch_function;
}

bool Macros::is_memory_macro(byte macro)
{
  return macro <= MAX_MEMORY_MACRO;
}

bool Macros::is_eeprom_macro(byte macro)
{
  return macro >= EEPROM_STARTING_MACRO && macro <= MAX_EEPROM_MACRO;
}

// get a read/write pointer to a memory macro slot
byte * Macros::get_memory_macro(byte macro)
{
  if(macro > MAX_MEMORY_MACRO)
    return NULL;
  return macros[macro];
}

// get an eeprom accessible pointer to an eeprom macro slot
byte * Macros::get_eeprom_macro(byte macro)
{
  byte effective_macro = macro - EEPROM_STARTING_MACRO;
  return (byte*)(effective_macro * NUM_MACRO_CHARS);
}

#ifndef NEW_ARG_ENCODING
byte Macros::num_bytes_from_arg_marker(byte arg_marker)
{
  return (arg_marker - ARG_MARKER_FIRST) + 1;
}

byte Macros::num_words_from_arg_marker(byte arg_marker)
{
  return num_bytes_from_arg_marker(arg_marker) / 2;
}
#endif

byte Macros::read_byte(byte * buffer, bool from_eeprom)
{
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
  byte * macro_data;
  bool to_eeprom = get_macro_ptr(macro, &macro_data);

  byte b;
  while((b = read_byte(buffer, from_eeprom)) != MACRO_END_MARKER)
  {
    write_byte(macro_data, to_eeprom, b);
    macro_data++;
    buffer++;

#ifndef NEW_ARG_ENCODING
    if(b >= ARG_MARKER_FIRST && b <= ARG_MARKER_LAST)
    {
      // copy the packed arguments, which can be any value 0-255
      // including the end of macro marker
      byte num_bytes = num_bytes_from_arg_marker(b);
      for(byte i = 0; i < num_bytes; i++)
     {
        write_byte(macro_data, to_eeprom, read_byte(buffer, from_eeprom));
        macro_data++;
        buffer++;
      }
    }
#else
    if(b & ARG_MARKER)
    {
      // copy the packed arguments, which can be any value 0-255
      // including the end of macro marker
      byte num_bytes = encoded_args_bytes(b);
      for(byte i = 0; i < num_bytes; i++)
      {
        write_byte(macro_data, to_eeprom, read_byte(buffer, from_eeprom));
        macro_data++;
        buffer++;
      }
    }
#endif
  }
  write_byte(macro_data, to_eeprom, MACRO_END_MARKER);
}

#ifndef NEW_ARG_ENCODING
void Macros::determine_arg_marker(byte &arg_marker, byte &num_args)
{
  int * sub_args = command_processor->sub_args;

  if(sub_args[2] != 0)
  {
    arg_marker = MACRO_ARG6_MARKER;
    num_args = 3;
  }
  else if(sub_args[1] != 0)
  {
    arg_marker = MACRO_ARG4_MARKER;
    num_args = 2;
  }
  else
  {
    int arg0 = sub_args[0];
    if(arg0 >= 0 && arg0 <= 255 )
    {
      arg_marker = MACRO_ARG1_MARKER;
    }
    else if(arg0 != 0)
    {
      arg_marker = MACRO_ARG2_MARKER;
      num_args = 1;
    } 
    else
    {
      num_args = 0;
    }
  }
}
#endif

bool Macros::get_macro_ptr(byte macro, byte **pptr)
{
  if(is_eeprom_macro(macro))
  {
    *pptr = get_eeprom_macro(macro);
    return true;
  }
  else
  {
    *pptr = get_memory_macro(macro);
    return false;
  }
}

#ifdef NEW_ARG_ENCODING
bool Macros::fits_in_byte(int value)
{
    char char_value = (char)value;
    return value == (int)char_value;
}

int Macros::encode_args(byte * buffer, bool to_eeprom)
{
    int * sub_args = command_processor->sub_args;
    byte final_marker = 0;
    byte * data = buffer + 1;
    for(int i = 0; i < NUM_SUB_ARGS; i++){
        byte marker = 0;
        int arg = sub_args[i];
        if(arg != 0){
            if(fits_in_byte(arg)){
                marker = 0x01;
                write_byte(data, to_eeprom, (byte)arg);
                data += 1;
            } else {
                marker = 0x02;
                write_word((word*)data, to_eeprom, (word)arg);
                data += 2;
            }
        }
        final_marker = final_marker | (marker << (2 * i));
    }
    write_byte(buffer, to_eeprom, final_marker | ARG_MARKER);
    return data - buffer;
}

int Macros::encoded_args_bytes(byte marker)
{
    int count = 1;
    for(int i = 0; i < NUM_SUB_ARGS; i++)
    {
        count += marker & 0x03;
        marker = marker >> 2;
    }
    return count;
}

int Macros::decode_args(byte marker, byte * buffer, bool from_eeprom)
{
    int * sub_args = command_processor->sub_args;
    int byte_count = 0;
    for(int i = 0; i < NUM_SUB_ARGS; i++)
    {
        int count = marker & 0x03;
        if(count == 1)
        {
            sub_args[i] = (int)((char)read_byte(buffer, from_eeprom));
            buffer += 1;
        } 
        else if(count == 2)
        {
            sub_args[i] = (int)((short)read_word((word*)buffer, from_eeprom));
            buffer += 2;
        } 
        else 
        {
            sub_args[i] = 0;
        }
        byte_count += count;
        marker = marker >> 2;
    }
    return byte_count;
}
#endif

byte Macros::set_macro(byte macro, char * commands){
  byte * macro_buffer;
  bool to_eeprom = get_macro_ptr(macro, &macro_buffer);

  if(!to_eeprom && macro_buffer == NULL)
    // not a valid memory macro location
    return 0;

  if(commands == NULL || *commands == '\0')
  {
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

  if(cmd == CMD_NULL)
  {
    // no commands; empty buffer
    write_byte(macro_buffer, to_eeprom, MACRO_END_MARKER);
    return 0;
  }

  byte byte_count = 0;
  do
  {
    if(cmd == CMD_NONE)
    {
      // this is a set of arguments

//#ifndef STRTOK_50_59_FIX
      // doesn't work for "50" through "59"
      command_processor->get_sub_args(command);
//#else
//      // no cause was identified for the odd strtok_r behavior
//      // however adding a trailing delimiter prevents the problem
//      char buf[20];
//      strcpy(buf, command);
//      strcat(buf, ",");
//      command_processor->get_sub_args(buf);
//#endif


#ifndef NEW_ARG_ENCODING
      // pack the arguments
      byte arg_marker;
      byte num_args = 0;
      determine_arg_marker(arg_marker, num_args);

      // write arguments marker
      write_byte(macro_buffer, to_eeprom, arg_marker);
      macro_buffer++;
      byte_count++;

      if(arg_marker == MACRO_ARG1_MARKER)
      {
        write_byte(macro_buffer, to_eeprom, (byte)command_processor->sub_args[0]);
        macro_buffer++;
        byte_count++;
      }
      else
      {
        for(byte i = 0; i < num_args; i++)
        {
          write_word((word*)macro_buffer, to_eeprom, (word)command_processor->sub_args[i]);
          macro_buffer += 2;
          byte_count += 2;
        }
      }
#else
      int count = encode_args(macro_buffer, to_eeprom);
      macro_buffer += count;
      byte_count += count;
#endif      
    }
    else
    {
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
byte Macros::set_macro_from_serial(byte macro)
{
  return set_macro(macro, command_processor->get_input_buffer());
}

// arg[2] for CMD_RETURN_IF
#define RETURN_IF_EQUAL 0
#define RETURN_IF_LESS 1
#define RETURN_IF_MORE 2

// arg[0] macro number to run, default = 0
// arg[1] number of times to run, default = 1
// arg[2] milliseconds delay between runs, default = no delay
void Macros::run_macro(byte macro, int times, int delay_)
{
  times = max(1, times);

  byte * cached_macro_buffer;
  bool from_eeprom = get_macro_ptr(macro, &cached_macro_buffer);

  // how supporting passing an arg0 as arg2
  // command_processor->reset_args();

  byte * macro_buffer = cached_macro_buffer;
 
  if(!from_eeprom && (macro_buffer == NULL || *macro_buffer == '\0'))
    // not a valid macro location or macro is empty
    return;

  int * sub_args = command_processor->sub_args;
  for(int i = 0; i < times; i++)
  {
    macro_buffer = cached_macro_buffer;

    byte cmd;
    while((cmd = read_byte(macro_buffer, from_eeprom)) != MACRO_END_MARKER)
    {
      macro_buffer++;
#ifndef NEW_ARG_ENCODING
      if(cmd >= ARG_MARKER_FIRST && cmd <= ARG_MARKER_LAST)
      {
        // unpack the arguments
        if(cmd == MACRO_ARG1_MARKER)
        {
          // the most common case a value 1-255
          sub_args[0] = read_byte(macro_buffer, from_eeprom);
          macro_buffer++;
        }
        else
        {
          byte num_args = num_words_from_arg_marker(cmd);
          for(byte i = 0; i < num_args; i++)
          {
            sub_args[i] = (int)read_word((word*)macro_buffer, from_eeprom);
            macro_buffer += 2;
          }
        }
      }
#else
      if(cmd & ARG_MARKER)
      {
        macro_buffer += decode_args(cmd, macro_buffer, from_eeprom);
      }
#endif
      else
      {
        if(cmd == CMD_SET_MACRO)
        {
          byte new_macro = sub_args[0];
          set_macro_from_macro(new_macro, macro_buffer, false);

          // remaining macro has been consumed
          return;
        }
        else if(cmd == CMD_RETURN_IF)
        {
          int a = command_processor->sub_args[0];
          int b = command_processor->sub_args[1];
          int type = command_processor->sub_args[2];
          if(type == RETURN_IF_EQUAL){
            if(a == b)
              return;
          } else if(type == RETURN_IF_LESS){
            if(a < b)
              return;
          } else if(type == RETURN_IF_MORE){
            if(a > b)
              return;
          }
        }
        else
        {
          if(!(this->dispatch_function)(cmd, macro_buffer))
            // the remaining macro buffer has been copied to set a macro
            // so there are no more commands to dispatch
            return;
        }
      }
    }
    if(delay_ > 0)
      delay(delay_);
  }
}

byte Macros::begin_dump_macro(byte macro, byte **position)
{
  bool from_eeprom;
  if(is_eeprom_macro(macro))
  {
    *position = get_eeprom_macro(macro);
    from_eeprom = true;
  }
  else
  {
    *position = get_memory_macro(macro);
    from_eeprom = false;
  }

  return read_byte(*position, from_eeprom);
}

byte Macros::continue_dump_macro(byte macro, byte **position)
{
  *position = *position + 1;
  return read_byte(*position, is_eeprom_macro(macro));
}

#endif
