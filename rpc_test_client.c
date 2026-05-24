#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#include "xpc_rpc/include/xpcapi_rpc.h"

#define SERVER_IP   "127.0.0.1"
#define SERVER_PORT 51243

static int sock;

static int send_recv(Message *msg, Response *resp) {
    if (send(sock, msg, sizeof(*msg), 0) < 0) {
        perror("send");
        return -1;
    }
    if (recv(sock, resp, sizeof(*resp), 0) < 0) {
        perror("recv");
        return -1;
    }
    return 0;
}

static int rpc_xPCOpenTcpIpPort(const char *addr, const char *port) {
    Message msg = {0};
    Response resp = {0};

    msg.func_id = XPC_OPEN_TCP_IP_PORT;

    // pack: [addr string] [null] [port string] [null]
    int addr_len = strlen(addr) + 1;
    int port_len = strlen(port) + 1;
    memcpy(msg.args, addr, addr_len);
    memcpy(msg.args + addr_len, port, port_len);

    if (send_recv(&msg, &resp) < 0)
        return -1;

    return resp.status;
}

static int rpc_xPCReOpenPort(int port) {
    Message msg = {0};
    Response resp = {0};

    msg.func_id = XPC_RE_OPEN_PORT;
    memcpy(msg.args, &port, sizeof(int));

    if (send_recv(&msg, &resp) < 0)
        return -1;

    int result;
    memcpy(&result, resp.data, sizeof(int));
    return result;
}

static void rpc_xPCClosePort(int port) {
    Message msg = {0};
    Response resp = {0};

    msg.func_id = XPC_CLOSE_PORT;
    memcpy(msg.args, &port, sizeof(int));

    send_recv(&msg, &resp);
}

int main(void) {
    // connect to wine server
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("socket");
        return 1;
    }

    struct sockaddr_in addr = {
        .sin_family      = AF_INET,
        .sin_port        = htons(SERVER_PORT),
        .sin_addr.s_addr = inet_addr(SERVER_IP),
    };

    printf("Connecting to server...\n");
    if (connect(sock, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        perror("connect");
        return 1;
    }
    printf("Connected\n");

    // test xPCOpenTcpIpPort
    printf("Testing xPCOpenTcpIpPort...\n");
    int ret = rpc_xPCOpenTcpIpPort("127.0.0.1", "22222");
    printf("xPCOpenTcpIpPort returned: %d\n", ret);

    printf("Testing xPCOpenTcpIpPort...\n");
    ret = rpc_xPCOpenTcpIpPort("127.0.0.1", "22221");
    printf("xPCOpenTcpIpPort returned: %d\n", ret);

    if (ret < 0) {
        printf("Failed to open port\n");
        close(sock);
        return 1;
    }

    close(sock);
    printf("Done\n");
    return 0;
}