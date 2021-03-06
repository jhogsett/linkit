#ifndef COMMAND_PROCESSOR_H
#define COMMAND_PROCESSOR_H

#define DELIMITER_CHAR ':'
#define DELIMITER_STR ":"
#define MAX_STRING_LENGTH 100
#define NUM_SUB_ARGS 3
#define CMD_NULL -1
#define CMD_NONE 0
#define CMD_FIRST 1

#define STRTOK_50_59_FIX

// this doesn't work; interferes with setting a macro within a macro
//#define ALT_STRTOK_50_59_FIX

class CommandProcessor
{
  public:

  char str[MAX_STRING_LENGTH];

  int sub_args[NUM_SUB_ARGS];
  int accumulators[NUM_SUB_ARGS];

  const char * const *commands;

  byte num_commands;
  HardwareSerial *serial;

  void begin(HardwareSerial *serial, const char* const *commands, byte num_commands);

  bool input_available();
  bool received_command();
  void acknowledge_command(bool force);
  void send_ack();
  int lookup_command(char * str);
  int get_command();
  void save_args(char * str);
  void reset_args();
  int process_command(char * str);
  char * begin_get_commands(char *buffer, char **saveptr);
  char * get_next_command(char **saveptr);
  char * get_input_buffer();
  void get_sub_args(char * str);
  void send_int(int value);
  void send_ints(int value);
  void send_str(char * value);
  byte get_num_args() { return NUM_SUB_ARGS; }
  byte get_max_string_length() { return MAX_STRING_LENGTH - 1; }

  private:

  bool str_equal_P(char *str1, const char *str2);
  bool is_command_P(char *str, const char *command);
};

void CommandProcessor::begin(HardwareSerial *serial, const char* const *commands, byte num_commands){
  this->serial = serial;
  this->commands = commands;
  this->num_commands = num_commands;
  this->serial = serial;

  for(int i = 0; i < NUM_SUB_ARGS; i++){
    sub_args[i] = 0;
    accumulators[i] = 0;
  }

  // force a command acknowledgement to wake up any script that may be halted
  // waiting for a character to be sent due to a new Arduino sketch being uploaded
  acknowledge_command(true);
}

bool CommandProcessor::input_available()
{
  return serial->available() > 0;
}

// read a single command into the in-memory buffer
// this does not need to be re-entrant
// the effective buffer size may be this buffer width + 64-char serial buffer input size
bool CommandProcessor::received_command()
{
  if(input_available())
  {
    byte c = serial->readBytesUntil(DELIMITER_CHAR, str, MAX_STRING_LENGTH-1);
    this->str[c] = 0;
    return true;
  }
  else
    return false;
}

// only acknowledge when there are no more commands available
// so that multiple additional commands if any are processed first
void CommandProcessor::acknowledge_command(bool force = false)
{
  if(!input_available() || force)
    send_ack();
}

void CommandProcessor::send_ack()
{
  serial->write("k");
}

void CommandProcessor::send_int(int value)
{
  serial->print(value);
}

#define INT_DELIMITER_CHAR ','

void CommandProcessor::send_ints(int value)
{
  serial->print(value);
  serial->print(INT_DELIMITER_CHAR);
}

void CommandProcessor::send_str(char * value)
{
  serial->print(value);
}

void CommandProcessor::save_args(char * args = NULL)
{
  get_sub_args(args);
}

// str2 is a pointer to a string in PROGMEM
bool CommandProcessor::str_equal_P(char *str1, const char *str2)
{
  for(byte i = 0; i < MAX_STRING_LENGTH; i++)
  {
    char c1 = *(str1 + i);
    char c2 = (char)pgm_read_byte(str2 + i);

//    if(c1 == 0 && c2 == 0)
    if(c2 == 0)
      // found
      return true;

    if(c2 != c1)
      // stop on firt mismatch
      return false;
  }
  return false;
}

