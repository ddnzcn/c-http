#include "HttpResponse.h"
#include "../headers/HttpHeader.h"
#include <stdlib.h>
#include <unistd.h>

const char *status_text_for_code(int code) {
  switch (code) {
  case 100:
    return "Continue";
  case 101:
    return "Switching Protocols";
  case 200:
    return "OK";
  case 201:
    return "Created";
  case 202:
    return "Accepted";
  case 204:
    return "No Content";
  case 301:
    return "Moved Permanently";
  case 302:
    return "Found";
  case 304:
    return "Not Modified";
  case 400:
    return "Bad Request";
  case 401:
    return "Unauthorized";
  case 403:
    return "Forbidden";
  case 404:
    return "Not Found";
  case 405:
    return "Method Not Allowed";
  case 418:
    return "I'm a teapot"; // RFC 2324, April 1st classic
  case 500:
    return "Internal Server Error";
  case 501:
    return "Not Implemented";
  case 502:
    return "Bad Gateway";
  case 503:
    return "Service Unavailable";
  default:
    return "Unknown";
  }
}

HttpResponse create_response_with_body(char *body, size_t content_size,
                                       int status_code) {
  HttpResponse res;

  res.status_code = status_code;
  res.status_text = status_text_for_code(status_code);

  init_headers(&res.headers);
  if (content_size > 0) {
    res.body = malloc(content_size);
    memcpy(res.body, body, content_size);
    res.body_length = content_size;
  } else {
    res.body = NULL;
    res.body_length = 0;
  }

  // compute content length here!
  char len_str[32];
  snprintf(len_str, sizeof(len_str), "%zu", res.body_length);
  push_header(&res.headers, "Content-Length", len_str);

  return res;
}

HttpResponse create_response_with_status(int status_code) {
  HttpResponse res;

  res.status_code = status_code;
  res.status_text = status_text_for_code(status_code);

  init_headers(&res.headers);
  // if (content_size > 0) {
  //   res.body = malloc(content_size);
  //   memcpy(res.body, body, content_size);
  //   res.body_length = content_size;
  // } else {
  //   res.body = NULL;
  //   res.body_length = 0;
  // }
  //
  res.body = "";
  res.body_length = 0;

  // compute content length here!
  char len_str[32];
  snprintf(len_str, sizeof(len_str), "%zu", res.body_length);
  push_header(&res.headers, "Content-Length", len_str);

  return res;
}

void write_response(int fd, HttpResponse *res) {
  dprintf(fd, "HTTP/1.1 %d %s\r\n", res->status_code, res->status_text);

  for (size_t i = 0; i < res->headers.count; i++) {
    dprintf(fd, "%s: %s\r\n", res->headers.headers[i].key,
            res->headers.headers[i].value);
  }

  dprintf(fd, "\r\n");

  if (res->body && res->body_length > 0) {
    write(fd, res->body, res->body_length);
  }
}
