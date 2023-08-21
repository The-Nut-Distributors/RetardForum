#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(void) {
  if (access("include/node_api/js_native_api_types.h", F_OK) ||
      system("curl -sL "
             "https://raw.githubusercontent.com"
             "/nodejs/node/v20.5.1/src/js_native_api_types.h "
             "-o include/node_api/js_native_api_types.h"))
    exit(1);
  if (access("include/node_api/js_native_api.h", F_OK) ||
      system("curl -sL "
             "https://raw.githubusercontent.com"
             "/nodejs/node/v20.5.1/src/js_native_api.h "
             "-o include/node_api/js_native_api.h"))
    exit(1);
  if (access("include/node_api/node_api_types.h", F_OK) ||
      system("curl -sL "
             "https://raw.githubusercontent.com"
             "/nodejs/node/v20.5.1/src/node_api_types.h "
             "-o include/node_api/node_api_types.h"))
    exit(1);
  if (access("include/node_api/node_api.h", F_OK) ||
      system("curl -sL "
             "https://raw.githubusercontent.com"
             "/nodejs/node/v20.5.1/src/node_api.h "
             "-o include/node_api/node_api.h"))
    exit(1);
  if (system("zig build-lib -dynamic "
             "-lc "
             "-lpython3.9 "
             "-L/usr/lib/python3.9/config-3.9-x86_64-linux-gnu "
             "-I/usr/include/python3.9 "

             "db.c"))
    exit(1);
  if (system("zig build-lib -dynamic -lc web_auth.c"))
    exit(1);
  if (rename("libweb_auth.so", "web_auth.node"))
    exit(1);
  if (system("deno run "
             "--unstable "
             "--allow-read "
             "--allow-net=0.0.0.0:8080 "
             "--allow-ffi "
             "web_server.ts"))
    exit(1);
  return 0;
}
