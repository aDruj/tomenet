/* TomeNET world server test code copyright 2002 Richard Smith. */

#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include <errno.h>

#include "world.h"
#include "externs.h"

#define PORT 18360
#define ADDR INADDR_ANY

struct sockaddr_in sa;

void loadservers();

int main(int argc, char *argv[]){
	int ser;
	initrand();
	loadservers();
	ser=createsocket(PORT, ADDR);
	if(ser!=-1){
		world(ser);
		close(ser);
	}
}

int createsocket(int port, unsigned long ip){
	int option=1;
	struct sockaddr_in s_in;
	int check;
	int ss;

	/* Create a socket */
	ss=socket(AF_INET, SOCK_STREAM, 0);
	if(ss < 0) return(-1);

	/* Set option on socket not to wait on shutdown */
	setsockopt(ss, SOL_SOCKET, SO_REUSEADDR, (void*)&option, sizeof(int));

	s_in.sin_family=AF_INET;
	s_in.sin_addr.s_addr=ip;
	s_in.sin_port=htons(port);

	check=bind(ss, (struct sockaddr *)&s_in, sizeof(struct sockaddr_in));
	if(check != -1){
		check=listen(ss, 5);
		if(check != -1) return(ss);
	}
	close(ss);
	return(-1);
}

void loadservers(){
	FILE *fp;
	char buf[80];
	int i=0;
	fp=fopen("servers", "r");
	if(fp==(FILE*)NULL) return;
	do{
		fscanf(fp, "%s%s\n", &slist[i].name, &slist[i].pass);
		i++;
	} while(!feof(fp) && i<MAX_SERVERS);
	snum=i;
	fclose(fp);
}
