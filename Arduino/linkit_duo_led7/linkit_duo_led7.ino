#include "dependencies.h"
#include "dispatch_command.h"

Dependencies dependencies;

void setup() { 
  dependencies.begin();
  dependencies.commands.reset();
  dependencies.buffer.erase(true);
  dependencies.commands.set_brightness_level();

  if(::needs_programming()){
#if defined(MINI_DISC_19)
  // monument animation
  // todo: rearrange with 0 as starting macro
  ::process_commands("era:100:lev");
  ::process_commands("0:stm:3:zon:0,2:pal:2,0,6:cpy");
  ::process_commands("1:stm:2:zon:0,2:pal:2,0,3:cpy");
  ::process_commands("2:stm:0:run:1:run:3:shf:rst");
  ::process_commands("3:stm:3:zon:rot:flu:rst");
  ::process_commands("4:stm:2:zon:rot:flu:rst");
  ::process_commands("800,3:sch");
  ::process_commands("1600,4:sch");
  ::process_commands("60000,2:sch");
  ::process_commands("2:run");
#elif defined(APOLLO_LIGHTS2)
  // fade into tungsten lamps
  // todo: rearrange with 10 as starting macro
  ::process_commands("70:lev:tun:flo:cfa:100:lev");
#elif defined(APOLLO_LIGHTS2_DEV) 
  // random colors with drains
  ::process_commands("10:stm:50,11:sch:25,12:sch:5000,13:sch");
  ::process_commands("11:stm:rng:pos:rnd:twi:flu:rst");
  ::process_commands("12:stm:rng:pos:sfd:flu:rst");
  ::process_commands("13:stm:14,8,5:run:200:del");
  ::process_commands("14:stm:15:run:16:run:17:run:flu");
  ::process_commands("15:stm:1:zon:blk:blk:2:zon:1:rev:blk:blk::rst");
  ::process_commands("16:stm:3:zon:blk:blk:4:zon:1:rev:blk:blk:rst");
  ::process_commands("17:stm:5:zon:blk:blk:6:zon:1:rev:blk:blk:rst");
#elif defined(WEARABLE_AND_GLASSES) || defined(RADIUS8)
  // random colors and random zone rotations
  ::process_commands("10:stm:era:50,11:sch:15,12:sch:200,16:sch");
  ::process_commands("11:stm:rng:pos:rnd:twi:flu:rst");
  ::process_commands("12:stm:rng:pos:sfd:flu:rst");
  ::process_commands("13:stm:-3:rng:zon");
  ::process_commands("14:stm:2:rng:rev");
  ::process_commands("15:stm:0,3:rng:rot");
  ::process_commands("16:stm:13:run:14:run:15:run:rst");
#endif
  }
  
  // run auto-start macro
  if(!::needs_programming()){
    ::process_commands("10:run");
  }

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

