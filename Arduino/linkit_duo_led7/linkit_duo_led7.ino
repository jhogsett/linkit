#include "dependencies.h"
//#include "default_macros.h"

Dependencies dependencies;
Commands * commands = &dependencies.commands;

void setup() { 
  dependencies.begin();
}

void loop(){ 
  commands->process_events();
}

