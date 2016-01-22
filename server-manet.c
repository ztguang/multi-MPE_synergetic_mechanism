# released under GPLv3
// gcc -lpthread server-manet.c -o server-manet
// gcc -I/usr/include/mysql/ -L/usr/lib64/mysql/ -lmysqlclient -lpthread server-manet.c -o server-manet
// indent -npro -kr -i8 -ts8 -sob -l280 -ss -ncs -cp1 *
/*
make testing in the same computer:

[client-manet.c] send to
[(MASTER) handle_client(void *arg) IN server-manet.c] send to
[(MASTER) send2slave(void *arg) IN server-manet.c] send to
[(SLAVE) slave(void *arg) IN server-manet-slave.c]

startup sequence:

[root@localhost server-client-pthread-c]# ./server-manet
[root@localhost server-client-pthread-c]# ./server-manet-slave
[root@localhost server-client-pthread-c]# ./client-manet

++++++++++++++++++++++++++++++++++++++++++++++
make testing in CORE:

1. 	在每个节点的主目录创建文件： ctrl.txt, masterip.txt, hosts, 
	根据batman-adv协议，选择 主节点，其它为从节点；要修改 上面3个文件的值

[client-manet.c] send to
[(MASTER) handle_client(void *arg) IN server-manet.c] send to
[(MASTER) send2slave(void *arg) IN server-manet.c] send to
[(SLAVE) slave(void *arg) IN server-manet.c]

//******************** server-manet.c

int ctrl;			//global variable
int changed=0;
int pipe_fd[2];
char *masterip;
char *self_ip;

//read * from client-manet.c
//to simulate process execution，Database update event，Event driven database synchronization
void *handle_client(void *arg)
{
	bind(listener, self_ip);
	int pipe_write = *((int *)arg);
	while (1) {
		CHK2(client, accept(listener, (struct sockaddr *)&peer, &socklen));
		while (1) {
			CHK2(len, recv(client, clientmsg, CLIENTMSG_SIZE, MSG_NOSIGNAL));
			CHK(write(pipe_write, clientmsg, strlen(clientmsg)));	//send MSG to send2slave(void *arg)
		}
	}
}

int main(int argc, char *argv[])
{
	sprintf(self_ip,"%s",getipaddress("enp13s0"));

	pthread_create(&readctrl, NULL, read_ctrl, NULL);
	pthread_create(&writer, NULL, handle_client, (void *)&pipe_fd[1]);
	while (1) {
			pthread_create(&tid, NULL, master, NULL);	//ctrl==1
		or
			pthread_create(&tid, NULL, slave, NULL);	//ctrl==0
	}
}

void *read_ctrl(void *arg)
{
	while (1) {
		ctrl <- read from ctrl.txt;					//global variable, echo 1 >ctrl.txt
		masterip <- read from masterip.txt;			//global variable, echo 1.1.1.1 >masterip.txt
		master_ip = ip2uint(getipaddress(masterip));
		if (master_ip!=prev_ip) {
			replaceline(hosts, line, "masterip   mpe.localhost");		//update hosts
		}
	}
}

void *slave(void *arg)
{
	connect(sock, masterip);
	while (1)
	{
		if (changed) break;
		recv(sock, buf, BUF_SIZE, MSG_NOSIGNAL);	//read * from master
	}
}

void *master(void *arg)
{
	sprintf(self_ip,"%s",getipaddress("enp13s0"));
	bind(listener, self_ip);
	while (1) {
		if (changed) break;
		CHK2(client, accept(listener, (struct sockaddr *)&peer, &socklen));
		int rt = pthread_create(&reader, NULL, send2slave, (void *)&client);
	}
}

//event_driven, read pipe_fd[0] from handle_client(void *arg) which read * from client-manet.c 
void *send2slave(void *arg)
{
	int client = *((int *)arg);
	CHK(epoll_ctl(epfd, EPOLL_CTL_ADD, pipe_fd[0], &ev));
	////Using poll to simulate process execution，Database update event，Event driven database synchronization
	while (1) {		//communication between master & slave
		if (changed) break;
		if((epoll_events_count = epoll_wait(epfd, events, 1, EPOLL_RUN_TIMEOUT)) < 0){
			sleep(1);
			continue;
		}
		for (int i = 0; i < epoll_events_count; i++) {
			if (events[i].data.fd == pipe_fd[0])	//reading end of pipe, accept MSG form client-manet.c
			{
				CHK2(res, read(pipe_fd[0], clientmsg, CLIENTMSG_SIZE));
				CHK(send(client, "synchronous data", strlen("synchronous data"), MSG_NOSIGNAL));
			}
		}
	}
}
//******************** server-manet.c


//******************** server-manet-slave.c
int main(int argc, char *argv[])
{
	sprintf(self_ip,"%s",getipaddress("enp13s0"));
	pthread_create(&readctrl, NULL, read_ctrl, NULL);
	while (1) {
		pthread_create(&tid, NULL, slave, NULL);
		pthread_join(tid, &tret);
	} //end while

}

void *read_ctrl(void *arg)
{
	char filename[6] = "hosts";	//set master_ip in /etc/hosts
	uint32_t me_ip = ip2uint(getipaddress("enp13s0"));
	while (1) {
		ctrl <- read from ctrl_slave.txt;			//global variable, echo 0 >ctrl_slave.txt
		masterip <- read from masterip.txt;			//global variable, echo 1.1.1.1 >masterip.txt
		master_ip = ip2uint(getipaddress(masterip));
		if (master_ip!=prev_ip) {
			replaceline(hosts, line, "masterip   mpe.localhost");		//update hosts
		}
	}
}

void *slave(void *arg)
{
	connect(sock, masterip);
	while (1)
	{
		if (changed) break;
		recv(sock, buf, BUF_SIZE, MSG_NOSIGNAL));
	}
}
//******************** server-manet-slave.c


//******************** client-manet.c
int main(int argc, char *argv[])
{
	masterip <- read from masterip.txt;			//global variable, echo 1.1.1.1 >masterip.txt
	connect(sock, masterip);
	CHK(send(sock, "running process, update database", strlen("running process, update database"), 0));
}
//******************** client-manet.c

*/
//Description: when master start, this algorithm will run as a deamon

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <netinet/tcp.h>
#include <netinet/in.h>
#include <net/if.h>
#include <arpa/inet.h>
#include <sys/epoll.h>
#include <pthread.h>

