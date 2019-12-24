#include <netdb.h>
#include <time.h>
#include <string.h>
#include <string>
#include <sys/socket.h>
#include <iostream>
#include <unistd.h>
#include <arpa/inet.h>
#include <fstream>
typedef enum
{
 GET,
 POST,
 HEAD,
 UNKNOWN,
}httpMethod;

class httpParser
{
public:
    httpParser(int client_d);
    httpMethod get_type();
    std::string get_reqest();
    std::string get_reqestBody();
    std::string get_file();
    bool get_dynamic();
private:
    httpMethod type;
    std::string reqest;
    std::string reqestBody;
    std::string file;
    bool dynamic;
    const int req_buff_size = 65536;

    void requestedFile();
    void GET_parse();
    void POST_Parse();
};


class client
{
public:
    client();
    ~client();
    static client* listen_port(int server_d);
    int get_client();
private:
    int client_d;
    sockaddr_storage client_addr;
};


class httpHandler
{
public:
    httpHandler(client *client);
    ~httpHandler();
    void handle();
private:
    client* newclient;
    httpParser* parser;
    void dynamic_handle();
    void static_handle();
    void PUT_handle();
};


class heading
{
public:
    heading(const int content_size, const std::string &file);
    heading();
    std::string get_head();
    static std::string error404();
private:
    std::string head;

};
