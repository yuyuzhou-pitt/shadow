pid=$(ps aux |grep "./pi" | head -n 1 | awk '{print $2}')
echo $pid
mkdir "$pid"
cd "$pid"
criu dump -t $pid -vvv -o dump.log --shell-job  && echo dump OK
