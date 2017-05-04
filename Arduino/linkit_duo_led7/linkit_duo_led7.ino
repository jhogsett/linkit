#include "dependencies.h"
#include "dispatch_command.h"

Dependencies dependencies;

void setup() { 
  dependencies.begin();
  dependencies.commands.reset();
  dependencies.buffer.erase(true);
  dependencies.commands.set_brightness_level();
  
  dependencies.self_test();

#if defined(MINI_DISC_19)
  ::process_commands("blk:flo:cfa:70:lev:1000:del");
  ::process_commands("0:stm:3:zon:rnd:flo:cfa");
  ::process_commands("1:stm:4:zon:rnd:flo:cfa");
  ::process_commands("5000,0,0:sch:8000,1,1:sch");

//#elif defined(APOLLO_LIGHTS2_DEV)
//  ::process_commands("era:blu:twi:9:rep");
//  ::process_commands("grn:twi:9:rep");
//  ::process_commands("red:twi:9:rep");
//  ::process_commands("wht:twi:9:rep");

#elif defined(APOLLO_LIGHTS2)
  ::process_commands("70:lev:tun:flo:cfa:100:lev");

#else
  ::process_commands("era:flu");
  ::process_commands("0:stm:1:rnd:4:rep:5,0,5:cpy:flu");
  ::process_commands("10000,0,0:sch");
  ::process_commands("1:stm:0:rng:pos:sfd:rst:flu");
  ::process_commands("20,1,1:sch");
  ::process_commands("2:stm:0:rng:pos:wht:brt:brt:ffd:rst:flu");
  ::process_commands("100,2,2:sch");


#endif

  // force a command acknowledgement to wake up any script that may be halted 
  // waiting for a character to be sent due to a new Arduino sketch being uploaded
  dependencies.command_processor.acknowledge_command(true);
}

void loop(){ 
  if(dependencies.command_processor.received_command())
  {
    ::dispatch_command(dependencies.command_processor.get_command());
    dependencies.command_processor.acknowledge_command();

    // resync the effects to a blank state to minimize visual artifacts 
    // of pausing and restarting if there are display changes
    dependencies.effects_processor.reset_effects();
  }
  else 
  {
    // do schedule processing here for now
    if(!dependencies.commands.is_paused()){
      process_schedules();
    }
    
    // process the effects and update the display if needed
    if(dependencies.effects_processor.process_effects())
      dependencies.commands.flush_all();
  }
}

