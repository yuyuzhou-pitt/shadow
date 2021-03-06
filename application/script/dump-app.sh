#!/bin/bash
# echo -e "\n\n\n" | ssh-keygen -t rsa
# cat ~/.ssh/id_rsa.pub | ssh root@192.168.0.104 'cat >> ~/.ssh/authorized_keys'
PID_FILE=../.1_dump_pid
rm -rf $PID_FILE

timestamp() {
    date +"%T.%N"
}

DUMP_DIR=`hostname -i | awk '{print $NF}'`
if [[ $# -lt 2 ]];then
    echo "USAGE: dump-app.sh <full-app-path> <shadow-ip:/tmp/>"
    echo "Example: dump-app.sh /root/shadow/application/pi 192.168.0.113:/tmp/"
    exit -1;
fi
pid=$(ps aux |grep "$1" | grep -v 'criu' | head -n 1 | awk '{print $2}')
mkdir -p "$pid"
cd "$pid"

printf "+--BEFORE-DUMP-------\n"
timestamp 
printf "+-------------------+\n"
criu dump -t "$pid" -vvv -o dump.log --shell-job --leave-stopped  && echo dump OK
printf "+--AFTER-DUMP-------+\n"
timestamp 
printf "+-------------------+\n"
cd ..
# overwrite target directory
scp -r "$pid" $2
rm -fr /tmp/"$pid"
cp -r "$pid" /tmp/"$pid"
kill -9 "$pid"
criu restore -d --tree "$pid" --images-dir /tmp/"$pid" -vvvv -o restore.log --shell-job && echo restore OK
printf "+--New AFTER-COPY-------+\n"
timestamp 
printf "+-------------------+\n"
sleep 1
echo $pid > $PID_FILE
