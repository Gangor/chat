#include <server.h>

void* mainProcess( void* ptr )
{
	int opt = 1;

	if(( sock = socket( AF_INET, SOCK_STREAM, IPPROTO_TCP )) <= 0 )
	{
		ERROR( "socket failed." );
		pthread_exit( NULL );
	}

	struct sockaddr_in server;

	server.sin_family = AF_INET;
	server.sin_addr.s_addr = INADDR_ANY;
	server.sin_port = htons( port );
	
	if( setsockopt( sock, SOL_SOCKET, SO_REUSEADDR | SO_BROADCAST, &opt, sizeof( opt ) ) != 0 )
	{
		ERROR( "Echec de paramétrage: %s" );
		pthread_exit( NULL );
	}

	if( bind( sock, ( struct sockaddr * ) &server, sizeof( server ) ) < 0 )
	{
		ERROR( "Echec de la liaison du port d'écoute." );
		pthread_exit( NULL );
	}

	if( listen( sock, 5 ) )
	{
		ERROR( "Echec de l'écoute." );
		pthread_exit( NULL );
	}

	socklen_t addrlen = sizeof( server );

	LOG( "Ecoute du port : %d", port );
	LOG( "En attente de connexion ..." );

	while( 1 )
	{
    	struct sockaddr_in addr;

		int fd 				= accept( sock, (struct sockaddr *)&addr, (socklen_t*)&addrlen );        
		client_t* client    = clientAdd( fd );

		if( client == NULL )
		{
			char err[] = "Le nombre maximum de client a été atteint.\n";
			
			send( fd, err, strlen( err ), 0 );
			close( fd );
			continue;
		}

		LOG( "Un client vient de se connecter.");

		if( pthread_create( &client->thread, NULL, clientProcess, (void *)client ) != 0 )
		{
			ERROR( "Echec de la création du thread client." );
			close( fd );
		}
		else
		{
			pthread_detach( client->thread );
		}
	}
}

void message( client_t* client, const char* format, ... )
{
	if( client->fd < 0 )
		return;

	char str[BUFFER];

	va_list arglist;
	va_start( arglist, format );

	size_t len = snprintf( NULL, 0, format, arglist ) + 1;
	vsprintf( str, format, arglist );

	va_end( arglist );

	send( client->fd, str, strlen( str ), 0 );
}

int readLine( client_t* client, char* buffer )
{
	bzero( buffer, BUFFER );

	int len = read( client->fd, buffer, BUFFER );
	if( len == -1  && errno != EAGAIN )
	{
		LOG( "Errno [%i] : %s\n", errno, strerror( errno ) );
		return EXIT_FAILURE;
	}

	if( len == 0 )
		return EXIT_FAILURE;

	if( *buffer == EOF )
		return EXIT_FAILURE;

	buffer[len] = '\0';

	char *pos;
	if(( pos = strchr( buffer, '\r' )) != NULL )
		*pos = '\0';

	return EXIT_SUCCESS;
}

int stop()
{
	LOG( "Destruction du serveur" );
	BROADCAST( "Le serveur est en cours d'arrêt...\n" );
	
	for( int i = 0; i < MAX_CLIENT; i++ )
	{
		client_t* client = &( clients[i] );

		if ( client->thread != 0 )
			pthread_cancel( client->thread );
		
		if( client->fd != UNUSED )
			close( client->fd );

		clientDestroy( client );
	}

	shutdown( sock, 0 );
	close( sock );
	return EXIT_SUCCESS;
}