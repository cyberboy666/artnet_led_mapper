# artnet_led_mapper

_an open-source cross-platform application for creating addressable led installation_

![image](https://github.com/cyberboy666/artnet_led_mapper/assets/12017938/b4277895-0828-4e6f-a25c-114f80a7b6fe)

- this software is distributed by __UNDERSCORES__ - compiled downloads for linux, windows, mac and arm64 are available to purchase from underscores.shop [put link to shop page here]
- the source code can be viewed and compiled [here](/src/)
- consider [donating](https://opencollective.com/underscores) to the underscores project to help us continue creating for the commons

## features

- _input video_ from connected devices (webcams, capture cards etc) or over NDI
- _output [artnet](https://art-net.org.uk/)_ across multiple universes to a hardware controller that can drive addressable leds 
- create an _led mapping_ using measurements, mouse, keyboard or directly editing human readable config.json
- runs on linux, windows, mac and sbc's such as raspberry and orange pi's
- code is open-source so you can modify it / add features that suit your needs

## background

this application is a companion to my [hardware led controller](https://github.com/cyberboy666/ethernet_led_controller) that drives addressable leds from an [artnet](https://art-net.org.uk/) network connection. it was built for [lateral movement](https://cyberboy666.com/lateralmovement/) club nights such as seen here:

![image](http://underscores.shop/wp-content/uploads/2024/06/leds.gif)


you can find some more background on this install and process here: [WORKS IN PROGRESS 04 - talk about controlling addressable leds for art installation](https://videos.scanlines.xyz/w/p/tUhDfYqJJPfjovscNMvFRL) 

## demo video

[coming soon]

## hardware options

<details><summary>hardware options</summary>

![image](https://github.com/cyberboy666/artnet_led_mapper/assets/12017938/bdb0a07a-c5ab-4fef-b998-f9529223bc16)

to map video onto addressable leds there are a few parts needed:

- computer running __artnet_led_mapper__ app (or otherwise) - takes a video source and converts it to artnet sent over network
- __hardware led controller__ takes artnet over network and converts to the addressable led strip protocol
- addressable led strips such as [WS281X](https://www.aliexpress.com/item/4001322411818.html) or [APA102](https://www.aliexpress.com/item/32322326979.html)
- [switching power supply](https://www.aliexpress.com/item/1005002843829663.html) in the voltage your strips take and providing enough amps to power them all

### software for outputing artnet

besides this app, also [resolume](https://resolume.com/), [touchdesigner](https://derivative.ca/download) and [qlc+](https://www.qlcplus.org/) can all output artnet to your controller

### hardware led controller

i recommend my [ethernet_led_controller](https://github.com/cyberboy666/ethernet_led_controller) for this - it supports multiple led protocols, can connect to network over ethernet, wifi or access_point and can control up 2000 leds across 12 universes. also it is open source hardware / firmware so possible to modify to meet your needs.

there are other options for led controller hardware - [heres one](https://www.aliexpress.com/item/32919779537.html) i found on aliexpress for __60euros__ supporting up to 4 universes...

### led strips and power supply

more info about this on [ethernet_led_controller repo](https://github.com/cyberboy666/ethernet_led_controller) where both [WS281X](https://www.aliexpress.com/item/4001322411818.html) or [APA102](https://www.aliexpress.com/item/32322326979.html) type strips are supported. for our installation we used the 12v powered WS2815 strips and a 300W [switching power supply](https://www.aliexpress.com/item/1005002843829663.html)
</details>

## how to use this app

<details><summary>how to use this app</summary>

### inputing source video

__artnet_led_mapper__ app can take input source from video devices on your computer (such as webcams and external capture cards)

if you have the [NDI SDKs installed](https://ndi.video/for-developers/ndi-sdk/download/) it will also be able to take input source from NDI

many programs for creating video such as resolume and touchdesigner can output to NDI (and to VIRTUAL CAMERA) giving you options for linking with __artnet_led_mapper__

if you are unsure where you will be getting source video from a good program to start with is [OBS](https://obsproject.com/download) - this can play videos, screencapture and much more plus can output to VIRTUAL CAMERA with __obs virtual cam__ and to NDI with [OBS NDI plugin](https://obsproject.com/forum/resources/obs-ndi-newtek-ndi%E2%84%A2-integration-into-obs-studio.528/) installed.

_NOTE: currently there is a [known bug](https://github.com/cyberboy666/artnet_led_mapper/issues/1) where windows version of __artnet_led_mapper__ crashes when taking input from __obs virtual cam__ - if you want to input source from obs on windows you will need to use NDI_

### creating _the map_

_the map_ is how you input where the led strips are placed relative to each other so that the software can correctly map the pixel colour for each led.

start by setting the `NUMBER OF STRIPS` to match your install.

after you have physically set up your led strips you will need to take some measurements with a _tape measure_:

- __canvas width__ and __canvas height__ is the total size of the space that the video is being mapped onto - if you imagine that the led installation is one big screen (with gaps between the led pixels) then this is the size of that screen. a good place to start is to imagine the `X AXIS` as the horizontal line running across from the top-most pixel of the installation, and the `Y AXIS` as the vertical line running up from left most pixel of the installation

- you can measure the `STRIP I X POS` - the distance from `Y AXIS` to first pixel on _strip i_, and `STRIP I Y POS` - the distance from `X AXIS` to first pixel on _strip i_
- also if needed can measure/approximate the `STRIP I ANGLE (degress)` going clockwise from horizontal (relative to the canvas) and also count the `STRIP I NUMBER OF LEDS`

_NOTE: the position measurements do not need to be exact - this will likely be adjusted slightly during calibration

![image](https://github.com/cyberboy666/artnet_led_mapper/assets/12017938/795c407c-b45c-4d24-a2ae-dddbf2db46cb)

once these are entered the __MAPTEST__ view in the app - black box with blue/red circles/pixel lines representing led strips - should be looking like your tv wall

- if you select __testcard 01__ from `VIDEO SOURCE` you should now see a test card being spread out over the leds in the wall. small adjustments so size/position can be made here so lines appear to line up between tvs.

### controls for adjusting the map

this info can be seen in the app by clicking the `HELP` tab at top of the interface:

MAPPING WITH A MOUSE:
- CLICK on a STRIP HEAD to select it (BLUE/RED larger circle)
- CLICK and drag a STRIP HEAD to position it
- hold SHIFT and drag around to rotate selected STRIP
- or RIGHT CLICK and drag around to rotate selected STRIP
MAPPING WITH A KEYBOARD:
- press ALT plus LEFT/RIGHT to select a STRIP
- press ARROW KEYS to position it
- press SHIFT plus UP/DOWN to rotate it
- press SHIFT plus LEFT/RIGHT to subtract/add leds
- press ALT plus UP/DOWN to set grid size (how much each press moves)
OTHER KEYBOARD SHORTCUTS:
- press F to toggle ON/OFF FULLSCREEN

### info about every setting in the interface:

- __HELP__ - this opens/closes info about mapping controls
- __save config__ - saves the current config to a json file (located at `bin/data/config.json`)
- __load config__ - loads the config from the file at `bin/data/config.json`
- __refresh inputs__ - this checks for any new `VIDEO SOURCE` - useful if you hotplug a capture card
- __VIDEO SOURCE__ - select where the source video is coming from - can take input from cameras/capture cards or from a NDI source
- _INPUT RESOLUTION_ - this displays the source resolution - usually it will match the monitor or a resolution that `VIDEO SOURCE` supports
- __HIDE MAPTEST__ - removes the MAPTEST - box showing the mapping - from the interface
- __HIDE PREVIEW__ - removes the PREVIEW - box showing video output - from the interface
- __OVERWRITE DEFAULTS__ - lets you play around with `INPUT WIDTH`, `INPUT HEIGHT` and `FRAMERATE` on the __video source__ - could be useful if having performance issues
- __fullscreen__ : _display_ | _preview_ | _maptest_ - toggles fullscreen - radio-buttons select what is shown fullscreen: either _display_ - the whole interface, _preview_ - the preview of input source, or _maptest_ - the simulated mapping of source to led pixels 
- __NUMBER OF STRIPS__ - the amount of straight lines used in this mapping (__NOTE: doesnt have to corrospond to outputs from controller - a single output can be connected in serial to create many strips__)
- __LED SPACING__ - sets the distance between leds on the strips - currently this is a globel setting - but it could be possible to add per strip / more options here if needed
- __TARGET IP__ - this is the IP of the __hardware led controller__ that recieves artnet packets (__NOTE: you need to press the UPDATE TARGET IP AND PORT button after changing this for it to update - it will revert the ip if it fails to connect to it__)
- __TARGET PORT__ - this is the PORT of the __hardware led controller__ that recieves artnet packets (__NOTE: you need to press the UPDATE TARGET IP AND PORT button after changing this for it to update - it will revert the ip if it fails to connect to it__)
- __send artsync__ - this toggles whether the app should send an [artsync](http://web.archive.org/web/20230411144055/https://art-net.org.uk/how-it-works/streaming-packets/artsync-packet-definition/) packets to signal the start of a frame - if toggled user can also configure the __ARTSYNC IP__ and __ARTSYNC PORT__ - by default it is same - but you may choose to broadcast this is syncing across multiple controllers
-  __CANVAS WIDTH__ - this is the width of the "screen" that video is mapped onto - if the led strips was one big screen
-  __CANVAS HEIGHT__ - this is the height of the "screen" that video is mapped onto - if the led strips was one big screen
-  __SELECTED STRIP__ - this is the currently highlighted strip in MAPTEST (red) - the one that will be affected by key presses etc
-  __GRID SIZE__ - this is the amount one key press will move the `SELECTED STRIP`
-  __STRIP I__ - click on a __strip__ to see values for it:
-  __STRIP I NUMBER LEDS__ - set the number of leds in this strip
-  __STRIP I X POS__ - set the distance from left most pixel to `Y AXIS`
-  __STRIP I Y POS__ - set the distance from top most pixel to `X AXIS`
-  __STRIP I ANGLE__ - set the angle in degrees that the strip is positioned at - from the first led going clockwise from horizontal
</details>

## how to install from source

<details><summary>how to install from source</summary>

you can support this project by buying compiled versions of this app from underscores.shop [put link to shop page here] - with a compiled version you only need to download & unzip a folder, then just run the app - no installing required!

you can also install and compile it yourself from the source code and instructions provided below. if this software is useful to you consider giving back by [donating](https://opencollective.com/underscores/donate) or contributing in other ways.

the app has been tested on:

- linux (pop_os, ubuntu, debian & xubuntu)
- windows 10
- mac_os Monterey (intel) - currently untested on ARM
- raspberry_pi_4 running _raspios-bookworm-arm64_
- orange_pi_5 running _debian_

this code is written in [openFrameworks](https://openframeworks.cc/) - an open source cpp library for creative coding. for the most part you can followw their [install instructions](https://openframeworks.cc/download/) for your chosen platform.

### install on linux:

- install and compile __openFrameworks12__ - in a terminal:

```
cd ~
wget https://github.com/openframeworks/openFrameworks/releases/download/0.12.0/of_v0.12.0_linux64gcc6_release.tar.gz
mkdir openframeworks
tar vxfz of_v0.12.0_linux64gcc6_release.tar.gz -C openframeworks --strip-components 1
cd openframeworks/scripts/linux/<YOUR DISTRIBUTION HERE>/
sudo ./install_dependencies.sh -y
make Release -C ~/openframeworks/libs/openFrameworksCompiled/project
```

- install NDI SDK (if using NDI - recommended):

download [NDI SDK](https://ndi.video/for-developers/ndi-sdk/download/) and unzip: `NDI SDK for Linux`

then copy the sdk into usr/lib: `sudo cp ~/NDI\ SDK\ for\ Linux/lib/x86_64-linux-gnu/* /usr/lib/`

- download __artnet_led_mapper__ and dependanices:

```
<INSTALL GIT IF YOU DO NOT HAVE IT>
cd ~/openframeworks/apps/myApps
git clone git@github.com:cyberboy666/artnet_led_mapper.git
cd artnet_led_mapper
./install_addons.sh
make
```

now it is compiled can run with `make run` or with `./bin/artnet_led_mapper`

### install on windows 10

follow the [openframeworks visual studio](https://openframeworks.cc/setup/vs/) guide:

- download [visual studio community 2019](https://www.visualstudio.com/thank-you-downloading-visual-studio/?sku=Community&rel=16) and install with _desktop development with c++_ plus other settings as listed on that oF page

install openframeworks and artnet_led_mapper:

- download and install [git for windows](https://gitforwindows.org/)
- download and unzip [openframeworks 12 for visual studio](https://github.com/openframeworks/openFrameworks/releases/download/0.12.0/of_v0.12.0_vs_release.zip)
- open up `git bash` and navigate to the openframeworks folder:

```
cd ~/path/to/of12_folder/
cd apps/myApps/
git clone git@github.com:cyberboy666/artnet_led_mapper.git
cd artnet_led_mapper
./install_addons.sh
```

install NDI SDK (if using NDI - recommended)

- download the [sdk](https://ndi.video/for-developers/ndi-sdk/download/)
- you may need to copy `Processing.NDI.Lib.x64.dll` from here into `artnet_led_mapper/bin` if it doesnt load automatically

build __artnet_led_mapper__ app:

- open the openframeworks __ProjectGenerator__ in top level of openframeworks folder
- from here import the __artnet_led_mapper__ folder and generate project
- now you can open the project in _visual studio_ with the file `artnet_led_mapper.sln`
- inside _visual studio_ find __BUILD__ / __RUN__  

### install on mac_os

follow the [openframeworks xcode](https://openframeworks.cc/setup/xcode/) guide:

- download xcode (14.1 or later) + Xcode's command line tools... (check internet archive if you can not access apple store)
- download and unzip [openframeworks 12 for xcode](https://github.com/openframeworks/openFrameworks/releases/download/0.12.0/of_v0.12.0_osx_release.zip)
- open up terminal and navigate to the openframeworks folder:

```
cd ~/path/to/openframeworks
cd apps/myApps/
git clone git@github.com:cyberboy666/artnet_led_mapper.git
cd artnet_led_mapper
./install_addons.sh
```

install NDI SDK (if using NDI - recommended)

- download the [sdk](https://ndi.video/for-developers/ndi-sdk/download/)

build __artnet_led_mapper__ app:

- open the openframeworks __ProjectGenerator__ in top level of openframeworks folder
- from here import the __artnet_led_mapper__ folder and generate project
- now you can open the project in _xcode_ with the file `Project.xcconfig`
- inside _xcode_ find __BUILD__ / __RUN__  

### install on raspberry_pi_4 or similar arm64:

- download the the raspberry pi os image: `2024-03-15-raspios-bookworm-arm64.img.xz` and flash to the sd card
- once the rpi is booted / connected to internet etc then you can download openframeworks:

```
cd ~
wget https://github.com/openframeworks/openFrameworks/releases/download/0.12.0/of_v0.12.0_linuxaarch64_release.tar.gz
mkdir openframeworks
tar vxfz of_v0.12.0_linuxaarch64_release.tar.gz -C openframeworks --strip-components 1
cd openframeworks/scripts/linux/debian/
sudo ./install_dependencies.sh -y
make Release -C ~/openframeworks/libs/openFrameworksCompiled/project
``` 

- install NDI SDK (if using NDI - recommended):

download [NDI SDK](https://ndi.video/for-developers/ndi-sdk/download/) and unzip: `NDI SDK for Linux`

then copy the sdk into usr/lib: `sudo cp ~/NDI\ SDK\ for\ Linux/lib/aarch64-rpi4-linux-gnueabi/* /usr/lib/`

- download __artnet_led_mapper__ and dependanices:

```
<INSTALL GIT IF YOU DO NOT HAVE IT>
cd ~/openframeworks/apps/myApps
git clone git@github.com:cyberboy666/artnet_led_mapper.git
cd artnet_led_mapper
./install_addons.sh
```

__TURN ON FLAG TO COMPILE FOR GLES__

since openframeworks12 on rpi doesnt work rn with standard glsl we need to switch to using GLES - this limits the program to only having one window so it can not be used with multiple display outputs

```
cd cd ~/openframeworks/apps/myApps/artnet_led_mapper
nano src/ofApp.h
```

and UNCOMMENT LINE 9:
```
#define PROJECT_TARGET_OPENGLES
```
and save (`CTRL+X`, `Y`, `ENTER`) - now compile the program:

```
make
```

now it is compiled can run with `make run` or with `./bin/artnet_led_mapper`


</details>
