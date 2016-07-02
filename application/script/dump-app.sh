#!/bin/bash
# echo -e "\n\n\n" | ssh-keygen -t rsa
# cat ~/.ssh/id_rsa.pub | ssh root@192.168.0.104 'cat >> ~/.ssh/authorized_keys'

if [[ $# -lt 2 ]];then
    echo "USAGE: dump-app.sh <full-app-path> <shadow-ip:/tmp/dump_$pid>"
    echo "Example: dump-app.sh /root/shadow/application/pi 10.10.2.205:/tmp/dump_4567"
    exit -1;
fi
pid=$(ps aux |grep "$1" | head -n 1 | awk '{print $2}')
echo $pid
mkdir "dump_$pid"
cd "dump_$pid"
criu dump -t "dump_$pid" -vvv -o dump.log --shell-job --prev-images-dir ../"dump_$pid" --leave-running  && echo dump OK
cd ..
# overwrite target directory
scp -prq "dump_$pid/." $2
