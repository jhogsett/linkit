# Linkit Smart Duo projects

#### Out of the box set up steps

    This procedure assumes using a Mac to set up.

1. Plug the unit into a 5V power supply.

    The chip has two USB ports. Plus a micro USB connector into the rightmost one. It supplies power to the chip and is also used to programs the Arduino side of the chip.
    
    The leftmost connector is a USB out connection. An OTG (on the go) USB adaptor is required to use it.

2. Wait for it to boot up

    On powering up, the chip's orange LED will blink once, then stay lit for about 30 seconds while it boots. After that it will blink on and off for a few seconds. When that stops, it's ready.

3. Check your computer wifi settings and look for an access point called "Linkit_Smart_xxxx_xxxxxx" (the x's will be hexadecimal digits).

4. Change your wifi settings to use the Linkit access point.

    When connected, the orange LED will blink three times per second.

5. In your browser, go to http://mylinkit.local

6. You'll get a "please set a password" prompt. Set a password for the board.

7. Next you'll be asked to log in 

8. Update the board's firmware (switch back to your main wifi access point as needed)

    Follow these instructions to update the board's firmware: https://labs.mediatek.com/site/global/developer_tools/mediatek_linkit_smart_7688/get_started/7688_duo/update/
    
    _Note: after updating, you'll need to start over again at step #1_ 

9. Set the chip to "station" mode instead of "access point" mode

    In the chip's web UI click on "Network" (the right right of "System Information". Then select Station mode, choose your main wifi access point and enter the passcode.
  
10. Confirm you can SSH into the device

    In terminal, ssh to root@mylinkit.local with the password you set above
  
11. Add a micro SD card for storage.

    _Note: the chip comes with a small amount of NAND Flash storage. But unlike a typical Flash drive, there is no usage leveling and atomic writes. Because NAND Flash storage has a limited number of erase/write cycles, it's best to use the NAND Flash drive only for critical use.
    
    * Power off the device
    * Insert a pre-formatted Micro SD card into the slot. I've had success with 1GB and 4GB cards.
    * Boot up the device
    * SSH in and confirm you can navigate to /Media/SD-P1 and see a usable drive

12. Create a dev folder

    * cd /Media/SD-P1
    * mkdir dev
    
13. Link to the dev folder in the root directory

    * cd ~
    * ln -s /Media/SD-P1/dev dev
    
    Now you can ssh in and type "cd dev" to get to that folder
    
    







#### Links
