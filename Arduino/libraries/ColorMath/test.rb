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

max_step = 20
steps = (0..max_step)
steps.each do |step|
  position = step / (max_step * 1.0)
  puts ((0.5 - 0.5 * Math.cos(Math::PI * position)) ** 0.5).round(15)
end





































