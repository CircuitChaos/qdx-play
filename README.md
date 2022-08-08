# qdx-play

An utility to play live audio from the **QDX** transveiver via another ALSA sound card with automatic gain control.

## Objective

**QDX** is a cool transceiver made by **QRP Labs**. It exposes two ports to the controlling computer: a serial port, which accepts CAT commands, and a sound card, 
which is an ordinary ALSA device.

By redirecting audio from this sound card to another sound card and controlling QDX with CAT commands (for example, with the **rigctl** program), one can use QDX as an ordinary SSB receiver.

There are several tools that can be used to redirect one ALSA device to another (**alsaloop** being an obvious example), but I haven't found any that would do 
automatic gain control. That's why I wrote this program.

This program handles only the sound card part of the QDX. There's a separate gain control accessible via the serial interface. This program doesn't touch it.

## How to install

The program is developed to work under Linux with the bare ALSA sound system (without PulseAudio, JACK or other sound servers).

To install it, you will need:

* **git** utility – to download the repository and to append commit hash to the software version string
* **scons** utility – scons is a nice, Python-based replacement for makefiles. It is needed to compile and install the program
* **g++** compiler with support for C++11 – as software is written in this C++ dialect
* **libasound2** and **libasound2-dev** packages – as software needs them to handle ALSA

On Debian, Raspbian, Ubuntu and other Debian-based systems you should be able to install these packages with:

`apt-get install git scons g++ libasound2 libasound2-dev`

To download, compile and install the program, you can use the following one-liner:

`git clone https://github.com/CircuitChaos/qdx-play && cd qdx-play && scons && sudo scons install`

If everything goes right, the program binary, called **qdx-play**, will be installed in the */usr/local/bin* directory – although, if you don't want to install it system-wide, you don't have to (just skip the `&& sudo scons install` part above). In this case, the binary will be available in *build* directory of the downloaded repository.

## How to use

Use `qdx-play -h` to display help. All arguments are optional and the program should run just fine without any arguments.

Recognized command-line options:

* -b *buffer size* – can be used to specify buffer size, in samples. Default value should be OK
* -c *capture device* – can be used to provide the capture device if the QDX autodetection fails
* -g *gain* – can be used to specify gain. A special value **auto** means automatic gain control and is the default
* -h – display help and exit
* -p *playback device* – can be used to provide the playback device, if the default doesn't work
* -v – display version and exit

Note that gain is only used to amplify audio from the QDX before sending it to the sound card. **qdx-play** doesn't touch the CAT interface of the radio. If you want to set gain of the QDX itself, use **rigctl** or another program.

## How to tune

It's not a part of **qdx-play**, but you can install package **libhamlib-utils** and use **rigctl** to control the rig.

Working example to set frequency to 7074 kHz:

`rigctl -m $(rigctl -l|grep TS-480|awk '{print $1}') -r /dev/ttyACM0 F 7074000`

## TODO (and call for volunteers!)

What's left to be done:

* add quiet and verbose mode
* make it more generic with regards to input and output parameters
* add FFT analysis to report dominant frequencies

## Contact with the author

Please use the GitHub issue reporting system for contact.
