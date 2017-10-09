#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include "stub.h"

int fn_socket_stub(int domain, int type, int protocol)
{
    printf("fn_socket_stub\r\n");

    return 0;
}

int main()
{
    int iRet = -1;
    struct func_stub socket_stub;

    stub_set(&socket_stub, socket, fn_socket_stub);
    iRet = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
    printf("%d\r\n", iRet);
    stub_reset(&socket_stub);
    iRet = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
    printf("%d\r\n", iRet);
    return 0;
}
