#pragma once
#include <stdarg.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define LOG_FILE "./chat_log"

void logger( char* level, char* file, int line, char* format, ... );

#define LOG( ... ) logger( "info", __FILE__, __LINE__, __VA_ARGS__ )
#define ERROR( ... ) logger( "error", __FILE__, __LINE__, __VA_ARGS__ )