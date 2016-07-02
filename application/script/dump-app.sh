#!/bin/bash
# echo -e "\n\n\n" | ssh-keygen -t rsa
# cat ~/.ssh/id_rsa.pub | ssh root@192.168.0.104 'cat >> ~/.ssh/authorized_keys'

DUMP_DIR=`hostname -i | awk '{print $NF}'`
if [[ $# -lt 2 ]];then
    echo "USAGE: dump-app.sh <full-app-path> <shadow-ip:/tmp/dump_$pid>"
    echo "Example: dump-app.sh /root/shadow/application/pi 10.10.2.205:/tmp/dump_4567"
    exit -1;
fi
pid=$(ps aux |grep "$1" | head -n 1 | awk '{print $2}')
echo $pid
mkdir -p "$DUMP_DIR"
cd "$DUMP_DIR"
criu dump -t "$pid" -vvv -o dump.log --shell-job --prev-images-dir ../"$DUMP_DIR" --leave-running  && echo dump OK
cd ..
# overwrite target directory
scp -r "$DUMP_DIR" $2
