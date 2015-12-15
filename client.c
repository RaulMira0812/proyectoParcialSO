/*
    C ECHO client example using sockets
*/
#include <stdio.h> //printf
#include <string.h>    //strlen
#include <sys/socket.h>    //socket
#include <arpa/inet.h> //inet_addr
#include <netdb.h>
 
int main(int argc , char *argv[])
{
    
    int sock;
    int port=2016;
    struct sockaddr_in direccion;
    struct hostent *host;
    int len;

    sock=socket(AF_INET , SOCK_STREAM ,0);
    if (sock <= 0)
    {
        fprintf(stderr,"Error: No se pudo crear el socket\n");
        return -3;
    }

    direccion.sin_family = AF_INET;
    direccion.sin_port = htons( port );
    direccion.sin_addr.s_addr=inet_addr("127.0.0.1");
    /*host = gethostbyname(argv[1]);
    if(!host){
        fprintf(stderr,"Error: Host desconocido");
        return -4;
    }

    memcpy(&direccion.sin_addr.s_addr,host->h_addr_list[0],host->h_length);
*/
    //Connect to remote server
    if (connect(sock , (struct sockaddr *)&direccion , sizeof(direccion)))
    {
        perror("Error. No se puede conectar al host");
        return -5;
    }


    char message[1000] , server_reply[2000];

    puts("Conectado\n");
     
    //keep communicating with server
    while(1)
    {
        //Receive a reply from the server
        if( recv(sock , server_reply , 2000 , 0) < 0)
        {
            puts("recv failed");
            break;
        }
         
        puts(server_reply);

        printf("Ingrese un comando: ");
        scanf("%s" , message);
         
        //Send some data
        if( send(sock , message , strlen(message) , 0) < 0)
        {
            puts("Envio fallido");
            return 1;
        }
         
        
        puts("Bienvenido cliente");
    }
     
    close(sock);
    return 0;    
    
}