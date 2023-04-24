#!/bin/bash

if [ $# -ne 0 ]; then
    echo "Usage: $0"
    exit 1
fi

#savepath=/media/haselab-08/Extreme\ SSD/log
#cd "$savepath"

filename="$(date +%Y.%m%d.%H%M)"
echo ${filename}
mkdir ${filename}
cd ${filename}
pwd
ssm-logger -l remote_localizer.log -n remote_localizer -i 1 
#ssm-logger -l localizer.log -n localizer -i 0
#bash
