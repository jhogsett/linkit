#ifndef COMMAND_PROCESSOR_H
#define COMMAND_PROCESSOR_H

#define DELIMITER_CHAR ':'
#define DELIMITER_STR ":"
#define MAX_STRING_LENGTH 20
#define NUM_SUB_ARGS 3
#define CMD_NULL -1
#define CMD_NONE 0
#define CMD_FIRST 1

class CommandProcessor
{
  public:
  char str[MAX_STRING_LENGTH];

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
  int get_command(char * str);
  void save_args(char * str);
  void reset_args();
  int process_command(char * str);
  int begin_get_commands(char * str, char **saveptr, char *);
  int get_next_command(char **saveptr);
  char * get_input_buffer();

  private:
  bool str_equal_P(char *str1, const char *str2);
  bool is_command_P(char *str, const char *command);
  void get_sub_args(char * str);
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
    this->str[c] = 0;
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

void CommandProcessor::save_args(char * args = NULL){
  get_sub_args(args);
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

void CommandProcessor::get_sub_args(char * args = NULL){
  if(args == NULL){
    args = this->str;
  }

  char *saveptr;
  char *token = strtok_r(args, ",", &saveptr);

  sub_args[0] = atoi(token);
  token = strtok_r(NULL, ",", &saveptr);
  sub_args[1] = atoi(token);
  token = strtok_r(NULL, ",", &saveptr);
  sub_args[2] = atoi(token);
}

void CommandProcessor::reset_args(){
  sub_args[0] = 0;
  sub_args[1] = 0;
  sub_args[2] = 0;
}
int CommandProcessor::lookup_command(char * str){
  if(str == NULL){
    return CMD_NULL;
  }

  for(byte i = 0; i < num_commands; i++){
    if(is_command_P(str, (char*)pgm_read_word(&(commands[i])))){
      return CMD_FIRST + i;
    }
  }
  return CMD_NONE;
}

int CommandProcessor::get_command(char * str = NULL){
  if(str == NULL){
    str = this->str;
  }
  return lookup_command(str);
}

int CommandProcessor::process_command(char * str){
  int command = lookup_command(str);

  // if CMD_NONE is returned, the command wasn't found
  // assume the string contains arguments to save
  //
  // the default save_args() should not be called by
  // the calling code in this case
  if(command == CMD_NONE){
    save_args(str);
  }

  return command;
}

int CommandProcessor::begin_get_commands(char * str, char **saveptr, char * buffer){
  // strcpy(this->str, str);
  strcpy(buffer, str);
  // return process_command(strtok_r(this->str, DELIMITER_STR, saveptr));
  return process_command(strtok_r(buffer, DELIMITER_STR, saveptr));
}

int CommandProcessor::get_next_command(char **saveptr){
  return process_command(strtok_r(NULL, DELIMITER_STR, saveptr));
}

char * CommandProcessor::get_input_buffer(){
  if(input_available()){
    int c = serial->readBytesUntil('\0', this->str, MAX_STRING_LENGTH-1);
    this->str[c] = 0;
    return this->str;
  } else {
    return NULL;
  }
}


#endif

 //need to prevent string getting corrupted by being tokenized
