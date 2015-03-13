# Android Screenshooter #

Capture Screen in command line on both PC and Device(mobile) side.

Support both emulator and Mobile Device.

Can be easily embedded into c code.

# Quick Start #

## PC Side ##

  * Download prebuilt version `adb_screenshoot` from here: http://code.google.com/p/android-fb2png/downloads/

  * Capture Android's screen and save it into the current working directory.
```
$ adb_screenshoot
```

## Device Side ##
  * Download prebuilt version `fb2png` from here: http://code.google.com/p/android-fb2png/downloads/
  * Push to device/emulator
```
$ adb push fb2png /data/local/
```
  * Grant execute permission
```
$ adb shell chmod 755 /data/local/fb2png
```
  * Capture screen and save to /data/local/fbdump.png
```
$ adb shell /data/local/fb2png /data/local/fbdump.png
```
  * Pull the screenshot back
```
$ adb pull /data/local/fbdump.png
```
  * View it with your favorite image viewer