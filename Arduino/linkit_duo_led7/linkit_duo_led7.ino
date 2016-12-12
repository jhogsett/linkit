// enable to change settings for the 8-LED wearable version
// #define WEARABLE

// enable to change settings for the 93-LED disc
// #define DISC93

// enable to change settings for a single 50-LED strand
// #define STRAND1

// enable to change settings for two 50-LED strands
#define STRAND2

#include "dependencies.h"
#include "dispatch_command.h"

Dependencies dependencies;

void setup() { 
  dependencies.begin();
  dependencies.commands.reset();
  dependencies.buffer.erase(true);
  dependencies.commands.set_brightness_level();
  dependencies.commands.do_demo();
}

void loop(){ 
  if(dependencies.command_processor.received_command())
  {
    // resync the effects to a blank state to minimize visual artifacts of pausing and restarting
    // dependencies.effects_processor.reset_effects();
    ::dispatch_command(dependencies.command_processor.get_command());
    dependencies.command_processor.acknowledge_command();

    // will this work here?
    dependencies.effects_processor.reset_effects();
    
  }
  else 
  {
    // process the effects and update the display if needed
    if(dependencies.effects_processor.process_effects())
      dependencies.commands.flush();
  }
}

