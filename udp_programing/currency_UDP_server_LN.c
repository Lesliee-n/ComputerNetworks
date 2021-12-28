//THIS UDP BASED SERVER IS A MICRO SERVICE THAT CONVERTS Canadian Dollar to US Dollar, Euro, British Pound, and Bitcoin currencies

//Include needed files
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>

//Constants
#define MAX_BUFFER_SIZE 512
#define PORT 44103	     

/////////////////////////////////////////////////////////////////////////////
//function for converting CAD to different  values
////////////////////////////////////////////////////////////////////////////

//convert CAD to USD based on 10/10/2021 conversion rates
float convert_CAD_to_USD (int amount)
{
    return amount * 0.79;
}

//convert CAD to EUR based on 10/10/2021 conversion rates
float convert_CAD_to_EUR (int amount)
{
    return amount * 0.69;
}

//convert CAD to GBP based on 10/10/2021 conversion rates
float convert_CAD_to_GBP (int amount)
{
    return amount * 0.59;
}

//convert CAD to BTC based on 10/10/2021 conversion rates
float convert_CAD_to_BTC (int amount)
{
    return amount * 0.000016;
}


int main()
  {
    struct sockaddr_in translate_server, translate_client;
    struct sockaddr *server, *client;
    int socket1, i, len=sizeof(translate_server);
    char messagein[MAX_BUFFER_SIZE];
    char messageout[MAX_BUFFER_SIZE];
    int readBytes;

    //creating connection
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
    
    fprintf(stderr, "Welcome to the CAD to USD, EUR, BGP, or BTC UDP server! Please enter your message as ($10 CAD US)\n");
    printf("server now listening on UDP port %d...\n", PORT);
    printf("-----------------------------------------------------------------------------------------\n");

    // server listening to client
    while(1)
    {
	    // clear out message buffers to be safe
	    bzero(messagein, MAX_BUFFER_SIZE);
	    bzero(messageout, MAX_BUFFER_SIZE);
	   
	    //Variables to store the strings of $amount CAD toCurrentcy
	    char amount[8];
	    char toCurrentcy[8];
	    
	    //token for parshing
	    char *token;
	    //counter to parshe 
	    int counter =0;

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
	    
	    //Print information recieved
	    printf("SERVER RECIEVED \"%s\" FROM IP %s PORT %d\n", messagein, inet_ntoa(translate_client.sin_addr), ntohs(translate_client.sin_port));
	    
	    char *temp_message = messagein; //[strlen(messagein)];
	    
	    while ((token = strtok_r(temp_message, " ", &temp_message)))
	    {
		    if (counter == 0)
		    {
			    strcpy(amount, &token[1]);
		    }
		    else if (counter == 2)
		    {
			    strcpy(toCurrentcy, token);
		    }
		    
		    counter++;
	    }
	    
	    //convery string of amount to int for calculations
	    int num = atoi(amount);
	    

	    //chcek the toCurrency to determne where to convery the amount
	    if (strcmp(toCurrentcy, "USD") == 0)
	    {
		    sprintf(messageout," %f", convert_CAD_to_USD(num));
	    }
	    else if (strcmp(toCurrentcy, "EUR") == 0)
	    {
		    sprintf(messageout," %f", convert_CAD_to_EUR(num));
	    }
	    else if (strcmp(toCurrentcy, "GBP") == 0 )
	    {
		    sprintf(messageout," %f", convert_CAD_to_GBP(num));
	    }
	    else if (strcmp(toCurrentcy, "BTC") == 0 )
	    {
		    sprintf(messageout," %f", convert_CAD_to_GBP(num));
	    }
	    else
	    {
		    sprintf(messageout," %f", 0.0);
	    }
	    
	    printf("MESSAGE TO BE SENT TO CLIENT: %f\n", messageout);
	    
	    //message back to the clients
	    sendto(socket1, messageout, strlen(messageout), 0, client, len);
	    printf("-----------------------------------------------------------------------------------------\n");
    }

    close(socket1);
    return 0;
  }
