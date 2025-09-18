#pragma once
#include "../request/HttpRequest.h"
#include <stdlib.h>

typedef void (*Handler)(HttpRequest *req, int fd);

typedef struct {
  char *method;
  char *path;
  Handler handler;
} Route;

typedef struct {
  Route *routes;
  size_t count;
  size_t capacity;
} RouteArray;

extern RouteArray *routes;

void init_routes();
void register_route(Handler handler, char *method, char *path);
Route *get_route_for_request(HttpRequest *req);
void handle_request(HttpRequest *req, int fd);

#define REG_ROUTE(method, path, func) register_route(func, method, path)