//#endif
#define BUF_SIZE 1024
#define NAME_SIZE 40
#define PROCESSID_SIZE 52

#define MASTER_PORT 11111
#define CLIENT_PORT 22222
#define CLIENTMSG_SIZE 62

#define EPOLL_RUN_TIMEOUT -1	//epoll: timeout
#define EPOLL_SIZE 10000		//epoll: Maximum number of listening clients
#define LISTEN_SIZE 10			//Monitor queue length

#define IF_NAME "eth0"			//network interface name
//#define IF_NAME "enp13s0"			//network interface name

#define CHK(eval) if(eval < 0){perror("eval"); exit(-1);}
#define CHK2(res, eval) if((res = eval) < 0){perror("eval"); exit(-1);}

//ctrl: Master or Slave, this value is set by batman-adv;
//  ctrl=0, SMD is Slave role;
//  ctrl=1, SMD is elected as a Master role;
int ctrl;

//changed: Master or Slave, this value is set by batman-adv;
//  changed=0, Master SMD is not changed, so, do not need to create thread in MAIN again for data synchronization;
//  changed=1, Master SMD is changed, so, need to create thread in MAIN again for data synchronization;
int changed=0;

//int changed_db=0;
int is_me=-1;
int prev_ctrl=-1;  //0:Slave; 1:Master;

char *masterip;
char *self_ip;

int pipe_fd[2];

//select
int i, maxi, maxfd, listenfd, connfd, sockfd;
int nready, client[FD_SETSIZE];	//save clients with data request
fd_set rset, allset;

int start=1;			//set initial state
pthread_t tid_master;	//used in pthread_join, pthread_cancel
pthread_t tid_slave;	//used in pthread_join, pthread_cancel
pthread_t tid_send2slave;

