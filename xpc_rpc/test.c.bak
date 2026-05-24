#include "include/xpcapi.h"
#include "include/xpcapiconst.h"

#include <stdio.h>

char target_ip[] = "127.0.0.1";
char target_port[] = "22222";

int xpcError() {
  const int err = xPCGetLastError();
  if (err == ENOERR) return 0;

  const char *err_msg = xPCErrorMsg(err, nullptr);
  fprintf(stderr, "xPC error %d: %s\n", err, err_msg);
  return err;
}

int main(int argc, char** argv) {
    printf("Running\n");
    int ret = xPCInitAPI();

    if (ret == -1) {
        printf("Failed to Init\n");
        return -1;
    }

    printf("Initialized\n");

    ret = xPCOpenTcpIpPort(target_ip, target_port);
    xpcError();

    printf("Done");

}
