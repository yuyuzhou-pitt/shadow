#!/bin/bash
if [[ $# -lt 2 ]];then
    echo "USAGE: restore-app.sh <full-app-path> <pid>"
    echo "Example: restore-app.sh /root/shadow/application/pi 23456"
    exit -1;
fi

# clear the old running app
pid=$(ps aux |grep "$1" | grep -v 'criu' | head -n 1 | awk '{print $2}')

timestamp() {
    date +"%T.%N"
}
printf "+--BEFORE-RESTORE---+\n"
timestamp
printf "+-------------------+\n"
# restore the saved app
criu restore -d --tree ${2} --images-dir /tmp/${2} -vvvv -o restore.log --shell-job && echo restore OK
printf "+--AFTER-RESTORE----+\n"
timestamp
printf "+-------------------+\n"
#kill -9 "$pid"
