#include <iostream>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>
#include <cmath>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>
#include <net/if.h>
#define RTT 150
#define MSS 1024
#define threshold 65535
#define BUFFER_SIZE 32768
#define CLI_PORT 10260

using namespace std;

typedef struct{
	short src_port;
	short dest_port;
	int seq_num;
	int ack_num;
	short head_len:4, 
		  not_use:6, 
		  urg:1, 
		  ack:1, 
		  psh:1, 
		  rst:1, 
		  syn:1, 
		  fin:1;
	short rwnd;
	short checknum;
	short urg_pointer;
	int options;
	char data[1024];
}Package;

int port;
char IP[100];
char buffer[BUFFER_SIZE];
char pkt_data[1024];

Package send_pkt, recv_pkt;

int main(int argc, char *argv[]){
	if (argc < 5) {
		fprintf(stderr,"Usage : %s <datafile>\n",argv[0]);
		exit(errno);
	}
	
	strcpy(IP, argv[1]);
	port = atoi(argv[2]);
	srand(time(NULL));
		
	static struct sockaddr_in server, client;
	int server_len = sizeof(struct sockaddr_in);
	int sockfd;
	
	/* Create server structure */
	if (inet_pton(AF_INET, argv[1], &server.sin_addr) == 0){
		printf("inet_pton : server empty\n");
		exit(errno);
	}
	server.sin_family = AF_INET;
	server.sin_port = htons(port);
	
	/* Create a UDP socket */
	if((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1){
		printf("socket fail\n");
		exit(errno);
	}
	
	/* Initialize address. */
	memset(&client, 0, sizeof(client));
	client.sin_family = AF_INET;
	client.sin_port = htons(CLI_PORT);
	client.sin_addr.s_addr = htonl(INADDR_ANY);
	
	if(bind(sockfd, (struct sockaddr *) &client, sizeof(client)) == -1){
		printf("bind fail\n");
		exit(errno);
	}
	
	/* Wait for a connection */
	printf("=====Start the three-way handshake=====\n");
	
	/* send SYN */
	memset(&send_pkt, 0, sizeof(send_pkt));
	send_pkt.src_port = CLI_PORT;
	send_pkt.dest_port = port;
	send_pkt.seq_num = (short)(rand() % 10000 + 1);
	//printf("send_pkt.seq_num = %d\n\n", send_pkt.seq_num);
	send_pkt.syn = 1;
	
	memset(&pkt_data, 0, sizeof(pkt_data));
	memcpy(pkt_data, &send_pkt, 24);
	if(sendto(sockfd, pkt_data, 24, 0, (struct sockaddr *)&server, server_len) == -1){
		printf("sendto fail\n");
		exit(errno);
	}
	printf("Send a packet(SYN) to %s : %hu\n", argv[1], port);
	
	/* Wait for ACK */
	if(recvfrom(sockfd, buffer, BUFFER_SIZE, 0, (struct sockaddr *)&server, (socklen_t *)&server_len) == -1){
		printf("recvfrom fail\n");
		exit(errno);
	}
	printf("Received a packet(SYN/ACK) from %s : %hu\n", inet_ntoa(server.sin_addr), ntohs(server.sin_port));
	
	memcpy(&recv_pkt, buffer, sizeof(recv_pkt));
	printf("\tReceive a packet (seq_num = %d, ack_num = %d)\n", recv_pkt.seq_num, recv_pkt.ack_num);
	
	if(recv_pkt.syn && recv_pkt.ack && (recv_pkt.ack_num == send_pkt.seq_num + 1)){
		/* send ACK */
		send_pkt.src_port = CLI_PORT;
		send_pkt.dest_port = port;
		send_pkt.syn = 0;
		send_pkt.ack = 1;
		send_pkt.ack_num = recv_pkt.seq_num + 1;
		send_pkt.seq_num = recv_pkt.ack_num;
						
		memset(&pkt_data, 0, sizeof(pkt_data));						
		memcpy(pkt_data, &send_pkt, 24);
		if(sendto(sockfd, pkt_data, 24, 0, (struct sockaddr *)&server, server_len) == -1){
			printf("sendto fail\n");
			exit(errno);
		}
		printf("Send a packet(ACK) to %s : %hu\n", argv[1], port);
	}
	else{
		printf("Not wanted SYN/ACK\n");
		exit(errno);
	}
	printf("=====Complete the three-way handshake=====\n");
	
	/* Send File Name */
	char file_buf[BUFFER_SIZE];
	FILE* new_fp;
	
	for(int i=4;i<argc;i++){	
		strcpy(file_buf, argv[i]);
		if(sendto(sockfd, file_buf, BUFFER_SIZE, 0, (struct sockaddr *)&server, server_len) == -1){
			printf("sendto fail\n");
			exit(errno);
		}
		
		new_fp = fopen(file_buf, "w");
		
		/* Wait For File */
		int data_size = recvfrom(sockfd, buffer, BUFFER_SIZE, 0, (struct sockaddr *)&server, (socklen_t *)&server_len);
		if(data_size == 0){
			printf("recvfrom fail\n");
			exit(errno);
		}
		printf("Received %s from %s : %hu\n", file_buf, inet_ntoa(server.sin_addr), ntohs(server.sin_port));
		
		while(1){
			memcpy(&recv_pkt, buffer, sizeof(recv_pkt));
			printf("\tReceive a packet (seq_num = %d, ack_num = %d)\n", recv_pkt.seq_num, recv_pkt.ack_num);
			
		    fwrite(recv_pkt.data, sizeof(char), data_size - 24, new_fp);

			/* Feedback */
			send_pkt.src_port = CLI_PORT;
			send_pkt.dest_port = port;
			send_pkt.syn = 1;
			send_pkt.ack = 1;
			send_pkt.ack_num = recv_pkt.seq_num + data_size - 24;
			send_pkt.seq_num = recv_pkt.ack_num;
			
			memset(&pkt_data, 0, sizeof(pkt_data));						
			memcpy(pkt_data, &send_pkt, 24);
			if(sendto(sockfd, pkt_data, 24, 0, (struct sockaddr *)&server, server_len) == -1){
				printf("sendto fail\n");
				exit(errno);
			}
			
			/* Get Anthoer Package */
			data_size = recvfrom(sockfd, buffer, BUFFER_SIZE, 0, (struct sockaddr *)&server, (socklen_t *)&server_len);
			if(data_size == 0){
				printf("recvfrom fail\n");
				exit(errno);
			}
			
			if(!strcmp(buffer, "[server] File transfer complete")) break;
		}
		printf("\n***%s Transfer Success!***\n", file_buf);
	}	
	strcpy(pkt_data, "[client] Received All");
	if(sendto(sockfd, pkt_data, sizeof(pkt_data), 0, (struct sockaddr *)&server, server_len) == -1){
		printf("sendto fail\n");
		exit(errno);
	}
	
	fclose(new_fp);
	close(sockfd);
	
	return 0;
}
