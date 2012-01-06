/* Ticket Server */

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

#define SERVER_PORT 39998
#define MAX_DEVICES 10

struct device_registry
{
    char mac_addr[25];
    unsigned short ticket;
}devices[MAX_DEVICES];

unsigned short num_devices = 0;

unsigned short cur_ticket = 1;

int main()
{
    int sock, connected, bytes_recieved , true = 1;  
    char send_data [20] , recv_data[20];       
    unsigned short num;

    struct sockaddr_in server_addr,client_addr;    
    int sin_size, i;
        
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("Socket");
        exit(1);
    }

    server_addr.sin_family = AF_INET;         
    server_addr.sin_port = htons(SERVER_PORT);     
    server_addr.sin_addr.s_addr = INADDR_ANY; 
    bzero(&(server_addr.sin_zero),8); 

    if (bind(sock, (struct sockaddr *)&server_addr, sizeof(struct sockaddr)) == -1) 
    {
        perror("Unable to bind");
        exit(1);
    }

    if (listen(sock, 5) == -1)
    {
        perror("Listen");
        exit(1);
    }
        
    printf("\nTicket Server Waiting for devices\r\n");

    while(1)
    {  

        sin_size = sizeof(struct sockaddr_in);

        connected = accept(sock, (struct sockaddr *)&client_addr,&sin_size);

        printf(">>(%s , %d)<<", inet_ntoa(client_addr.sin_addr),ntohs(client_addr.sin_port));

        /* Read data from the client */
        bytes_recieved = recv (connected, recv_data, 20, 0);

        if (bytes_recieved < 12 || bytes_recieved > 18)
        {
            close (connected);
            continue;
        }

        /* Search for a device */

        for (i = 0; i < num_devices; i++)
        {
            if (0x00 == strcmp (devices[i].mac_addr, recv_data))
            {
                break;
            }
        }

        if (i < num_devices)
        {
            num = htons(devices[i].ticket);
        }
        else
        {
           num = htons(cur_ticket);
           strcpy(devices[i].mac_addr, recv_data);
           devices[i].ticket = cur_ticket;
           cur_ticket++;
           num_devices++;
        }
        memcpy((void *)send_data, &num, sizeof(num));

        /* Send the data */
        send (connected, send_data, sizeof(num), 0);

        /* close the socket */
        close (connected);

    }       

    close(sock);
    return 0;
}

 
