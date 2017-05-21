to check video capabilities

    ffmpeg -f v4l2 -list_formats all -i /dev/video0

to capture video (first frame is blank)

    ffmpeg -i /dev/video0 -vframes 2 test%0d.jpg

to generate separate gifs

    ffmpeg -y -i /dev/video0 -vframes 2 -f image2 test%0d.gif
    
Python image library

    opkg update
    opkg install python-imglib
    
Loading a jpeg

    python
    > import os,sys
    > import Image
    > jpgfile = Image.open("test.jpg")
    
Get pixel RGB value

    g = Image.open("test2.gif")
    > size = 16,16
    g.thumbnail(size, Image.ANTIALIAS)
    g.save("test2-tinythumb.gif")
    r = g.convert("RGB")
    r,g,b = r.getpixel((1,1))
    print r,g,b
    43 59 45

Piping FFMPEG data via python

    http://zulko.github.io/blog/2013/09/27/read-and-write-video-frames-in-python-using-ffmpeg/

Quickly capturing video frames

    ffmpeg -y -f v4l2 -framerate 30 -video_size 640x480 -i /dev/video0 -vframes 2 test%0d.jpg
    