void *send2slave(void *arg);

void perr_exit(const char *s)
{
	perror(s);
	exit(1);
}

//******************************* Replace a specified Line in a Text File

//filename: "/etc/hosts"
//str: "mpe.localhost"
//return the matched line number
int findline(char *filename, char *str)
{
	// open the file for reading
	FILE *file = fopen(filename, "r");

	// make sure the file opened properly
	if(NULL == file)
	{
		fprintf(stderr, "Cannot open file: %s\n", filename);
		return 1;
	}

	// set up the buffer to read the line into. Don't worry too much
	// if some of the lines are longer than 80 characters - the buffer
	// will be made bigger if need be.
	size_t buffer_size = 80;
	char *buffer = malloc(buffer_size * sizeof(char));

	// read each line and print it to the screen
	int line_number = 0, find = 0;
	while(-1 != getline(&buffer, &buffer_size, file))
	{
		line_number++;
		if(NULL != strstr(buffer, str))
		{
			find = 1;
			break;
		}
		//printf("%d: %s", line_number, buffer);
	}

	if(find == 0) line_number = 0;

	fclose(file);
	free(buffer);

	return line_number;
} //end findline()

void replaceline(char *filename, int delete_line, char *newcontent)
{
	FILE *fileptr1, *fileptr2;
	//char filename[40];
	char c;
	int temp = 1;

	fileptr1 = fopen(filename, "r");

/*
	//print the contents of file .
	while ((c = getc(fileptr1)) != EOF)
	{
		printf("%c", c);
	}
//*/

	//take fileptr1 to start point.
	rewind(fileptr1);
	//open tempinterm.txt in write mode
	fileptr2 = fopen("tempinterm.txt", "w");

	while ((c = getc(fileptr1)) != EOF)
	{
		//till the line to be deleted comes,copy the content to other
		if (temp != delete_line) {
			putc(c, fileptr2);
			while ((c = getc(fileptr1)) != '\n') putc(c, fileptr2);
			putc('\n', fileptr2);
			temp++;
		} else {
			while ((c = getc(fileptr1)) != '\n');	//read and skip the line
			//while ((c = getchar()) != '\n') putc(c, fileptr2);
			while(*newcontent != '\0') putc(*newcontent++, fileptr2);
			putc('\n', fileptr2);
			temp++;
		}
	}

	//append a new line at the end of file
	if(delete_line == 0)
	{
		while(*newcontent != '\0') putc(*newcontent++, fileptr2);
		putc('\n', fileptr2);
	}

	fclose(fileptr1);
	fclose(fileptr2);
	remove(filename);
	rename("tempinterm.txt", filename);
} //end replaceline()
//******************************* Replace a specified Line in a Text File


//******************************* set ctrl & master_ip in /etc/hosts
//convert ip address string into uint
uint32_t ip2uint(const char *ip) {
    int a, b, c, d;
    uint32_t addr = 0;

    if (sscanf(ip, "%d.%d.%d.%d", &a, &b, &c, &d) != 4)
       return 0;

    addr = a << 24;
    addr |= b << 16;
    addr |= c << 8;
    addr |= d;
    return addr;
} //end ip2uint()

