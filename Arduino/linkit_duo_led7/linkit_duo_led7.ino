#include "dependencies.h"
#include "dispatch_command.h"

Dependencies dependencies;

void setup() { 
  dependencies.begin();
  dependencies.commands.reset();
  dependencies.buffer.erase(true);
  dependencies.commands.set_brightness_level();

//#if !defined(MINI_DISC_19) && !(defined RADIUS8)
//  dependencies.self_test();
//#endif

#if defined(MINI_DISC_19)
  ::process_commands("era:flu:70:lev");
  ::process_commands("0:stm:3:zon:rnd:brt:brt:5:rep:cfa:rst");
  ::process_commands("1:stm:3:zon:rnd:dim:dim:5:rep:cfa:rst");
  ::process_commands("2:stm:0:run:1:run:flu:rst");
  ::process_commands("3:stm:3:zon:rot:rst");
  ::process_commands("60000,2:sch");
  ::process_commands("1000,3:sch");
  ::process_commands("2:run");

#elif defined(APOLLO_LIGHTS2)
  ::process_commands("70:lev:tun:flo:cfa:100:lev");

#elif defined(APOLLO_LIGHTS2_DEV) 
  ::process_commands("era:flu");
  ::process_commands("20:stm:rng:pos:rnd:twi:flu:rst");
  ::process_commands("50,20:sch");
  ::process_commands("21:stm:rng:pos:sfd:flu:rst");
  ::process_commands("25,21:sch");
  ::process_commands("25:stm:26,8,5:run:200:del");
  ::process_commands("5000,25:sch");
  ::process_commands("26:stm:27:run:28:run:29:run:flu");
  ::process_commands("27:stm:6:zon:blk:blk:7:zon:1:rev:blk:blk::rst");
  ::process_commands("28:stm:8:zon:blk:blk:9:zon:1:rev:blk:blk:rst");
  ::process_commands("29:stm:10:zon:blk:blk:11:zon:1:rev:blk:blk:rst");

#elif defined(RADIUS8)
  ::process_commands("era:flu");
  ::process_commands("10:stm:rng:pos:rnd:twi:flu:rst");
  ::process_commands("50,10:sch");
  ::process_commands("11:stm:rng:pos:sfd:flu:rst");
  ::process_commands("15,11:sch");
  ::process_commands("12:stm:-2,-2:rng:zon:2:rng:rev:0,3,1:rng:rot:rst");
  ::process_commands("200,12:sch");

#elif defined(WEARABLE_AND_GLASSES)
  ::process_commands("era:flu");
  ::process_commands("0:stm:rng:pos:rnd:twi:flu:rst");
  ::process_commands("50,0:sch");
  ::process_commands("1:stm:rng:pos:sfd:flu:rst");
  ::process_commands("15,1:sch");
  ::process_commands("10:stm:-2,-2:rng:zon");
  ::process_commands("11:stm:2:rng:rev");
  ::process_commands("12:stm:0,3,1:rng:rot");
  ::process_commands("13:stm:10:run:11:run:12:run:rst");
  ::process_commands("200,13:sch");

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

