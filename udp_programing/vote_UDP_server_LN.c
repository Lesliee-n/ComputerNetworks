//THIS UDP BASED SERVER IS A MICRO SERVICE THAT TRANSLATES THE WORDS "hello, egg, banana, bread, apple" TO FRENCH

//Include needed files
#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

//Constants
#define MAX_BUFFER_SIZE 512
#define PORT 44106


int main()
  {
    struct sockaddr_in translate_server, translate_client;
    struct sockaddr *server, *client;
    int socket1, i, len=sizeof(translate_server);
    char messagein[MAX_BUFFER_SIZE], messageout[MAX_BUFFER_SIZE];
    
    //Message to sent when request to show candidates
    char candidate_message[] = "Your candiates and their IDs are: Person0=0 Person1=1 Person2=2 Person3=3"; 
    //Candidates
    char candidates[4][8] = {"Person0", "Person1", "Person2", "Person3"};
    
    // Person0's ID=0
    // Person1's ID=1
    // Person2's ID=2
    // Person3's ID=3
    
    //Pre-assigned number of votes for candidates
    int votes[] = {4, 3, 2 ,1};
    
    //varible for encryption
    int key_array[1] = {0};

    int readBytes, has_vote_flag=0, key;
    
    //Call for random number
    srand(time(NULL));

    //varibles for the time for the bonus
    int hours, minutes, time_flag, hour_control=10;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////

    //Creating connection
    memset((char *) &translate_server, 0, sizeof(translate_server));
    translate_server.sin_family = AF_INET;
    translate_server.sin_port = htons(PORT);
    translate_server.sin_addr.s_addr = htonl(INADDR_ANY);
    
    //Making socket
    if ((socket1=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP))==-1) 
    {
        perror("SOCKET FAILED\n");
        return 1;
    }
    
    server = (struct sockaddr *) &translate_server;
    client = (struct sockaddr *) &translate_client;

    //Bind socket
    if (bind(socket1, server, sizeof(translate_server))==-1)
    {
	    perror("BIND FAILED\n");
	    return 1;
    }
    
    fprintf(stderr, "Welcome to voting server. You can vote for Person1, Person2, Person3, or Person4 \n");
    printf("server now listening on UDP port %d...\n", PORT);
    printf("-----------------------------------------------------------------------------------------\n");

    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    
    // server listening to client
    while(1)
    {

	    //check the time
	    time_t now;
	    time(&now);
	    struct tm *local = localtime(&now);
	    hours = local->tm_hour;
	    minutes = local->tm_min;

	    //after the hour for voting
	    if (hours>hour_control)
	    {
		    time_flag =1;
	    }
	    else
	    {
		    time_flag=0;
	    }

	    printf("time flag is %d\n", time_flag);

            // clear out message buffers to be safe
	    bzero(messagein, MAX_BUFFER_SIZE);
	    bzero(messageout, MAX_BUFFER_SIZE);

	    // Recive from client, if anything
	    if ((readBytes=recvfrom(socket1, messagein, MAX_BUFFER_SIZE, 0, client, &len)) < 0)
	    {
		    printf("READ FAILED\n");
		    return -1;
	    }
	    else
	    {
		    printf("SERVER RECIEVED  %d BYTES\n", readBytes);
	    }
	    
	    //Information recieved
	    printf("SERVER RECIEVED \"%s\" FROM IP %s PORT %d\n", messagein, inet_ntoa(translate_client.sin_addr), ntohs(translate_client.sin_port));
	    
	    //Check for client requests
	    //Client ask for candidates with "MENU"
	    if (strcmp(messagein, "MENU") == 0 && time_flag==1)
	    {
		    sprintf(messageout, "%s", candidate_message);
		    printf("MESSAGE TO BE SENT TO CLIENT: %s\n", messageout);
		    sendto(socket1, messageout, strlen(messageout), 0, client, len);
	    }
	    else if (strcmp(messagein, "KEY") == 0 && time_flag==1)
	    {
		    //make key for TCP server
		    //flag the has_vote_flag to indocate the client has voted and can request for results
		    has_vote_flag = 1;

		    //set inital keyto the value of zero
		    key = 0;
		    
		    while (key == 0)
		    {
			    //assign a random number from 0 to 9 to key
			    key = rand() % 9;
		    }
		    
		    //Store key in key_array to recall
		    key_array[0] = key;
		    
		    printf("KEY IS %d\n", key);
		    //load key to be sent to TCP server
		    sprintf(messageout, "%d", key);
		    sendto(socket1, messageout, strlen(messageout), 0, client, len);
	    }
	    //When client asks for results
	    else if (strcmp(messagein, "RESULTS") ==0 && time_flag==1)
	    {
		    //chcek if the client has voted
		    if(has_vote_flag == 1)
		    {
			    sprintf(messageout, "VOTE RESULTS: Person0=%d Person1=%d Person2=%d, Person3=%d", votes[0], votes[1], votes[2], votes[3]);
		    }
		    else
		    {
			    sprintf(messageout, "YOU HAVE NOT VOTED YET%s\n","");
		    }
		    
		    printf("MESSAGE TO BE SENT TO CLIENT: %s\n", messageout);
		    sendto(socket1, messageout, strlen(messageout), 0, client, len);
	    }
	    else if (time_flag==0)
	    {
		    sprintf(messageout, "%s", "YOU CANNOT VOTE AND CANNOT SEE RESULTS BECAUSE OF THE TIME!");
		    sendto(socket1, messageout, strlen(messageout), 0, client, len);
	    }
	    else
	    {
		    //when not request for candiates or a key then client is voting and giving server encrypted vote
		    
		    //Recieve encrypted key and convert it from a string to and int
		    int encrypted_key = atoi(messagein);
		    printf("     passed atoi with key=%d and eKey=%d\n", key, encrypted_key);
		    
		    //Decrypt with key to get candidated id
		    int candidate_id = encrypted_key / key;
		    printf("     passed %d / %d = candidate id is %d\n", encrypted_key, key, candidate_id);

		    //update votes
		    votes[candidate_id]++;
		    printf("VOTES HAVE BEEN UPDATED\n");
	    }
	    
	    //printing votes
	    for(int v=0; v<4; v++)
	    {
		    printf("Person%d = %d votes | ", v, votes[v]);
	    }

	    bzero(messagein, sizeof(messagein));
	    bzero(messageout, sizeof(messageout));
	    printf("---------------------------------------------------------------------------\n");
    }

    close(socket1);
    return 0;
  }