//read ip address
char * getipaddress(char * interface)
{
	int fd;
	struct ifreq ifr;

	fd = socket(AF_INET, SOCK_DGRAM, 0);
	ifr.ifr_addr.sa_family = AF_INET;	//I want to get an IPv4 IP address
	strncpy(ifr.ifr_name, interface, IFNAMSIZ - 1);	//I want IP address attached to "eth0"
	ioctl(fd, SIOCGIFADDR, &ifr);
	close(fd);
	return inet_ntoa(((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr);
} //end getipaddress()

//read_ctrl, set URL of application system. Access master_ip in browser.
void *read_ctrl(void *arg)
{
	char filename[6] = "hosts";	//set master_ip in /etc/hosts
	int line = -1;				//the matched line number
	char newline[32];			//255.108.162.227   mpe.localhost, added into /etc/hosts

	uint32_t prev_ip = ip2uint("0.0.0.0");
	uint32_t me_ip = ip2uint(getipaddress(IF_NAME));
	uint32_t master_ip;

	size_t buffer_size = 16;
	masterip = malloc(buffer_size * sizeof(char));

	FILE *fp1;
	char c;

	while (1) {		//running for ever
		// gcc -lpthread server-manet.c -o server-manet
		//read the value of ctrl set by batman-adv;
		fp1 = fopen ("ctrl.txt", "r");	//echo 1 >ctrl.txt

		//gcc -lpthread server-manet.c -o server-manet-slave
		//fp1 = fopen ("ctrl_slave.txt", "r");	//echo 0 >ctrl_slave.txt

		c = fgetc(fp1);
		ctrl = c - '0';
		//printf("%d\n", ctrl);
		fclose(fp1);

		//read master_ip of Master SMD;
		fp1 = fopen ("masterip.txt", "r");	//echo 1.1.1.1 >masterip.txt
		//memset(masterip, 0, buffer_size);
		bzero(masterip, buffer_size);

		getline(&masterip, &buffer_size, fp1);
		/* strip trailing newline */
		for (int i = 0; i < strlen(masterip); i++)
		{
			if ( masterip[i] == '\n' || masterip[i] == '\r' )
				masterip[i] = '\0';
		}

		//printf("%s\n", masterip);
		//printf("(%d)-<%s> masterip: %s\n", __LINE__, __FUNCTION__, masterip);
		fclose(fp1);

		master_ip = ip2uint(masterip);

		if (master_ip == me_ip && is_me != -1) {
			prev_ctrl = is_me;
			is_me = 1;
		} else if (master_ip != me_ip && is_me != -1) {
			prev_ctrl = is_me;
			is_me = 0;
		}

		//printf("(%d)-<%s> prev_ip: %u --- master_ip: %u\n", __LINE__, __FUNCTION__, prev_ip, master_ip);

		if (master_ip!=prev_ip) {
			changed=1;
			prev_ip=master_ip;

			//add (or update) "master_ip mpe.localhost" in /etc/hosts;
			line = findline(filename, "mpe.localhost");
			//memset(newline, 0, 32);
			bzero(newline, 32);
		 	sprintf(newline,"%s%s",masterip,"   mpe.localhost");	//produce "10.108.162.227   mpe.localhost"
			replaceline(filename, line, newline);
			//replaceline(filename, line, "10.108.162.227   mpe.localhost");
		} else {
			changed=0;
		}

		//printf("(%d)-<%s> changed: %d --- prev_ctrl: %d\n", __LINE__, __FUNCTION__, changed, prev_ctrl);

		if (changed) {
			if (prev_ctrl == 0) pthread_cancel(tid_slave);		//kill slave thread
			if (prev_ctrl == 1) {
				pthread_cancel(tid_master);		//kill master thread
				pthread_cancel(tid_send2slave);		//kill send2slave thread
			}
		}

		sleep(1);
	}	//end while

	//free(masterip);
} //end read_ctrl()
//******************************* set ctrl & master_ip in /etc/hosts

//communication between master & slave
//arg is sock, arg is NULL
void *slave(void *arg)
{
	int sock;

	struct tcp_info info;
	int leng = sizeof(info);

	struct sockaddr_in seraddr;
	seraddr.sin_family = PF_INET;
	seraddr.sin_port = htons(MASTER_PORT);
	seraddr.sin_addr.s_addr = inet_addr(masterip);	//Master ip address, added in /etc/hosts
	//seraddr.sin_addr.s_addr = inet_addr(SERVER_HOST);

	CHK2(sock, socket(PF_INET, SOCK_STREAM, 0));
	CHK(connect(sock, (struct sockaddr *)&seraddr, sizeof(seraddr)) < 0);

	int len;
	char buf[BUF_SIZE];

	while (1)		//communication between master & slave
	{
		//printf("(%d)-<%s> slave changed: %d\n", __LINE__, __FUNCTION__, changed);

/*
		if (changed) {
			CHK(close(sock));
 			break;
		}
//*/
		//before reading and writing, first to judge whether the "socket connection" is normal
		getsockopt(sock, IPPROTO_TCP, TCP_INFO, &info, (socklen_t *) &leng);
		if ((info.tcpi_state == TCP_ESTABLISHED)) {		//socket connected 
			bzero(buf, BUF_SIZE);
			//printf("(%d)-<%s> will Recv data\n", __LINE__, __FUNCTION__);
			CHK2(len, recv(sock, buf, BUF_SIZE, MSG_NOSIGNAL));	//read * from master
			printf("(%d)-<%s> Recv data from %s: [ %s ]\n", __LINE__, __FUNCTION__, masterip, buf);
		} else {		// socket disconnected 
			CHK(close(sock));
			//return NULL;
			seraddr.sin_addr.s_addr = inet_addr(masterip);	//Master ip address
			CHK2(sock, socket(PF_INET, SOCK_STREAM, 0));
			CHK(connect(sock, (struct sockaddr *)&seraddr, sizeof(seraddr)) < 0);
			printf("(%d)-<%s> reconnect Master: [ %s ]\n", __LINE__, __FUNCTION__, masterip);
			sleep(1);
			continue;
		}

		//update correlative tables in database;
		//todo
	} //end while
} //end slave()

//communication between master & slave
//arg is sock, arg is NULL
void *master(void *arg)
{
	int listenfd;		//监听socket
	struct sockaddr_in addr, peer;
	addr.sin_family = PF_INET;
	addr.sin_port = htons(MASTER_PORT);
	addr.sin_addr.s_addr = inet_addr(self_ip);		// Master IP ADDRESS
	//addr.sin_addr.s_addr = inet_addr(SERVER_HOST);
	socklen_t socklen;
	socklen = sizeof(struct sockaddr_in);

	int i;
	socklen_t cliaddr_len;
	struct sockaddr_in cliaddr;
	char str[INET_ADDRSTRLEN];

	struct tcp_info info;
	int leng = sizeof(info);

	//send MSG to Slave
	//pthread_t tid_send2slave;
	//int rt = pthread_create(&tid_send2slave, NULL, send2slave, (void *)&client);
	int rt = pthread_create(&tid_send2slave, NULL, send2slave, NULL);
	if (-1 == rt) {
		printf("server: thread creation error\n");
		return NULL;
	}

	CHK2(listenfd, socket(PF_INET, SOCK_STREAM, 0));

	//to avoid: Error: Address already in use  
	int on = 1;
	if ((setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on))) < 0) {
		perror("server: setsockopt failed");
		exit(EXIT_FAILURE);
	}

	CHK(bind(listenfd, (struct sockaddr *)&addr, sizeof(addr)));

	//printf("listen\n");
	CHK(listen(listenfd, LISTEN_SIZE));
	//printf("(%d)-<%s> Master: waiting Slave to connect\n", __LINE__, __FUNCTION__);

	maxfd = listenfd;
	maxi = -1;
	for (i = 0; i < FD_SETSIZE; i++)
		client[i] = -1;
	FD_ZERO(&allset);
	FD_SET(listenfd, &allset);	//add listenfd to allset

	//select, add all connect fd to allset
	while (1) {
		//sleep(1);

		//clear invalid fd
		for (int i = 0; i <= maxi; i++) {
			if ((sockfd = client[i]) < 0 || listenfd == client[i] )
				continue;

			//send data to other smartphones (other slaves), [ void *slave(void *arg) ]
			//before reading and writing, first to judge whether the "socket connection" is normal
			getsockopt(sockfd, IPPROTO_TCP, TCP_INFO, &info, (socklen_t *) &leng);
			if ((info.tcpi_state == TCP_ESTABLISHED)) {		// socket connected 
				;
			} else {		// socket disconnected 
				//printf("(%d)-<%s> Slave closed\n", __LINE__, __FUNCTION__);
				CHK(close(sockfd));
				FD_CLR(sockfd, &allset);
				client[i] = -1;
			}
		}
/*
		//printf("(%d)-<%s> changed: %d\n", __LINE__, __FUNCTION__, changed);
		if (changed) {
			for (int i = 0; i <= maxi; i++) {
				if ((sockfd = client[i]) < 0 || listenfd == client[i] )
					continue;

				CHK(close(sockfd));
				FD_CLR(sockfd, &allset);
				client[i] = -1;
			}

			CHK(close(listenfd));
			break;
		}
//*/

		rset = allset;
		nready = select(maxfd + 1, &rset, NULL, NULL, NULL);
		if (nready < 0)
			perr_exit("select error");

		if (FD_ISSET(listenfd, &rset)) {
			cliaddr_len = sizeof(cliaddr);
			CHK2(connfd, accept(listenfd, (struct sockaddr *)&cliaddr, &cliaddr_len));

			printf("(%d)-<%s> connected by [ %s at PORT %d ]\n", __LINE__, __FUNCTION__, inet_ntop(AF_INET, &cliaddr.sin_addr, str, sizeof(str)), ntohs(cliaddr.sin_port));

			for (i = 0; i < FD_SETSIZE; i++)
				if (client[i] < 0) {
					client[i] = connfd;
					break;
				}
			if (i == FD_SETSIZE) {
				fputs("too many clients\n", stderr);
				exit(1);
			}

			FD_SET(connfd, &allset);	//add connfd to allset
			if (connfd > maxfd)
				maxfd = connfd;
			if (i > maxi)
				maxi = i;

			if (--nready == 0)
				continue;
		} //end if
	} //end while
} //end master()

