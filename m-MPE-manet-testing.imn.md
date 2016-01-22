/*
* released under GPLv3
*/
++++++++++++++++++++++++++++++++++++++++++++++

make testing in single PC without CORE:

[client-manet.c] send to
[(MASTER) handle_client(void *arg) IN server-manet.c] send to
[(MASTER) m_thread(void *arg) IN server-manet.c] send to
[(SLAVE) slave(void *arg) IN server-manet-slave.c]

startup sequence:
[root@localhost server-client-pthread-c]# ./server-manet
[root@localhost server-client-pthread-c]# ./server-manet-slave
[root@localhost server-client-pthread-c]# ./client-manet

++++++++++++++++++++++++++++++++++++++++++++++

make testing by using CORE in single PC:

1. 	在每个节点的主目录创建文件： ctrl.txt, masterip.txt, hosts, 
	根据batman-adv协议，选择 主节点，其它为从节点；要修改 上面3个文件的值

[client-manet.c] send to
[(MASTER) handle_client(void *arg) IN server-manet.c] send to
[(MASTER) m_thread(void *arg) IN server-manet.c] send to
[(SLAVE) slave(void *arg) IN server-manet.c]

startup sequence:
[root@localhost server-client-pthread-c]# ./server-manet
[root@localhost server-client-pthread-c]# ./client-manet

2.  create three files (ctrl.txt, masterip.txt, hosts) in every node directory.

steps:

[1]
[10.0.0.1] ~ [10.0.0.10]
elect the max IP and max links number as Master, other as Slave.

[2]
Now, the Master is [10.0.0.10],
# execute the following commands in gnome-terminal
echo 10.0.0.10 > /tmp/pycore.40288/n1.conf/masterip.txt
echo 10.0.0.10 > /tmp/pycore.40288/n2.conf/masterip.txt
echo 10.0.0.10 > /tmp/pycore.40288/n3.conf/masterip.txt
echo 10.0.0.10 > /tmp/pycore.40288/n4.conf/masterip.txt
echo 10.0.0.10 > /tmp/pycore.40288/n5.conf/masterip.txt
echo 10.0.0.10 > /tmp/pycore.40288/n6.conf/masterip.txt
echo 10.0.0.10 > /tmp/pycore.40288/n7.conf/masterip.txt
echo 10.0.0.10 > /tmp/pycore.40288/n8.conf/masterip.txt
echo 10.0.0.10 > /tmp/pycore.40288/n9.conf/masterip.txt
echo 10.0.0.10 > /tmp/pycore.40288/n10.conf/masterip.txt
echo 10.0.0.10 > /tmp/pycore.40288/n12.conf/masterip.txt

echo 0 > /tmp/pycore.40288/n1.conf/ctrl.txt
echo 0 > /tmp/pycore.40288/n2.conf/ctrl.txt
echo 0 > /tmp/pycore.40288/n3.conf/ctrl.txt
echo 0 > /tmp/pycore.40288/n4.conf/ctrl.txt
echo 0 > /tmp/pycore.40288/n5.conf/ctrl.txt
echo 0 > /tmp/pycore.40288/n6.conf/ctrl.txt
echo 0 > /tmp/pycore.40288/n7.conf/ctrl.txt
echo 0 > /tmp/pycore.40288/n8.conf/ctrl.txt
echo 0 > /tmp/pycore.40288/n9.conf/ctrl.txt
echo 1 > /tmp/pycore.40288/n10.conf/ctrl.txt

echo "10.0.0.101   mpe.localhost" > /tmp/pycore.40288/n1.conf/hosts
echo "10.0.0.101   mpe.localhost" > /tmp/pycore.40288/n2.conf/hosts
echo "10.0.0.101   mpe.localhost" > /tmp/pycore.40288/n3.conf/hosts
echo "10.0.0.101   mpe.localhost" > /tmp/pycore.40288/n4.conf/hosts
echo "10.0.0.101   mpe.localhost" > /tmp/pycore.40288/n5.conf/hosts
echo "10.0.0.101   mpe.localhost" > /tmp/pycore.40288/n6.conf/hosts
echo "10.0.0.101   mpe.localhost" > /tmp/pycore.40288/n7.conf/hosts
echo "10.0.0.101   mpe.localhost" > /tmp/pycore.40288/n8.conf/hosts
echo "10.0.0.101   mpe.localhost" > /tmp/pycore.40288/n9.conf/hosts
echo "10.0.0.101   mpe.localhost" > /tmp/pycore.40288/n10.conf/hosts

