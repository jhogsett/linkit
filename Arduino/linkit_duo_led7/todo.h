// the command processor has a 60-char buffer
// this should be reusable for tokenizing strings
// add functions to command processor to access that buffer, like
// - get pointer to the buffer

// visual representation of eeprom memory

// vesions of mirror and flip that blend the colors

// have virtual colors that always point to a position in the palette
// how? colors are stored as RGB
// can't add a flag indicating to treat it as an integer
// could have an effects bit that means get from palette

// use palette as a rubber stamp buffer for copying and pasting patterns

// improvement: when the input buffer has too many incoming chars
// unget the current building command so it can be gotten next
// or discard the current building command and the rest of the buffer
// or ignore the entire request as potentially truncated (though it's already processed commands)

// take advantage of rest_of_buffer to allow sequencing:
// 10,5:seq:pal:5:rep:flu --> would pass 5 thru 9 as arg[0] to the rest of the command line repeatedly


// how to toggle macros without rewriting?
// - could toggle by setting which macro the schedule runs
//   
//

// would be nice to be able to repeat the same random color in different zones
// need some way to find out what the color is when doing something later
// could have a pal(ette) command that takes a color number, 
//   and a (sh)u(f)fle command that mixes them up uniquely
//   then to reuse the first two random colors
//     shf:1:zon:0:pal:2:zon:1:pal:3:zon:0:pal:4:zon:1:pal

// eeprom for macros
// 
// setting a macro: 
//   three cases
//     setting from serial
//       copy remainder of buffer from serial to eeprom followed by zero
//     setting from string
//       copy remainder of string to eeprom followed by zero
//     setting from eeprom
//       copy from eeprom to eeprom until the value is zero followed by zero 
// 
// running a macro:
//   have a special version of process_command() for eeproms
//     need a replacement for strtok to parse the commands  
// 
// the eeprom can't be used for real-time rewriting for toggling because it will wear out too quickly
// maybe have a small in-memory macro buffer just for toggling use, macro #2 and higher would operate out of the eeprom
// have a variable number of in-memory vs eeprom macros depending on available memory and application

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

// could store macros pre-looked up by storing the cmd ID instead of the string
//    would allow for longer macros
//    have to deal with arguments somehow 

// pretty wifi output
// logging
// wifi mode change

// can delays in macros be made non-blocking?

// some way to sequence numbers

// super slow fade "decay"

// update read.me with http_command, rsync, 

// idea: a brightness curve applied across a zone; perhaps with a sweeping animation

// change arguments to all be ints, and create one more for a total of four

// del command delays, perhaps logarithmically 

// should be able to rerun the last set of commands with an automatic macro

// for zones, have several configurations that can be chosen in real time by device type

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


//apollo: patterns that look like "it's charging up"

/* return values: 'k' followed by additional bytes, or just return an integer value
use cases:

get number of leds
get light level
read value of a pin (analog or digital)
read current set brightness level



add reboot command

h\ave a schedule that ges triggers upon a carry being set, could automatically wired together two zones
-- could have other kinds of triggers

-- figure out when to automatically reset blink period to default, like on clear


also: set an analog value on a pin
*/

// setting blink period to zero didn't reset to default

// html needs all the commands shortened

// show/hide by zone


// handle ^d on color_command.py:
//command: Traceback (most recent call last):
//  File "/root/color_command.py", line 52, in <module>
//    loop()
//  File "/root/color_command.py", line 37, in loop
//    cmd = raw_input('command: ')
//EOFError








// ensure sub arg 0 is an unsigned in (?)

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






// carry the color and effect that drops off the end of a buffer operation,
//    then have a special "color" that inserts it.

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
// crossfade - leaves only pixels inside the zone (??? in further testing it seemed to work just fine)

// rotation using power shift timing


// clean up
/////////////////////

// store blink states as bits not bools

// move zones to a class (especially for self explanatory comments)

// move mechanical buffer operations like mirror to Buffer class

// animation routines may be inefficient

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

// would be nice for force it to hi power

// detect non-battery power and stay in hi power

// demo mode - keeps going until non-demo command.

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









