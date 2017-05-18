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
    jpgfile = Image.open("test.jpg")
    
