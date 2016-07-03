#!/bin/bash
if [[ $# -lt 1 ]];then
    echo "USAGE: restore-app.sh <pid>"
    echo "Example: restore-app.sh 23456"
    exit -1;
fi
timestamp() {
    date +"%T.%N"
}
printf "+--BEFORE-RESTORE---+\n"
timestamp
printf "+-------------------+\n"
criu restore -d --tree ${1} --images-dir /tmp/${1} -vvvv -o restore.log --shell-job && echo restore OK
printf "+--AFTER-RESTORE----+\n"
timestamp
printf "+-------------------+\n"
