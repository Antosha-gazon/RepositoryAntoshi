#include <string>
#include <iostream>
#include <fstream>
#include <stack>
#include <netdb.h>
#include <time.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>



class configurator
{
public:
    configurator(std:: string conf);
    ~configurator();
private:
    std::stack<std::string> variables;
};


class server
{
public:
    server();
    ~server();
    int create_socket(const char *Port);
    int get_socket();
private:
    int socket_;
    void check_addr(addrinfo *servinfo);
    const int max_conaction = 100000;
};
