/* 
    File: requestchannel.C

    Author: R. Bettati
            Department of Computer Science
            Texas A&M University
    Date  : 2012/07/11

*/

/*--------------------------------------------------------------------------*/
/* DEFINES */
/*--------------------------------------------------------------------------*/

    /* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* INCLUDES */
/*--------------------------------------------------------------------------*/

#include <cassert>
#include <cstring>
#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <errno.h>

#include <sys/socket.h>
#include <netdb.h>

#include "networkreqchannel.h"

//using namespace std;

/*--------------------------------------------------------------------------*/
/* DATA STRUCTURES */ 
/*--------------------------------------------------------------------------*/

    /* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* CONSTANTS */
/*--------------------------------------------------------------------------*/

    /* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* FORWARDS */
/*--------------------------------------------------------------------------*/

    /* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* PRIVATE METHODS FOR CLASS   R e q u e s t C h a n n e l  */
/*--------------------------------------------------------------------------*/



/*--------------------------------------------------------------------------*/
/* CONSTRUCTOR/DESTRUCTOR FOR CLASS   R e q u e s t C h a n n e l  */
/*--------------------------------------------------------------------------*/
/* Creates a CLIENT-SIDE local copy of the channel. The channel is connected to the given port number at the given server host. THIS CONSTRUCTOR IS CALLED BY THE CLIENT. */
NetworkRequestChannel::NetworkRequestChannel(const char* _server_host_name, const char* _port_no){
    struct addrinfo hints, *res;
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    
    int status = 0;
    
    if ((status = getaddrinfo(_server_host_name, _port_no, &hints, &res)) != 0)
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(status));
    // make a socket:
    sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    if (sockfd < 0)
        perror ("Error creating socket\n");
    // connect!
    if (connect(sockfd, res->ai_addr, res->ai_addrlen)<0)
        perror ("connect error\n");
}

/* Creates a SERVER-SIDE local copy of the channel that is accepting connections at the given port number. NOTE that multiple clients can be connected to the same server-side end of the request channel. Whenever a new connection comes in, it is accepted by the server, and the given connection handler is invoked. The parameter to the connection handler is the file descriptor of the slave socket returned by the accept call. NOTE that the connection handler does not want to deal with closing the socket. You will
 have to close the socket once the connection handler is done. */
NetworkRequestChannel::NetworkRequestChannel(const char* _port_no, void * (*connection_handler)(void*), int backlog){
    struct addrinfo hints, *serv;
    struct sockaddr_storage their_addr; // connector's address information
    socklen_t sin_size;
    char s[INET6_ADDRSTRLEN];
    int rv;
    
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;
    
    if ((rv = getaddrinfo(NULL, _port_no, &hints, &serv)) != 0){
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        exit(1);
    }
    
    if ((sockfd = socket(serv->ai_family, serv->ai_socktype, serv->ai_protocol)) == -1){
        perror("server: socket");
        exit(1);
    }
    
    if (::bind(sockfd, serv->ai_addr, serv->ai_addrlen) == -1) {
        close(sockfd);
        perror("server: bind");
        exit(1);
    }
    
    freeaddrinfo(serv); // all done with this structure
    
    if (listen(sockfd, backlog) == -1) {
        perror("listen");
        exit(1);
    }
    char buf [1024];
    pthread_t tid;
    printf("server: waiting for connections...\n");
    while(1) {  // main accept() loop
        sin_size = sizeof their_addr;
        newfd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size);
        if (newfd == -1) {
            perror("accept");
            continue;
        }
        
        pthread_create(&tid, NULL, connection_handler, &newfd);
    }
}

/* Destructor of the local copy of the channel. */
NetworkRequestChannel::~NetworkRequestChannel(){
    close(sockfd);
}

/* Send a string over the channel and wait for a reply. */
string NetworkRequestChannel::send_request(string _request){
    cwrite(_request);
    string s = cread();
    return s;
}

/* Blocking read of data from the channel. Returns a string of characters read from the channel. Returns NULL if read failed. */
string NetworkRequestChannel::cread(){
    char buf[255];
    if (recv (sockfd, buf, sizeof(buf), 0) < 0) {
        perror("Error recieving message");
    }
    string s = buf;
    return s;
}

/* Write the data to the channel. The function returns the number of characters written to the channel. */
int NetworkRequestChannel::cwrite(string _msg){
    if (_msg.length() >= 255) {
        cerr << "Message exceeds max length\n";
        return -1;
    }
    const char * msg = _msg.c_str();
    if (send(sockfd, msg, strlen(msg)+1, 0) < 0) {
        perror("Error sending message");
    }
    return 0;
}
