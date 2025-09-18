#pragma once

#include "../headers/HttpHeader.h"
#include <stdio.h>

typedef struct {
  char *version;
  char *method;
  char *path;

  HeaderArray headers;

  char *body;
  size_t body_length;

} HttpRequest;

HttpRequest *parse_request(int fd);
void print_http_request(const HttpRequest *req);
void free_req(HttpRequest *req);
