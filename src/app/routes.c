#include "../request/HttpRequest.h"
#include "../response/HttpResponse.h"
#include "../router/Router.h"
#include <stdio.h>
#include <strings.h>

void test_handler(HttpRequest *req, int fd) {
  printf("Handling request for %s = %s\n", req->method, req->path);

  HttpResponse res = {.body = req->path, .body_length = strlen(req->path)};

  write_response(fd, &res);
}

void init_test_module() {
  register_route(test_handler, "POST", "/test");
  register_route(test_handler, "GET", "/test");
}
