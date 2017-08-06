// goals: sequence crawling from zone to zone, move a sequencing zone back and forth slowly

// maybe setting sequence should leave arg set to low value


// could have an arg command that does various stuff
// examples:
//   0:arg - restore previous arg0
//   1:arg - restore previous arg0, arg1
//   2:arg - 


// can the wheel sequence be switched in direction?

// could have 8-bit versions of sequencers for speed


// need triggers, want the cosine swing and wheel to trigger other ones
//   also need one-time action sequencers
//   could set trigger macros per type of sequencer

// trigger idea: when filling up and ran out of open positions

// to sequence hsl color sweep starting angle, need the output of the sequencer to be in arg1 wit arg0 being a fixed value
// --> maybe reverse the arguments? might be more likely to sequence the starting angle instead of the step angle

// sequencer will leave value in arg0, how to get it into arg1?
// another sequencer type?

// arg command: shifts args up and puts values in arg0
// for instnace: existing values are 1,2,3, then 120:arg, now they're 120,1,2
// 120,5:arg would produce 120,5,1


// rep doesn't seem to be repeating effects

// replace getters/settings with C macros

// rst should not set display back to zero

// introduce setting for "primary" and "secondary" display so the particular display can be changed in real time outside of running program

// could expand rev command into insertion/operation mode and add things like: xor-remixed position insertion, other insertion modes

// ideas for reducing code footprint
// remove getters/settings and use variables directly (maybe with a special naming convention so they stand out)
// remove the individual color commands and use the palette only
// cache dereferenced array element pointers
// remove commands that are effecively the same like rep and cpy (keep cpy)
// remove overly-protective guard code
// move additional settings to the cfg command
// remove low-use commands/features
// segment features so they can be enabled/disabled easily
// DRY up the blink code
// remove the range of blink commands and set with an argument
// combine the various switch statements in dispatch command back to single switch 
// could switch the test code in/out 
// remove all floating point constants / math
// pass a pointer to dependencies instead of passing individual arguments if/where possible
// convert setters/getters into macros


// modify clr to reset config to defaults

// a mode when doing random position: only even, or only odd, or only mod N


// -1,0:pal could do random palette color, -1,5:pal do random within first 5 colors


// for sequencers, use simple byte versions of the range fits 0-255 for speed

// need to be able to manipulate arguments
// example: quantize (to allow position offsets)
// also: combine two separate random numbers into arg0, arg1


// ----- IDEAS --------------------------------------------------------------
// could build upon macros to form the concept of apps
// apps:
// - have a way to start up
// - have a way to shut down
// - perhaps other common behaviors

// could have an effect that runs a macro at render time to do whatever it wants, with the rendering pixel as the current window
// - this could be very slow
// - perhaps call the macro one time per scan line rather than per incidence of effect in buffer
//
// - example of use: 
//   - could have a dynamic display without needing to have a macro running all the time
// --> beginning of rendering could be a trigger

// palette: shift, rotate, sweeps that modify existing palette





// ------ COLORS --------------------------------------------------------------
// add a 15,15,15 gray
// add high pressure sodium vapor lamp

// water other recognizable colors are common?
// xenon white
// argon purple
// green light green
// yelllow light amber

// add remaining 5's colors (they may be took dark)



// cfg command - signal to reset to default config value



// shf: need to rotate and transform palette in various ways

// what if arg1 was the # of steps taken, or the number skipped, and that could be transformed to allow filling gaps

// triggers:
// run a macro each time a sequence has an event (events: start (wheel), start, mid-way (swing))
// how would this work?
//   one trigger per sequencer (so, 6)
//   triggers are built into sequencers, not separate (though a different command is used to set them up)
//   set a trigger: t,s,m:trg 
//     t=trigger number
//     s=sequence number, if s is -1, the trigger is cleared
//     m=macro number, for two-event sequences (swing) it calls this macro and m+1 
//   each sequence incrementer will fire the trigger at bounding events
//   the trigger will then call the macro(s)

// the color commands could be a shortcut to colors in the initial palette instead of fixed values

// hardware triggers, such as button presses, will be called something else, like Actions

// other sequencers:
// brownian motion
// power ease
// seeded random
// gravity bounce
// ones useful in 2D such as triangle wave, spirograph, Lissajous curves

