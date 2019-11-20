#include "Core.h"
#include <arpa/inet.h>
#include <sys/epoll.h>
int main()
{
    Server wood;

    if (wood.create_socket("8000") < 0)
        throw std::invalid_argument( "error server created\n" );
    std::cout << "server created!" << std::endl;

    while (true){
        Client current(wood.get_socket());
        HTTP_Reqest req(current.get_client());
        req.request();
    }
    return 0;
}
