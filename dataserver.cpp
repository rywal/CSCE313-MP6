/* 
    File: dataserver.C

    Author: R. Bettati
            Department of Computer Science
            Texas A&M University
    Date  : 2012/07/16

    Dataserver main program for MP3 in CSCE 313
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
#include <sstream>
#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>

#include <pthread.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <getopt.h>

#include "networkreqchannel.h"

using namespace std;

/*--------------------------------------------------------------------------*/
/* DATA STRUCTURES */ 
char const* port = "50001"; // ayy - port number for server
int backlog; // backlog of the server socket

/*--------------------------------------------------------------------------*/
/* CONSTANTS */
/*--------------------------------------------------------------------------*/

    /* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* VARIABLES */
/*--------------------------------------------------------------------------*/

static int nthreads = 0;
int MAX_MSG = 255;

/*--------------------------------------------------------------------------*/
/* FORWARDS */
/*--------------------------------------------------------------------------*/

//void handle_process_loop(NetworkRequestChannel & _channel);

/*--------------------------------------------------------------------------*/
/* LOCAL FUNCTIONS -- SUPPORT FUNCTIONS */
/*--------------------------------------------------------------------------*/

string int2string(int number) {
   stringstream ss;//create a stringstream
   ss << number;//add number to the stream
   return ss.str();//return a string with the contents of the stream
}

string server_read(int * fd){
    char buf[MAX_MSG];
    
    read(*fd, buf, MAX_MSG);
    string s = buf;
    return s;
}

int server_write(int * fd, string m){
    if(m.length() >= MAX_MSG)
        cerr << "Message too big\n";
    
    if(write(*fd, m.c_str(), m.length()+1) < 0){
        cerr << "Write error\n";
        return -1;
    } else {
        return 0;
    }
    
}

/*--------------------------------------------------------------------------*/
/* LOCAL FUNCTIONS -- THREAD FUNCTIONS */
/*--------------------------------------------------------------------------*/


/*--------------------------------------------------------------------------*/
/* LOCAL FUNCTIONS -- INDIVIDUAL REQUESTS */
/*--------------------------------------------------------------------------*/

void process_hello(NetworkRequestChannel *reqchan, const string & _request) {
  reqchan->cwrite("hello to you too");
}

void process_data(NetworkRequestChannel *reqchan, const string &  _request) {
  usleep(1000 + (rand() % 5000));
    reqchan->cwrite(int2string(rand() % 100));
}

/*--------------------------------------------------------------------------*/
/* LOCAL FUNCTIONS -- THE PROCESS REQUEST LOOP */
/*--------------------------------------------------------------------------*/

void process_request(NetworkRequestChannel *reqchan, const string & _request) {

  if (_request.compare(0, 5, "hello") == 0) {
    process_hello(reqchan, _request);
  }
  else if (_request.compare(0, 4, "data") == 0) {
    process_data(reqchan, _request);
  }

}

void *connection_handler(void * arg) {
    char buf[1024];
    NetworkRequestChannel *chan = (NetworkRequestChannel*)arg;
    for(;;) {
        recv(chan->read_socket(), buf, sizeof(buf), 0);
        string request = buf;

        if (request.compare("quit") == 0) {
            chan->cwrite("bye");
            usleep(8000);
            break;
        }

        process_request(chan, request);
    }
  
    return 0;
}

/*--------------------------------------------------------------------------*/
/* MAIN FUNCTION */
/*--------------------------------------------------------------------------*/

int main(int argc, char * argv[]) {
    int opt;
    while ((opt = getopt(argc, argv, "p:b:")) != -1) {
        switch (opt) {
            case 'b':
                backlog = atoi(optarg);
                break;
            case 'p':
                port = optarg;
                break;
            default:
                port = "5001";
                backlog = 10;
        }
    }

  //  cout << "Establishing control channel... " << flush;
    cout << "SERVER STARTED: Port " << port << endl;
    NetworkRequestChannel server(port, connection_handler, backlog);

}

