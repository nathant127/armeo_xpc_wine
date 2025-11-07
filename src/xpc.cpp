#include "xpc.h"
#include <cstdio>

#include "xpcapi.h"
#include "xpcapiconst.h"

int xpcError() {
  const int err = xPCGetLastError();
  if (err == ENOERR) return 0;

  const char *err_msg = xPCErrorMsg(err, nullptr);
  fprintf(stderr, "xPC error %d: %s\n", err, err_msg);
  return err;
}