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

#include <errno.h>

#include "networkreqchannel.h"

using namespace std;

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
NetworkRequestChannel::NetworkRequestChannel(const string _server_host_name, const unsigned short _port_no){
    struct addrinfo hints;
    struct addrinfo* server_info;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = NULL;
    getaddrinfo(_host_server_name.c_str(), _port_no, &hints, &server_info);
    server_addr = (sockaddr_in)(server_info[0].ai_addr);
    freeaddrinfo(server_info);
    int error = accept(socket_client, (struct sockaddr*)&server_addr, sizeof(server_addr));
    if (error < 0)
        printf("Error in creating new request channel\n");
}

/* Creates a SERVER-SIDE local copy of the channel that is accepting connections at the given port number. NOTE that multiple clients can be connected to the same server-side end of the request channel. Whenever a new connection comes in, it is accepted by the server, and the given connection handler is invoked. The parameter to the connection handler is the file descriptor of the slave socket returned by the accept call. NOTE that the connection handler does not want to deal with closing the socket. You will
 have to close the socket once the connection handler is done. */
NetworkRequestChannel::NetworkRequestChannel(const unsigned short _port_no, void * (*connection_handler) (int *)){
    socket_server = socket(AF_INET, SOCK_STREAM, 0);
    struct addrinfo hints;
    struct addrinfo * server_info;
    memset(hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;
    getaddrinfo(NULL, _port_no, &hints, &server_info);
    server = (sockaddr_in)(server_info[0].ai_addr);
    freeaddrinfo(server_info);
    bind(socket_server, &server, sizeof(server));
    listen(socker_server, _backlog);
    while (1){
        phtread_t thread_id;
        int newsock = accept(socket_server, NULL, NULL);
        pthread_create(&thread_id, NULL, connection_handler, &newsock);
        pthread_detach(thread_id);
    }
}

/* Destructor of the local copy of the channel. */
NetworkRequestChannel::~NetworkRequestChannel(){
    close(socket_server);
    close(socket_client);
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
    if (read(socket_client, buf, 255) < 0) {
        perror(string("Request Channel (" + my_name + "): Error reading from pipe!").c_str());
    }
    string s = buf;
    return s;
}

/* Write the data to the channel. The function returns the number of characters written to the channel. */
int NetworkRequestChannel::cwrite(string _msg){
    if (_msg.length() >= 255) {
        cerr << "Message too long for Channel!\n";
        return -1;
    }
    const char * s = _msg.c_str();
    if (write(socket_client, s, strlen(s) + 1) < 0) {
        perror(string("Request Channel (" + my_name + ") : Error writing to pipe!").c_str());
    }
}

int NetworkRequestChannel::get_fd(){
    return socket_client;
}
