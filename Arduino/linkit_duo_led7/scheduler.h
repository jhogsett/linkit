#ifndef SCHEDULER_H
#define SCHEDULER_H

#include "config.h"

// for a schedule, need
// - a macro number to run, a number of times to run, an interval between runs


#define NUM_SCHEDULES (NUM_MACROS)

unsigned int schedule_period[NUM_SCHEDULES];  // zero means the schedule is turned off
byte macro_number[NUM_SCHEDULES];    
byte run_times[NUM_SCHEDULES];       
unsigned int schedule_counter[NUM_SCHEDULES];

void process_schedules(){
  for(int i = 0; i < NUM_SCHEDULES; i++){

    if(schedule_period[i] == 0){
      continue;
    }
    
    schedule_counter[i] = (schedule_counter[i] + 1) % schedule_period[i];

    if(schedule_counter[i] == 0){
      ::run_macro(macro_number[i], run_times[i]);
    }
  }
}

void reset_schedule(int schedule_number){
  macro_number[schedule_number]     = 0;
  run_times[schedule_number]        = 0;
  schedule_period[schedule_number]  = 0;
  schedule_counter[schedule_number] = 0;
}

void reset_all_schedules(){
  for(int i = 0; i < NUM_SCHEDULES; i++){
    reset_schedule(i); 
  }
}

// if schedule period is -1, it clears all schedules
void set_schedule(unsigned int schedule_period_, byte schedule_number, byte macro_number_, byte run_times_ = 1){
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
  run_times[schedule_number]        = run_times_;

  // set to zero for a complete schedule period to pass before it runs the macro (probably best)
  // could set to schedule_period - 1 to have the macro run immediately upon being set
  schedule_counter[schedule_number] = 0;
}


#endif

