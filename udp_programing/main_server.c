#include<stdio.h>
#include<string.h>	//strlen
#include<sys/socket.h>
#include<arpa/inet.h>	//inet_addr
#include<unistd.h>	//write
#include<stdlib.h>
#include<time.h>

#define MAX_BUFFER_SIZE 5000

/////////////////////////////////////////////////////////////
//YOU MIGHT NEED TO CHANGE THE IP ADDRESS
////////////////////////////////////////////////////////////
#define SERVER_IP "136.159.5.27"

//Ports of UDP servers
#define TRANSLATE_PORT 44100
#define CONVERT_PORT 44103
#define VOTE_PORT 44106
#define CLIENT_PORT 44111

int main(int argc, char *argv[]){

	int socket_desc, client_sock;
	struct sockaddr_in server, client;
	char client_message[MAX_BUFFER_SIZE];
	char buf[MAX_BUFFER_SIZE];
	char serverMsg[MAX_BUFFER_SIZE];
	int readBytes, len, server_flag;

	///////////////////////////////////////////////////////////////////////////
	//TCP CONNECTION TO TCP CLIENT
	///////////////////////////////////////////////////////////////////////////
	
	//Create socket
	socket_desc = socket(AF_INET, SOCK_STREAM, 0);
	if (socket_desc == -1)
	{
		printf("Could not create socket\n");
	}
	puts("Socket created\n");
	
	//Prepare the sockaddr_in structure
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = INADDR_ANY;
	server.sin_port = htons(CLIENT_PORT);
	
	//Bind
	int bindStatus = bind(socket_desc, (struct sockaddr *)&server, sizeof(server));
	if( bindStatus == -1){
		//print the error message
		perror("Binding failed");
		return 1;
	}
	printf("Binding done.\n");
	
	//Listen
	listen(socket_desc , 3);
	
	//Accept and incoming connection
	printf("Waiting for clients...\n");
	
	//accept connection from an incoming client
	client_sock = accept(socket_desc, NULL, NULL);
	if (client_sock < 0){
		perror("Connection failed");
		return 1;
	}
	printf("Connection accepted\n");
	
	
	///////////////////////////////////////////////////////////////////////////
	//UDP CLIENT FOR TRNASLATE SERVER
	///////////////////////////////////////////////////////////////////////////
	
	struct sockaddr_in si_server;
	struct sockaddr *server_UDP;
	int s;
	
	//Make socket to UDP translate server
	if ((s=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP))==-1)
	{
		printf("Could not set up UDP socket!\n");
		exit(-1);
	}
	
	memset((char *) &si_server, 0, sizeof(si_server));
	si_server.sin_family = AF_INET;
	si_server.sin_port = htons(TRANSLATE_PORT);
	server_UDP = (struct sockaddr *) &si_server;
	
	if (inet_pton(AF_INET, SERVER_IP, &si_server.sin_addr)==0)
	{
		printf("inet_pton() TRANSLATE UDP failed\n");
		exit(-1);
	}
	
	printf("UDP TRANSLATE SERVER IS SET!!\n");


	///////////////////////////////////////////////////////////////////////////
	//UDP CLIENT FOR CONVERT SERVER
	///////////////////////////////////////////////////////////////////////////


	struct sockaddr_in si_server1;
	struct sockaddr *server_UDP1;
	int s1;
	
	//Make socket to UDP currency convert server
	if ((s1=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP))==-1)
	{
		printf("Could not set up UDP socket!\n");
		exit(-1);
	}
	
	memset((char *) &si_server1, 0, sizeof(si_server1));
	si_server1.sin_family = AF_INET;
	si_server1.sin_port = htons(CONVERT_PORT);
	server_UDP1 = (struct sockaddr *) &si_server1;
	
	if (inet_pton(AF_INET, SERVER_IP, &si_server1.sin_addr)==0)
	{
		printf("inet_pton() CONVERT UDP failed\n");
		exit(-1);
	}
	
	printf("UDP CURRENCY CONVERT SERVER IS SET!!\n");

	///////////////////////////////////////////////////////////////////////////
	//UDP CLIENT FOR VOTING SERVER
	///////////////////////////////////////////////////////////////////////////
	
	struct sockaddr_in si_server2;
	struct sockaddr *server_UDP2;
	int s2, vote_message_flag, vote_encrypt_flag = 0;
	char key[3] = "KEY";

	//Make socket to UDP voting server
        if ((s2=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP))==-1)
        {
                printf("Could not set up VOTING UDP socket!\n");
                exit(-1);
        }

        memset((char *) &si_server2, 0, sizeof(si_server2));
        si_server2.sin_family = AF_INET;
        si_server2.sin_port = htons(VOTE_PORT);
        server_UDP2 = (struct sockaddr *) &si_server2;

        if (inet_pton(AF_INET, SERVER_IP, &si_server2.sin_addr)==0)
        {
                printf("inet_pton() VOTE UDP failed\n");
                exit(-1);
        }

        printf("UDP VOTE SERVER IS SET!!\n");

	///////////////////////////////////////////////////////////////////////////


	//Receive a message from client
	int recvStatus;
	while(1){

		//Recieved message from TCP client
		recvStatus = recv(client_sock, client_message, 5000, 0);
		if (recvStatus==-1)
		{
			printf("Error in receiving!");
			break;
		}

		if (strcmp(client_message,"bye") ==0)
		{
			break;
		}

		//Load message from client to buffer
		strcpy(buf, client_message);

		//check the time to do something reasonable if no response is received from one of the UDP-based micro-services within a certain time limit
		int hours, minutes;
		time_t now;
		time(&now);
		struct tm *local = localtime(&now);
		hours = local->tm_hour;
		minutes = local->tm_min;


		//Check if the client request a server and set flag 
		//SERVER1 is the translate server and its flag is 1
		//SERVER2 is the currency convert server and its flag is 2
		//SERVER3 is the voting server and its flag is 3
		if (strcmp(client_message, "SERVER1") == 0)
		{
			server_flag=1;
		}
		else if (strcmp(client_message, "SERVER2") == 0)
		{
			server_flag=2;
		}
		else if (strcmp(client_message, "SERVER3") == 0)
		{
			server_flag=3;
		}
		else if (strcmp(client_message, "bye") == 0)
		{
			break;
		}
		
		//check the server requested and message from client
		printf("Client message: %s and Server flag is %d\n", client_message, server_flag);

		//send message given by client to the correct server
		//Translate UDP server
		if (server_flag == 1)
		{
			//Sending to UDP translate server
			if (sendto(s, buf, strlen(buf), 0, server_UDP, sizeof(si_server)) == -1)
			{
				printf("sendto translate udp server failed\n");
				exit(-1);
			}
			
			//Receviving from UDP translating server
			if ((readBytes=recvfrom(s, buf, MAX_BUFFER_SIZE, 0, server_UDP, &len))==-1)
			{
				printf("Read error with translate server!\n");
				exit(-1);
			}
		}
		//Currency convert UDP server
		else if (server_flag == 2)
		{
                     //Sending to UDP currentcy converter server
                        if (sendto(s1, buf, strlen(buf), 0, server_UDP1, sizeof(si_server1)) == -1)
                        {
                                printf("sendto currency convert udp server failed\n");
                                exit(-1);
                        }

                        //Receviving from UDP currency  server
                        if ((readBytes=recvfrom(s1, buf, MAX_BUFFER_SIZE, 0, server_UDP1, &len))==-1)
                        {
                                printf("Read error with currency convert server!\n");
                                exit(-1);
                        }
		}
		//Voting UDP server
		else if (server_flag == 3)
		{
			printf("in vote server else if buf is %s\n", buf);
			
			if (strcmp(buf, "MENU") == 0 || strcmp(buf, "RESULTS") == 0)
			{
				vote_message_flag = 1;
			}
			//if the message is the encrypted vote
			else if (strstr(buf, "*") != NULL)
			{
				vote_message_flag = 2;
			}
			//VOTING for candidate
			else if (strstr(buf, "#") != NULL)
			{
				//when message given is a vote for the canidate ids
				vote_message_flag = 0;
			}
			else
			{
				vote_message_flag = 3;
			}

			printf("vote mesage flag is %d\n", vote_message_flag);

			if (vote_message_flag == 1)
			{
				//Sending to UDP voting  server
				if (sendto(s2, buf, strlen(buf), 0, server_UDP2, sizeof(si_server2)) == -1)
                        	{
                                	printf("sendto voting udp server failed\n");
                                	exit(-1);
                        	}
				
				//Receviving from UDP currency  server
				if ((readBytes=recvfrom(s2, buf, MAX_BUFFER_SIZE, 0, server_UDP2, &len))==-1)
                        	{
                                	printf("Read error with voting server!\n");
                                	exit(-1);
                        	}
			}
			else if (vote_message_flag == 2)
			{
				//forward encrpted vote
				char temp[MAX_BUFFER_SIZE];
				strcpy(temp, buf+1);
				if (sendto(s2, temp, strlen(temp), 0, server_UDP2, sizeof(si_server2)) == -1)
				{
					perror("failed to send vote\n");
				}
			}
			else if(vote_message_flag == 0)
			{
				char key_given[8];
				if (sendto(s2, key, 3, 0, server_UDP2, sizeof(si_server2)) == -1)
				{
					printf("KEY REQUEST FAILED\n");
					exit(-1);
				}
				
				if ((readBytes=recvfrom(s2, key_given, sizeof(key_given), 0, server_UDP2, &len))==-1)
				{
					printf("Read error!\n");
					exit(-1);
				}

				strcpy(buf, key_given);
				printf("key given by vote server is %s and buf is %s\n", key_given,buf);
			}


		}

		
		
		//check the time to do something reasonable if no response is received from one of the UDP-based micro-services within a certain time limit
                int hours1, minutes1;
                time_t now1;
                time(&now1);
                struct tm *local1 = localtime(&now);
                hours1 = local1->tm_hour;
                minutes1 = local1->tm_min;

		if(minutes1 == minutes+1)
		{
			printf("TOO LONG OF A RESPONSE!!!\n");
		}



		buf[readBytes] = '\0';
		
		strcat(serverMsg, buf);
		write(client_sock , serverMsg , strlen(serverMsg));
		
		bzero(serverMsg, sizeof(serverMsg));
		bzero(client_message, sizeof(client_message));
		bzero(buf, sizeof(buf));
		printf("---------------------------------------------------------------\n");
	}
	
	close(client_sock);
	close(socket_desc);
	close(s);
	close(s1);
	close(s2);

	
	return 0;
}
