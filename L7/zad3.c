#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#define BUFFSIZE 128
#define SERVER_PORT 9000
#define NUMB_OF_FDS 5
#define HOST "localhost"
#define MAX_CLIENTS 20 
typedef struct {
  int sockets[MAX_CLIENTS];
  char nicknames[MAX_CLIENTS][30];
} Clients;

//prototypes of functions to write them under main
int setup_server(short port);
int accept_new_connection(int server_socket);
int handle_new_connections(int* server_socket, Clients* clients, fd_set* readfds);
void handle_messages(int* client, int* server, Clients* clients, fd_set* readfds);
void send_to_user(int socket, char *from_user, char *message);

int main(int argc, char **argv) {
    Clients clients;
    for(int i=0; i<MAX_CLIENTS; i++)
      clients.sockets[i]=0;
    
    memset(clients.nicknames, 0, sizeof(clients.nicknames));
    int server_socket = setup_server(SERVER_PORT);
    int max_socket_yet = 0;    
    fd_set main_fd, cli_fds; 	//two because fds tend to desappear
    FD_ZERO(&cli_fds);
    FD_ZERO(&main_fd);
    FD_SET(server_socket, &main_fd);
    //FD_SET(server_socket, &cli_fd);
    max_socket_yet = server_socket;
    printf("Waiting for connection...\n");
   
    while(1){
        cli_fds = main_fd;
        
        if(select(max_socket_yet+1, &cli_fds, NULL, NULL, NULL) < 0) {
            perror("select error");
            exit(EXIT_FAILURE);
        }
        
        for(int i=0; i <= max_socket_yet; i++) {
            if(FD_ISSET(i, &cli_fds)) {
                if(i == server_socket){
                    //new connection
                    int client = handle_new_connections(&server_socket, &clients, &main_fd);
                    FD_SET(client, &main_fd);
                    if(client > max_socket_yet)
                        max_socket_yet = client;
                        
                }
                else {
                  
                  handle_messages(&i, &server_socket, &clients, &main_fd);
                  //FD_CLR(i, &main_fd);
                }
            }
           
        }
        
    }

    return 0;
}

