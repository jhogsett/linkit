#include "dependencies.h"
#include "dispatch_command.h"

Dependencies dependencies;

#define DEFAULT_MACRO 10

void setup() { 
  dependencies.begin();
  dependencies.commands.reset();
  dependencies.buffer.erase(true);
  dependencies.commands.set_brightness_level();

//                    0         1         2         3         4         5         6
//                    0123456789012345678901234567890123456789012345678901234567890

  if(!::is_programmed(DEFAULT_MACRO)){

#if defined(MINI_DISC_19)
    // monument animation
    ::process_commands("10:stm:100:lev:800,14:sch:1600,15:sch:60000,13:sch");
    ::process_commands("11:stm:3:zon:0,2:pal:2,0,6:cpy");
    ::process_commands("12:stm:2:zon:0,2:pal:2,0,3:cpy");
    ::process_commands("13:stm:3:shf:11:run:12:run:rst");
    ::process_commands("14:stm:3:zon:rot:flu:rst");
    ::process_commands("15:stm:2:zon:rot:flu:rst");
#elif defined(APOLLO_LIGHTS2)
    // fade into tungsten lamps
    ::process_commands("10:stm:70:lev:tun:flo:cfa:100:lev");
#elif defined(APOLLO_LIGHTS2_DEV) 
    // random colors with drains
    ::process_commands("10:stm:25:run");

    ::process_commands("11:stm:50,12:sch:25,13:sch:5000,14:sch");
    ::process_commands("12:stm:rng:pos:rnd:twi:flu:rst");
    ::process_commands("13:stm:rng:pos:sfd:flu:rst");
    ::process_commands("14:stm:15,8,5:run:200:del");
    ::process_commands("15:stm:16:run:17:run:18:run:flu");
    ::process_commands("16:stm:1:zon:blk:blk:2:zon:1:rev:blk:blk::rst");
    ::process_commands("17:stm:3:zon:blk:blk:4:zon:1:rev:blk:blk:rst");
    ::process_commands("18:stm:5:zon:blk:blk:6:zon:1:rev:blk:blk:rst");
    ::process_commands("19:stm:0,11:sch:0,12:sch:0,13:sch");

    // bubbling up dots
    ::process_commands("20:stm:50,21:sch:50,22:sch:25,23:sch");
    ::process_commands("21:stm:9:zon:rnd:blk:4:rng:rep:flu:500,50,1:rng:sch");
    ::process_commands("22:stm:10:zon:car:flu");
    ::process_commands("23:stm:11:zon:car:flu");
    ::process_commands("24:stm:0,21:sch:0,22:sch:0,23:sch");

    ::process_commands("25:stm:24:run:11:run:10000,25,26:sch");
    ::process_commands("26:stm:19:run:20:run:10000,25,25:sch");

#elif defined(WEARABLE_AND_GLASSES) || defined(RADIUS8) || defined(PROJECTOR)
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
  if(::is_programmed(DEFAULT_MACRO)){
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

