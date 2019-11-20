#include "Core.h"
#include <sys/io.h>
#include <arpa/inet.h>
Server::Server(){
socket_ = 0;
}

Server::~Server(){
    close(socket_);
}



int Server::get_socket(){
    return socket_;
}


bool Server::brutForse_addr(addrinfo *servinfo){
    addrinfo *current = servinfo;
    for (; current != NULL; current = current -> ai_next){
        socket_ = socket(current->ai_family, current->ai_socktype, current ->ai_protocol);
        int yes;
        if (setsockopt(socket_, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1)
        {
           std::cout << "setsockopt error ";
           close(socket_);
           freeaddrinfo(servinfo); // очистка памяти
           return false;
         }

         if(bind(socket_, current->ai_addr, current->ai_addrlen) == -1)
         {
           close(socket_);
           continue;
         }

         break;
    }
    if (current == NULL){
        std::cout << "error server failed to find address";
        return false;
    }
    return true;
}


int Server::create_socket(const char *Port){
    addrinfo hints;
    addrinfo *servinfo;
    memset(&hints, 0, sizeof(hints));

    hints.ai_family   = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags    = AI_PASSIVE;

    int result = getaddrinfo(NULL, Port, &hints, &servinfo);

    if (result != 0){
        std::cout << "error getaddrinfo()";
        return -1;
    }
    if (brutForse_addr(servinfo) == false)
        return -2;

    freeaddrinfo(servinfo);

    if (listen(socket_, MAX_CONNECTION) == -1){
        std::cout << "error listen";
        return -3;
    }
    return 0;
}




Client::Client(int socket){
    socklen_t s_size = sizeof(client_addr);
    client_ = accept(socket, (sockaddr*)&client_addr, &s_size);
    if (client_ < 0)
        throw std::invalid_argument( "error accept\n" );
}

Client::~Client(){
    close(client_);
}

int Client::get_client(){
    return  client_;
}


HTTP_Reqest::HTTP_Reqest(int client_d){
   this->client_d = client_d;
}


void HTTP_Reqest::request(){
    int bytes_recv = recv(client_d, &buffer, REQEST_BUFFER_SIZE - 1, 0);
    if (bytes_recv < 0){
         throw std::invalid_argument( "error recv\n" );
    }

    buffer[REQEST_BUFFER_SIZE] = '\0';
    parse();
    if (type == eHTTP_GET || type == eHTTP_HEAD){
        send_massage( "Maxim pidor<br><center><h1>PERSHIN HUILA</h1></center> KOT - KOTIACHIY HUY: 20<br><a href=\"https://vk.com/id178024485\">external</a><br><a href=\"internal\">internal</a>");
    }

    else
        send_404();
}

void HTTP_Reqest::parse(){
    type = eHTTP_UNKNOWN;
    if (buffer[0] == 'G' && buffer[1] == 'E' && buffer[2] == 'T')
        type = eHTTP_GET;
    if (buffer[0] == 'H' && buffer[1] == 'E' && buffer[2] == 'A' && buffer[3] == 'D')
        type = eHTTP_HEAD;
    if (buffer[0] == 'P' && buffer[1] == 'O' && buffer[2] == 'S' && buffer[3] == 'T'){

    }
}

void HTTP_Reqest::send_massage(  const char *inf){
    char MSGbuffer[REQEST_BUFFER_SIZE] = {0};
    strcat(MSGbuffer, "HTTP/1.1 200 OK\n\n");
    if (type == eHTTP_GET){
    strcat(MSGbuffer, "<h1>");
    strcat(MSGbuffer, inf);
    strcat(MSGbuffer, "</h1>");
    }
    int MSGsize = strlen(MSGbuffer);
    send(client_d, MSGbuffer, MSGsize, 0);

}


void HTTP_Reqest::send_404(){
    const char *buffer = "HTTP/1.1 404 \n\n";
    int MSGsize = strlen(buffer);
    send(client_d, buffer, MSGsize, 0);
}

