to check video capabilities
ffmpeg -f v4l2 -list_formats all -i /dev/video0

to capture video (first frame is blank)
ffmpeg -i /dev/video0 -vframes 2 test%0d.jpg
