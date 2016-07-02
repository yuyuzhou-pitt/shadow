Before starting the cluster, make sure:
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
* Passwordless ssh from Main to Shadow (for criu scp):
  # echo -e "\n\n\n" | ssh-keygen -t rsa
  # cat ~/.ssh/id_rsa.pub | ssh root@<shadow_ip> 'cat >> ~/.ssh/authorized_keys'
