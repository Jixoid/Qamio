#pragma once

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>


typedef size_t  ohid;
typedef void*   point;
typedef size_t  size;

typedef uint8_t byte;


typedef struct{
  size  Size;
  point Point;
} data;
