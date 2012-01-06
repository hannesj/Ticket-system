/* Ticket Client */

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>


#define HOST_NAME "192.16.126.84"
#define SERVER_PORT 39998

int main(int argc, char ** argv)
{

    int sock, bytes_recieved;
    char send_data[30],recv_data[30];
    struct hostent *host;
    struct sockaddr_in server_addr;
    unsigned short ticket, num;
    host = gethostbyname(HOST_NAME);

    if (argc != 2)
    {
        exit (-1);
    }

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        perror("Socket");
        exit(1);
    }

    server_addr.sin_family = AF_INET;     
    server_addr.sin_port = htons(SERVER_PORT);   
    server_addr.sin_addr = *((struct in_addr *)host->h_addr);
    bzero(&(server_addr.sin_zero),8); 

    if (connect(sock, (struct sockaddr *)&server_addr,
                sizeof(struct sockaddr)) == -1) 
    {
        perror("Connect");
        exit(1);
    }

    /* Send the mac address to the server */
    send (sock, argv[1], strlen (argv[1]), 0);

    /* Wait for a ticket */
    bytes_recieved = recv (sock, recv_data, 5, 0);

    num = *((unsigned short *)(recv_data));
    ticket = ntohs (num);

    printf ("Ticket => %d\r\n", ticket);

    close(sock);
    
    exit (ticket);

    return 0;
}
