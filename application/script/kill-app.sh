#!/bin/bash
if [[ $# -lt 2 ]];then
    echo "USAGE: kill-app.sh <full-app-path> <pid>"
    echo "Example: kill-app.sh /root/shadow/application/pi 3456"
    exit -1;
fi

# clear the old running app
pid=$(ps aux |grep "$1" | grep -v "$2" | head -n 1 | awk '{print $2}')
kill -9 "$pid"
