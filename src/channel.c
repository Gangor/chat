#include <channel.h>

channel_t* channelAdd( char* name )
{
	for( int i = 0; i < MAX_CHANNEL; i++ )
	{
		channel_t* channel = &( channels[i] );

		if( channel->name == NULL )
		{
            channelCreate( channel, name );
			return channel;
		}
	}
	return NULL;
}

void channelInit()
{
	for( int i = 0; i < MAX_CHANNEL; i++ )
        channels[i].name = NULL;

    channels[ DEFAULT_CHANNEL ].name = "Accueil";
    channels[ DEFAULT_CHANNEL ].home = 1;
}

int channelCreate(channel_t* channel, const char* name )
{
    int len = strlen( name );

    if( channel->name == NULL )
        channel->name = calloc( len, sizeof(char) );
    else
        channel->name = realloc( channel->name, len * sizeof(char) );

    if( channel->name == NULL )
        return EXIT_FAILURE;

    strcpy( channel->name, name );
    return EXIT_SUCCESS;
}

void channelDestroy(channel_t* channel)
{
    free( channel->name );
}

channel_t* channelFind( const char* name )
{
	for( int i = 0; i < MAX_CHANNEL; i++ )
	{
		channel_t* channel = &( channels[i] );

		if( channel->name == NULL )
			continue;

		if( stricmp( channel->name, name ) == 0 )
			return channel;
	}

	return NULL;
}