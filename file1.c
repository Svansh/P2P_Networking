#pragma comment(lib, "Ws2_32.lib")
#include "file1.h"
#include <stdio.h>
#include <winsock2.h>
#include <ws2tcpip.h>


PORT to_network_order(PORT port){
    return htons(port); 
}

PORT to_host_order(PORT port){
    return ntohs(port);
}

PORT set_network_order(PORT port){
    port = to_network_order(port);
}

void to_network_orderAddrv4(ADDRESS address, struct in_addr* buf){
    InetPton(AF_INET, address, buf);
}

void to_string_Addrv4(struct in_addr* networkorderAddress, ADDRESS buf, size_t bufsize){
    InetNtop(AF_INET, networkorderAddress, buf, bufsize);
}

int set_nonblocking(SOCKET sockfd){
    u_long mode = 1;
    int set_nonblocking_status = ioctlsocket(sockfd, FIONBIO, &mode);
    if(set_nonblocking_status != 0){
        return -1;
    }
    return 0;
}


SOCKET create_socket(int af, int type){
    SOCKET const sockfd = socket(af, type, 0);
    int nonblocking_status = set_nonblocking(sockfd);
    if(nonblocking_status == -1){
        perror("Switching to non-blocking mode failed in function create socket!");
        return -1;
    }

    static int bufflogged = 0;
    if(!bufflogged){

        // Displaying size of sending buffer
        int i = 0;
        socklen_t size = sizeof(i);
        // printf("%d", size);
        getsockopt(sockfd, SOL_SOCKET, SO_SNDBUF, (char*)&i, &size);
        printf("Size of Sending Buffer %d\n", i);
        
        // Displaying size of receiving buffer
        i = 0;
        size = sizeof(i);
        getsockopt(sockfd, SOL_SOCKET, SO_RCVBUF, (char*)&i, &size);
        printf("Size of Receiving buffer %d\n", i);

        bufflogged = 1;
    }

    return sockfd;
}

SOCKADDR_STORAGE to_sockaddr(struct address_port* sock_addr){
    SOCKADDR_STORAGE ss;
    SOCKADDR_IN* sin4 = (SOCKADDR_IN*)&ss;
    sin4->sin_addr = sock_addr->address.addr4;
    sin4->sin_port = to_network_order(sock_addr->port);
    sin4->sin_family = sock_addr->af;

    return ss;
}

struct address_port* to_address_port(SOCKADDR* sockaddr){
    struct address_port* addrport;
    SOCKADDR_IN* sin4 = (SOCKADDR_IN*)&sockaddr;
    addrport->address.addr4 = sin4->sin_addr;
    addrport->port = to_host_order(sin4->sin_port);
    addrport->af = sin4->sin_family;

    return addrport;
}

int bindSocket(SOCKET sockfd, struct address_port* sock_addr){
    SOCKADDR_STORAGE ss = to_sockaddr(sock_addr);
    int bind_status = bind(sockfd, (SOCKADDR*)&ss, sizeof(SOCKADDR_IN));
    if(bind_status == -1){
        perror("Binding Error!!!\n");
        return -1;
    }
    int listening_status = listen(sockfd, SOMAXCONN);
    if(listening_status == -1){
        perror("Listen function error!!!\n");
        return -1;
    }
    return 0;
}

int acceptConnection(SOCKET listening_sockfd){
    SOCKADDR_STORAGE ss;
    socklen_t size = sizeof(ss);
    SOCKET sockfd = accept(listening_sockfd, (SOCKADDR*)&ss, &size);
    struct address_port* addrport = to_address_port((SOCKADDR*)&ss);
    if(set_nonblocking(sockfd) == -1) {
        perror("Non-Blocking Error in acceptConnection!\n");
        return -1;
    }
    return 0;
}



int main(){
    WSADATA wsadata;
    WSAStartup(MAKEWORD(2, 2), &wsadata);
    SOCKET fd = create_socket(AF_INET, SOCK_STREAM);
    PORT prt = 80;
    ADDRESS addressv4 = "127.0.0.1";
    struct in_addr* buf;
    to_network_orderAddrv4(addressv4, buf);
    union addr ff;
    ff.addr4 = *buf;
    struct address_port addport = {ff, prt, AF_INET};
    bindSocket(fd, &addport);
    return 0;
}