bool CommandProcessor::is_command_P(char *str, const char *command)
{
  return str_equal_P(str, command);
}

void CommandProcessor::get_sub_args(char * args = NULL)
{

#ifndef STRTOK_50_59_FIX
  // doesn't work for "50" through "59"
  if(args == NULL)
  {
    args = this->str;
  }

  char *saveptr = NULL;
  sub_args[0] = atoi(strtok_r(args, ",", &saveptr));
  sub_args[1] = atoi(strtok_r(NULL, ",", &saveptr));
  sub_args[2] = atoi(strtok_r(NULL, ",", &saveptr));

#else
  // no cause was identified for the odd strtok_r behavior
  // however adding a trailing delimiter prevents the problem

  if(args == NULL)
  {
    args = this->str;
  }

//#ifndef ALT_STRTOK_50_59_FIX
  // -99999,-99999,-99999,
  char buf[21];
  strcpy(buf, args);
  strcat(buf, ",");

  char *saveptr = NULL;
  sub_args[0] = atoi(strtok_r(buf, ",", &saveptr));
  sub_args[1] = atoi(strtok_r(NULL, ",", &saveptr));
  sub_args[2] = atoi(strtok_r(NULL, ",", &saveptr));
//#else
//  // this doesn't work - interferes with setting macros inside macros
//  byte len = strlen(args);
//  args[len] = ',';
//  args[len + 1] = 0; // this is OK because arguments fall short of the buffer length
//
//  char *saveptr = NULL;
//  sub_args[0] = atoi(strtok_r(args, ",", &saveptr));
//  sub_args[1] = atoi(strtok_r(NULL, ",", &saveptr));
//  sub_args[2] = atoi(strtok_r(NULL, ",", &saveptr));
//#endif

#endif
}



//#ifndef STRTOK_50_59_FIX
//      // doesn't work for "50" through "59"
//      command_processor->get_sub_args(command);
//#else
//      // no cause was identified for the odd strtok_r behavior
//      // however adding a trailing delimiter prevents the problem
//      char buf[20];
//      strcpy(buf, command);
//      strcat(buf, ",");
//      command_processor->get_sub_args(buf);
//#endif





void CommandProcessor::reset_args()
{
  sub_args[0] = 0;
  sub_args[1] = 0;
  sub_args[2] = 0;
}

int CommandProcessor::lookup_command(char * str)
{
  if(str == NULL)
    return CMD_NULL;

  for(byte i = 0; i < num_commands; i++)
    if(is_command_P(str, (char*)pgm_read_word(&(commands[i]))))
      return CMD_FIRST + i;

  return CMD_NONE;
}

// look up a command copied to the input buffer
int CommandProcessor::get_command()
{
  return lookup_command(this->str);
}

// process a command from a string or macro
// this ensures arguments are parsed and saved
int CommandProcessor::process_command(char * str)
{
  int command = lookup_command(str);

  // if CMD_NONE is returned, the command wasn't found; parse the string for arguments
  // the default save_args() should not be called by; the calling code in this case
  if(command == CMD_NONE)
    save_args(str);

  return command;
}

char * CommandProcessor::begin_get_commands(char * buffer, char **saveptr)
{
  return strtok_r(buffer, DELIMITER_STR, saveptr);
}

char * CommandProcessor::get_next_command(char **saveptr)
{
  return strtok_r(NULL, DELIMITER_STR, saveptr);
}

// read the contents serial buffer into the char buffer
// this is used for setting macros that come in over serial
char * CommandProcessor::get_input_buffer()
{
  if(input_available())
  {
    // a timeout is not needed - the macro bytes are already present in the buffer
    // serial->setTimeout(0);
    int c = serial->readBytesUntil('\0', this->str, MAX_STRING_LENGTH-1);
    // serial->setTimeout(1000);
    this->str[c] = 0;
    return this->str;
  }
  else
    return NULL;
}

#endif
