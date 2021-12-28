#include <stdio.h>	//printf
#include <string.h>	//strlen
#include <sys/socket.h>	//socket
#include <arpa/inet.h>	//inet_addr
#include <unistd.h>
#include <stdlib.h>

////////////////////////////////////////////////////////////////////////
//YOU MIGHT NEED TO CHANGE THE IP 
//////////////////////////////i///////////////////////////////////////
#define SERVER_IP "136.159.5.27"
#define PORT 44111

int main(int argc , char *argv[])
{
	int sock, server_flag = 0, vote_message_flag = 3;
	struct sockaddr_in server;
	char vote_char[10], message[1000] , server_reply[2000];
	int vote;
	
	//Create socket
	sock = socket(AF_INET , SOCK_STREAM , 0);
	if (sock == -1)
	{
		printf("Could not create socket");
	}
	
	server.sin_addr.s_addr = inet_addr(SERVER_IP);
	server.sin_family = AF_INET;
	server.sin_port = htons(PORT);

	//Connect to remote server
	if (connect(sock , (struct sockaddr *)&server , sizeof(server)) < 0)
	{
		perror("connect failed. Error");
		exit(-1);
	}
	
	//keep communicating with server
	while(1)
	{
		printf("CHOOSE UDP SERVICE TRANSLATE BY ENTERING \"SERVER1\" CHOOSE UDP SERVICE CURRENCY CONVERT BY ENTERING \"SERVER2\" OR CHOOSE UDP SERVICE VOTE BY ENTERING \"SERVER3\" ANY TIME.\n");
		printf("Enter message or \"bye\" to exit: ");
		//take in the whole line of the message
		scanf("%[^\n]%*c" , message);

		//If message is bye end program
		if(strcmp(message, "bye") == 0)
		{
			break;
		}

		//If the client request the voting server set the server flag
		if (strcmp(message, "SERVER3") == 0)
		{
			server_flag = 3;
			//Send the message
			if( send(sock , message , strlen(message) , 0) < 0)
               		{
                        	puts("Send failed");
                        	exit(-1);
               		}
		}
		//if the client request the translate or currency convert server set the server flag
		else if (strcmp(message, "SERVER1") == 0 || strcmp(message, "SERVER2") == 0)
		{
			server_flag = 0;
		}
		
		
		//if the translate or currency convert
		if (server_flag !=3)
		{
			//Send the message
			if( send(sock , message , strlen(message) , 0) < 0)
			{
				puts("Send failed");
				exit(-1);
			}
		
			//Receive a reply from the server
			if( recv(sock , server_reply , 2000 , 0) < 0)
			{
				puts("recv failed");
				break;
			}
			
			puts("Server reply :");
			puts(server_reply);
		}
		//when the voting server is requested
		else if (server_flag ==3)
		{
			//if the client has requested the list of candidates or results flag the message flag to give back string
			if (strcmp(message, "MENU") == 0 || strcmp(message, "RESULTS") == 0)
			{
				vote_message_flag = 1;
			}
			//Vote for candidate
			//save the vote in the vote_char and set vote_message_flag to send key and encrypt vote
			else if (strcmp(message,"1") == 0)
			{
				strcpy(vote_char, "1");
				vote_message_flag = 0;
			}
			else if (strcmp(message,"2") == 0)
			{
				strcpy(vote_char, "2");
				vote_message_flag = 0;
			}
			else if (strcmp(message,"3") == 0)
			{
				strcpy(vote_char, "3");
				vote_message_flag = 0;
			}
			else if (strcmp(message,"0") == 0)
			{			
				strcpy(vote_char, "0");
				vote_message_flag = 0;
			}


			//when the candidate or results are requested  forward the message and given back string message
                        if (vote_message_flag == 1)
			{
				if( send(sock , message , strlen(message) , 0) < 0)
				{
					puts("Send failed");
                                	exit(-1);
				}
                        	//Receive a reply from the server
				if( recv(sock , server_reply , 2000 , 0) < 0)
				{
                                	puts("recv failed");
                                	break;
                        	}

				//sometimes the program will read the previous message so I have to read again to get the correct messge
				//this is a dirty fix for the issue
				if(strstr(server_reply, "*") !=NULL)
				{
					if( recv(sock , server_reply , 2000 , 0) < 0)
					{
						puts("recv failed");
						break;
					}
				}

                        	puts("Server reply :");
                        	puts(server_reply);
			}
			//when voting 
			else if (vote_message_flag == 0)
			{
				//load vote with # in front to sent to TCP server to know to request key
				char vote_temp[1000];
				sprintf(vote_temp, "#%s", message);

				//Send vote to request key from TCP server
				if( send(sock , vote_temp , strlen(vote_temp) , 0) < 0)
				{
					puts("Send failed");
                                        exit(-1);
                                }

				//Recieve key from TCP server
                                if( recv(sock , server_reply , 2000 , 0) < 0)
                                {
                                        puts("recv failed");
                                        break;
				}

				//Some times my TCp server will only send back * so I need to recieve again to get the key
				
				//this is a dirty fix for the issue
				if (strcmp(server_reply, "*") ==0)
				{
					if( recv(sock , server_reply , 2000 , 0) < 0)
                                	{
                                        	puts("recv failed");
                                        	break;
                                	}
				}
				//printf("second  read %sEND\n", server_reply);
				
				//Encrpting vote
				//convert key string to int
				int encryption_key = atoi(server_reply);

				//convery vote_char to int
				int vote_num = atoi(vote_char);

				//implement simple encryption function
				int results = vote_num*encryption_key;

				//printf("vote is %d | key is %d | ekey is %d\n", vote_num, encryption_key, results);
				
				//Load encrpyted vote with * so that TCP server knows that the message is the encrypted vote
				char temp[8];
				sprintf(temp, "*%d", results);
				
				//Sending encryped vote
				if (send(sock, temp, strlen(temp), 0) < 0)
				{
                        		perror("failed to send encrypted vote\n");
				}
			}
		}


		bzero(message, sizeof(message));
		bzero(server_reply, sizeof(server_reply));

		printf("------------------------------------------------------------------------\n");
	}
	
	close(sock);
	return 0;
}
