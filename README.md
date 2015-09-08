# android-fb2png

> Capture framebuffer of Android and save it in png format.

## Features

- Run CLI on both PC and Mobile side.
- Support both emulator and device.
- API can be easily embedded into c code.

## Getting started

### PC Side

Download prebuilt version adb_screenshoot from here: http://code.google.com/p/android-fb2png/downloads/

Capture Android's screen and save it into the current working directory.

```bash
adb_screenshoot
```

### Device Side

Download prebuilt version fb2png from here: http://code.google.com/p/android-fb2png/downloads/

Push to device/emulator and grant execute permission

```bash
adb push fb2png /data/local/
adb shell chmod 755 /data/local/fb2png
```

Capture screen and save to /data/local/fbdump.png

```bash
adb shell /data/local/fb2png /data/local/fbdump.png
```

Pull the screenshot back

```bash
adb pull /data/local/fbdump.png
```

Then you can view it with your favorite image viewer
