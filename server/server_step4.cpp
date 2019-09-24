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
#define BUFFER_SIZE 32768
#define PKT_PORT 10240
#define SLOW_START 1
#define CONGESTION_ADVOIDANCE 2

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
int cwnd = 1;
int rwnd = 10240;
int threshold = 65535;

int temp = 0;
Package send_pkt, recv_pkt;

int Pkt_loss(){
	if(rand()%100 == 0) 
		return 1;
	else
		return 0;
}

int main(int argc, char *argv[]){
	if (argc != 2) {
		fprintf(stderr,"Usage : %s <datafile>\n",argv[0]);
		exit(errno);
	}
	
	port = atoi(argv[1]);
	char IP_name[100];
    struct hostent *p;
    gethostname(IP_name, sizeof(IP_name));
    p = gethostbyname(IP_name);
    char *temp; 
    temp = inet_ntoa(*(struct in_addr*)(p->h_addr_list[0]));
	strcpy(IP, temp);
	srand(time(NULL));
	
	/* Program start */
	printf("====Parameter====\n");
	printf("The RTT delay = %d ms\n", RTT);
	printf("The threshold = %d bytes\n", threshold);
	printf("The MSS = %d bytes\n", MSS);
	printf("The buffer size = %d bytes\n", BUFFER_SIZE);
	printf("Server's IP is %s\n", IP);
	printf("Server is listening on port %d\n", port);
	puts("===============");
	
	static struct sockaddr_in server, client;
	int client_len = sizeof(struct sockaddr_in);
	int sockfd;
	
	/* Create a UDP socket */
	if((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1){
		printf("socket fail\n");
		exit(errno);
	}
	
	/* Initialize address. */
	memset(&server, 0, sizeof(server));
	server.sin_family = AF_INET;
	server.sin_port = htons(port);
	server.sin_addr.s_addr = htonl(INADDR_ANY);
	
	if(bind(sockfd, (struct sockaddr *) &server, sizeof(server)) == -1){
		printf("bind fail\n");
		exit(errno);
	}
	while(1){	
		/* Wait for a connection */
		printf("Listening for client...\n");
		printf("=====Start the three-way handshake=====\n");
		
		/* Wait for SYN */
		if(recvfrom(sockfd, buffer, BUFFER_SIZE, 0, (struct sockaddr *)&client, (socklen_t *)&client_len) == -1){
			printf("recvfrom fail\n");
			exit(errno);
		}
		printf("Received a packet(SYN) from %s : %hu\n", inet_ntoa(client.sin_addr), ntohs(client.sin_port));
		
		memcpy(&recv_pkt, buffer, sizeof(recv_pkt));
		printf("\tReceive a packet (seq_num = %d, ack_num = %d)\n", recv_pkt.seq_num, recv_pkt.ack_num);
		
		int pid = fork();
		if(pid == 0){
			port = PKT_PORT;
			
			/* Create a UDP socket */
			if((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1){
				printf("socket fail\n");
				exit(errno);
			}
			
			/* Initialize address. */
			memset(&server, 0, sizeof(server));
			server.sin_family = AF_INET;
			server.sin_port = htons(port);
			server.sin_addr.s_addr = htonl(INADDR_ANY);
			
			bind(sockfd, (struct sockaddr *) &server, sizeof(server));
			
			if(recv_pkt.syn){
				/* send SYN/ACK */
				send_pkt.dest_port = recv_pkt.src_port;
				send_pkt.src_port = port;
				send_pkt.syn = 1;
				send_pkt.ack = 1;
				send_pkt.ack_num = recv_pkt.seq_num + 1;
				send_pkt.seq_num = (short)(rand() % 10000 + 1);
				
				memset(&pkt_data, 0, sizeof(pkt_data));
				memcpy(pkt_data, &send_pkt, 24);
				if(sendto(sockfd, pkt_data, 24, 0, (struct sockaddr *)&client, client_len) == -1){
					printf("sendto fail\n");
					exit(errno);
				}
				printf("Send a packet(SYN/ACK) to %s : %hu\n", inet_ntoa(client.sin_addr), recv_pkt.src_port);
			}
			else{
				printf("send_pkt SYN bit error\n");
				exit(errno);
			}
			
			/* Wait for ACK */
			if(recvfrom(sockfd, buffer, BUFFER_SIZE, 0, (struct sockaddr *)&client, (socklen_t *)&client_len) == -1){
				printf("recvfrom fail\n");
				exit(errno);
			}
			printf("Received a packet(ACK) from %s : %hu\n", inet_ntoa(client.sin_addr), ntohs(client.sin_port));
			
			memcpy(&recv_pkt, buffer, sizeof(recv_pkt));
			printf("\tReceive a packet (seq_num = %d, ack_num = %d)\n", recv_pkt.seq_num, recv_pkt.ack_num);
			
			printf("=====Complete the three-way handshake=====\n");
			
			/* Start Sending Files */
			FILE* fp;
			int recv_p = 0;
			int print_congestion = 1;
			
			while(1){
				/* Get File Name */
				if(recvfrom(sockfd, buffer, BUFFER_SIZE, 0, (struct sockaddr *)&client, (socklen_t *)&client_len) == -1){
					printf("recvfrom fail\n");
					exit(errno);
				}
				
				if(!strcmp(buffer, "[client] Received All")) break;
				
				char file_buf[1024];
				strcpy(file_buf, buffer);
				//printf("file_buf = %s\n", file_buf);
				
				fp = fopen(file_buf, "r");
				if(!fp){
					printf("File open fail\n");
					exit(errno);
				}
				
				fseek(fp, 0, SEEK_END);
				int file_size = ftell(fp);
				fseek(fp, 0, SEEK_SET);
				
				printf("Start to send %s to %s : %hu, the file size is %d bytes.\n", file_buf, inet_ntoa(client.sin_addr), recv_pkt.src_port, file_size);
				printf("*****Slow start*****\n");
				
				int state = SLOW_START;
				int tmp_cwnd = 1;
				int file_num = 1;
				int read_byte = 1;
				int data_size;
				int print_msg = 1, tmp = 0, count = 0;
				
				while(1){
					data_size = fread(send_pkt.data, sizeof(char), cwnd, fp);
					if(data_size == 0) break;
				
					/* Send File */
					send_pkt.dest_port = recv_pkt.src_port;
					send_pkt.src_port = port;
					send_pkt.syn = 1;
					send_pkt.ack = 1;
					send_pkt.ack_num = recv_pkt.seq_num + 1;
					send_pkt.seq_num = file_num;
					
					if(cwnd < 1024) printf("cwnd = %d, rwnd = %d, threshold = %d\n", cwnd, rwnd, threshold);
					else{
						if(pow(2, 10+tmp) > threshold && print_congestion){ 
							printf("******Congestion advoidance*****\n");
							state = CONGESTION_ADVOIDANCE;
							tmp_cwnd = tmp_cwnd+MSS*(MSS/cwnd);
							if(tmp_cwnd < 1024) cwnd = tmp_cwnd;
							print_congestion = 0;
						}
						
						if(state == SLOW_START) tmp_cwnd = (int)pow(2, 10+tmp);
						else if(state == CONGESTION_ADVOIDANCE) tmp_cwnd = tmp_cwnd+MSS*(MSS/cwnd);
						
						if(print_msg) printf("cwnd = %d, rwnd = %d, threshold = %d\n", tmp_cwnd, rwnd, threshold);
						print_msg = 0;
						count++;
						
						if(count >= pow(2, tmp)){
							tmp++;
							print_msg = 1;
							count = 0;
						}
					}
					
					if(!Pkt_loss()){
						memcpy(pkt_data, &send_pkt, data_size + 24);
						if(sendto(sockfd, pkt_data, data_size + 24, 0, (struct sockaddr *)&client, client_len) == -1){
							printf("sendto fail\n");
							exit(errno);
						}
						if(cwnd < 1024) printf("\tSend a packet at : %d bytes\n", read_byte);
						else printf("\tSend a packet at : %d bytes\n", read_byte);
					}
					else{
						if(cwnd < 1024) printf("\t*** Send a packet at : %d bytes\n", read_byte);
						else printf("\t*** Send a packet at : %d bytes\n", read_byte);
					}
					
					bzero(&pkt_data, sizeof(pkt_data));
					
					if(recv_p%2){
						/* Wait for Client */
						if(recvfrom(sockfd, buffer, BUFFER_SIZE, 0, (struct sockaddr *)&client, (socklen_t *)&client_len) == -1){
							printf("recvfrom fail\n");
							exit(errno);
						}
						
						memcpy(&recv_pkt, buffer, sizeof(recv_pkt));
						
						/* Check recv_pkt info */
						if(recv_pkt.ack && recv_pkt.ack_num == read_byte + data_size){
							printf("\tReceive a packet (seq_num = %d, ack_num = %d)\n", recv_pkt.seq_num, recv_pkt.ack_num); 
							file_num += cwnd;
							if(cwnd < 1024) cwnd = cwnd * 2;
							read_byte += data_size;
						}else{
							printf("\t< Packet loss : ACK number = %d >\n", recv_pkt.ack_num);
							fseek(fp, recv_pkt.ack_num, SEEK_SET);
							
							printf("*****Slow start*****\n");
							state = SLOW_START;
							cwnd = 1; 
							print_msg = 1; 
							tmp = 0;
							count = 0;
							print_congestion = 1;
						}
					}
					else{
						file_num += cwnd;
						if(cwnd < 1024) cwnd = cwnd * 2;
						read_byte += data_size;
						recv_pkt.seq_num++;	
					}
					recv_p++;
				}	
				strcpy(pkt_data, "[server] File transfer complete");
				if(sendto(sockfd, pkt_data, sizeof(pkt_data), 0, (struct sockaddr *)&client, client_len) == -1){
					printf("sendto fail\n");
					exit(errno);
				}
				puts(""); cwnd = 1;
			}
			fclose(fp);
			return 0;
		}
	}
	close(sockfd);
	
	return 0;
}
