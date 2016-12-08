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
  char arg[MAX_STRING_LENGTH];
  int sub_args[NUM_SUB_ARGS];
  char **commands; 
  byte num_commands;
  HardwareSerial *serial;

  void begin(HardwareSerial *serial, char *commands[], byte num_commands);
  bool input_available();
  bool received_command();
  void acknowledge_command();
  void send_ack();
  int get_command();
  void save_args();
  void reset_args();

  private:
  bool str_equal(char *str1, char *str2);
  bool is_command(char *str, char *command);
  void get_sub_args();
};

void CommandProcessor::begin(HardwareSerial *serial, char **commands, byte num_commands){
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

void CommandProcessor::acknowledge_command(){
  if(!input_available()) 
    // only acknowledge when there are no more commands available
    // so that multiple additional commands if any are processed first
    send_ack();
}

void CommandProcessor::send_ack(){
  serial->write("k");
}

void CommandProcessor::save_args(){
  strcpy(arg, str); 
  get_sub_args();
}

bool CommandProcessor::str_equal(char *str1, char *str2){
  int l = strlen(str1);
  if(l != strlen(str2))
    return false;

  for(int i = 0; i < l; i++){
    if(str1[i] != str2[i])
      return false;
  }
  return true;
}

bool CommandProcessor::is_command(char *str, char *command){
  return str_equal(str, command);  
}

void CommandProcessor::get_sub_args(){
  char *token = strtok(arg, ",");
  sub_args[0] = atoi(token);
  token = strtok(NULL, ",");
  sub_args[1] = atoi(token);
  token = strtok(NULL, ",");
  sub_args[2] = atoi(token);
}

void CommandProcessor::reset_args(){
  strcpy(arg, "");
  sub_args[0] = 0;
  sub_args[1] = 0;
  sub_args[2] = 0;
}

int CommandProcessor::get_command(){
  for(byte i = 0; i < num_commands; i++){
    if(is_command(str, commands[i])){
      return CMD_FIRST + i;
    }
  }
  return CMD_NONE;
}

#endif

