// problems with offset and window
// pshifto - starts at [0]
// crossfade - leaves only pixels inside the zone (??? in further testing it seemed to work just fine)

// command to rotate within a zone

// clean up
/////////////////////

// fix auto brightness

// move mechanical buffer operations like mirror to Buffer class

// wipe command should do a real rendering so it looks right

// animation routines may be inefficient

// shorten commands

// see if auto brigtness needs to use that ram for its buffer

// store blink states as bits not bools

// move zones to a class


// new architecture
///////////////////////// 

// seems like it should be simple to change the orientation

// more complex blinking with arbitrary timing

// may need to speed up effects with three 72-led strips

// store standard colors in program memory


// new commands
////////////////////

// hi-going instead of low-going blinks (like twinkling stars)

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

// rotation

// full frame animation in/out using back buffer

// fast blink, breathe

// static, flame

// invert hue, swing hue, back and forth between two arbitrary colors, rgb cube

// shooting up

// transition: all leds get sucked into one LED position









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
