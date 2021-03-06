#include <chat.h>

int endSignal = 0;

void signal_callback_handler( int signum )
{
	LOG( "SIG %i", signum );
	endSignal = 1;
}

int main(int argc, char *argv[])
{
	if ( argc < 2 )
	{
		printf( "Utilisation : %s [port]\n", argv[0] );
		return EXIT_FAILURE;
	}

	port = atoi( argv[1] );

	if( port < 0 && port > 65535 )
	{
		printf( "Le port %i n'est pas un port valide.\n", port );
		return EXIT_FAILURE;
	}

	#ifndef DEBUG

    pid_t pid = fork();

    if( pid < 0 )
        exit( EXIT_FAILURE );

    if( pid > 0 )
        exit( EXIT_SUCCESS );

    if( setsid() < 0 )
        exit( EXIT_FAILURE );
	
	#endif

	signal( SIGINT, signal_callback_handler );
	signal( SIGTERM, signal_callback_handler );

	clientInit();
	channelInit();

	pthread_t mainThread;

	pthread_create( &mainThread, NULL, mainProcess, NULL);
	pthread_detach( mainThread );

	while( !endSignal );
	
	pthread_cancel( mainThread );

	stop();

	return EXIT_SUCCESS;
}
