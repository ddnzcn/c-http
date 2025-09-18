#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
  char *key;
  char *value;
} HttpHeader;

typedef struct {
  HttpHeader *headers;
  size_t count;
  size_t capacity;
} HeaderArray;

void init_headers(HeaderArray *arr);
void push_header(HeaderArray *arr, const char *key, const char *value);
char *get_header_value(HeaderArray *arr, const char *key);
void free_headers(HeaderArray *arr);
