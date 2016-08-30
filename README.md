Before starting the cluster, make sure:
1. Socket communication
* All the servers IP are added into /etc/hosts on each node
* Firewall disabled: 
  # systemctl disable firewalld
  # systemctl stop firewalld
* iptables disabled
  # yum install iptables-services
  # systemctl disable iptables 
  # systemctl stop iptables
* Selinux disabled (reboot required)
  # setenforce 0
  # tail /etc/selinux/config | grep ^SELINUX=
  SELINUX=disabled

2. CRIU support
* criu has been installed on Main and Shadow machine (https://criu.org/Installation)

3. Passwordless SSH from Main to Shadow (for criu scp)
*  On Main machine:
  # echo -e "\n\n\n" | ssh-keygen -t rsa
  # cat ~/.ssh/id_rsa.pub | ssh root@<shadow_ip> 'mkdir -p ~/.ssh; touch ~/.ssh/authorized_keys; cat >> ~/.ssh/authorized_keys

Demo of executing:
git clone this repo at /root

1. start supervisor
[root@supervisor]# cd /root/shadow/supervisor; make clean; make
[root@supervisor]# ./supervisor

(supervisor): Staring supervisor...
(supervisor): Supervisor started (ip port): xxx.xxx.xxx.18 40592.
(supervisor): please track log file for detail: shadow.log

== WELCOME TO TERMINAL FOR SHADOW! ==

Enter the commands 'help' for usage.

(supervisor)# 

2. Start two manager
[root@manager1]# cd /root/shadow/manager; make clean; make
[root@manager1]# ./manager xxx.xxx.xxx.18 40592
(manager): Staring manager...
(manager): Manager started (ip port): xxx.xxx.xxx.25 59683.
(manager): Ready to receive jobs, please track log file for detail: shadow.log


[root@manager2]# cd /root/shadow/manager; make clean; make
[root@manager2]# ./manager xxx.xxx.xxx.18 40592
(manager): Staring manager...
(manager): Manager started (ip port): xxx.xxx.xxx.28 59683.
(manager): Ready to receive jobs, please track log file for detail: shadow.log

3. Run noise in second manasger
[root@manager2]# cd /root/shadow/application/script; sh launch-noise.sh

4. Launch and leap application via supervisor:
(supervisor)# launch xxx.xxx.xxx.25 xxx.xxx.xxx.28 /root/shadow/application/script/launch.sh
(supervisor)# leap xxx.xxx.xxx.25 xxx.xxx.xxx.28 /root/shadow/application/pi