// when using sine sequencer, have a way to fill in missing pixels (missing due to skipping over them in order to speed up)
// -- would need a sequencer that knew how to draw pixels and fill them in
//      have a way for sequencer to accept a macro to run to do the drawing, this is like a trigger
//      it could call the same memory macro as its sequence number, to avoid passing the macro #
//

// maybe alternate between blink, breathe and fade processing to allow scheduling more often

// upon sequencing, leave in arg0 a count of the number of skipped (black) pixels, so a macro could use 'rpt' to fill in  

// maybe rst should clear the arguments
// this is for cases where arg0 is optionally left with a value other than zero after some operation


// specs need to account for swapped colors on strands

// may be able to save space by storing crossfade and ease fractions as an 8-bit values n/255
// -- may allow using integer math instead of floating point math


// divide blink wheel by blink period, for instnace 5999 / 1000 to get 0 through 5, 
//   store that figure and only display blink if it matches effect blink number

// command to run a range of macros, like 20,29:rmm runs macros 20 through 29 in order

// 0,255,255:hsl at 100:lev is less bright than red at 100:lev (hsl brightness compensation problem?)

// palette, need to change brightness up/down, distribute HSL colors various ways

// allow setting a seed value for random number generation
// -- would have to be a private seed (feed value back as seed?) to have a repeatable sequence

// more colors: argon, neon, xenon, sodium

// need a way to set a sequence relatively, like to cover a zone

// need a way to pause background effects but keep schedules running

// ability to set current sequence value to a random number in range

// opposite value doesn't seem to work proper except starting a zero

// ignore out of range sequencer numbers

// need a way to reset all sequncers (-1:seq?)

// need to be able to link sequencers (triggers?)

// have the ability to set drawing mode: overwrite, add, etc.

// concept of "objects"
// - hidden/shown
// - moved around
// - could be as simple as offset + window + color & effect
// - could be a complex pattern like a bell curve of lit leds around a center point

// rng means random number generator, could have a sequential number generator
// 10:seq - sequences numbers 0 - 9 then repeats
// 10:5:set - sequences numbers 5 - 9 then repeats
// 10:5:1:set - sequences numbers 5 - 9, then 8 -6, then repeat
//
// need to be able to have multiple sequencers
// 10:seq - sequences numbers 0 - 9 for sequencer #0 then repeats
// 10:1:seq - sequences numbers 0 - 9 for sequencer #1 then repeats
// 10:2:5:seq - sequences numbers 5 - 9 for sequencer #0 then repeats
// 10:osc - oscillates numbers 0 - 9, then 8 - 1, for oscillator #0, then repeats
// 10:1:osc - same but for osc #1
// 10:2:5:osc - sequences 5 - 9, then 8 -6 for #2 then repeats
//
// might be better if arg0 is the seq/osc #
// then, a call like 1,10:seq sets up a sequencer for 0-9, then a call like 1:seq uses it (1,10:seq would also user it? or would reset it?)
//
// for seq might want a way to stop rather than repeat
//
// could have types: sequence once, sequence circular, sequence oscillatory

// simplest sng
// 10:sng
// 0:sng (arg0 is now zero)
// 0:sng (arg0 is now one) ...


// need to ensure not getting same random color twice in a row (will make things prettier)




////
// 
// sqi, sqj, sqk
// 
// -1,0,1000:sqi - set to range 0-999, repeating
// 0:sqi - get the next number
// 1:sqi - get the current number
// 2:sqi - get the previous number (for erasure)
// -2:sqi - reset to the start of the current range
// -1:sqi - reset to the default range (0 - # LEDs)
// j & k are the same, usable at the same time
// 
// sqx, sqy, sqz
// 
// -1,0,1000:sqx - set to range 0-999, then 998-1, repeating
// 
// y & z are the same, usable at the same time
//
// to bounce a ball in 1D in full width
// 0:set:-1:sqx
// 1:set:sqx:off:















// rearrange config into device profiles

// running a macro destroys the arguments
// -- it could be more helpful if it restored the previous arguments so arguments could be passed into macros
// -- could have a command that recovers the previous args

// replace color feature

// effect: the color value refers to a palette position not an RGB value
//   this would also allow for 16 extra bits of effects data
//     for instance fading into a color

// where possible, special mode ints should be positive so they can be randomized

// for red/green swapped led strands, the carry color comes back flipped

// cpy needs to overlap the end

// the infinite loop protection could also be a trigger, when all full or all empty
// triggers could be a set of slots that get set to a macro to run when triggers

// could have certain commands that end the macro under certain conditions

