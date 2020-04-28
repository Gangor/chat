#pragma once
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <helper.h>
#include <struct.h>

#define MAX_CHANNEL 15
#define DEFAULT_CHANNEL 1

channel_t channels[ MAX_CHANNEL ];

void channelInit();

channel_t* channelAdd( char* name );
int channelCreate( channel_t* channel, const char* name );
void channelDestroy( channel_t* channel );
channel_t* channelFind( const char* name );