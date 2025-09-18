#include "HttpRequest.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define INIT_BUF_SIZE 1024;

char *read_http_headers(int fd, size_t *out_len, char **extra,
                        size_t *extra_len) {
  size_t buf_size = INIT_BUF_SIZE;
  size_t total = 0;
  char *buf = malloc(buf_size);
  if (!buf)
    return NULL;

  while (1) {
    ssize_t n = read(fd, buf + total, buf_size - total);
    if (n <= 0)
      break;
    total += n;

    // grow buffer if needed
    if (total == buf_size) {
      buf_size *= 2;
      buf = realloc(buf, buf_size);
      if (!buf)
        return NULL;
    }

    // check for end of headers
    char *end = strstr(buf, "\r\n\r\n");
    if (end) {
      size_t header_len = (end - buf) + 4;
      *extra_len = total - header_len;
      if (*extra_len > 0) {
        *extra = malloc(*extra_len);
        memcpy(*extra, buf + header_len, *extra_len);
      } else {
        *extra = NULL;
        *extra_len = 0;
      }
      total = header_len;
      break;
    }
  }

  buf[total] = '\0';
  *out_len = total;
  return buf;
}

void parse_request_line(HttpRequest *data, const char *raw_headers) {
  const char *end = strstr(raw_headers, "\r\n");
  size_t line_len = end ? (size_t)(end - raw_headers) : strlen(raw_headers);
  char *line = strndup(raw_headers, line_len);
  if (!line)
    return;

  char *ptr = line;
  char *token;

  // Parse method
  token = strsep(&ptr, " ");
  if (token)
    data->method = strdup(token);

  // Parse path
  token = strsep(&ptr, " ");
  if (token)
    data->path = strdup(token);

  // Parse HTTP version
  token = strsep(&ptr, " ");
  if (token)
    data->version = strdup(token);

  free(line);
}

void parse_headers(HttpRequest *data, const char *raw_headers) {
  HeaderArray arr;
  init_headers(&arr);

  // Find start of headers (first CRLF after request line)
  const char *header_start = strstr(raw_headers, "\r\n");
  if (!header_start)
    return;
  header_start += 2; // skip the CRLF

  // Make a copy so we can mutate safely
  char *header_copy = strdup(header_start);
  if (!header_copy)
    return;

  char *line_ptr = header_copy;
  char *cur_line;

  while ((cur_line = strsep(&line_ptr, "\r\n")) != NULL) {
    if (*cur_line == '\0')
      continue; // skip empty lines

    char *key = cur_line;
    char *value = strchr(cur_line, ':');
    if (!value)
      continue;

    *value++ = '\0'; // split at ':'
    while (*value == ' ')
      value++; // trim leading spaces

    push_header(&arr, key, value);
  }

  free(header_copy);
  data->headers = arr;
}

void read_string_body(int fd, HttpRequest *request, size_t content_length,
                      char *extra, size_t extra_len) {
  char *buf = malloc(content_length + 1); // +1 for null terminator
  if (!buf)
    return;

  size_t total = 0;

  // copy extra bytes first
  if (extra && extra_len > 0) {
    memcpy(buf, extra, extra_len);
    total = extra_len;
  }

  while (total < content_length) {
    ssize_t n = read(fd, buf + total, content_length - total);
    if (n <= 0) {
      free(buf);
      request->body = NULL;
      request->body_length = 0;
      return;
    }
    total += n;
  }

  buf[content_length] = '\0';
  request->body = buf;
  request->body_length = content_length;
}

HttpRequest *parse_request(int fd) {
  HttpRequest *data = malloc(sizeof(HttpRequest));
  if (!data)
    return NULL;

  size_t total_read = 0;
  char *extra = NULL;
  size_t extra_len = 0;

  char *raw_headers = read_http_headers(fd, &total_read, &extra, &extra_len);
  if (!raw_headers) {
    free(data);
    return NULL;
  }

  parse_request_line(data, raw_headers);
  parse_headers(data, raw_headers);

  const char *cl = get_header_value(&data->headers, "Content-Length");
  if (cl) {
    size_t content_length = strtoul(cl, NULL, 10);

    read_string_body(fd, data, content_length, extra, extra_len);
  } else {
    data->body = NULL;
    data->body_length = 0;
  }

  free(extra);
  free(raw_headers);

  return data;
}

void free_req(HttpRequest *request) {
  free_headers(&request->headers);

  free(request->version);
  free(request->method);
  free(request->path);
  free(request->body);

  free(request);
}

void print_http_request(const HttpRequest *req) {
  if (!req)
    return;

  printf("HttpRequest {\n");
  printf("  method: %s\n", req->method ? req->method : "(null)");
  printf("  path: %s\n", req->path ? req->path : "(null)");
  printf("  version: %s\n", req->version ? req->version : "(null)");

  printf("  headers (%zu):\n", req->headers.count);
  for (size_t i = 0; i < req->headers.count; i++) {
    printf("    %s: %s\n",
           req->headers.headers[i].key ? req->headers.headers[i].key : "(null)",
           req->headers.headers[i].value ? req->headers.headers[i].value
                                         : "(null)");
  }

  printf("  body_length: %zu\n", req->body_length);
  printf("  body: %.*s\n", (int)req->body_length,
         req->body ? req->body : "(null)"); // use length in case body has nulls
  printf("}\n");
}