// the fade problem is when running from within a schedule. Running a macro N times from another macro works from the command line
// the fade counter only decides when to automatically flush, not the rate at which fading happens; repeated renders will automatically fade the buffer color
// the multi-run from the command line is spectacular, why is not applying the fade at the same rate?

// optimize BlinkEffects::process()

// a form of rotate that automatically shifts in carry color

// how to simplify switching?
//    ::process_commands_P(F("19:stm:-1:sch:30000,19,20:sch:17:run"));
//    ::process_commands_P(F("20:stm:-1:sch:60000,19,21:sch:18:run"));
//    ::process_commands_P(F("21:stm:-1:sch:60000,19,19:sch:23:run"));

// shooting star problem
// need to continuously advance the position and place the latest color at the leading edge
// could have a special form of run that sequences a series of arg[0] values
// 3,10,5:seq = run macro three for each value from 5 to 10 
// or 10,3,5:seq - allowing the count to be random
// 20:seq - runs macro #0 20 times, with arg0 from 0-19
// 20,1:seq = runs macro #1 20 times
// 20,2,5:seq = runs macro 2 15 times, from 5 to 19
// could have an advance command that sets the position to the next position
// shooting star would be
// 5:set:wht:brt:brt:sfd:flu
// rps - random starting position
// 2:rng:rev - random direction
// 20,1,5:rng - random # between 1 and 19 with arg1 = 5
// seq - sequence arg0 from 0 to n-1 and run macro 5 each time

// could have a non-blinking blink, just display the dim color, for background decoration

// need a way to clear carry color or not set it during a buffer shift

// apply ease animation timing to rotations

// when setting position and placing a rubber stamp, it only places one pixel (which is expected)
// need a way to expand the window to easily place the whole stamp
// for instance, -6:win could mean to expand the current window +6

// would be cool to somehow do mappying where the zone rotations on radius8 could be done on the opposite axis
// with mapping in place you should be able to apply rotations to rows and colums, in fact multiple rows and columns
// it's possible that when mapping is in place, there's no such thing as zones
// unmapped pixels just would be ignored
// the expectation is it can be treated as an X x Y grid
// radius8: w16 x h16
// sphere: w10 x h17
// disc (cylinder): w6 x h32 (this could simplify some effects on the projector)
// disc mapping
// col 6: full 0-31                 01234567890123456789012345678901 
// col 5: 0-23 interpolated to 0-31 01223455678890112344567789001233
// col 4: 0-15 interpolated to 0-31 00112233445566778899001122334455
// col 3: 0-11 interpolated to 0-31 00011222333445556667788899900111
// col 2: 0-7 interpolated to 0-31  00001111222233334444555566667777
// col 1: 0 interpolated to 0-31    00000000000000000000000000000000
// mapping would need to be done at the buffer level
// pos = map(x,y)
// x,y:pos 
// if a second argument is provided, it could mean  use x,y mapping
// in the case of the zero default, the x positions would still be valid positions
// need to implement mapping for radius8 & sphere

// how would I draw a line?
// 3,4:pos - to set the starting position
// 10,11:lin - sets the ending position
//  but what would it do next?
// idea: 10,11,7:lin - sets the ending position, and calls macro 7 for each distinct point

// vesions of mirror and flip that blend the colors

// for zones, have several configurations that can be chosen in real time by device type

// move mechanical buffer operations like mirror to Buffer class










// be able to set the fade rate, to four digits, 0.9999 would be 9999:sfr

// need a dbg or tst command
// 0:tst - returns current display buffer data over serial
// 1:tst - returns current render buffer
// 2:tst - returns contents of macros
// 3:tst - last received command
// etc 

// 20,10:rng:psh:30,20:rng
//           psh pushes arg0 to arg1, and arg1 to arg2

// 3:rnd = random palette color 

// have a way of generating palette colors
// one hue, many lightnesses
// one lightness, many hues, varying step angle

// would be nice to flood by adding instead of replacing colors, maybe an effect that governs how the color gets placed (whether it adds or overwrites)

// want to generate x, y
// fr0: for new arg0 = arg1-1 to arg0 
//   pass through arg2 as arg1
//   for each cycle 
//     need an int to store value
//     increment value
//     set arg0 to value
// fr1: for new arg1 = arg0 to arg2 exclusive

// for command could be x,y,z:for
// x = start, y = end, z = macro to run passing in arg0

