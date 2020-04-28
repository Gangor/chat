#include <client.h>

void clientInit()
{
	for( int i = 0; i < MAX_CLIENT; i++ )
    {
		clients[i].channel = NULL;
        clients[i].pseudo = NULL;
        clients[i].thread = 0;
        clients[i].fd = UNUSED;
    }
}

client_t* clientAdd( int fd )
{
	if( fd < 0 )
		return NULL;

	for( int i = 0; i < MAX_CLIENT; i++ )
	{
		client_t* client = &( clients[i] );

		if( client->fd == UNUSED )
		{
            client->fd = fd;
			return client;
		}
	}
	return NULL;
}

int clientChannel( client_t* p, channel_t* channel )
{
	if( channel == NULL )
		return EXIT_FAILURE;

	p->channel = NULL;
	p->channel = channel;

	BROADCAST_CHANNEL( p->channel, "%s vient de rejoindre le canal %s\n", p->pseudo, p->channel->name );
	return EXIT_SUCCESS;
}

client_t* clientFind( const char* pseudo )
{
	for( int i = 0; i < MAX_CLIENT; i++ )
	{
		client_t* client = &( clients[i] );

		if( client->pseudo == NULL )
			continue;

		if( stricmp( client->pseudo, pseudo ) == 0 )
			return client;
	}

	return NULL;
}

void* clientProcess( void* ptr )
{
	char buffer[256];
	client_t * client = (client_t *)ptr;

	MESSAGE( client, "Bienvenue sur le chat EPSI v1.\n" );
	MESSAGE( client, "Consulter l'aide en tapant /help.\n" );
	MESSAGE( client, "Veuillez saisir votre pseudo : " );

	readLine( client, buffer );

	clientPseudo( client, buffer );
	clientChannel( client, &channels[ DEFAULT_CHANNEL ] );
	
	BROADCAST( "%s vient de se joindre au serveur.\n", client->pseudo );

	while( 1 )
	{
		if( readLine( client, buffer ) != 0 )
			break;

		// prompt cursor
		MESSAGE( client, "\033[F" );
		MESSAGE( client, "\033[K" );

		if( proc_command( client, buffer ) == 0 )
			continue;

		if( strlen( buffer ) == 0 )
			continue;

		BROADCAST_CHANNEL( client->channel, "%s: %s\n", client->pseudo, buffer );
	}
	
	BROADCAST( "%s vient de quitter le serveur.\n", client->pseudo );

	close( client->fd );
	clientDestroy( client );
	pthread_exit( NULL );
}

int clientPseudo( client_t* client, const char* pseudo )
{
    int len = strlen( pseudo );

    if( client->pseudo == NULL )
        client->pseudo = calloc( len, sizeof(char) );
    else
        client->pseudo = realloc( client->pseudo, len * sizeof(char) );

    if( client->pseudo == NULL )
        return EXIT_FAILURE;

    strcpy( client->pseudo, pseudo );
    return EXIT_SUCCESS;
}

void clientDestroy(client_t* p)
{
    p->fd = UNUSED;
	p->channel = NULL;
    p->thread = 0;
    free(p->pseudo);
}