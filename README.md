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
  # cat ~/.ssh/id_rsa.pub | ssh root@<shadow_ip> 'mkdir -p ~/.ssh; touch ~/.ssh/authorized_keys; cat >> ~/.ssh/authorized_keys'