// set red at each position
// 90,0:fr0:pos:red

// blast through a random section
// pick a random starting position in current width
// pick a random ending position, in the going direction, up to the end of the current width
// -- could have a command that chooses a random range in current width going in current direction
//      -x:rps

// need command to set offset and window from arg0, arg1

// simpler form of 'for' - calls macro for each pixel in the current window, setting position to just that pixel, moving according to reverse flag

// would pop/push args be helpful?

// slow fade doesn't work when running a macro repeatedly like 1000,0:run. It works when running it manually over serial.

// idea: per-zone settable settings that affect rendering
// - brightness, black level, direction, 
// - when you set certain values, they stick to the zone you're on

// repeat command doesn't work in reverse

// have soem kind of auto-incrementor  

// 0:set:20,0,0:rgb:0,20,0:rgb:0,0,20:rgb:flu
// this resutls in red, followed by two blacks
//    maybe args getting reset at wrong time

// need to be able to choose a set of random numbers, and share them across macros
//   palette could be used for this - could hold three arguments worth of random numbers

// if macro run more often than fade period, no fading at all happens

// command to restore pre-programmed macros

// would be nice to be able to measure the current usage via an analog pin

// need to be able to have a white flash but then resume the previous color/effect
// - maybe reverts when the luminosity matches so it seems to fade out
// - could have a trigger that could detect the end of fading out; macro would need to know the position
// - there could be a standard after-fade color (default black) 

// could have a random macro command
// 0,2,5:rnm = randomly run 0, 1 or 2, with 5 as arg0

// would be nice if running a macro preserved the original args

// need a command to dump out all the macros

// could have commands that take the rest of the input and do something with them iteratively, like an instant macro
// 9:mac:commands:commands -> would run the commands 9 times
// this could save macro space for one-off special repeat functions
// 9,100:mac:commands -> would run the commands 9 times with 100ms delay between each

// could have conditional macro running
// arg0 = 0 or 1
// arg1 = macro to run if 1
// arg2 = macro to run if 0

// passing two random numbers
// 10,5:rng - now arg0 = number between 5-9 in arg0
// 100,50:rng - now arg0 = number between 50-99 and accum = previous arg0
// could do:

// shift command:
// moves accum->arg0, arg0->arg1, arg1->arg2, arg2->accum
// would this help?
// want to do random rotation speed
// need 0=times, 1=1 to show, 2=steps
//   50:rng:    
//   now a random number of times is in arg0
//   3,1:rng
//   now a random number of steps is in arg0 and times in the accumulator
// accum=times
// arg0=steps

// arg could do:
// accum->arg0
// arg0->arg1
// but need arg0 to go to arg2

// need arg0 to be times
// need arg1 to be 1
// need arg2 to be steps

// arg could do
// rotate args0-2 N times forward (0->1, 1->2, 2->0), then swap arg0 with accumulator 

// arg could:
// preserve the arguments present
// swap accum with 

//:arg            :rot

// visual representation of eeprom memory

// have virtual colors that always point to a position in the palette
// how? colors are stored as RGB
// can't add a flag indicating to treat it as an integer
// could have an effects bit that means get from palette

// improvement: when the input buffer has too many incoming chars
// unget the current building command so it can be gotten next
// or discard the current building command and the rest of the buffer
// or ignore the entire request as potentially truncated (though it's already processed commands)

// take advantage of rest_of_buffer to allow sequencing:
// 10,5:seq:pal:5:rep:flu --> would pass 5 thru 9 as arg[0] to the rest of the command line repeatedly

// other inputs to rnd: choose one of any of the available colors, choose one of the b&w colors

//// need a way to place a complimentary color to the last placed color

// would be nice to have a way to choose two colors that look nice together instead of arbitrary colors

// other canned values
// current zone, current offset, current window, current display, current reversal

// modulo could be used to toggle:
// 2:rng:1:add:2:mod

// have a way of setting the second arg to a random number too
  // need to be able to run a macro a random # of times

// rng, leaves # as arg[0]
// rng, sets accum to old arg[0], leaves new # as arg[0]

// now with arg[0] and accum, a command could combine into args,
// args[0] -> arg[1]
// accum -> arg[0]

// 10,5:rng   accum = 0, arg[0] = n 
// 100,1:rng  accum = arg[0], arg[0] = n
// cmb        arg[1] = arg[0], arg[0] = accum

// 10,5:rng:100,1:rng:cmb --> 5-10, 1-100

