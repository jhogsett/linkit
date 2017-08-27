#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <macros.h>

#define NUM_SCHEDULES NUM_MACROS

class Scheduler
{
  public:

  void begin(Macros * macros);
  void set_schedule(unsigned int schedule_period_, byte schedule_number, byte macro_number_);
  void process_schedules();
  void reset_all_schedules();
  void reset_schedule(byte schedule_number);

  private:

  static unsigned int schedule_period[NUM_SCHEDULES];  // zero means the schedule is turned off
  static byte macro_number[NUM_SCHEDULES];             // could leave this off and assume schedule is same as macro to run,
  static unsigned int schedule_counter[NUM_SCHEDULES]; // but the ability to switch schedules to run different macros is the basis for toggling

  Macros * macros;
};

unsigned int Scheduler::schedule_period[NUM_SCHEDULES];  // zero means the schedule is turned off
byte Scheduler::macro_number[NUM_SCHEDULES];             // could leave this off and assume schedule is same as macro to run,
unsigned int Scheduler::schedule_counter[NUM_SCHEDULES]; // but the ability to switch schedules to run different macros is the basis for toggling

void Scheduler::begin(Macros * macros){
  this->macros = macros;
}

void Scheduler::process_schedules(){
  for(byte i = 0; i < NUM_SCHEDULES; i++){

    // skip disabled schedules
    if(schedule_period[i] == 0)
      continue;

    schedule_counter[i] = (schedule_counter[i] + 1) % schedule_period[i];

    if(schedule_counter[i] == 0){
      macros->run_macro(macro_number[i]);
    }
  }
}

// arg[0] schedule period 0-65534, -1 clears all schedules
// arg[1] schedule number, default schedule #0
// arg[2] macro number, default same as schedule #
void Scheduler::set_schedule(unsigned int schedule_period_, byte schedule_number, byte macro_number_){
  if((int)schedule_period_ == -1){
    reset_all_schedules();
    return;
  }

  if(macro_number_ == 0){
    // default the macro to be the same as the schedule
    macro_number_ = schedule_number;
  }

  reset_schedule(schedule_number);
  schedule_period[schedule_number]  = schedule_period_;
  macro_number[schedule_number]     = macro_number_;

  // set to zero for a complete schedule period to pass before it runs the macro (probably best)
  // could set to schedule_period - 1 to have the macro run immediately upon being set
  schedule_counter[schedule_number] = 0;
}

void Scheduler::reset_schedule(byte schedule_number){
  macro_number[schedule_number]     = 0;
  schedule_period[schedule_number]  = 0;
  schedule_counter[schedule_number] = 0;
}

void Scheduler::reset_all_schedules(){
  for(byte i = 0; i < NUM_SCHEDULES; i++){
    reset_schedule(i);
  }
}

#endif
