#!/bin/bash
if [[ $# -lt 1 ]];then
    echo "USAGE: restore-app.sh </tmp/dump_$pid>"
    echo "Example: restore-app.sh /tmp/dump_4567"
    exit -1;
fi
criu restore --tree $1 --images-dir ./$1 -vvvv -o restore.log --shell-job && echo restore OK
