


// in going in and out of lo/hi power, reset the position counter

// cleanup
// look for ints that can be chars: byte is the type to use

// zones

// would be nice to set strip orientation remotely

// synchronize effects each time the low power led positions changes

// Python: also show the high power mode if any of the statuses change

// detect when external power is applied

// after pause, allow breathing and blinking to come to a halt first
// restrict flood, etc. to window
// reverse, inverse mirror
// copy -- everything up to black is copied on top of black
// strobe / flash
// audo brightness, set brightness

// have a low-power mode, low brightness and/or scanning the LEDs one by one until turned off.

// rainbow colors incremental, or continuously changing
// rotation
// transitions
// demo - do random stuff
// full frame animation in/out using back buffer
// efade is broken
// be able to reverse shift/animation direction
// screen saver
// fast blink, breathe
// static, flame
// stop sign timing and opposite timing
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


 
*/
