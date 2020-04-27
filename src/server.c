#include <server.h>

client_t* addClient( int fd )
{
	if( fd < 0 )
		return NULL;

	for( int i = 0; i < MAX_CLIENT; i++ )
	{
		client_t* client = &clients[i];

		if ( client->fd == UNUSED )
		{
			client_init( client, fd );
			return client;
		}
	}
	return NULL;
}

void* clientProcess( void* ptr )
{
	char buffer[ BUFFER ];
	client_t * client = (client_t *)ptr;

	MESSAGE( client->fd, "Bienvenue sur le chat EPSI v1.\n" );
	MESSAGE( client->fd, "Consulter l'aide en tapant /help.\n" );
	MESSAGE( client->fd, "Veuillez saisir votre pseudo : " );

	readLine( client->fd, buffer );
	client_pseudo( client, buffer );
	
	BROADCAST( "%s vient de se joindre à la discussion.\n", client->pseudo );

	while( 1 )
	{
		if( readLine( client->fd, buffer ) == EXIT_FAILURE )
			break;

		// deletion of the draft line
		MESSAGE( client->fd, "\033[F" );
		MESSAGE( client->fd, "\033[K" );

		// reset color
		MESSAGE( client->fd, "\033[0m" );

		if( strcmp( buffer, "^]" ) == 0 ||
			strcmp( buffer, "^C" ) == 0 )
			break;

		if( proc_command( client, buffer ) == 0 )
			continue;

		if( strlen( buffer ) == 0 )
			continue;

		BROADCAST( "%s: %s\n", client->pseudo, buffer );
	}

	close( client->fd );
	client_destroy( client );
	pthread_exit( NULL );
}

client_t* findClient( const char* pseudo )
{
	for( int i = 0; i < MAX_CLIENT; i++ )
	{
		if( clients[i].pseudo == NULL )
			continue;

		if( strcasecmp( clients[i].pseudo, pseudo ) == 0 )
			return &( clients[i] );
	}

	return NULL;
}

void* mainProcess( void* ptr )
{
	int opt = 1;    

	for( int i = 0; i < MAX_CLIENT; i++ )
		client_init( &clients[i], UNUSED );

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
		int fd 				= accept( sock, (struct sockaddr *)&server, (socklen_t*)&addrlen );        
		client_t* client    = addClient( fd );

		if( client != NULL )
		{
			if( pthread_create( &client->thread, NULL, clientProcess, (void *)client ) != 0 )
			{
				close( fd );
				ERROR( "Echec de la création du thread client pour la connexion entrante." );
			}
		}
	}
}

void message( int fd, const char* format, ... )
{
	if( fd < 0 )
		return;

	va_list arglist;

	va_start( arglist, format );

	int len = snprintf( NULL, 0, format, arglist ) + 1;
	char str[len];

	vsprintf( str, format, arglist );

	va_end( arglist );

	send( fd, str, strlen( str ), 0 );
}

int readLine( int fd, char* buffer )
{
	bzero( buffer, BUFFER );

	int len = read( fd, buffer, BUFFER );
	if( len <= 0 )
		return EXIT_FAILURE;

	buffer[len] = '\0';

	char *pos;
	if(( pos = strchr( buffer, '\r' )) != NULL)
		*pos = '\0';

	return EXIT_SUCCESS;
}

int stop()
{
	LOG( "Destruction du serveur" );
	BROADCAST( "Le serveur est en cours d'arrêt...\n" );
	
	for( int i = 0; i < MAX_CLIENT; i++ )
	{
		client_t client = clients[i];

		if( client.fd != UNUSED )
		{
			pthread_cancel( client.thread );

			close( client.fd );
			client_destroy( &client );
		}
	}

	return EXIT_SUCCESS;
}