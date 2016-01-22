/*
* released under GPLv3
*/
// gcc client-manet.c -o client-manet
// indent -npro -kr -i8 -ts8 -sob -l280 -ss -ncs -cp1 *

//client-manet send MESSAGE to daemon WAIT

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define CLIENT_PORT 22222	//SERVER listening port, IN server-manet.c

#define MSG "running process"

#define CHK(eval) if(eval < 0){perror("eval"); exit(-1);}
#define CHK2(res, eval) if((res = eval) < 0){perror("eval"); exit(-1);}

//to simulate process execution，Database update event，Event driven database synchronization
//send MSG to [ void *handle_client(void *arg) ] in server-manet.c
int main(int argc, char *argv[])
{
	//***************** GET masterip
	size_t buffer_size = 16;
	char *masterip = malloc(buffer_size * sizeof(char));

	//read master_ip of Master SMD;
	FILE *fp1;
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
	fclose(fp1);
	//***************** GET masterip

	int sock, pid;

	struct sockaddr_in seraddr;
	seraddr.sin_family = PF_INET;
	seraddr.sin_port = htons(CLIENT_PORT);
	seraddr.sin_addr.s_addr = inet_addr(masterip);

	CHK2(sock, socket(PF_INET, SOCK_STREAM, 0));
	//printf("socket\n");

	CHK(connect(sock, (struct sockaddr *)&seraddr, sizeof(seraddr)) < 0);
	//printf("connect\n");

/*
[client-manet.c] send to
[(MASTER) handle_client(void *arg) IN server-manet.c] send to
[(MASTER) m_thread(void *arg) IN server-manet.c] send to
[(SLAVE) slave(void *arg) IN server-manet.c]
*/
/*
	while (1) {
		double df= 15987632.1231131*15987632.1231131*15987632.1231131*15987632.1231131;
	}
//*/

	printf("(%d)-<%s> I am Client, access workflow system\n", __LINE__, __FUNCTION__);

	for (int i = 0; i < 1; i++) {
		CHK(send(sock, MSG, strlen(MSG), 0));
		printf("(%d)-<%s> Send [ %s ] to %s\n", __LINE__, __FUNCTION__, MSG, masterip);
		sleep(1);
	}
	close(sock);
	return 0;
}
