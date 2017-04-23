// for zones, have several configurations that can be chosen in real time by device type

// each device should periodically check for new versions of http_command.py and http_command.html and copy to /root any new versions
// how often? 4 hours?

// copy with pixel expansion (6-pixel rainbow, expand it to 12 pixels each for 72 pixels)

// "safe" colors for car

/*
 * {
  "access-control-request-method": "*",
  "server": "thin",
  "content-type": "application/vnd.usertesting.lab-v1+json",
  "access-control-allow-origin": "*",
  "cache-control": "no-store, must-revalidate, private, max-age=0",
  "connection": "keep-alive",
  "content-length": "4",
  "x-request-id": "393bf80d-718b-4ee7-8907-49babe6a1ef0"
}
 */

// python: when re-running a command, include all the arguments

// stop using blinking blue

// allow stop command to accept a program name so I can stop something run over ssh


// sparkle: specify width of segments and gaps

/*
144 led demo5 and demohsl4

copy to http_command-projector.html


add glasses

breathing glasses

copy: third argument is the number of pixels per source pixel

*/


//apollo: patterns that look like "it's charging up"

/* return values: 'k' followed by additional bytes, or just return an integer value
use cases:

get number of leds
get light level
read value of a pin (analog or digital)
read current set brightness level



add reboot command
rearrange links page


have a schedule that ges triggers upon a carry being set, could automatically wired together two zones
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




/* idea: a scheduler
     specify timing (command issued this often)
     specify a set of commands

     example:
       60000,0:sch:3:zon:rot -- for schedule zero, at each 60000 out of 6000 cycles (~ 1 second, so ~ 10 seconds) , rot zone 3
       0,0:sch -- clear this schedule
       
       3000,0:sch:red:green:blue -- insert RGB 

       1000,1:sch:0:rev:2:zon:rot:1:rev:3:zon:rot:0:rev:4:zon:rot:1:rev:5:zon:rot:0:rev:5:zon:rot


1000,1:sch:
       
0:rev:2:zon:rot:1:rev:3:zon:rot:0:rev:4:zon:rot:1:rev:5:zon:rot:0:rev:5:zon:rot = 80 chars, allow for 100 chars, allow for 6 schedules, adjust as free memory gets low


1000,0:sch:6:zon:rot
1000,1:sch:5:zon:rot
1000,2:sch:4:zon:rot
1000,3:sch:3:zon:rot
1000,4:sch:2:zon:rot
1000,0:sch:1:zon:rot


100 seems like too many; just do more tha one schedule if it goes long

so have more schedules, each shorter for max flexibility and efficient ram use

do them in order, on the zero mark of the timing cycle

each schedule needs:
  command buffer
  event period, 0=disabled
  event time counter


10 schedules of 20 chars each including \0 == 200 bytes


6 blink cycles in 10 seconds, blink period is 6000


 have storage for a few schedules


     
*/




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











