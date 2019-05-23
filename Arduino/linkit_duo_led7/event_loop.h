#ifndef EVENT_LOOP
#define EVENT_LOOP

void Commands::process_events()
{
  if(command_processor->received_command())
  {
    dispatch_command(command_processor->get_command());
    command_processor->acknowledge_command();

    // resync the effects to a blank state to minimize visual artifacts 
    // of pausing and restarting if there are display changes
    if(reset_effects_on_command)
      effects_processor->reset_effects();
  }
  else 
  {
    // do schedule processing
    if(!schedules_paused)
      scheduler.process_schedules();

    // process the effects and update the display if needed
    if(!effects_paused && effects_processor->process_effects())
#ifdef USE_MULTIPLE_DISPLAYS    
      flush_all();
#else
      flush();
#endif

#ifdef USE_KEYBOARD
    if(!keyboard_paused)
      process_keyboard();
#endif
  }
}
#endif
