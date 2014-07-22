#!/bin/sh
# Force Bourne shell in case tcsh is default.
#
 
#
# Reads the bundle type
#
 
# echo "========================================================"
# echo "    Bundle creation script"
# echo "========================================================"
# echo ""
# echo " Please select which kind of bundle you would like to build:"
# echo ""
# echo " 1 > Debug bundle"
# echo " 2 > Release bundle"
# echo " 0 > Exit"
 
# read command
 
# case $command in
 
#   1) ;;
 
#   2) ;;
 
#   0) exit 0;;
 
#   *) echo "Invalid command"
#      exit 0;;
 
# esac
 
#
# Creates the bundle
#
 
appname=spotfilter
appfolder=$appname.app
macosfolder=$appfolder/Contents/MacOS
plistfile=$appfolder/Contents/Info.plist
appfile=spotfilter
icon=macicon.icns

otherexe=scaletracker
otherexename=scaletracker
otherdir=../scaletracker/

# PkgInfoContents="APPLMAG#"
 
#
if ! [ -e $appfile ]; then
    echo "$appfile does not exist"
    exit
fi

if [ -d $appfolder ]; then
    rm -fr $appfolder
fi

echo "Creating $appfolder..."
mkdir $appfolder
mkdir $appfolder/Contents
mkdir $appfolder/Contents/MacOS
mkdir $appfolder/Contents/Resources

#
# For a debug bundle,
# Instead of copying executable into .app folder after each compile,
# simply create a symbolic link to executable.
#
# if [ $command = 1 ]; then
#   ln -s ../../../$appname $macosfolder/$appname
# else
#   cp $appname $macosfolder/$appname
# fi  

cp $appfile $macosfolder/$appname


# Copy the resource files to the correct place
# cp *.bmp $appfolder/Contents/Resources
# cp icon3.ico $appfolder/Contents/Resources
# cp icon3.png $appfolder/Contents/Resources
cp $icon $appfolder/Contents/Resources
# cp docs/*.* $appfolder/Contents/Resources
#
# Create PkgInfo file.
# echo $PkgInfoContents >$appfolder/Contents/PkgInfo



# Create information property list file (Info.plist).
echo '<?xml version="1.0" encoding="UTF-8"?>' >$plistfile
echo '<!DOCTYPE plist PUBLIC "-//Apple Computer//DTD PLIST 1.0//EN" "http://www.apple.com/DTDs/PropertyList-1.0.dtd">' >>$plistfile
echo '<plist version="1.0">' >>$plistfile
echo '<dict>' >>$plistfile
echo '  <key>CFBundleDevelopmentRegion</key>' >>$plistfile
echo '  <string>English</string>' >>$plistfile
echo '  <key>CFBundleExecutable</key>' >>$plistfile
echo '  <string>'$appname'</string>' >>$plistfile
echo '  <key>CFBundleIconFile</key>' >>$plistfile
echo '  <string>'$icon'</string>' >>$plistfile
echo '  <key>CFBundleIdentifier</key>' >>$plistfile
echo '  <string>org.'$appname'.'$appname'</string>' >>$plistfile
echo '  <key>CFBundleInfoDictionaryVersion</key>' >>$plistfile
echo '  <string>6.0</string>' >>$plistfile
echo '  <key>CFBundlePackageType</key>' >>$plistfile
echo '  <string>APPL</string>' >>$plistfile
echo '  <key>CFBundleSignature</key>' >>$plistfile
echo '  <string>MAG#</string>' >>$plistfile
echo '  <key>CFBundleVersion</key>' >>$plistfile
echo '  <string>1.0</string>' >>$plistfile
echo '</dict>' >>$plistfile
echo '</plist>' >>$plistfile


./dylibbundler -od -b -x ./$appname.app/Contents/MacOS/$appname -d ./$appname.app/Contents/libs/

cp $otherdir/$otherexe ./$appname.app/Contents/MacOS/$otherexename

