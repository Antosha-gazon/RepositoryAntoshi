#include <stdexcept>
#include <netdb.h>
#include <time.h>
#include <string.h>
#include <string>
#include <sys/socket.h>
#include <iostream>
#include <unistd.h>

#define MAX_CONNECTION 10000
#define REQEST_BUFFER_SIZE 65536
class Server
{
public:
    Server();
    ~Server();
    int create_socket(const char *Port);
    int get_socket();
private:
    int socket_;
    bool brutForse_addr(addrinfo *servinfo);

};

struct Client  // ПЕРЕПИСАТЬ
{
    Client(int socket);
    ~Client();
    int get_client();
    void *get_addr();
private:
    sockaddr_storage client_addr;
    int client_;

};

class HTTP_Reqest{
public:
    HTTP_Reqest(int clietn_d);
    void request();
private:
    typedef enum
    {
     eHTTP_UNKNOWN,
     eHTTP_CONNECT,
     eHTTP_DELETE,
     eHTTP_GET,
     eHTTP_HEAD,
     eHTTP_OPTIONS,
     eHTTP_PATCH,
     eHTTP_POST,
     eHTTP_PUT,
     eHTTP_TRACE,
    }eHTTPMethod;
     int client_d;
     eHTTPMethod type;
   // char putch[255];

    char buffer[REQEST_BUFFER_SIZE];
    void parse();
    void send_massage(const char *inf);
    void send_404();
};