// 100:cmb -> arg[1] = 100, arg[0] = whatever is in the accum, arg[1] = 100
// or is it more natural that new argument goes to arg[0]?
// could be: the command just sets arg[1] from the accumulator
//
// some calc, accum = old arg[0], arg[0] = result
// some other calc, accum = result #1

// "100" -- arg[0] automatically gets set
// 

// could have sto and rcl commands to save/restore values of arg[0]

// randomly scheduled macro
// 0:set:rnd:500:rng:sch (it could stop itself)
// 5:set:2:rng:rev:rnd:flu:500,1,5:rng:sch:rst (can't stop itself and also not macro #0)

// to simplify macros
// rps combine 0:rng:pos into a random position command
// rzn combine -2,-2,rng:zon into a random fine zone command
// combine 2:rng:rev into a random reversal command
// a way to simplify random scheduling such as 500,1,5:rng:sch
// being able to specify a number of times and whether to animate for any color push

// need to be able to clear all macros

// not sure raw command works -- there's no way to get a raw value into the buffer

// arg command: 5,1:rng:20,1,0:arg:rot (args that are zero get filled with accumulator)
// 0 could interfere with defaults
// try 5,1:rng:20,1,-1:arg:rot -- rotates 20 times with animation at random speeds
// could do something like: 5,1:rng:20,1,x:arg:rot
// don't like the "x" because it can't be computed from something else
// the arg shuffling feels too tricky, want to find a more natural way
// maybe sto and rcl?
// need to be able to specify a random number and use it in any argument field as well as specify the other fields
// for each random number, need to specify a min,max
// -1 replacement with arg may be best

// how to use arg to fill all three positions?
// 10,5:rng:0:sto:100,50:rng:1:sto:1000,500:rng:2:sto:arg
// 360:rng:st0:256:rng:st1:256:rng:st2:args:hsl

// be able to save/restore macros/schedules to eeprom 

// remix

// flip command to reverse all bits in current zone

// save last arg[0] to be referenced in certain commands
// 0:rng:3:div - get a random # from the width, divide by 3

// div:
// arg[0]: divisor
// arg[1]: dividend, default = previous arg[0]
// this isn't useful for rotation because the 1 parameter can't be passed through
// may be better to have another rotation command that automatically includes flushing, or have an animation flag

// mul:
// arg[0]: multiplicand
// arg[1]: multiplicand, default = previous arg[0]

// add:
// arg[0]: amount to add to previous arg[0]

// sub:
// sub[0]: amount to subtract from previous arg[0]

// maybe a flush is implied when running a macro, or maybe it would in certain cases (even number macros auto-flush

// there could be a special delimiter that takes the first byte of the buffer as a pre-looked-up command number
// the delimiter could mark a series of bytes to be processed that way
// macros would be stored this way for reduced memory use
// python programs would be able to use the delimiter when setting macros with binary values
// -- the delimiter is problematic
// instead accept a series of binary bytes of data followed by a special command for storing binary macros
// -- would require holding onto the input buffer (adding this back)

// could use the top 3 bits of the effects array to store the carry bits from color math overruns
// -- could probably get the same results by preventing the numbers from exceeding 0-255

// when setting raw color make sure there's no scaling of stored value
// have an effect that renders the buffer raw without any brightness adjustment
//   would be better if this was a flag on the effect like bit #8

// have a way to add/subtract color

// map twinkle ratio to breathe scale
// maybe should be a distribution curve (so they're always twinkling a little)

// problem how to keep the twinkle looking right as the brightness scales down?

// triggers would be handy to check if a position is exceeded and toggle

// save a pattern and rubber-stamp it anywhere

// how to have the cpy command cpy to a specific location?

// command to rerun macro 

// shortcut for continuing with another macro

// third arg to "rng" could be the quantization ("2" means all even values, "3" means all multiples of 3)

// built-in macros
// -- something to make it easy to toggle and swing

// move simple built-in effects to their own class

// color command ignore lines starting with pound

// third parameter to run macro: delay between runs

// pretty wifi output
// logging
// wifi mode change

// can delays in macros be made non-blocking?

// some way to sequence numbers

// super slow fade "decay"

// update read.me with http_command, rsync, 

// idea: a brightness curve applied across a zone; perhaps with a sweeping animation

// change arguments to all be ints, and create one more for a total of four

// should be able to rerun the last set of commands with an automatic macro

