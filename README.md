# Linkit Smart Duo projects

### [TL;DR](#quick-start)

#### Out of the box set up steps

    This procedure assumes using a Mac to set up.

1. Plug the unit into a 5V power supply.

    The chip has two USB ports. Plus a micro USB connector into the rightmost one. It supplies power to the chip and is also used to programs the Arduino side of the chip.
    
    The leftmost connector is a USB out connection. An OTG (on the go) USB adaptor is required to use it.

1. Wait for it to boot up

    On powering up, the chip's orange LED will blink once, then stay lit for about 30 seconds while it boots. After that it will blink on and off for a few seconds. When that stops, it's ready.

1. Check your computer wifi settings and look for an access point called "Linkit_Smart_xxxx_xxxxxx" (the x's will be hexadecimal digits).

1. Change your wifi settings to use the Linkit access point.

    When connected, the orange LED will blink three times, pause and repeat.

1. In your browser, go to http://mylinkit.local

1. You'll get a "please set a password" prompt. Set a password for the board.

1. Next you'll be asked to log in 

1. Update the board's firmware (switch back to your main wifi access point as needed)

    Follow these instructions to update the board's firmware: https://labs.mediatek.com/site/global/developer_tools/mediatek_linkit_smart_7688/get_started/7688_duo/update/
    
    _Note: after updating, you'll need to start over again at step #1_ 

    * Firmware downloads page: https://labs.mediatek.com/site/global/developer_tools/mediatek_linkit_smart_7688/sdt_intro/index.gsp

    _Note: the firmware image file ends with a .img extension that Mac OS thinks is a disk image. This is just the firmware images and the .img cannot be opened as if it is a disk image on Mac._
    
    In general, you will
    * switch back to the Linkit access point
    * click the "upgrade firmware" button on the Linkit webUI
    * point to the lks7688.img firmware file
    * click Upgrade & Restart
    * wait 3 minutes while the chip upgrades the firmware (showing a continuously blinking wifi LED)
    * wait 30 seconds for the OS to boot (showing a solid wifi LED for 30 seconds)

1. Set the chip to "station" mode instead of "access point" mode

    In the chip's web UI click on "Network" (the right right of "System Information". Then select Station mode, choose your main wifi access point and enter the passcode.
  
1. Confirm you can SSH into the device

    In terminal, ssh to root@mylinkit.local with the password you set above
  
    _Note: if you have ssh'd into a different linkit board, you may need to delete its entry from ~/.ssh/known_hosts_  
  
1. Add a micro SD card for storage.

    _Note: the chip comes with a small amount of NAND Flash storage. But unlike a typical Flash drive, there is no usage leveling and atomic writes. Because NAND Flash storage has a limited number of erase/write cycles, it's best to use the NAND Flash drive only for critical use.
    
    * Power off the device
    * Insert a pre-formatted (FAT32) Micro SD card into the slot. I've had success with 1GB and 4GB cards.
    * Boot up the device
    * SSH in and confirm you can navigate to /Media/SD-P1 and see a usable drive

1. Create a dev folder

    * cd /Media/SD-P1
    * mkdir dev
    
1. Link to the dev folder in the root directory

    * cd ~
    * ln -s /Media/SD-P1/dev dev
    
    Now you can ssh in and type "cd dev" to get to that folder
    
1. Create an SSH key (to use services such as github.com)

    Generate your identity key

    * mkdir ~/.ssh
    * dropbearkey -t rsa -f ~/.ssh/id_rsa

    Convert public key from dropbear binary to openssh text
    
    * dropbearkey -y -f ~/.ssh/id_rsa | head -n 2 | tail -1
    
    Copy the public key that's output and add it to your account at Github.com
    
    See: https://forum.openwrt.org/viewtopic.php?id=47551
    
    And: https://groups.google.com/forum/#!msg/beagleboard/h6XiKjT9-ZI/xgA0kIGViKgJ
    
1. Change the git ssh command to use Dropbear
    
        * echo "#!/bin/sh" > ~/.gitssh.sh
        * echo "dbclient -y -i ~/.ssh/id_rsa \$\*" >> ~/.gitssh.sh
        * chmod +x ~/.gitssh.sh
        * echo "export GIT_SSH=\$HOME/.gitssh.sh" >> /etc/profile

    Log out and back in
    
        * ^D
        * ssh root@mylinkit.local
    
    NOTE: if this doesn't work, check your copying and pasting.
    
1. Clone a Github.com repository

        * cd ~/dev
        * git clone git@github.com:jhogsett/linkit.git
    
1. Install the Python Requests library for reaching the web

        * pip install requests
    
    See: http://docs.python-requests.org/en/master/
    
1. Run a Python program to verify things are set up

        * cd ~/dev/linkit/python
        * python circleci7.py {YOUR CIRCLECI KEY}
    
    Tail the log file to see if it's reaching CirclCI
    
        * tail -f ~/dev/linkit/python/circleci.log
  
1. Establish your Git identify

    Before you can commit changes, you need to let Git know who you are.

        * git config --global user.email "{YOUR EMAIL}"
        * git config --global user.name "{YOUR FULL NAME}"
  
        Tip: use git config --list to confirm the settings

1. Set up Git for pulling and pushing commits

    A _git pull_ is a _git fetch_ followed by a _git merge_. To do the merge, Git needs to know how. (One might think this would have been set up correctly to begin with, since Git comes pre-installed.)

        * ln -s $(which git) /usr/lib/git-core/git
    
    See: https://community.onion.io/topic/785/git-error-merge-not-found
  
1. Fix Git diff command

    As installed, the diff command displays differences by opening them in the default editor, including the terminal color codes. It's completely useless.
    
    This command will change it to show properly on the console:
    
        * git config --global core.pager cat

    see: http://stackoverflow.com/questions/2183900/how-do-i-prevent-git-diff-from-using-a-pager 
    
    and: http://stackoverflow.com/questions/17077973/how-to-make-git-diff-write-to-stdout

    _Note: I'm not sure how to make it use `less` like on a Mac_

1. Apply other git fixes

        * git config --global core.excludesfile ~/.gitignore
        * git config --global push.default simple

1. Set up global Git ignore

        * vim ~/.gitignore
        * logs/
        * *.log
    
1. Customize the 'ls' command

    Edit your profile and add an alias to customize the _ls_ command.

        * vim /etc/profile (for all users)
    
    or
    
        * vim /root/.profile (for root user)
    
    Then add
    
        * alias ls='ls -al' (or your desired customization)
    
    Then log out and back in.
    
    Add any other profile scripts to either file (there is no _.bashrc_).
    
    See: http://www.cesareriva.com/openwrt-shell-alias/
    
1. Add your SSH key so you don't have to enter a password 

    Get your SSH public key. On a Mac it should be /Users/USERNAME/.ssh/id_rsa.pub
    
        * vim /etc/dropbear/authorized_keys
    
    Add your key and save the file. Then, log out and back in, and if it worked, you won't be asked for a password.

    See: https://wiki.openwrt.org/doc/howto/dropbear.public-key.auth

1. Add a CircleCI API token to the environment

        * Edit the all users profile as in step #22
        * add export KEY={THE KEY}

1. Create a service start-up script for a Python script

    To have a script automatically start up on system boot, create a script such as the following 

    _Tip: place your Python script in `/root` so it can be started up even if the SD card has not finished initializing_

        * vim /etc/init.d/circleci
    
    Example script:
    
        #!/bin/sh /etc/rc.common

        SCRIPT_NAME="Build Status"
        SCRIPT_PATH="/root/circleci6.py {YOUR KEY}"
        LOG_FILE="/tmp/circleci.log"
        START=99
        STOP=10

        start() {       
                echo "Starting $SCRIPT_NAME"
                $SCRIPT_PATH >> $LOG_FILE 2>&1 &
        }                

        stop() {         
                echo "Stopping $SCRIPT_NAME"
                killall -9 `basename $SCRIPT_PATH`
        }

    _Note: the ENV var `$KEY` cannot be used because it is only loaded when there's a login_

        * chmod +x /etc/init.d/circleci

    Enable or disable the automatic start up with:
    
        * /etc/init.d/circleci enable
        * /etc/init.d/circleci disable
    
    When enabled, new start up and shutdown scripts can be seen in /etc/rc.d such as:
    
    ```K10circleci -> ../init.d/circleci```
    
    ```S99circleci -> ../init.d/circleci```

    Now you can start and stop the service by using:
    
        * /etc/init.d/circleci start
        * /etc/init.d/circleci stop
        * /etc/init.d/circleci restart

    _Notes:_
    
    * The START and STOP variables specify where in the start up and shutdown process this script is handled. The higher the number, the later in the process the script is handled.
    * I had to add `-9` to the `killall` command to have it successfully shutdwon the running Python script

1. Set up a cron job to restart the script hourly

    First, start up the cron daemon and have it start up automatically on restart
    
        * /etc/init.d/cron start
        * /etc/init.d/cron enable

    Then edit the cron table

        * crontab -e
    
    Add
    
        * 0 */1 * * * /etc/init.d/circleci restart

    The service will be restarted hourly at the top of the hour

1. Install the Screen utility

        * opkg update
        * opkg install screen

    Starting a screen session:
    
        * screen -S {NAME}

    Disconnecting from a screen session you want to reconnect to later:
    
        * ^A^D
    
    Reconnecting to the session
    
        * screen -x {NAME}
    
    Disconnect and stop the screen session
    
        * ^D

    _Tip:_ If you get the error `/var/run/utmp: No such file or directory` when quitting screen, do the following:
    
        * touch /var/run/utmp

1. Getting a simple CGI script running

        * vim /www/cgi-bin/test.cgi
    
    Then add:
    
        #!/bin/ash
        echo "Content-type: text/html"
        echo ""
        echo "<html><head><title>Bash as CGI"
        echo "</title></head><body>"
        echo "<h1>Hello world</h1>"
        echo "Today is $(date)"
        echo "</body></html>"

    Then do:
    
        * chmod +x test.cgi
    
    Then go to http://mylinkit.local/cgi-bin/test.cgi and it should say “Hello World” along the date & time

    _Tip:_ If the name `mylinkit.local` cannot be resolved, do the following to restart the multicast DNS (mDNS) service:
    
        * /etc/init.d/avahi-daemon

1. Safe Shutdown

        * Openwrt doesn't have the `shutdown` command
        * Use the `sync` command to flush data to the SD card before removing power
    
    See: https://labs.mediatek.com/forums/posts/list/3605.page

    _Note: The `reboot` command can be used to reboot the Openwrt OS_

----

#### Quick Start

Connect to the device

    * look for a Linkit... wifi hotspot after the chip finishes booting
    * connect to it
    * load http://mylinkit.local in the browser to confirm connectivity

Update the firmware

    https://docs.labs.mediatek.com/resource/linkit-smart-7688/en/downloads

SSH into the device

    ssh root@mylinkit.local

Set up dev link to SD card

    cd /Media/SD-P1
    mkdir dev
    cd ~
    ln -s /Media/SD-P1/dev dev

Create and grab an RSA public key

    mkdir ~/.ssh
    dropbearkey -t rsa -f ~/.ssh/id_rsa
    dropbearkey -y -f ~/.ssh/id_rsa | head -n 2 | tail -1
    
_Copy the public key that's output and add it to your account at Github.com_

Add your Mac public key for no-password ssh login

    vim /etc/dropbear/authorized_keys
    
_Add your local key from ~/.ssh/id_rsa.pub and save the file_

Fix Git in various ways

```bash
echo "#!/bin/sh" > ~/.gitssh.sh
echo "dbclient -y -i ~/.ssh/id_rsa $*" >> ~/.gitssh.sh
chmod +x ~/.gitssh.sh
echo "export GIT_SSH=\$HOME/.gitssh.sh" >> /etc/profile
```       
       
_NOTE: if this doesn't work, check your copying and pasting (check that \$\* is $*)_
    
    git config --global user.email "{YOUR EMAIL}"
    git config --global user.name "{YOUR FULL NAME}"
    ln -s $(which git) /usr/lib/git-core/git
    git config --global core.pager cat
    git config --global core.excludesfile ~/.gitignore
    git config --global push.default simple

Set up global Git ignore

    vim ~/.gitignore
    logs/
    *.log

Set up global and user profiles

_for all users:_

    vim /etc/profile
    alias ls='ls -al'

_for root user:_

    vim /root/.profile
    export KEY={CircirCI Key}
    export WX_KEY={Openweathermap.org Key}

    alias cc='/root/color_command.py'
    alias pm='/root/dev/linkit/python/progmac.py'
    alias lt='/root/dev/linkit/python/led_specs.py'

Switch the device to station mode and connect it to your wifi network

    * go to http://mylinkit.local
    * log in
    * choose Network
    * choose Station Mode
    * Select your WiFi hotspot and enter the passphrase
    * restart the chip

_NOTE: switch back to your main WiFi at this point_

Prepare to sync files between the SD card and the root directory

    opkg update
    opkg install rsync

Prepare to run Python programs that call out to the Internet

    pip install requests

Get the Python code to run locally

    cd ~/dev
    git clone https://github.com/jhogsett/linkit.git
    cd linkit/python
    ./scripts/refresh

Add the http_command start-up

    cd /etc/init.d
    vim http_command
    
```bash
#!/bin/sh /etc/rc.common

SCRIPT_NAME="LED Web UI"
SCRIPT_PGM="http_command.py"
SCRIPT_PATH="/root/http_command.py /root/http_command.html /root/dev/linkit/python/"
#LOG_FILE="/root/dev/linkit/python/http_command.log"
LOG_FILE="/dev/null"

START=99
STOP=50

start() {
        echo "Starting $SCRIPT_NAME"
        $SCRIPT_PATH >> $LOG_FILE 2>&1 &
}

stop() {
        echo "Stopping $SCRIPT_NAME"
        killall -9 `basename $SCRIPT_PGM`
}
```

    chmod +x http_command
    ./http_command enable
    ./http_command start

Add the udp_command start-up

    cd /etc/init.d
    vim udp_command

```bash
#!/bin/sh /etc/rc.common

SCRIPT_NAME="LED Multicast Receiver"
SCRIPT_PGM="udp_command.py"
SCRIPT_PATH="/root/udp_command.py"
#LOG_FILE="/root/dev/linkit/python/udp_command.log"
LOG_FILE="/dev/null"

START=98
STOP=50

start() {
        echo "Starting $SCRIPT_NAME"
        $SCRIPT_PATH >> $LOG_FILE 2>&1 &
}

stop() {
        echo "Stopping $SCRIPT_NAME"
        killall -9 `basename $SCRIPT_PGM`
}
```


    chmod +x udp_command
    ./udp_command enable
    ./udp_command start

Add the weather start-up

    cd /etc/init.d
    vim weather

```bash
#!/bin/sh /etc/rc.common

SCRIPT_NAME="LED Weather Forecaster"
SCRIPT_PGM="weather.py"
SCRIPT_PATH="/root/weather.py -k d59a165785d20ad2a84f6a4a85f9a4a2 -z 94565 -f 12 -s"
#LOG_FILE="/root/dev/linkit/python/weather.log"
LOG_FILE="/dev/null"

START=98
STOP=50

start() {
        echo "Starting $SCRIPT_NAME"
        $SCRIPT_PATH >> $LOG_FILE 2>&1 &
}

stop() {
        echo "Stopping $SCRIPT_NAME"
        killall -9 `basename $SCRIPT_PGM`
}
```

    chmod +x weather
    ./weather enable
    ./weather start

Add the sunrise start-up

    cd /etc/init.d
    vim sunrise

```bash
#!/bin/sh /etc/rc.common

SCRIPT_NAME="LED Sunrise Handler"
SCRIPT_PGM="sunrise.py"
SCRIPT_PATH="/root/sunrise.py -k d59a165785d20ad2a84f6a4a85f9a4a2 -z 94565 -f 60 -t 8 -o -15 /root/dev/linkit/python/awakening.py -b"
LOG_FILE="/root/dev/linkit/python/logs/sunrise.log"
#LOG_FILE="/dev/null"

START=98
STOP=50

start() {
        echo "Starting $SCRIPT_NAME"
        $SCRIPT_PATH >> $LOG_FILE 2>&1 &
}

stop() {
        echo "Stopping $SCRIPT_NAME"
        killall -9 `basename $SCRIPT_PGM`
}
```

    chmod +x sunrise
    ./sunrise enable
    ./sunrise start

Add the sunset start-up

    cd /etc/init.d
    vim sunset

```bash
#!/bin/sh /etc/rc.common

SCRIPT_NAME="LED Sunset Handler"
SCRIPT_PGM="sunrise.py"
SCRIPT_PATH="/root/sunrise.py -e sunset -k d59a165785d20ad2a84f6a4a85f9a4a2 -z 94565 -f 60 -t 8 -o -20 /root/dev/linkit/python/udp_send.py 10:run"
LOG_FILE="/root/dev/linkit/python/logs/sunrise.log"
#LOG_FILE="/dev/null"

START=98
STOP=50

start() {
        echo "Starting $SCRIPT_NAME"
        $SCRIPT_PATH >> $LOG_FILE 2>&1 &
}

stop() {
        echo "Stopping $SCRIPT_NAME"
        killall -9 `basename $SCRIPT_PGM`
}

```

    chmod +x sunset
    ./sunset enable
    ./sunset start

Install the Screen utility

    opkg update
    opkg install screen


----

#### TODO

1. Formatting the SD card for Linux FS

1. Extending the root file system onto the SD card

1. Installing Ruby

#### Links

* MediaTek Starter Guide: 
   
https://labs.mediatek.com/fileMedia/download/4ef033b8-80ca-4cdb-9ad6-1c23836c63de
   
* MediaTek Software and Tools
   
https://labs.mediatek.com/site/global/developer_tools/mediatek_linkit_smart_7688/sdt_intro/index.gsp

#### utilities

    opkg update && opkg install e2fsprogs

#### Misc

add `export GIT_MERGE_AUTOEDIT=no` to `~/.profile` to prevent vim opening when pulling changed files

