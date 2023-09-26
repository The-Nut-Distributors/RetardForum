#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#define DEBUG 1

void http_get(char *url, char *path) {
  if (DEBUG)
    printf("http_get(\"%s\", \"%s\")\n", url, path);
  char buf[4096];
  size_t off = 0;

  strcpy(buf + off, "curl -sL ");
  off += strlen("curl -sL ");

  strcpy(buf + off, url);
  off += strlen(url);

  strcpy(buf + off, " -o ");
  off += strlen(" -o ");

  strcpy(buf + off, path);
  off += strlen(path);

  buf[off] = '\0';

  if (system(buf))
    exit(1);
}

void http_get_cached(char *url, char *path) {
  if (DEBUG)
    printf("\x1b[30mhttp_get_cached(\"%s\", \"%s\")\x1b[0m\n", url, path);
  if (access(path, F_OK))
    http_get(url, path);
}

void fetch_remote_files(void) {
  mkdir("include", 0755);
  mkdir("include/node_api", 0755);
  {
    http_get_cached("https://raw.githubusercontent.com/nodejs/node/v20.5.1/src/"
                    "js_native_api_types.h",
                    "include/node_api/js_native_api_types.h");
    http_get_cached("https://raw.githubusercontent.com/nodejs/node/v20.5.1/src/"
                    "js_native_api.h",
                    "include/node_api/js_native_api.h");
    http_get_cached("https://raw.githubusercontent.com/nodejs/node/v20.5.1/src/"
                    "node_api_types.h",
                    "include/node_api/node_api_types.h");
    http_get_cached("https://raw.githubusercontent.com/nodejs/node/v20.5.1/src/"
                    "node_api.h",
                    "include/node_api/node_api.h");
  }

  mkdir("include/base64", 0755);
  {

    http_get_cached(
        "https://opensource.apple.com/source/QuickTimeStreamingServer/"
        "QuickTimeStreamingServer-452/CommonUtilitiesLib/base64.h",
        "include/base64/base64.h");
    http_get_cached(
        "https://opensource.apple.com/source/QuickTimeStreamingServer/"
        "QuickTimeStreamingServer-452/CommonUtilitiesLib/base64.c",
        "include/base64/base64.c");
  }
}

void build_argon2(void) {
  if (DEBUG)
    printf("\x1b[30mbuild_argon2()\x1b[0m\n");
  if (chdir("argon2"))
    exit(1);
  if (system("make"))
    exit(1);
  if (chdir(".."))
    exit(1);
}

void build_db_lib(void) {
  if (DEBUG)
    printf("\x1b[30mbuild_db_lib()\x1b[0m\n");
  if (chdir("db"))
    exit(1);
  if (system("zig build-lib -dynamic "
             "-lc "
             "-lpython3.9 "
             "-L/usr/lib/python3.9/config-3.9-x86_64-linux-gnu "
             "-I/usr/include/python3.9 "

             "db.c"))
    exit(1);
  if (chdir(".."))
    exit(1);
}

void build_auth_lib(void) {
  if (DEBUG)
    printf("\x1b[30mbuild_auth_lib()\x1b[0m\n");
  if (chdir("auth"))
    exit(1);
  if (system("zig build-lib -dynamic -lc web_auth.c -I../argon2/include"
             " ../argon2/src/argon2.c"
             " ../argon2/src/core.c"
             " ../argon2/src/encoding.c"
             " ../argon2/src/ref.c"
             " ../argon2/src/thread.c"
             " ../argon2/src/blake2/blake2b.c"))
    exit(1);
  if (rename("libweb_auth.so", "web_auth.node"))
    exit(1);
  if (chdir(".."))
    exit(1);
}

void run(void) {
  if (DEBUG)
    printf("\x1b[30mrun()\x1b[0m\n");
  if (system("deno run "
             "--unstable "
             "--allow-read "
             "--allow-net=0.0.0.0:8080 "
             "--allow-ffi "
             "server/web_server.ts"))
    exit(1);
}

int main(void) {
  fetch_remote_files();
  build_argon2();
  build_db_lib();
  build_auth_lib();
  run();
  return 0;
}
