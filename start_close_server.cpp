#include "start_close_server.h"


/* CLASS SERVER */

server::server(){
socket_ = 0;
}

server::~server()
{
    close(socket_);
}

int server::get_socket()
{
    return socket_;
}


void server::check_addr(addrinfo *servinfo)
{
    socket_ = socket(servinfo->ai_family, servinfo->ai_socktype, servinfo ->ai_protocol); // cоздает кончную точку соединения и возвращает ее описатель
    int yes;
    if (setsockopt(socket_, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1)// устанавливаем флиг соккета
    {
      close(socket_);
      std::string exception = "Error setsockopt. errno = " + std::to_string(errno);
      throw std::invalid_argument(exception);
    }

    if(bind(socket_, servinfo->ai_addr, servinfo->ai_addrlen) == -1) // привязывает к сокету sockfd локальный адрес my_addr длиной addrlen.
    {
      close(socket_);
      std::string exception = "Error bind. errno = " + std::to_string(errno);
      throw std::invalid_argument(exception);
    }
    freeaddrinfo(servinfo); // очистка памяти
}


int server::create_socket(const char *Port)
{
    addrinfo hints;
    addrinfo *servinfo;
    memset(&hints, 0, sizeof(hints)); // заполнение структуры hints нулями
    // проставляем необходимые поля
    hints.ai_family   = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags    = AI_PASSIVE;

    int result = getaddrinfo(NULL, Port, &hints, &servinfo); //создает структуру адресса сокета

    if (result != 0)
    {
        std::string exception = "Error error getaddrinfo. errno = " + std::to_string(errno);
        throw std::invalid_argument(exception);
    }

    check_addr(servinfo);

    if (listen(socket_, max_conaction) == -1) //слушаем соединения на сокете
    {
        std::string exception = "Error error listen. errno = " + std::to_string(errno);
        throw std::invalid_argument(exception);
    }

    return 0;
}



/*CLASS CONFIGURATOR */

configurator::configurator(std::string conf)
{
 std::ifstream config(conf);
 if (config)
 {
     while(!config.eof())
     {
         std::string body;
         std::getline(config, body);

         int del = body.find("=");
         std::string var = body.substr(0, del);
         std::string value = body.substr(del + 1);
         setenv(var.c_str(), value.c_str(), 1);
         variables.push(var);
     }
 }
 else
      throw std::invalid_argument( "error config\n" );

 config.close();
}




configurator::~configurator()
{
    while (!variables.empty())
    {
        unsetenv(variables.top().c_str());
        variables.pop();
    }
}
