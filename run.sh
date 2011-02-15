#!/bin/sh
#
# helper script for capture picture on device
#
# Kyan He <kyan.ql.he@gmail.com> @ Tue Feb 15 12:42:48 CST 2011
#
#

PNG=/data/local/fbdump

if [ ! "$FB2PNG" = "" ];
then

adb -d push $FB2PNG /data/local
adb -d shell chmod 777 /data/local
adb -d shell /data/local/fb2png $PNG

adb -d pull $PNG.png
adb -d shell rm $PNG.png
else
    echo "define \$FB2PNG first"
fi
