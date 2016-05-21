# Data Server #

This project includes a data server and client to facilitate a high-performance data query and processing system. This implementation modifies Machine problems 4 and 5 to handle requests over a network request channel via a TCP socket.

Client program can be run using the following:

```client  -n <number of data requests per person>
           -b <size of bounded buffer in requests>
           -w <number of request channels>
           -h <server hostname>
           -p <server port number>```

The Data Server can be run using the following:
```
dataserver -p <port number for data server>
           -b <backlog of server socket>```

_ Completed as part of Machine Problem 6 for CSCE 313. _

Contributors:
* Ryan Walters
* Garrett Haynes
