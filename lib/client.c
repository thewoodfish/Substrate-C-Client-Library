#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>
#include "lib.h"
 
int main(void)
{
    int sockfd = 0, n = 0;
    int bytescopied;
    struct sockaddr_in serv_addr;
    char url[] = "google.com";

    memset(&serv_addr, 0x00, sizeof(struct sockaddr_in));

    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0 ) {
        printf("\n Error : Could not create socket \n");
        return 1;
    }
    
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(80);
    serv_addr.sin_addr.s_addr = inet_addr(IPToURL(url));
    
    if (connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
        printf("\n Error : Connect Failed \n");
        return 1;
    }

    bytescopied = copyfile(sockfd, STDOUT_FILENO);
    
    return 0;
}