//send MSG to Slave
//the goal is to synchronize process data between master & slave, send data to other smartphones
void *send2slave(void *arg)
{
	//int client = *((int *)arg);
	//sleep(5);
	char buf[BUF_SIZE];
	int res, len;
	char clientmsg[CLIENTMSG_SIZE];

	struct tcp_info info;
	int leng = sizeof(info);

	//int pipe_fd[2], epfd;
	int epfd;

	int epoll_events_count;
	static struct epoll_event ev, events[1];
	ev.events = EPOLLIN | EPOLLET;
	//CHK(pipe(pipe_fd));
	CHK2(epfd, epoll_create(EPOLL_SIZE));
	ev.data.fd = pipe_fd[0];	// reader
	CHK(epoll_ctl(epfd, EPOLL_CTL_ADD, pipe_fd[0], &ev));

	//Using epoll to simulate process execution，Database update event，Event driven database synchronization
	while (1) {		//communication between master & slave

		//printf("(%d)-<%s> changed: %d\n", __LINE__, __FUNCTION__, changed);

		//if (changed) break;

		//event_driven, read pipe_fd[0] from handle_client(void *arg) which read * from client-manet.c 
		//epoll_wait(int epfd, struct epoll_event * events, int maxevents, int timeout)
		//参数events用来从内核得到事件的集合，maxevents告之内核这个events有多大，
		//这个 maxevents的值不能大于创建epoll_create()时的size，
		//参数timeout是超时时间 , 毫秒，0会立即返回，-1将不确定，也有说法说是永久阻塞
		if((epoll_events_count = epoll_wait(epfd, events, 1, EPOLL_RUN_TIMEOUT)) < 0){
			//perror("evall");
			//printf("EINTR [%d] [%d]\n", EINTR, epoll_events_count);
			//exit(-1);
			sleep(1);
			continue;
		}

		//epoll
		for (int i = 0; i < epoll_events_count; i++) {

			if (events[i].data.fd == pipe_fd[0])	//reading end of pipe, accept MSG from client-manet.c
			{
				bzero(&clientmsg, CLIENTMSG_SIZE);
				CHK2(res, read(pipe_fd[0], clientmsg, CLIENTMSG_SIZE));
				//printf("(%d)-<%s> clientmsg: %s\n", __LINE__, __FUNCTION__, clientmsg);

/*
				//send data to other smartphones (other slaves), [ void *slave(void *arg) ]
				//before reading and writing, first to judge whether the "socket connection" is normal
				getsockopt(client, IPPROTO_TCP, TCP_INFO, &info, (socklen_t *) &leng);
				if ((info.tcpi_state == TCP_ESTABLISHED)) {		// socket connected 
					CHK(send(client, "synchronous data", strlen("synchronous data"), MSG_NOSIGNAL));
				} else {		// socket disconnected 
					printf("(%d)-<%s> SIGURG: socket disconnected FCD\n", __LINE__, __FUNCTION__);
					CHK(close(client));
					return NULL;
				}
*/

				//select, send "synchronous data" to all Slaves
				//while (1) {
					//printf("(%d)-<%s> enter select\n", __LINE__, __FUNCTION__);
					//rset = allset;
					//nready = select(maxfd + 1, &rset, NULL, NULL, NULL);
					//printf("(%d)-<%s> select nready: %d\n", __LINE__, __FUNCTION__, nready);

					//if (nready < 0)
					//	perr_exit("select error");

					//if (!FD_ISSET(listenfd, &rset)) {
						for (int i = 0; i <= maxi; i++) {
							if ((sockfd = client[i]) < 0 || listenfd == client[i] )
								continue;
                    
/*							if (FD_ISSET(sockfd, &rset)) {
								if ((n = Read(sockfd, buf, MAXLINE)) == 0) {
									Close(sockfd);
									FD_CLR(sockfd, &allset);
									client[i] = -1;
								} else {
									for (i = 0; i <= maxi; i++)
										Write(client[i], buf, n);
								}
*/
							//if (FD_ISSET(sockfd, &rset)) {

								//send data to other smartphones (other slaves), [ void *slave(void *arg) ]
								//before reading and writing, first to judge whether the "socket connection" is normal
								getsockopt(sockfd, IPPROTO_TCP, TCP_INFO, &info, (socklen_t *) &leng);
								if ((info.tcpi_state == TCP_ESTABLISHED)) {		// socket connected 
									//for (int i = 0; i <= maxi; i++)
									CHK(send(client[i], "synchronous data", strlen("synchronous data"), MSG_NOSIGNAL));
									//printf("(%d)-<%s> send MSG to Slave\n", __LINE__, __FUNCTION__);
								}// else {		// socket disconnected 
									//printf("(%d)-<%s> client closed\n", __LINE__, __FUNCTION__);
									//CHK(close(sockfd));
									//FD_CLR(sockfd, &allset);
									//client[i] = -1;
								//}

								//if (--nready == 0)
								//	break;
							//} // end if
						} // end for
					//} // end if
				//} //end while
			} // end if
		} // end for
	}
} //end send2slave()

