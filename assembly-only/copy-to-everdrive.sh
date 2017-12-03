#!/bin/bash
EVERDRIVE=/Volumes/EVERDRIVE

if [ ! -d "$EVERDRIVE" ]; then
	echo "Everdrive SD not mounted at $EVERDRIVE"
	exit 1
fi

cp *.bin $EVERDRIVE/Sega\ Mega\ Drive/Personal/
echo Sorting FAT...
SD_DEVICE=$(mount | grep $EVERDRIVE | cut -d ' ' -f1)
diskutil unmount $SD_DEVICE
sudo fatsort -no a $SD_DEVICE
echo Done - SD dismounted


