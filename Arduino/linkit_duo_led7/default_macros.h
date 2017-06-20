//#ifndef DEFAULT_MACROS_H
//#define DEFAULT_MACROS_H
//
//void Commands::run_default_macro(){
//
//// #define FORCE_PROGRAM_MACROS
//
//#if !defined(FORCE_PROGRAM_MACROS)
//  if(!macros.is_macro_programmed(DEFAULT_MACRO)){
//#endif
//
////#if defined(SPHERE) || defined(DUAL_STRIP) || defined(WEARABLE_AND_HARDHAT) || defined(RADIUS8) // || defined(APOLLO_LIGHTS2) //|| defined(WEARABLE_AND_DISC93)
//#if defined(THREE_WAY_MACROS)
//    process_commands(F("10:set:19:run"));
//
//    process_commands(F("11:set:wht:brt:brt:flo:flu:30:del"));
//    process_commands(F("12:set:rnd:brt:brt:sfd:flo:flu"));
//    process_commands(F("13:set:11:run:12:run"));
//    process_commands(F("14:set:13:run:20000,10000,14:rng:sch"));
//    
//    process_commands(F("15:set:rps:wht:brt:brt:sfd:rst"));
//    process_commands(F("16:set:rps:1:rnd:sfd:flu:rst"));
//    process_commands(F("17:set:fad:50,15:sch:20000,14:sch"));
//    process_commands(F("18:set:fad:50,16:sch"));
//    
//    process_commands(F("19:set:-1:sch:60000,19,20:sch:17:run"));
//    process_commands(F("20:set:-1:sch:60000,19,21:sch:18:run"));
//    process_commands(F("21:set:-1:sch:60000,19,19:sch:23:run"));
//
//    process_commands(F("22:set:-3:rng:zon:art:flu:rst"));
//    process_commands(F("23:set:fad:2:rnd:flo:flu:50,22:sch"));
//
//#elif defined(MINI_DISC_19)
//    // monument animation
//    process_commands(F("10:set:100:lev:800,14:sch:1600,15:sch:60000,13:sch"));
//    process_commands(F("11:set:3:zon:0,2:pal:2,0,6:cpy"));
//    process_commands(F("12:set:2:zon:0,2:pal:2,0,3:cpy"));
//    process_commands(F("13:set:3:shf:11:run:12:run:rst"));
//    process_commands(F("14:set:3:zon:art:flu:rst"));
//    process_commands(F("15:set:2:zon:art:flu:rst"));
//
////#elif defined(APOLLO_LIGHTS2) //|| defined(APOLLO_LIGHTS2_DEV)
//#elif defined(APOLLO_MACROS)
//    // random colors into warm white
//    process_commands(F("10:set:clr:-1:sch:1:pau:21:run"));
//
//    // sparking colors with drain
//    process_commands(F("11:set:clr:100,13:sch:10000,14:sch"));
//
////    // sparkling one color at a time
////    process_commands(F("12:set:clr:100,19:sch:5000,20:sch"));
//
//    process_commands(F("12:set:clr:30:run"));
//
//    process_commands(F("13:set:-1:rps:1:rnd:sfd:flu:rst"));
//    process_commands(F("14:set:15,15:run:500:del"));
//    process_commands(F("15:set:16:run:17:run:18:run:flu"));
//    process_commands(F("16:set:1:zon:1:blk:2:zon:1:rev:1:blk:rst"));
//    process_commands(F("17:set:3:zon:1:blk:4:zon:1:rev:1:blk:rst"));
//    process_commands(F("18:set:5:zon:1:blk:6:zon:1:rev:1:blk:rst"));
//
////    process_commands(F("19:set:-1:rps:2:rng:pal:sfd:flu:rst"));
////    process_commands(F("20:set:3:shf"));
//
//    process_commands(F("21:set:era:50:lev:13,120:run:tun:flo:cfa:100:lev"));
//
//    // fade into tungsten lamps
//    // process_commands(F("10:set:70:lev:tun:flo:cfa:100:lev"));
//
//    process_commands(F("30:set:1:pau:40:run:44:run:46:run"));
//
//    process_commands(F("31:set:0:sqs:pos:0:pal:sfd:rst:flu"));
//    process_commands(F("32:set:1:sqs:pos:1:pal:sfd:rst"));
//    process_commands(F("33:set:2:sqs:pos:2:pal:sfd:rst"));
//    process_commands(F("34:set:3:sqs:pos:3:pal:sfd:rst"));
//    process_commands(F("35:set:4:sqs:pos:4:pal:sfd:rst"));
//    process_commands(F("36:set:5,:sqs:pos:5:pal:sfd:rst"));
//   
//    process_commands(F("37:set:0,15,0:sqs:1,30,15:sqs"));
//    process_commands(F("38:set:2,45,30:sqs:3,60,45:sqs"));
//    process_commands(F("39:set:4,75,60:sqs:5,90,75:sqs"));
//    process_commands(F("40:set:37:run:38:run:39:run"));
//
////    process_commands(F("41:set:31,31:sch:32,32:sch"));
////    process_commands(F("42:set:33,33:sch:34,34:sch"));
////    process_commands(F("43:set:35,35:sch:36,36:sch"));
////    process_commands(F("44:set:41:run:42:run:43:run"));
//
//    process_commands(F("41:set:21,31:sch:22,32:sch"));
//    process_commands(F("42:set:23,33:sch:24,34:sch"));
//    process_commands(F("43:set:25,35:sch:26,36:sch"));
//    process_commands(F("44:set:41:run:42:run:43:run"));
//
//    process_commands(F("45:set:3:shf"));
//    process_commands(F("46:set:700,45:sch"));
//
//
//
//
//
//
////#elif defined(APOLLO_LIGHTS2_DEV) 
//#elif defined(TWO_WAY_MACROS) 
//    // start up
////    process_commands(F("10:set:30:run"));
////
////    // random colors with drains
////    process_commands(F("11:set:60,12:sch:20,13:sch:5000,14:sch"));
////    process_commands(F("12:set:-1:rps:rnd:twi:flu:rst"));
////    process_commands(F("13:set:-2:rps:sfd:flu:rst"));
////    process_commands(F("14:set:15,8,5:run:200:del"));
////    process_commands(F("15:set:16:run:17:run:18:run:flu"));
////    process_commands(F("16:set:1:zon:2:blk:2:zon:1:rev:2:blk:rst"));
////    process_commands(F("17:set:3:zon:2:blk:4:zon:1:rev:2:blk:rst"));
////    process_commands(F("18:set:5:zon:2:blk:6:zon:1:rev:2:blk:rst"));
////
////    // bubbling up dots
////    process_commands(F("20:set:60,21:sch:30,22:sch:15,23:sch"));
////    process_commands(F("21:set:9:zon:rnd:flu:3,1,1:rng:blk:flu")); 
////    process_commands(F("22:set:10:zon:car:flu"));
////    process_commands(F("23:set:11:zon:car:flu"));
////
////    // switch betwen the two patterns
////    process_commands(F("25:set:-1:sch:11:run:10000,25,26:sch"));
////    process_commands(F("26:set:-1:sch:20:run:10000,25,25:sch"));
////
////    process_commands(F("30:set:clr:100,31:sch:5000,32:sch"));
////    process_commands(F("31:set:-1:rps:pal:sfd:flu:rst"));
////    process_commands(F("32:set:3:shf"));
//
//    process_commands(F("10:set:20:run:24:run"));
//
//    process_commands(F("11:set:0:sqs:pos:0:pal:ffd:rst"));
//    process_commands(F("12:set:1:sqs:pos:1:pal:ffd:rst"));
//    process_commands(F("13:set:2:sqs:pos:2:pal:ffd:rst"));
//    process_commands(F("14:set:3:sqs:3,-2:sqs:pos:3:pal:ffd:rst"));
//    process_commands(F("15:set:4:sqs:4,-2:sqs:pos:4:pal:ffd:rst"));
//    process_commands(F("16:set:5:sqs:5,-2:sqs:pos:5:pal:ffd:rst"));
//
//    process_commands(F("17:set:0,14,0:sqs:1,29,15:sqs"));
//    process_commands(F("18:set:2,44,30:sqs:3,59,45:sqs"));
//    process_commands(F("19:set:4,74,60:sqs:5,89,75:sqs"));
//    process_commands(F("20:set:17:run:18:run:19:run"));
//
//    process_commands(F("21:set:50,11:sch:50,12:sch"));
//    process_commands(F("22:set:50,13:sch:50,14:sch"));
//    process_commands(F("23:set:50,15:sch:50,16:sch"));
//    process_commands(F("24:set:21:run:22:run:23:run"));
//
////    process_commands(F("25:set:0,-3:sqs:1,-3:sqs"));
////    process_commands(F("26:set:2,-3:sqs:3,-3:sqs"));
////    process_commands(F("27:set:4,-3:sqs:5,-3:sqs"));
////    process_commands(F("28:set:25:run:26:run:27:run"));
//
////#elif defined(WEARABLE_AND_GLASSES) || defined(PROJECTOR) || defined(WEARABLE_AND_DISC93) || defined(DISC93) // || defined(RADIUS8) 
////    // random colors and random zone rotations
////#ifdef DISC93
////    process_commands(F("10:set:100:lev:era:50,11:sch:15,12:sch:200,16:sch"));
////#else    
////    process_commands(F("10:set:era:50,11:sch:15,12:sch:200,16:sch"));
////#endif
//#elif defined(ONE_WAY_MACROS)
//    process_commands(F("10:set:era:50,11:sch:15,12:sch:200,16:sch"));
//    process_commands(F("11:set:rng:pos:rnd:twi:flu:rst"));
//    process_commands(F("12:set:rng:pos:sfd:flu:rst"));
//    process_commands(F("13:set:-3:rng:zon"));
//    process_commands(F("14:set:2:rng:rev"));
//    process_commands(F("15:set:0,3:rng:art"));
//    process_commands(F("16:set:13:run:14:run:15:run:rst"));
//#endif
//
//#if !defined(FORCE_PROGRAM_MACROS)
//  }
//#endif
//  
//  // run auto-start macro
//  if(macros.is_macro_programmed(DEFAULT_MACRO)){
//    process_commands(F("10:run"));
//  }
//
//}
//
//#endif

