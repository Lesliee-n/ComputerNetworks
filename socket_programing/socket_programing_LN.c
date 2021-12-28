//importing standard packages
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//importing needed packages
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <signal.h>
#include <netinet/in.h>
#include <netdb.h>

#define MESSAGE_LENGTH 1024

//Source of this function is https://stackoverflow.com/questions/47116974/remove-a-substring-from-a-string-in-c
//This function is used to remove substrings from a string and is ment to remove blocked words
char *str_remove(char *str, const char *sub) {
    size_t len = strlen(sub);
    if (len > 0) {
        char *p = str;
        while ((p = strstr(p, sub)) != NULL) {
            memmove(p, p + len, strlen(p + len) + 1);
        }
    }
    return str;
}


int main(){

    //Setting up inital needed variables
    int listening_sock, accept_sock, tcp_sock, web_sock;
    struct sockaddr_in server, client;
    char message_in[MESSAGE_LENGTH];
    char *error_page = "<html><title>Simple URL Censorship Error Page for CPSC 441 Assignment 1</title><body><h1>NO!!</h1><p>Sorry, but the Web page that you were trying to accesshas a <b>URL</b> that is inappropriate.</p><p>The page has been blocked to avoid insulting your intelligence.</p><p>Web Censorship Proxy</p></body></html>";
    int lstn_port = 44111;

    //Creating the listening socket for proxy
    listening_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (listening_sock < 0) {
        perror("SOCKET FAILED\n");
        exit(-1);
    } else{
	    printf("SOCKET CREATED\n");
    }

    //Initializing the Address
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = htonl(INADDR_ANY);
    server.sin_port = htons(lstn_port);
    
    
    //Binding the socket to address and port
    int bind_status;
    bind_status = bind(listening_sock, (struct sockaddr *) &server, sizeof(struct sockaddr_in));
    if (bind_status < 0) {
        perror("BIND FAILED\n");
        exit(-1);
    } else {
        printf("BIND SUCCESS\n");
    }
    
    //Listening on binded port number
    int listen_status;
    listen_status = listen(listening_sock, 10);
    if (listen_status < 0) {
        perror("LISTEN FAILED\n");
        exit(-1);
    } else{
	    printf("LISTEN SUCCESS\n");
    }

    //Creating message strings
    bzero(message_in, MESSAGE_LENGTH);

    //List of blocked words
    char blocked_words[MESSAGE_LENGTH] = "";

    int program_run = 1;
    while(program_run == 1){
	    
	    //Asking for input in terminal for dynamtic blocking 
	    char str1[25];  // declaration of char array  
	    char str2[25];  // declaration of char array  
	    int value; // declaration of integer variable  
	    printf("Enter 1 to add to blocked list or 2 to remove a word or 3 to continue blocking current set of words or 4 to end program:  ");
	    //reading word and taking it in as a varible 
	    scanf("%s",str1);
	    //Adding word to blocked string
	    if(strcmp(str1,"1")==0){
		    printf("Enter word you want to block:  ");  
		    scanf("%s",str2);
		    //adding word to current string of blocked words by concatenation
		    strcat(blocked_words, ",");
		    strcat(blocked_words, str2);
	    } 
	    //Removing word from blocking string
	    else if(strcmp(str1,"2")==0){
		    printf("Enter word you want to remove:  ");
		    scanf("%s",str2);
		    //removing woord from current string of blocked words
		    str_remove(blocked_words, str2);
	    } 
	    else if(strcmp(str1, "4")==0){
		    program_run = 0;
		    exit(-1);
	    }
	    //Continue to block the current set of words
	    else{
		    //do nothing
	    }


	    //Accepting from listining socket
	    accept_sock = accept(listening_sock, NULL, NULL);
	    if (accept_sock <0){
		    perror("ACCEPT FAILED\n");
	    }
	    
	    //Receving HTTP message from client
	    int client_recv_status;
	    client_recv_status = recv(accept_sock, message_in, sizeof(message_in),0);
	    if (client_recv_status <0){
		    perror("CLIENT RECV FAILED\n");
	    } 
	    
	    
	    //strings for storing parshed informatio
	    char host[1024];
	    char url[1024];
	    char path[1024];
	    
	    //Parsing host, url, and path from HTTP message 
	    char * get_request = strtok(message_in, "\r\n");
	    //Get the url
	    if (sscanf(get_request, "GET http://%s", url) == 1){
		    printf("URL = %s\n", url);
	    }
	    
	    //Extracting host and path
	    for (int i = 0; i < strlen(url); i++) {
		    if (url[i] == '/') {
			    strncpy(host, url, i); //copy out the hostname
			    host[i] = '\0';
			    strcpy(path, strchr(url, '/')); //copy out the path 
			    break;
		    }
	    }
	
	    //printing host
	    printf( "HOST = %s\n", host );
	    //printing path
	    printf( "PATH = %s\n", path );
	    
	    struct sockaddr_in server_addr;
	    struct hostent *host_server;
	    
	    //get server information
	    host_server = gethostbyname(host);
	    if (host_server == NULL){
		    perror("GET HOST BY NAME FAILED\n");
		    exit(-1);
	    }
	    
	    tcp_sock = socket(AF_INET, SOCK_STREAM, 0);
	    if (tcp_sock <0){
		    perror("TCP SOCK CREATION FAILED\n");
		    exit(-1);
	    }
	    
	    //Initialize socket struct
	    bzero((char *) &server_addr, sizeof(server_addr));
	    server_addr.sin_family = AF_INET;
	    bcopy((char *) host_server->h_addr, (char *) &server_addr.sin_addr.s_addr, host_server->h_length);
	    server_addr.sin_port = htons(80);
	    
	    //Connecting to web server
	    int connect_status;
	    connect_status = connect(tcp_sock, (struct sockaddr *) &server_addr,sizeof(server_addr));
	    if (connect_status < 0) {
		    perror("CONNECTION TO WEB SOCKET FAILED\n");
		    exit(-1);
	    }
	    
	    //Getting head request
	    char head_request[100000];
	    snprintf(head_request, sizeof(head_request), "GET %s HTTP/1.1\r\nHost: %s\r\nContent-Type: text/html\r\n\r\n",path,host);
	    
	    //Sending head request to web server
	    int send_status;
	    send_status = send(tcp_sock, head_request, strlen(head_request), 0);
	    if (send_status <0){
		    perror("SEND HTTP REQUEST FAILED\n");
		    exit(-1);
	    }
	    
	    //Receving the head request from the web server
	    int recv_head_status;
	    char server_response[100000];
	    recv_head_status = recv(tcp_sock, server_response, sizeof(server_response),0);
	    if (recv_head_status <0){
		    printf("RECV HEAD FAILED\n");
	    }
	   
	    
	    // printf("THE SERVER RECEIVED THE HEAD RESPONSE OF %s\n", server_response);


	    //sending client web page if the contents are blocked or not
	    int error_flag = 0;
	    
	    //copying string of blocked words into string
	    char str[strlen(blocked_words)];
	    strcpy(str, blocked_words);
	    char * pch;
	    printf ("CURRENT SET OF BLOCKED WORDS ARE: \"%s\"\n",str);
	    pch = strtok (str,",");
	    while (pch != NULL){
		    //if a block word is in the body of the web page then increment the error flag
		    if(strstr(server_response, pch) != NULL){
			    error_flag++;
		    }
		    //if a blocked word is in the url of the web page then increment the error flag
		    if(strstr(url, pch) != NULL){
			    error_flag++;
		    }
		    pch = strtok (NULL, ",");
	    }
	    

	    //Check error flag to see which page to send back to client 
	    if(error_flag == 0){
		    send_status = send(accept_sock, server_response, strlen(server_response),0);
		    if (send_status <0){
			    perror("SEND BACK HEAD REQUEST FAILED\n");
			    exit(-1);
		    }
	    } else {
		    send_status = send(accept_sock, error_page, strlen(error_page), 0);
	    }
	    
	    close(accept_sock);

	    printf("\n-----------------------------------------------------------------------------------------\n");
    
    }

    return 0;
}
