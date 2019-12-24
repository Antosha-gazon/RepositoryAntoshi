#include <iostream>
#include <string>
#include "start_close_server.h"
#include "core.h"

using namespace std;

int main()
{
    configurator serv("/home/anton/Cone_SERVER/CONE.conf");
    server cone;
    if (cone.create_socket(getenv("PORT")) < 0)
        throw std::invalid_argument( "error server created\n" );
    std::cout << "server created!" << std::endl;

        for(;;)
        {
          client *current = client::listen_port(cone.get_socket());
          pid_t child = fork();
          if (child == 0)
          {
             httpHandler worker(current);
             worker.handle();
             _exit(EXIT_SUCCESS);
          }
          delete current;
       }

    return 0;
}