/bin/cp /root/桌面/server-client-pthread-c/m-MPE-manet-testing/server-manet /tmp/pycore.40288/n1.conf
/bin/cp /root/桌面/server-client-pthread-c/m-MPE-manet-testing/server-manet /tmp/pycore.40288/n2.conf
/bin/cp /root/桌面/server-client-pthread-c/m-MPE-manet-testing/server-manet /tmp/pycore.40288/n3.conf
/bin/cp /root/桌面/server-client-pthread-c/m-MPE-manet-testing/server-manet /tmp/pycore.40288/n4.conf
/bin/cp /root/桌面/server-client-pthread-c/m-MPE-manet-testing/server-manet /tmp/pycore.40288/n5.conf
/bin/cp /root/桌面/server-client-pthread-c/m-MPE-manet-testing/server-manet /tmp/pycore.40288/n6.conf
/bin/cp /root/桌面/server-client-pthread-c/m-MPE-manet-testing/server-manet /tmp/pycore.40288/n7.conf
/bin/cp /root/桌面/server-client-pthread-c/m-MPE-manet-testing/server-manet /tmp/pycore.40288/n8.conf
/bin/cp /root/桌面/server-client-pthread-c/m-MPE-manet-testing/server-manet /tmp/pycore.40288/n9.conf
/bin/cp /root/桌面/server-client-pthread-c/m-MPE-manet-testing/server-manet /tmp/pycore.40288/n10.conf

/bin/cp /root/桌面/server-client-pthread-c/m-MPE-manet-testing/client-manet /tmp/pycore.40288/n12.conf

看系统中的vnoded进程占用资源的情况：
top -p `pgrep vnoded | tr "\\n" "," | sed 's/,$//'`
也可以通过ps命令先找到进程号，然后通过进程号来查看：
top -p1212 -p2323

[3] not use the following commands, use terminals in CORE directly.
# execute the following commands in gnome-terminal
cd /tmp/pycore.40288/n1.conf; gnome-terminal --hide-menubar --title=`pwd` --geometry=80x25+0+0 -x bash -c "./server-manet; sleep 10000"
cd /tmp/pycore.40288/n2.conf; gnome-terminal --hide-menubar --title=`pwd` --geometry=80x25+20+80 -x bash -c "./server-manet; sleep 10000"
cd /tmp/pycore.40288/n3.conf; gnome-terminal --hide-menubar --title=`pwd` --geometry=80x25+40+130 -x bash -c "./server-manet; sleep 10000"
gnome-terminal --hide-menubar --title=`pwd` --geometry=80x25+60+180 -x bash -c "cd /tmp/pycore.40288/n4.conf; ./server-manet; sleep 10000"
gnome-terminal --hide-menubar --title=`pwd` --geometry=80x25+80+230 -x bash -c "cd /tmp/pycore.40288/n5.conf; ./server-manet; sleep 10000"
gnome-terminal --hide-menubar --title=`pwd` --geometry=80x25+380+0 -x bash -c "cd /tmp/pycore.40288/n6.conf; ./server-manet; sleep 10000"
gnome-terminal --hide-menubar --title=`pwd` --geometry=80x25+420+80 -x bash -c "cd /tmp/pycore.40288/n7.conf; ./server-manet; sleep 10000"
gnome-terminal --hide-menubar --title=`pwd` --geometry=80x25+460+130 -x bash -c "cd /tmp/pycore.40288/n8.conf; ./server-manet; sleep 10000"
gnome-terminal --hide-menubar --title=`pwd` --geometry=80x25+500+180 -x bash -c "cd /tmp/pycore.40288/n9.conf; ./server-manet; sleep 10000"
gnome-terminal --hide-menubar --title=`pwd` --geometry=80x25+540+230 -x bash -c "cd /tmp/pycore.40288/n10.conf; ./server-manet; sleep 10000"
sleep 3
gnome-terminal --hide-menubar --title=`pwd` -x bash -c "cd /tmp/pycore.40288/n12.conf; ./client-manet; sleep 10000"

