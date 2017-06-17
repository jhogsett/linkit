#include "dependencies.h"

Dependencies dependencies;
Commands * commands = &dependencies.commands;

void setup() { 
  dependencies.begin();
}

void loop(){ 
  commands->process_events();
}

