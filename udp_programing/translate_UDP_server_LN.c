//THIS UDP BASED SERVER IS A MICRO SERVICE THAT TRANSLATES THE WORDS "hello, egg, banana, bread, apple" TO FRENCH

//Include needed files
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

//Constants
#define MAX_BUFFER_SIZE 512
#define PORT 44100

#define DEBUG 1

int main()
  {
    struct sockaddr_in translate_server, translate_client;
    struct sockaddr *server, *client;
    int socket1, i, len=sizeof(translate_server);
    char messagein[MAX_BUFFER_SIZE];
    char messageout[MAX_BUFFER_SIZE];
    char translated_words[5][8] = {"bonjour", "oeuf", "banane", "pain", "pomme"};
    int readBytes;

    //setting up connection
    memset((char *) &translate_server, 0, sizeof(translate_server));
    translate_server.sin_family = AF_INET;
    translate_server.sin_port = htons(PORT);
    translate_server.sin_addr.s_addr = htonl(INADDR_ANY);
    
    //making socket
    if ((socket1=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP))==-1) 
    {
        perror("SOCKET FAILED\n");
        return 1;
    }
    
    server = (struct sockaddr *) &translate_server;
    client = (struct sockaddr *) &translate_client;

    if (bind(socket1, server, sizeof(translate_server))==-1)
    {
	    perror("BIND FAILED\n");
	    return 1;
    }
    
    fprintf(stderr, "Welcome to the English to french translate UDP server! Please enter one word of either (hello, egg, banana, bread, apple)\n");
    printf("server now listening on UDP port %d...\n", PORT);
    printf("-----------------------------------------------------------------------------------------\n");

    // server listening to client
    while(1)
    {
	    // clear out message buffers to be safe
	    bzero(messagein, MAX_BUFFER_SIZE);
	    bzero(messageout, MAX_BUFFER_SIZE);

	    // Recive from client
	    if ((readBytes=recvfrom(socket1, messagein, MAX_BUFFER_SIZE, 0, client, &len)) < 0)
	    {
          	printf("READ FAILED\n");
          	return -1;
	    }
	    else
	    {
		    printf("SERVER RECIEVED  %d BYTES\n", readBytes);
	    }
	   
	    //checking what was recieved
	    printf("SERVER RECIEVED \"%s\" FROM IP %s PORT %d\n", messagein, inet_ntoa(translate_client.sin_addr), ntohs(translate_client.sin_port));
	    
	    //based on word recieved load correct message/translated word to send back
	    if (strcmp(messagein, "hello") == 0)
	    {
		    sprintf(messageout," %s", translated_words[0]);
	    }
	    else if (strcmp(messagein, "egg") == 0)
	    {
	      sprintf(messageout," %s", translated_words[1]);
	    }
	    else if (strcmp(messagein, "banana") == 0)
	    {
		    sprintf(messageout," %s", translated_words[2]);
	    }
	    else if (strcmp(messagein, "bread") == 0)
	    {
		    sprintf(messageout," %s", translated_words[3]);
	    }
	    else if (strcmp(messagein, "apple") == 0)
	    {
		    sprintf(messageout," %s", translated_words[4]);
	    }
	    else
	    {
		    sprintf(messageout," %s", "NOT A VAID WORD");
	    }
	    
	    printf("MESSAGE TO BE SENT TO CLIENT: %s\n", messageout);
	    
	    //message back to the client
	    sendto(socket1, messageout, strlen(messageout), 0, client, len);
	    printf("-----------------------------------------------------------------------------------------\n");
    }

    close(socket1);
    return 0;
  }
