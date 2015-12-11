/*
    File: reqchannel.H

    Author: R. Bettati
            Department of Computer Science
            Texas A&M University
    Date  : 2012/07/11

*/

#ifndef _netreqchannel_H_                   // include file only once
#define _netreqchannel_H_

/*--------------------------------------------------------------------------*/
/* DEFINES */
/*--------------------------------------------------------------------------*/

/* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* INCLUDES */
/*--------------------------------------------------------------------------*/

#include <iostream>
#include <fstream>
#include <string>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

using namespace std;

/*--------------------------------------------------------------------------*/
/* DATA STRUCTURES */ 
/*--------------------------------------------------------------------------*/

/* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* FORWARDS */ 
/*--------------------------------------------------------------------------*/

/* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* CLASS   R e q u e s t C h a n n e l */
/*--------------------------------------------------------------------------*/

class NetworkRequestChannel {

private:
    int sockfd, newfd, status;
    struct sockaddr_in server, server_addr;

public:

    /* -- CONSTRUCTOR/DESTRUCTOR */

    /* Creates a CLIENT-SIDE local copy of the channel. The channel is connected to the given port number at the given server host. THIS CONSTRUCTOR IS CALLED BY THE CLIENT. */
    NetworkRequestChannel(const char* _server_host_name, const char* _port_no);

    /* Creates a SERVER-SIDE local copy of the channel that is accepting connections at the given port number. NOTE that multiple clients can be connected to the same server-side end of the request channel. Whenever a new connection comes in, it is accepted by the server, and the given connection handler is invoked. The parameter to the connection handler is the file descriptor of the slave socket returned by the accept call. NOTE that the connection handler does not want to deal with closing the socket. You will
     have to close the socket once the connection handler is done. */
    NetworkRequestChannel(const char* _port_no, void * (*connection_handler)(void*), int backlog);

    /* Destructor of the local copy of the channel. */
    ~NetworkRequestChannel();

    /* Send a string over the channel and wait for a reply. */
    string send_request(string _request);

    /* Blocking read of data from the channel. Returns a string of characters read from the channel. Returns NULL if read failed. */
    string cread();

    /* Write the data to the channel. The function returns the number of characters written to the channel. */
    int cwrite(string _msg);
    
    void process_request(const string & _request);
    void process_hello(const string & _request);
    void process_data(const string &  _request);
    
    int read_socket(){ return sockfd; }
};


#endif


