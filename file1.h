#pragma comment(lib, "Ws2_32.lib")
#include <stdio.h>
#include <stdint.h>
#include <winsock2.h>
#include <ws2tcpip.h>


typedef uint16_t PORT;
typedef char* const ADDRESS;

union addr{
    struct in_addr addr4;
    struct in6_addr addr6;
};


struct address_port{
    union addr address;
    PORT port;
    int af;
};

int bindSocket(SOCKET , struct address_port*);

enum address_family{
    IPV4,
    IPV6,
    UNDEFINED
};

PORT to_network_order(PORT port);
PORT to_host_order(PORT port);
PORT set_network_order(PORT port);

void to_network_orderAddrv4(ADDRESS address, struct in_addr* buf);





int set_nonblocking(SOCKET sockfd); // Set a socket file descriptor into non-blocking mode
SOCKET create_socket(int af, int type); // Create a socket fd


