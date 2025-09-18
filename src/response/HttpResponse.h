#pragma once
#include "../headers/HttpHeader.h"
#include <stddef.h>

typedef struct {
  int status_code;
  const char *status_text;
  HeaderArray headers;
  char *body;
  size_t body_length;
} HttpResponse;

void write_response(int fd, HttpResponse *res);
HttpResponse create_response_with_body(char *body, size_t content_size,
                                       int status_code);
HttpResponse create_response_with_status(int status_code);
