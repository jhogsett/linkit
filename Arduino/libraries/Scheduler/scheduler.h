#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <macros.h>

#define NUM_SCHEDULES NUM_MACROS

class Scheduler
{
  public:

  void begin(Macros * macros);
  void set_schedule(unsigned int schedule_period_, byte schedule_number);
  void process_schedules();
  void reset_all_schedules();

  private:

  static unsigned int schedule_period[NUM_SCHEDULES];  // zero means the schedule is turned off
  static unsigned int schedule_counter[NUM_SCHEDULES]; // but the ability to switch schedules to run different macros is the basis for toggling

  Macros * macros;
};

unsigned int Scheduler::schedule_period[NUM_SCHEDULES];  // zero means the schedule is turned off
unsigned int Scheduler::schedule_counter[NUM_SCHEDULES]; // but the ability to switch schedules to run different macros is the basis for toggling

void Scheduler::begin(Macros * macros)
{
  this->macros = macros;
}

void Scheduler::process_schedules()
{
  for(byte i = 0; i < NUM_SCHEDULES; i++)
  {
    int sch_period = schedule_period[i];

    // skip disabled schedules
    if(sch_period > 0)
    {
      unsigned int *sch_counter = &schedule_counter[i];
      *sch_counter = (*sch_counter + 1) % sch_period;

      if(*sch_counter == 0)
        macros->run_macro(i);
    }
  }
}

// arg[0] schedule period 0-65534, -1 clears all schedules
// arg[1] schedule number, default schedule #0
// arg[2] macro number, default same as schedule #
void Scheduler::set_schedule(unsigned int schedule_period_, byte schedule_number)
{
  if((int)schedule_period_ == -1)
  {
    reset_all_schedules();
    return;
  }

  schedule_period[schedule_number]  = schedule_period_;

  // set to zero for a complete schedule period to pass before it runs the macro (probably best)
  // could set to schedule_period - 1 to have the macro run immediately upon being set
  schedule_counter[schedule_number] = 0;
}

void Scheduler::reset_all_schedules()
{
  for(byte i = 0; i < NUM_SCHEDULES; i++)
  {
    schedule_period[i]  = 0;
    schedule_counter[i] = 0;
  }
}

#endif
