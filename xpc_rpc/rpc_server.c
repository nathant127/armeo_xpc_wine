#include "include/xpcapi_rpc.h"
#include "include/xpcapi.h"
#include "include/xpcapiconst.h"

#include <winsock2.h>

#include <string.h>
#include <stdio.h>

// Simple message format:
// [4 bytes: function id] [4 bytes: arg count] [N bytes: args]

void handle_req(const Message* msg, Response* resp);

int main(int argc, char** argv) {

    if (xPCInitAPI() == -1) {
        printf("Failed to load .dll\n");
        return 1;
    }

    WSADATA wsa;
    WSAStartup(MAKEWORD(2,2), &wsa);

    SOCKET server = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in addr = {
        .sin_family      = AF_INET,
        .sin_port        = htons(51243),
        .sin_addr.s_addr = INADDR_ANY,
    };

    bind(server, (struct sockaddr*)&addr, sizeof(addr));
    printf("Listening\n");
    listen(server, 1);

    SOCKET client = accept(server, NULL, NULL);

    Message msg;
    Response resp;
    while (recv(client, (char*)&msg, sizeof(msg), 0) > 0) {
        printf("Got Req\n");
        handle_req(&msg, &resp);
        send(client, (char*)&resp, sizeof(resp), 0);
        printf("Sent Resp\n");
    }

    closesocket(client);
    closesocket(server);
    WSACleanup();
}

void handle_req(const Message* msg, Response* resp) {
    switch (msg->func_id) {
        case XPC_RE_OPEN_PORT: {  // xPCReOpenPort
            int port;
            memcpy(&port, msg->args, sizeof(int));
            int result = xPCReOpenPort(port);
            memcpy(resp->data, &result, sizeof(int));
            resp->status = 0;
            break;
        }
        case XPC_CLOSE_PORT: {  // xPCClosePort
            int port;
            memcpy(&port, msg->args, sizeof(int));
            xPCClosePort(port);
            resp->status = 0;
            break;
        }
        case XPC_OPEN_TCP_IP_PORT: {
            printf("Triggered XPC_OPEN_TCP_IP_PORT\n");
            const char* addr = msg->args;
            int addr_len = strlen(addr);

            const char* port = &msg->args[addr_len] + 1;
            printf("Addr: %s, Port: %s\n", addr, port);
            resp->status = xPCOpenTcpIpPort(addr, port);
        }
        // etc.
    }
}
