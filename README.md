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
