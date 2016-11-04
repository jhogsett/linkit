#include <PololuLedStrip.h>
#include <random_seed.h>
#include <auto_brightness.h>
#include <color_math.h>
#include <colors.h>
#include <command_processor.h>
#include <power_ease.h>
#include <elastic_ease.h>
#include <blink_effects.h>
#include <breathe_effects.h>
#include <effects_processor.h>
#include <buffer.h>
#include <render.h>
#include "command_defs.h"
#include "commands.h"
#include "dispatch_command.h"

#include "config.h"
Config config;

#include "dependencies.h"
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
    dispatch_command(dependencies.command_processor.get_command());
    dependencies.command_processor.acknowledge_command();
  }
  else 
  {
    // process the effects and update the display if needed
    if(dependencies.effects_processor.process_effects())
      dependencies.commands.flush();
  }
}