//to simulate process execution，Database update event，Event driven database synchronization
//handle_client on SMD that waiting data sent by client-manet.c
//client-manet send MESSAGE to handle_client
//arg refer to pipefd
void *handle_client(void *arg)
{
	//socket to listen
	int listener;		//监听socket
	struct sockaddr_in addr, peer;
	addr.sin_family = PF_INET;
	addr.sin_port = htons(CLIENT_PORT);
	addr.sin_addr.s_addr = inet_addr(self_ip);
	socklen_t socklen;
	socklen = sizeof(struct sockaddr_in);
	int len;
	int client;

	struct tcp_info info;
	int leng = sizeof(info);

	CHK2(listener, socket(PF_INET, SOCK_STREAM, 0));

	// to avoid: Error: Address already in use
	int on = 1;
	if ((setsockopt(listener, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on))) < 0) {
		perror("client: setsockopt failed\n");
		exit(EXIT_FAILURE);
	}

	CHK(bind(listener, (struct sockaddr *)&addr, sizeof(addr)));

	//printf("listen\n");
	CHK(listen(listener, LISTEN_SIZE));
	//printf("(%d)-<%s> client: listening\n", __LINE__, __FUNCTION__);
	//socket to listen

	int pipe_write = *((int *)arg);
	char clientmsg[CLIENTMSG_SIZE];

	while (1) {
		CHK2(client, accept(listener, (struct sockaddr *)&peer, &socklen));
		//printf("(%d)-<%s> client: accepted\n", __LINE__, __FUNCTION__);

		while (1) {
			//------------------read * from client-manet

			//before reading and writing, first to judge whether the "socket connection" is normal
			getsockopt(client, IPPROTO_TCP, TCP_INFO, &info, (socklen_t *) &leng);
			if ((info.tcpi_state == TCP_ESTABLISHED)) {		//socket connected 
				bzero(clientmsg, CLIENTMSG_SIZE);			//receive MESSAGE from client-manet.c
				CHK2(len, recv(client, clientmsg, CLIENTMSG_SIZE, MSG_NOSIGNAL));
				if (len > 0) {
					//printf("(%d)-<%s> write pipe_write: %s\n", __LINE__, __FUNCTION__, clientmsg);
					CHK(write(pipe_write, clientmsg, strlen(clientmsg)));	//send MSG to send2slave(void)
				}
			} else {		// socket disconnected 
				printf("(%d)-<%s> client-manet disconnected\n", __LINE__, __FUNCTION__);
				CHK(close(client));
				break;
			}
		} //end while
	} //end while

	return NULL; // not reach there
} //end handle_client thread