// each device should periodically check for new versions of http_command.py and http_command.html and copy to /root any new versions
// how often? 4 hours?

// "safe" colors for car

// sweeping crossfade effect

// maybe have a special schedule that can run even if paused, would allow pausing and unpausing automatically

// need to be able to pause effects processing but keep schedule processing going

// might want to make the delay non-blocking

// allow stop command to accept a program name so I can stop something 
// that had been run over ssh

// sparkle: specify width of segments and gaps

/* 

add reboot command

have a schedule that gets triggered upon a carry being set, could automatically wired together two zones
-- could have other kinds of triggers

-- figure out when to automatically reset blink period to default, like on clear

also: set an analog value on a pin
*/

// setting blink period to zero didn't reset to default

// html needs all the commands shortened

// show/hide by zone

/* glasses

high tech color choices (green rings with spaced out single reds looks high tech)
Movements that appear Borg-like
  one eye, radar/clock-ish constantly moving 
  other eye, occasional rotation, varying direction and duration
  sides - some back and forth, and blinking
  occasional fast spinning or bright flashing
figure-8
the cops
ggggggg

*/

/* car lights ideas

fast flashing colors
slow crossfading colors
nighttime sleeping
cron job to turn on and off in the middle of the night
random lengths of colors
very slow/occasional blinking / deterrent
multiple rotating zones
short white burst like once a minute
kitt kar like back and forth
  with hsl color
  */

/*

  custom blink:

  on period
  off period

  if the entire period is 6000 ticks
    3000/3000 would be a slow on/off blink
    1/1 would be the fastest possible blinking
    1000/6000 would be a blink for 1/6 of the period
    1000/3000 would be two blinks each for 1/6 of the period
    100/6000 would be a short flash at the start of the period
  

 */

// if paused, bre effect renders black, causing probems transitioning between animations and breathing (blink renders ok)

// for wearable, have both ways to launch circleci7

// add autobrightness to circleci7

// add a command line switch to turn off logging for circleci7

// need to be able to set offset override to zero (case: a zone is selected and want to override with the value zero)

// re-randomization on repeat/copy/mirror, etc.

// send a different acknowledgement code when first started up, so the script can detect and refresh (for instance, an uppercase K)

// html: have links to other pages on same device, and other devices

// problems with offset and window
// pshifto - starts at [0]

// clean up
/////////////////////

// store blink states as bits not bools

// see if auto brigtness needs to use that ram for its buffer


// new effects
////////////////////

// strobe, like blink but goes from normal to high instead of from low to normal to do the blinking

// flash -- flashes bright white 1 or 3 times

// lightning 

// add color groups




// new commands
////////////////////

// hide / show

// move command for placement, like:
//    red:57:move

// swap command like move (or instead of)

// shutter / slide in / slide out / random change / etc. transitions

// synchronize effects each time the low power led positions changes
// after pause, allow breathing and blinking to come to a halt first

// reverse, inverse mirror
// strobe / flash

// random flashes more like a mechanical blinking light

// rainbow colors incremental, or continuously changing

// full frame animation in/out using back buffer

// b&w static, flame

// invert hue, swing hue, back and forth between two arbitrary colors, rgb cube

// shooting up

// transition: all leds get sucked into one LED position

// new hardware features

// command to set an analog output to a particular value

// python:
// ci-skip build status is "not_run"
// fade based on build time
// change refresh rate based on time of day (afterhours it doesn't need to check that often)
// show something if the data hasn't updated in a long time
// show something if the data has been unchanged for some time, like after hours
// brief flash, or something, to alert to a new (green?) build
// some way to visually tell which are changes from a little while ago (like the fading yellow highlight on web pages)

// should time out after a while if the ACK is not received and just assume it, could be the Arduino side was reset 

// quit the crossfade early if things stop changing, for faster switch between similar colors

/*

python: 

if there's no new builds, display an alternate low power version
- dim for all the colors
- breathing and blinking still happen normally

seems like it would be better to let the arduino handle the low power rendering so the python script isn't in charge of an effect.

lowpower command means:

 - could be: normal display effects, but all-but-one LEDs are blacked out (or not rendered).
 - the position rendered would vary slowly and go low-high (not back and forth)

 opposite name? normal

 flush can call the alternate low-power rendering 

// Python: also show the high power mode if any of the statuses change

 

*/

// need to close a running app before starting circleci
// need to stop circleci if running an app

