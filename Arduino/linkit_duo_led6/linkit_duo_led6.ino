#include "dependencies.h"
#include "dispatch_command.h"

Dependencies dependencies;

void setup() { 
  dependencies.begin();
  dependencies.commands.reset();
  dependencies.buffer.erase(true);
  dependencies.commands.do_demo();
}

void loop(){ 
  if(dependencies.command_processor.received_command())
  {
    // resync the effects to a blank state to minimize visual artifacts of pausing and restarting
    dependencies.effects_processor.reset_effects();
    ::dispatch_command(dependencies.command_processor.get_command());
    dependencies.command_processor.acknowledge_command();
  }
  else 
  {
    // process the effects and update the display if needed
    if(dependencies.effects_processor.process_effects())
      dependencies.commands.flush();
  }
}