int main(int argc, char *argv[])
{
	//size_t buffer_size = 16;
	//char self_ip[16];
	//memset(self_ip, 0, 16);

	CHK(pipe(pipe_fd));

	self_ip = malloc(16 * sizeof(char));
	bzero(self_ip, 16);
	sprintf(self_ip,"%s",getipaddress(IF_NAME));

	//------- create thread: read_ctrl
	pthread_t readctrl;
	int rc = pthread_create(&readctrl, NULL, read_ctrl, NULL);
	if (-1 == rc) {
		printf("read_ctrl thread creation error\n");
		return -1;
	}
	//------- create thread: read_ctrl

	//------- create thread: handle_client
	pthread_t writer;
	int rt = pthread_create(&writer, NULL, handle_client, (void *)&pipe_fd[1]);
	if (-1 == rt) {
		printf("(%d)-<%s> thread creation error\n", __LINE__, __FUNCTION__);
		return -1;
	}
	//------- create thread: handle_client

	sleep(3);		//waiting for read_ctrl to read the value of ctrl and master_ip of Master SMD
	//int start=1;		//set initial state

	//pthread_t tid;	//used in pthread_join

	void *tret;		//used in pthread_join

	while (1) {
		//printf("(%d)-<%s> start: %d\t ctrl: %d\t changed: %d\n", __LINE__, __FUNCTION__, start, ctrl, changed);

		if (start==1 && ctrl==1) {
			//printf("(%d)-<%s> start: %d\t ctrl: %d\t changed: %d\n", __LINE__, __FUNCTION__, start, ctrl, changed);
			printf("(%d)-<%s> I am Master\n", __LINE__, __FUNCTION__);
			start=0;
			is_me=1;
			pthread_create(&tid_master, NULL, master, NULL);
			pthread_join(tid_master, &tret);
		} else if (start==1 && ctrl==0) {
			//printf("(%d)-<%s> start: %d\t ctrl: %d\t changed: %d\n", __LINE__, __FUNCTION__, start, ctrl, changed);
			printf("(%d)-<%s> I am Slave\n", __LINE__, __FUNCTION__);
			start=0;
			is_me=0;
			pthread_create(&tid_slave, NULL, slave, NULL);
			pthread_join(tid_slave, &tret);
		} else if (start==0 && changed==1 && ctrl==1) {
			sleep(1);	//waiting to socket address 
			//printf("(%d)-<%s> start: %d\t ctrl: %d\t changed: %d\n", __LINE__, __FUNCTION__, start, ctrl, changed);
			printf("(%d)-<%s> I am Master\n", __LINE__, __FUNCTION__);
			pthread_create(&tid_master, NULL, master, NULL);
			pthread_join(tid_master, &tret);
		} else if (start==0 && changed==1 && ctrl==0) {
			sleep(6);	//waiting master initialize
			//printf("(%d)-<%s> start: %d\t ctrl: %d\t changed: %d\n", __LINE__, __FUNCTION__, start, ctrl, changed);
			printf("(%d)-<%s> I am Slave\n", __LINE__, __FUNCTION__);
			pthread_create(&tid_slave, NULL, slave, NULL);
			pthread_join(tid_slave, &tret);
		} else continue;	//start==0 and changed==0

		//sleep(1);
	} //end while

} //end main()
