#include "Router.h"
#include "../response/HttpResponse.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

RouteArray *routes = NULL;

void init_routes() {
  routes = malloc(sizeof(RouteArray));
  if (!routes) {
    perror("failed to initialize routes\n");
    exit(1);
  }

  routes->count = 0;
  routes->capacity = 12;
  routes->routes = malloc(sizeof(Route) * routes->capacity);
}

void register_route(Handler handler, char *method, char *path) {
  if (routes->count == routes->capacity) {
    routes->capacity *= 2;
    routes->routes = realloc(routes->routes, sizeof(Route) * routes->capacity);
  }

  routes->routes[routes->count].handler = handler;
  routes->routes[routes->count].method = strdup(method);
  routes->routes[routes->count].path = strdup(path);
  routes->count++;
}

Route *get_route_for_request(HttpRequest *req) {
  for (size_t i = 0; i < routes->count; i++) {
    Route *r = &routes->routes[i];
    if (strcmp(r->method, req->method) == 0 &&
        strcmp(r->path, req->path) == 0) {
      return r;
    }
  }

  return NULL;
}

void handle_request(HttpRequest *req, int fd) {
  Route *r = get_route_for_request(req);

  if (!r) {
    HttpResponse not_found_res = create_response_with_status(404);
    write_response(fd, &not_found_res);
    return;
  }
  // TODO: Middlewares maybe?
  r->handler(req, fd);
}
