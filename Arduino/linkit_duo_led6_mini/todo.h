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


// python:
// ci-skip build status is "not_run"
// fade based on build time
// change refresh rate based on time of day (afterhours it doesn't need to check that often)
// show something if the data hasn't updated in a long time
// show something if the data has been unchanged for some time, like after hours
// brief flash, or something, to alert to a new (green?) build
// some way to visually tell which are changes from a little while ago (like the fading yellow highlight on web pages)


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
