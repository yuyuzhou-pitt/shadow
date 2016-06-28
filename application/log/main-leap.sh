pid=$(ps aux |grep "./pi" | head -n 1 | awk '{print $2}')
echo $pid
mkdir "$pid"-2
cd "$pid"-2
criu dump -t "$pid"-2 -vvv -o dump.log --shell-job --prev-images-dir ../"$pid" --leave-running  && echo dump OK
cd ..
scp -r 5414-2 192.168.117.133:/home/yuyuzhou/code/shadow/application/log
