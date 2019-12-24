#include "core.h"

/*           CLASS HTTPPARSER            */

httpParser::httpParser(int client_d)
{
    char buffer[req_buff_size];

    int bytes_recv = recv(client_d, buffer,req_buff_size - 1, 0); // получаем сообщение из сокета
    if (bytes_recv < 0)
    {
        send(client_d, heading::error404().c_str(), heading::error404().length(), 0);
    }
    buffer[bytes_recv] = '\0';
    reqest = buffer;
    std::cout << reqest <<std::endl; // для тестов
    if (bytes_recv < 0)
        throw std::invalid_argument( "error recv\n" );

    if (reqest.find("GET") == 0)
    {
        type = GET;
        GET_parse();
    }
    else if (reqest.find("POST") == 0)
    {
        type = POST;
        POST_Parse();
    }
    else if (reqest.find("HEAD") == 0)
        type = HEAD;
    else
         throw std::invalid_argument( "invalid REQ\n" );

    requestedFile();
}


void httpParser::requestedFile()
{
    std::string check;
    check.reserve(4);
    int start = reqest.find("/");
    for (int i = start + 2; i != start + 2 + 4; i++)
        check.push_back(reqest[i]);

    if (check == "HTTP")
    {
        file = "index.html";
        return;
    }

            if (dynamic == true && type == GET)
        {
            int end = reqest.find("?");
            file.reserve(start - end - 1);
            for (int i = start + 1; reqest[i] != '?'; i++)
                file.push_back(reqest[i]);
            return;
        }
         int end = reqest.find(" ");
         file.reserve(start - end - 1);
         for (int i = start + 1; reqest[i] != ' '; i++)
            file.push_back(reqest[i]);
}


void httpParser::GET_parse()
{
    int start = -1;
    int end = reqest.find("HTTP");
    for (int i = 0; i < end; i++)
    {
        if (reqest[i] == '?')
        {
            start = i;
            break;
        }
    }
    if (start >= 0)
    {
     dynamic = true;
     reqestBody = reqest.substr(start + 1, end - start - 2);
     return;
    }
     dynamic = false;
}


void httpParser::POST_Parse()
{
    std::string key = "Content-Length:";
    int start = reqest.find(key) + key.length();
    int end = start;
    for (; reqest[end] != '\r'; end++);
    std::string Content_lenght = reqest.substr(start, end);
    if (atoi(Content_lenght.c_str()) > 0)
    {
        dynamic = true;
        reqestBody = reqest.substr(reqest.length() - atoi(Content_lenght.c_str()), reqest.length());
        return;
    }
    dynamic = false;
}


httpMethod httpParser::get_type()
{
    return type;
}


std::string httpParser::get_reqest()
{
    return reqest;
}


std::string httpParser::get_reqestBody()
{
    return reqestBody;
}


std::string httpParser:: get_file()
{
    return file;
}


bool httpParser::get_dynamic()
{
    return dynamic;
}


/*                      CLASS CLIENT                  */


client::client()
{
    client_d = 0;
}


client::~client()
{
    close(client_d);
}


client* client::listen_port(int server_d)
{
    client *temp = new client();
    socklen_t s_size = sizeof(temp->client_addr);
    temp->client_d = accept(server_d, (sockaddr*)&temp->client_addr, &s_size); //вынимает из очереди соединение
    if (temp->client_d < 0)
        throw std::invalid_argument( "error accept\n" );

    return temp;
}


int client::get_client()
{
    return client_d;
}


/*                CLASS HTTPHEANDLER                                       */

httpHandler::httpHandler(client *client)
{
    newclient = client;
    parser = new httpParser(newclient->get_client());
}


httpHandler::~httpHandler()
{
    delete newclient;
    delete parser;
}
void httpHandler::handle()
{
    if (parser->get_type() == HEAD)
    {
        heading head;
        send(newclient->get_client(), head.get_head().c_str(), head.get_head().length(), 0);
        return;
    }
    if (parser->get_type() == UNKNOWN)
        send(newclient->get_client(), heading::error404().c_str(), heading::error404().length(), 0);

    if (!parser->get_dynamic())
        static_handle();

    else
        dynamic_handle();
}


