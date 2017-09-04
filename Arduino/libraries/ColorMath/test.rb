#!/usr/bin/ruby

print "\x1b[2J\x1b[1;1H"

# @times = 20
# @crossfade = []
# @c1_last = 0
# @c1 = 10
# @c2 = 90
#
# def generate_equal_power_cross_fade(times)
#   (0..times).each do |i|
#     position = i / (times * 1.0)
#     @crossfade[i] = (0.5 - 0.5 * Math.cos( Math::PI * position)) ** 0.5
#   end
#
#   (0..times).each do |i|
#     puts "-" * (50 * @crossfade[i]) + "+" * (50 * @crossfade[@times - i])
#   end
# end
#
# # reconstruct previous c1 by undoing math
# def do_one_step_3(step)
#   if step > 1
#     prev_step = step - 1
#     prevc2 = @c2 * @crossfade[prev_step]
#     restored_c1 = (@c1 - prevc2) / @crossfade[@times - prev_step]
#     @c1 = restored_c1
#   end
#
#   newc1 = @c1 * @crossfade[@times - step]
#   newc2 = @c2 * @crossfade[step]
#   @c1 = newc1 + newc2
#
#   puts "-" * newc1 + "+" * newc2
#   #puts "*" * @c1
# end
#
#
#
# def doit
#   (0..@times).each do |i|
#     do_one_step_3(i)
#   end
# end
#
# generate_equal_power_cross_fade(@times)
# doit
#
#
#
#




# for(int i = 0; i <= CROSSFADE_STEPS; i++){
#   float position = i / (CROSSFADE_STEPS * 1.0);
# crossfade[i] = pow((0.5 - 0.5 * cos(M_PI * position)), 0.5);
# }

# max_step = 20
# steps = (0..max_step)
# steps.each do |step|
#   position = step / (max_step * 1.0)
#   puts ((0.5 - 0.5 * Math.cos(Math::PI * position)) ** 0.5).round(15)
# end
#

@power=3
maxsteps=24
midpoint = maxsteps / 2
ease = []
def ease_in(percent) percent ** @power; end
def ease_out(percent) 1 - ((1 - percent) ** @power); end
(0..midpoint).each do |i|
  percent = i / (midpoint * 1.0)
  ease[i] = (0.5 * ease_in(percent)).round(15)
  ease[i + midpoint] = ((0.5 * (ease_out(percent))) + 0.5).round(15)
end
ease.each do |e|
  puts e
end






max_step = 20
steps = (0..max_step)
steps.each do |step|
  position = step / (max_step * 1.0)
  puts (((0.5 - 0.5 * Math.cos(Math::PI * position)) ** 0.5) * 255.0).round(0)
end





flu
era
rot
rep
cpy
flo
mir
dis
zon
win
off
rev
rgb
hsl
red
grn
blu
blk
yel
org
pur
cyn
mag
pnk
wht
gry
lgr
sea
lbl
dgr
rnd
ble
max
dim
brt
bli
bl1
bl2
bl3
bl4
bl5
bl6
blr
bla
blb
blp
bre
efr
sta
pau
cnt
rst
clr
lev
fad
wip
esh
psh
pso
cfa
lop
hip
pon
pof
dem

flush
erase
rotate
repeat
copy
flood
mirror
display
zone
window
offset
reverse
rgbcolor
hslcolor
red
green
blue
black
yellow
orange
purple
cyan
magenta
pink
white
gray
ltgreen
seafoam
ltblue
dkgray
random
blend
max
dim
bright
blink
blink1
blink2
blink3
blink4
blink5
blink6
blinkr
blinka
blinkb
blinkp
breathe
effectr
static
pause
continue
reset
clear
level
fade
wipe
eshift
pshift
pshifto
cfade
lopower
hipower
pinon
pinoff
demo






















