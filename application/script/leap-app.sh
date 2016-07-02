#!/bin/bash
# echo -e "\n\n\n" | ssh-keygen -t rsa
# cat ~/.ssh/id_rsa.pub | ssh root@192.168.0.104 'cat >> ~/.ssh/authorized_keys'

if [[ $# -lt 2 ]];then
    echo "USAGE: leap-app.sh <full-app-path> <shadow-ip:/shadow/application/script>"
    echo "Example: leap-app.sh /root/shadow/application/pi 10.10.2.205:/root/shadow/application/script"
    exit -1;
fi
pid=$(ps aux |grep "$1" | head -n 1 | awk '{print $2}')
echo $pid
mkdir "$pid"-2
cd "$pid"-2
criu dump -t "$pid"-2 -vvv -o dump.log --shell-job --prev-images-dir ../"$pid" --leave-running  && echo dump OK
cd ..
scp -r "$pid"-2 $2