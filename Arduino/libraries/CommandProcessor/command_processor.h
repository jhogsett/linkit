#ifndef COMMAND_PROCESSOR_H
#define COMMAND_PROCESSOR_H

#define DELIMITER_CHAR ':'
#define MAX_STRING_LENGTH 20
#define NUM_SUB_ARGS 3
#define CMD_NONE 0
#define CMD_FIRST 1

class CommandProcessor 
{
  public:
  char str[MAX_STRING_LENGTH];

#ifdef USE_ARG_BUFFER
  char arg[MAX_STRING_LENGTH];
#endif

  int sub_args[NUM_SUB_ARGS];

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
  void save_args();
  void reset_args();

  private:
  bool str_equal_P(char *str1, const char *str2);
  bool is_command_P(char *str, const char *command);
  void get_sub_args();
};

void CommandProcessor::begin(HardwareSerial *serial, const char* const *commands, byte num_commands){
  this->serial = serial;
  this->commands = commands;
  this->num_commands = num_commands;
  this->serial = serial;
}

bool CommandProcessor::input_available(){
  return serial->available() > 0;
}

bool CommandProcessor::received_command(){
  if(input_available()){
    int c = serial->readBytesUntil(DELIMITER_CHAR, str, MAX_STRING_LENGTH-1);
    str[c] = 0;
    return true;
  } else {
    return false;
  }
}

// only acknowledge when there are no more commands available
// so that multiple additional commands if any are processed first
void CommandProcessor::acknowledge_command(bool force = false){
  if(!input_available() || force)
    send_ack();
}

void CommandProcessor::send_ack(){
  serial->write("k");
}

void CommandProcessor::save_args(){
#ifdef USE_ARG_BUFFER
  strcpy(arg, str);
#endif
  get_sub_args();
}

// str2 is a pointer to a string in PROGMEM
bool CommandProcessor::str_equal_P(char *str1, const char *str2){
  for(int i = 0; i < MAX_STRING_LENGTH; i++){
    char c1 = *(str1 + i);
    char c2 = (char)pgm_read_byte(str2 + i);
    if(c1 == 0 && c2 == 0) return true;
    if(c2 != c1) return false;
  }
  return false;
}

bool CommandProcessor::is_command_P(char *str, const char *command){
  return str_equal_P(str, command);
}

void CommandProcessor::get_sub_args(){
#ifdef USE_ARG_BUFFER
  char *token = strtok(arg, ",");
#else
  char *token = strtok(str, ",");
#endif
  sub_args[0] = atoi(token);
  token = strtok(NULL, ",");
  sub_args[1] = atoi(token);
  token = strtok(NULL, ",");
  sub_args[2] = atoi(token);
}

void CommandProcessor::reset_args(){
#ifdef USE_ARG_BUFFER
  strcpy(arg, "");
#endif
  sub_args[0] = 0;
  sub_args[1] = 0;
  sub_args[2] = 0;
}
int CommandProcessor::lookup_command(char * str){
  for(byte i = 0; i < num_commands; i++){
    if(is_command_P(str, (char*)pgm_read_word(&(commands[i])))){
      return CMD_FIRST + i;
    }
  }
  return CMD_NONE;
}

int CommandProcessor::get_command(){
  return lookup_command(this->str);
}

#endif