void httpHandler::static_handle()
{
    std::string buffer; // создаем буфер в который будем формировать сообщение, отправляемое браузеру
    std::ifstream file(getenv("path") + parser->get_file(), std::ifstream::binary); // открываем читаемый файл c конца, скливая переменную окружения "путь" и имя файла
    if (file) // если файл открылся, продолжаем
     {
         auto const size{file.tellg()}; //узнаем позицию курсора
         file.seekg(0); //обнуляем позицию курсора
         char current;
         buffer.reserve(size);
         while(file.get(current)) //читаем файл
             buffer.push_back(current);
         heading head(buffer.length(), parser->get_file()); // этот класс формирует заголовок
         buffer = head.get_head() + buffer; // приклеиваем полученный заголовок к буферу
         send(newclient->get_client(), buffer.c_str(), buffer.length(), 0); // отправляем полученный буфер браузеру
      }
      else
         send(newclient->get_client(), heading::error404().c_str(), heading::error404().length(), 0);

     file.close();
}


void httpHandler::dynamic_handle()
{
    pid_t pid;
    int pipe_in[2];
    int pipe_out[2];
    pipe(pipe_in);
    pipe(pipe_out);
    if ((pid = fork()) > 0) // родительский процесс
    {
         close(pipe_in[0]);  // закрываем возможность чтения
         close(pipe_out[1]); // закрываем возможность записи

         std::string type; // определяем тип сообщения
         if (parser->get_type() == POST)
            type= "|POST|";
         if(parser->get_type() == GET)
            type= "|GET|";

         std::string msg = parser->get_file() + type + parser->get_reqestBody() +"\n"; // сообщение, передаваемое дочернему процессу составляется из имени файла, типа запроса и тела запроса

         char buf[1024];
         write(pipe_in[1], msg.c_str(), msg.length()); //отправляем сообщение в stdin стандартного ввода


        int size = read(pipe_out[0], &buf, 1024); //читаем сообщение из stdout стандартного вывода
        if (size < 0)
            send(newclient->get_client(), heading::error404().c_str(), heading::error404().length(), 0);//если неудачно, отправляем 404
        buf[size] = '\0';
        std::string buffer = buf;
        buffer = "<p>" + buffer + "<p>";
        heading head(buffer.length(), parser->get_file()); //составляем заголовок
        buffer = head.get_head() + buffer; //склеиваем заголовок с сообщением
        send(newclient->get_client(), buffer.c_str(), buffer.length(), 0);
    }
    else
    {
        close(pipe_in[1]); // закрываем возможность записи
        close(pipe_out[0]); // закрываем возможность чтения

        dup2(pipe_in[0], 0); //переопределяем канал на стандартный вывод
        dup2(pipe_out[1], 1);//переопроеделяем канал на стандартный ввод
        if(execlp("php","php", getenv("CGI_path"),  NULL) < 0)
        {
            send(newclient->get_client(), heading::error404().c_str(), heading::error404().length(), 0);
            _exit(EXIT_SUCCESS);
        }
    }
}


/*               CLASS HEADING                                     */


heading::heading(const int content_size, const std::string &file)
{
    head = "HTTP/1.1 200 OK\r\n";

    std::string Content_Type, Content_Length;
    std::string extension = file.substr(file.find(".") + 1);

    if (extension == "png" || extension == "gif")
        Content_Type = "Content-Type: image/apng\r\n";
    else if(extension == "jpg")
        Content_Type = "Content-Type: image/jpeg\r\n";
    else
        Content_Type = "Content-Type: text/html\r\n";

    Content_Length = "Content-Lenght: " + std::to_string(content_size) + "\n\n";
    head = head + "Server: Cone /r/n" + Content_Type + Content_Length;
}

heading::heading()
{
    head = "HTTP/1.1 200 OK\r\n";
    head = head + "Server: Cone /n/n";
}

std::string heading::get_head()
{
    return head;
}

std::string heading::error404()
{
    std::string temp;
    temp = "HTTP/1.1 404 \n\n";
    return temp;
}

