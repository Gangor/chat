#pragma once
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <client.h>
#include <channel.h>
#include <server.h>

extern int endSignal;

void signal_callback_handler( int signum );