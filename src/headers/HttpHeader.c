#include "HttpHeader.h"
#include <stddef.h>
#include <stdio.h>
#include <strings.h>

void init_headers(HeaderArray *arr) {
  arr->count = 0;
  arr->capacity = 4;
  arr->headers = malloc(sizeof(HttpHeader) * arr->capacity);
}

void push_header(HeaderArray *arr, const char *key, const char *value) {
  if (arr->count == arr->capacity) {
    arr->capacity *= 2;
    arr->headers = realloc(arr->headers, sizeof(HttpHeader) * arr->capacity);
  }
  arr->headers[arr->count].key = strdup(key);
  arr->headers[arr->count].value = strdup(value);
  arr->count++;
}

void free_headers(HeaderArray *arr) {
  for (size_t i = 0; i < arr->count; i++) {
    free(arr->headers[i].key);
    free(arr->headers[i].value);
  }
  free(arr->headers);
  arr->count = 0;
  arr->capacity = 0;
}

char *get_header_value(HeaderArray *arr, const char *key) {
  for (size_t i = 0; i < arr->count; i++) {
    if (strcasecmp(arr->headers[i].key, key) == 0) {
      return arr->headers[i].value;
    }
  }

  return NULL;
}
