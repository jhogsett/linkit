#include "dependencies.h"
#include "dispatch_command.h"

Dependencies dependencies;

void setup() { 
  dependencies.begin();
  dependencies.commands.reset();
  dependencies.buffer.erase(true);
  dependencies.commands.set_brightness_level();

//#if defined(STRIP3) || defined(STRIP2)
  for(int i = 0; i < NUM_BUFFERS; i++){
    //dependencies.commands.set_buffer((NUM_BUFFERS - 1) - i);
    dependencies.commands.set_display((NUM_BUFFERS - 1) - i);
    dependencies.commands.do_demo();
  }

//#else
//  dependencies.commands.do_demo();
//#endif


//  ::dispatch_command(dependencies.command_processor.lookup_command("demo"));

  // force a command acknowledgement to wake up any script
  // that may be halted waiting for a character to be sent
  // due to a new Arduino sketch being uploaded
  dependencies.command_processor.acknowledge_command(true);
}

void loop(){ 
  if(dependencies.command_processor.received_command())
  {
    // dependencies.effects_processor.reset_effects();
    ::dispatch_command(dependencies.command_processor.get_command());
    dependencies.command_processor.acknowledge_command();

    // resync the effects to a blank state to minimize visual artifacts of pausing and restarting
    dependencies.effects_processor.reset_effects();
  }
  else 
  {
    // process the effects and update the display if needed
    if(dependencies.effects_processor.process_effects())
      dependencies.commands.flush_all();
  }
}

