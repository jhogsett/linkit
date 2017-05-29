#ifndef DEFAULT_MACROS_H
#define DEFAULT_MACROS_H

#define DEFAULT_MACRO 10

extern class Dependencies dependencies;

void run_default_macro(){

// #define FORCE_PROGRAM_MACROS

#if !defined(FORCE_PROGRAM_MACROS)
  if(!dependencies.commands.is_programmed(DEFAULT_MACRO)){
#endif

#if defined(SPHERE) || defined(DUAL_STRIP) || defined(WEARABLE_AND_HARDHAT) || defined(RADIUS8) // || defined(APOLLO_LIGHTS2) //|| defined(WEARABLE_AND_DISC93)
    dependencies.commands.process_commands_P(F("10:set:19:run"));

    dependencies.commands.process_commands_P(F("11:set:wht:brt:brt:flo:flu:30:del"));
    dependencies.commands.process_commands_P(F("12:set:rnd:brt:brt:sfd:flo:flu"));
    dependencies.commands.process_commands_P(F("13:set:11:run:12:run"));
    dependencies.commands.process_commands_P(F("14:set:13:run:20000,10000,14:rng:sch"));
    
    dependencies.commands.process_commands_P(F("15:set:rps:wht:brt:brt:sfd:rst"));
    dependencies.commands.process_commands_P(F("16:set:rps:1:rnd:sfd:flu:rst"));
    dependencies.commands.process_commands_P(F("17:set:fad:50,15:sch:20000,14:sch"));
    dependencies.commands.process_commands_P(F("18:set:fad:50,16:sch"));
    
    dependencies.commands.process_commands_P(F("19:set:-1:sch:60000,19,20:sch:17:run"));
    dependencies.commands.process_commands_P(F("20:set:-1:sch:60000,19,21:sch:18:run"));
    dependencies.commands.process_commands_P(F("21:set:-1:sch:60000,19,19:sch:23:run"));

    dependencies.commands.process_commands_P(F("22:set:-3:rng:zon:rot:flu:rst"));
    dependencies.commands.process_commands_P(F("23:set:fad:2:rnd:flo:flu:50,22:sch"));

#elif defined(MINI_DISC_19)
    // monument animation
    dependencies.commands.process_commands_P(F("10:set:100:lev:800,14:sch:1600,15:sch:60000,13:sch"));
    dependencies.commands.process_commands_P(F("11:set:3:zon:0,2:pal:2,0,6:cpy"));
    dependencies.commands.process_commands_P(F("12:set:2:zon:0,2:pal:2,0,3:cpy"));
    dependencies.commands.process_commands_P(F("13:set:3:shf:11:run:12:run:rst"));
    dependencies.commands.process_commands_P(F("14:set:3:zon:rot:flu:rst"));
    dependencies.commands.process_commands_P(F("15:set:2:zon:rot:flu:rst"));

#elif defined(APOLLO_LIGHTS2) //|| defined(APOLLO_LIGHTS2_DEV)
    // random colors into warm white
    dependencies.commands.process_commands_P(F("10:set:clr:era:70:lev:13,120:run:tun:flo:cfa:100:lev"));

    // sparking colors with drain
    dependencies.commands.process_commands_P(F("11:set:clr:100,13:sch:10000,14:sch"));

    // sparkling white
    dependencies.commands.process_commands_P(F("12:set:clr:200,19:sch"));

    dependencies.commands.process_commands_P(F("13:set:-1:rps:1:rnd:sfd:flu:rst"));
    dependencies.commands.process_commands_P(F("14:set:15,15:run:500:del"));
    dependencies.commands.process_commands_P(F("15:set:16:run:17:run:18:run:flu"));
    dependencies.commands.process_commands_P(F("16:set:1:zon:1:blk:2:zon:1:rev:1:blk:rst"));
    dependencies.commands.process_commands_P(F("17:set:3:zon:1:blk:4:zon:1:rev:1:blk:rst"));
    dependencies.commands.process_commands_P(F("18:set:5:zon:1:blk:6:zon:1:rev:1:blk:rst"));

    dependencies.commands.process_commands_P(F("19:set:-1:rps:wht:sfd:flu:rst"));

    // fade into tungsten lamps
    dependencies.commands.process_commands_P(F("10:set:70:lev:tun:flo:cfa:100:lev"));

#elif defined(APOLLO_LIGHTS2_DEV) 
    // start up
    dependencies.commands.process_commands_P(F("10:set:25:run"));

    // random colors with drains
    dependencies.commands.process_commands_P(F("11:set:60,12:sch:20,13:sch:5000,14:sch"));
    dependencies.commands.process_commands_P(F("12:set:-1:rps:rnd:twi:flu:rst"));
    dependencies.commands.process_commands_P(F("13:set:-2:rps:sfd:flu:rst"));
    dependencies.commands.process_commands_P(F("14:set:15,8,5:run:200:del"));
    dependencies.commands.process_commands_P(F("15:set:16:run:17:run:18:run:flu"));
    dependencies.commands.process_commands_P(F("16:set:1:zon:2:blk:2:zon:1:rev:2:blk:rst"));
    dependencies.commands.process_commands_P(F("17:set:3:zon:2:blk:4:zon:1:rev:2:blk:rst"));
    dependencies.commands.process_commands_P(F("18:set:5:zon:2:blk:6:zon:1:rev:2:blk:rst"));

    // bubbling up dots
    dependencies.commands.process_commands_P(F("20:set:50,21:sch:50,22:sch:25,23:sch"));
    dependencies.commands.process_commands_P(F("21:set:9:zon:rnd:4,1:rng:blk:flu")); 
    dependencies.commands.process_commands_P(F("22:set:10:zon:car:flu"));
    dependencies.commands.process_commands_P(F("23:set:11:zon:car:flu"));

    // switch betwen the two patterns
    dependencies.commands.process_commands_P(F("25:set:-1:sch:11:run:10000,25,26:sch"));
    dependencies.commands.process_commands_P(F("26:set:-1:sch:20:run:10000,25,25:sch"));

#elif defined(WEARABLE_AND_GLASSES) || defined(PROJECTOR) || defined(WEARABLE_AND_DISC93) || defined(DISC93) // || defined(RADIUS8) 
    // random colors and random zone rotations
#ifdef DISC93
    dependencies.commands.process_commands_P(F("10:set:100:lev:era:50,11:sch:15,12:sch:200,16:sch"));
#else    
    dependencies.commands.process_commands_P(F("10:set:era:50,11:sch:15,12:sch:200,16:sch"));
#endif
    dependencies.commands.process_commands_P(F("11:set:rng:pos:rnd:twi:flu:rst"));
    dependencies.commands.process_commands_P(F("12:set:rng:pos:sfd:flu:rst"));
    dependencies.commands.process_commands_P(F("13:set:-3:rng:zon"));
    dependencies.commands.process_commands_P(F("14:set:2:rng:rev"));
    dependencies.commands.process_commands_P(F("15:set:0,3:rng:rot"));
    dependencies.commands.process_commands_P(F("16:set:13:run:14:run:15:run:rst"));
#endif

#if !defined(FORCE_PROGRAM_MACROS)
  }
#endif
  
  // run auto-start macro
  if(dependencies.commands.is_programmed(DEFAULT_MACRO)){
    dependencies.commands.process_commands_P(F("10:run"));
  }

}
#endif