cd /root/桌面/server-client-pthread-c/m-MPE-manet-testing

[4]
elect the max IP and max links as Master, other as Slave.
Now, the Master is [10.0.0.9],
# execute the following commands in gnome-terminal
echo 10.0.0.9 > /tmp/pycore.40288/n1.conf/masterip.txt
echo 10.0.0.9 > /tmp/pycore.40288/n2.conf/masterip.txt
echo 10.0.0.9 > /tmp/pycore.40288/n3.conf/masterip.txt
echo 10.0.0.9 > /tmp/pycore.40288/n4.conf/masterip.txt
echo 10.0.0.9 > /tmp/pycore.40288/n5.conf/masterip.txt
echo 10.0.0.9 > /tmp/pycore.40288/n6.conf/masterip.txt
echo 10.0.0.9 > /tmp/pycore.40288/n7.conf/masterip.txt
echo 10.0.0.9 > /tmp/pycore.40288/n8.conf/masterip.txt
echo 10.0.0.9 > /tmp/pycore.40288/n9.conf/masterip.txt
echo 10.0.0.9 > /tmp/pycore.40288/n10.conf/masterip.txt
echo 10.0.0.9 > /tmp/pycore.40288/n12.conf/masterip.txt

echo 0 > /tmp/pycore.40288/n1.conf/ctrl.txt
echo 0 > /tmp/pycore.40288/n2.conf/ctrl.txt
echo 0 > /tmp/pycore.40288/n3.conf/ctrl.txt
echo 0 > /tmp/pycore.40288/n4.conf/ctrl.txt
echo 0 > /tmp/pycore.40288/n5.conf/ctrl.txt
echo 0 > /tmp/pycore.40288/n6.conf/ctrl.txt
echo 0 > /tmp/pycore.40288/n7.conf/ctrl.txt
echo 0 > /tmp/pycore.40288/n8.conf/ctrl.txt
echo 1 > /tmp/pycore.40288/n9.conf/ctrl.txt
echo 0 > /tmp/pycore.40288/n10.conf/ctrl.txt

[5]
elect the max IP and max links as Master, other as Slave.
Now, the Master is [10.0.0.10],
# execute the following commands in gnome-terminal
echo 10.0.0.10 > /tmp/pycore.40288/n1.conf/masterip.txt
echo 10.0.0.10 > /tmp/pycore.40288/n2.conf/masterip.txt
echo 10.0.0.10 > /tmp/pycore.40288/n3.conf/masterip.txt
echo 10.0.0.10 > /tmp/pycore.40288/n4.conf/masterip.txt
echo 10.0.0.10 > /tmp/pycore.40288/n5.conf/masterip.txt
echo 10.0.0.10 > /tmp/pycore.40288/n6.conf/masterip.txt
echo 10.0.0.10 > /tmp/pycore.40288/n7.conf/masterip.txt
echo 10.0.0.10 > /tmp/pycore.40288/n8.conf/masterip.txt
echo 10.0.0.10 > /tmp/pycore.40288/n9.conf/masterip.txt
echo 10.0.0.10 > /tmp/pycore.40288/n10.conf/masterip.txt
echo 10.0.0.10 > /tmp/pycore.40288/n12.conf/masterip.txt

echo 0 > /tmp/pycore.40288/n1.conf/ctrl.txt
echo 0 > /tmp/pycore.40288/n2.conf/ctrl.txt
echo 0 > /tmp/pycore.40288/n3.conf/ctrl.txt
echo 0 > /tmp/pycore.40288/n4.conf/ctrl.txt
echo 0 > /tmp/pycore.40288/n5.conf/ctrl.txt
echo 0 > /tmp/pycore.40288/n6.conf/ctrl.txt
echo 0 > /tmp/pycore.40288/n7.conf/ctrl.txt
echo 0 > /tmp/pycore.40288/n8.conf/ctrl.txt
echo 0 > /tmp/pycore.40288/n9.conf/ctrl.txt
echo 1 > /tmp/pycore.40288/n10.conf/ctrl.txt

