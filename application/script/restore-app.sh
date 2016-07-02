#!/bin/bash
if [[ $# -lt 1 ]];then
    echo "USAGE: restore-app.sh <pid>"
    echo "Example: restore-app.sh 23456"
    exit -1;
fi
criu restore --tree ${1} --images-dir /tmp/${1}-2 -vvvv -o restore.log --shell-job && echo restore OK
