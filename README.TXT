SDUMP
=====

SDUMP is an MSDOS utility that can take any file and dump it's contents over a serial port.

Insstallation:

1. Download the latest release from the Github releases page and unzip it to `C:\SDUMP`. You can find the releases here: https://github.com/akhoury6/sdump/releases

2. Edit `SDUMP.INI` with the settings for your serial connection.

3. Run `SDUMP.EXE <filename>` to pipe that file over the serial port you configured.

Running the latest build:

If you'd like to build the codebase yourself, you'll need MS-DOS and a copy of Borland C++ 3.1. Make sure the `bcc` compiler is in your $PATH, then run `make` to compile the excutables.

License:

GPLv3