int setup_server(short port){
    int server_socket;
    struct sockaddr_in server_addr;
    struct hostent* he;

    if((server_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket error");
        exit(EXIT_FAILURE);
    }
    bzero(&server_addr, sizeof(server_addr));
    
    if (!(he = gethostbyname(HOST))) {
    perror("gethostbyname");
    exit(EXIT_FAILURE);
    }
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr = *(struct in_addr*)(he->h_addr_list[0]);
    server_addr.sin_port = htons(port);

    if ((bind(server_socket, (struct sockaddr*) &server_addr, sizeof(server_addr))) < 0) {
        perror("binging error");
        exit(EXIT_FAILURE);
    }

    if((listen(server_socket, 3) < 0)) {
        perror("listening error");
        exit(EXIT_FAILURE);
    }
    printf("Sever binded and listen on port: %d, fd: %d \n", SERVER_PORT, server_socket);
    return server_socket;
}

int accept_new_connection(int server_socket){
    int addr_size = sizeof(struct sockaddr_in);
    int client_socket;
    struct sockaddr_in client_addr;
    if((client_socket = accept(server_socket, (struct sockaddr*) &client_addr, (socklen_t*) &addr_size)) < 0){
        perror("accept error");
        exit(EXIT_FAILURE);
    }
    return client_socket;
}
/* Sends message to all users about the new client connection of given id*/
void user_connected_notification(int id, Clients* clients) {
  char *new_user_nickname = clients->nicknames[id];

  for (int i = 0; i < MAX_CLIENTS; i++) {
    if (i != id) {
      int socket = clients->sockets[i];
      char message[128];
      strncpy(message, "\n\033[1m", sizeof(message));
      strcat(message, new_user_nickname);
      strcat(message, " connected\033[0m\n\n");
      send(socket, message, strlen(message), 0);
    }
  }
}

/* 
Sends message with a list of all connected 
to a user of a given socket.
*/
void show_users(int socket, Clients* clients) {
  int count = 1;
  char message[30];
  char nickname[30];
  char *firstmessage = "\033[1m\nConnected users:\n";\
  send(socket, firstmessage, strlen(firstmessage), 0);
  for (int j = 0; j < MAX_CLIENTS; j++) {
    
    if (clients->sockets[j] != 0) {
      if (clients->sockets[j] != socket) {
        
       // strncpy(nickname, "", sizeof(nickname));
        memset(nickname, 0, sizeof(nickname));
        strcat(nickname, clients->nicknames[j]);
        sprintf(message, "%d", count);
        strcat(message, ". ");
        strcat(message, nickname);
        strcat(message, "\n");
        
        count++;
      } 
      else {
        sprintf(message, "%d", count);
        strcat(message, ". ");
        strcat(message, "You\n");
        count++;
      }
      
      send(socket, message, strlen(message), 0);
    }
  }
  send(socket, "\n\033[0m", strlen("\n\033[0m"), 0);
}
/* Handles new clients, sets nickname*/
int handle_new_connections(int* server_socket, Clients* clients, fd_set* readfds) {

    int client_socket = accept_new_connection(*server_socket);
    char buffer[BUFFSIZE];
    char nick[30];
    //FD_SET(client_socket, readfds);
    
    printf("New connection , socket fd is %d \n", client_socket);
     
    
    // welcome message
    char *message = "\033[38;2;88;254;0m\n*************\n    Witam \n*************\nPodaj nick:\033[38;2;222;43;244m ";
    send(client_socket, message, strlen(message), 0);
    puts("WELCOME MESSAGE HAS BEEN SEND.");
    
    // reading nickname
    int len = read(client_socket, nick, sizeof(nick));
    //printf("%d\n", len);
    if (len > 0) {
      nick[len-2] = '\0';
    }
    for (int i = 0; i < MAX_CLIENTS; i++) {
      if (clients->sockets[i] == 0) {
        clients->sockets[i] = client_socket;
        strncpy(clients->nicknames[i], nick, sizeof(clients->nicknames[i]));
        printf("Adding to list of sockets as %d position\n" , i);
        printf("nickname is : <%s>\n", clients->nicknames[i]);
        
        char greet[50];
        strcpy(greet, "\033[38;2;88;254;0mWelcome \033[38;2;222;43;244m");
        strcat(greet, nick);
        strcat(greet, "\n\033[38;2;88;254;0m");
        send(clients->sockets[i], greet, strlen(greet), 0);

        user_connected_notification(i, clients);
        show_users(clients->sockets[i], clients); 

        break;
      }
    } 
    return client_socket;     
  }

/*
Sends a message to user of given socket.
*/
void send_to_user(int socket, char *from_user, char *message) {
  send(socket, from_user, strlen(from_user), 0);
  send(socket, ": ", strlen(": "), 0);
  send(socket, message, strlen(message), 0);
  send(socket, "\n", strlen("\n"), 0);
}
/*
Return first word in given string.
*/
char *first_word(char *buffer) {
  if (strlen(buffer) > 0) {
    int inputLength = strlen(buffer);
    char *inputCopy = (char*) calloc(inputLength + 1, sizeof(char));
    strncpy(inputCopy, buffer, inputLength);
    char *word, *context;
    word = strtok_r(inputCopy, " ", &context);
    return word;
  } else {
    return "";
  }
}
/*
This function returns a socket for a given nickname
if the nickname has been found, otherwise returns -1.
*/
int find_socket_by_nickname(char *nickname, Clients* clients) {
  for (int i = 0; i < MAX_CLIENTS; i++) {
    if (strcmp(clients->nicknames[i], nickname) == 0) {
      return clients->sockets[i];
    }
  }
  free(nickname);
  return -1;

}
void handle_messages(int* client, int* server, Clients* clients, fd_set* readfds) {
  int len;
  char buffer[BUFFSIZE];
  int counter=0;
  char message[30];
  memset(message,0,sizeof(message));
  memset(message,0,sizeof(buffer));

  if (FD_ISSET(*client, readfds)){
    len = read(*client, buffer, sizeof(buffer));
    if(len>0){
      buffer[len - 2] = '\0';
        
      int target_socket;
      char *nickname = first_word(buffer);
      printf("Pierwsze slowo: <%s>\n", nickname);
      target_socket = find_socket_by_nickname(nickname, clients);
      while(clients->sockets[counter]!=*client) counter++;
      if (target_socket > -1) {
        printf("wiadomosc od <%s> do <%s>: <%s>\n", clients->nicknames[counter], nickname, buffer + strlen(nickname) + 1);
        send_to_user(target_socket, clients->nicknames[counter], buffer + strlen(nickname) + 1);
      } 
      else if ( strcmp("users", buffer) == 0 ) {
        show_users(*client, clients);
      } 
      else {
        send_to_user(*client, "you", "Nie znaleziono użytwkonika");
        printf("no user found\n");
      }
    }
    else{
      while(clients->sockets[counter]!=*client) counter++;
      printf("Host disconnected , fd: %d, nick: %s\n", *client,clients->nicknames[counter]);   
      FD_CLR(*client, readfds); 
      clients->sockets[counter] = 0;
      strcpy(message, clients->nicknames[counter]);
      memset(clients->nicknames[counter], 0, sizeof(clients->nicknames[counter]));
      strcat(message,  " disconnected\n");
      for (int j = 0; j < MAX_CLIENTS; j++) {
        if (j != counter)  
          send(clients->sockets[j], message, sizeof(message), 0);
        }
    } 
  }
}
  
   
    

    


