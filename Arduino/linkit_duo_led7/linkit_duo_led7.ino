#include "dependencies.h"
#include "default_macros.h"

Dependencies dependencies;
CommandProcessor * command_processor = &dependencies.command_processor;
Commands * commands = &dependencies.commands;
EffectsProcessor * effects_processor = &dependencies.effects_processor;
Scheduler * scheduler = &commands->scheduler;

void setup() { 
  dependencies.begin();

  // force a command acknowledgement to wake up any script that may be halted 
  // waiting for a character to be sent due to a new Arduino sketch being uploaded
  command_processor->acknowledge_command(true);
}

void loop(){ 
  //CommandProcessor &command_processor = dependencies.command_processor;
  if(command_processor->received_command())
  {
    commands->dispatch_command(command_processor->get_command());
    command_processor->acknowledge_command();

    // resync the effects to a blank state to minimize visual artifacts 
    // of pausing and restarting if there are display changes
    effects_processor->reset_effects();
  }
  else 
  {
    // do schedule processing
    if(!commands->is_paused()){
      scheduler->process_schedules();
    }
    
    // process the effects and update the display if needed
    if(effects_processor->process_effects())
      commands->flush_all();
  }
}

