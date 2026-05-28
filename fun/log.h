#ifndef __LOG_H__
#define __LOG_H__

#include <stdio.h>
#include "main.h"

#define LOG_INFO(mod, fmt, ...)  printf("[%6lu][%-6s][INFO] " fmt "\r\n", (unsigned long)uwTick, mod, ##__VA_ARGS__)
#define LOG_ERR(mod, fmt, ...)   printf("[%6lu][%-6s][ERR ] " fmt "\r\n", (unsigned long)uwTick, mod, ##__VA_ARGS__)
#define LOG_DATA(mod, fmt, ...)  printf("[%6lu][%-6s][DATA] " fmt "\r\n", (unsigned long)uwTick, mod, ##__VA_ARGS__)

#endif /* __LOG_H__ */
