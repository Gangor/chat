#include <command.h>

command_t cmds[] = 
{
	// cmd			usage						description							fonction
	{ "/addchannel","/channel [name]",			"Ajouter un channel.",				&cmd_add_channel	},
	{ "/channels",	"/channels",				"Liste des channels.",				&cmd_channels		},
	{ "/exit",		"/exit",					"Permet de vous déconnecter.",		&cmd_exit 			},
	{ "/help",		"/help",					"Affiche toute les commandes.",		&cmd_help 			},
	{ "/kick",		"/kick [pseudo]",			"Déconnecter un utilisateur.",		&cmd_kick 			},
	{ "/mp",		"/mp [pseudo] [message]",	"Envoyer un message privé.",		&cmd_mp 			},
	{ "/gochannel",	"/gochannel [name]",		"Ce déplacer sur un channel.",		&cmd_move_channel	},
	{ "/nickname",	"/nickname [pseudo]",		"Modifier votre pseudo.",			&cmd_nickname 		},
	{ "/remchannel","/remchannel [name]",		"Supprimer un channel.",			&cmd_remove_channel	},
	{ "/shutdown",	"/shutdown",				"Arrêter le serveur.",				&cmd_shutdown 		},
	{ "/users",		"/users",					"Liste les utilisateurs connecté.",	&cmd_users 			},
};

size_t cmd_count = sizeof( cmds ) / sizeof( command_t );

int proc_command( client_t* client, const char* buff )
{
	if ( *buff != '/' )
		return EXIT_FAILURE;

	char* copy = calloc( strlen( buff ), sizeof( char ) );

	strcpy( copy, buff );

	char* cheat = strtok( copy, " " );
	char* args = strtok( NULL, "\0" );

    for( int i = 0; i < cmd_count; i++ )
    {
        command_t* command = &cmds[i];
        int len = strlen( command->cmd );

        if ( strcmp( cheat, command->cmd ) == 0 )
        {

            LOG( "User %s executed the command : %s.", client->pseudo, buff );
            command->func(client, command, args );
			break;
        }
    }

	free( copy );
    return EXIT_SUCCESS;
}

void cmd_exit( client_t* client, command_t* command, char* token )
{
    MESSAGE( client, "Aurevoir !\n");
	
    close( client->fd );
    clientDestroy( client );

    pthread_cancel( client->thread );
}

void cmd_help( client_t* client, command_t* command, char* token )
{
	MESSAGE( client, "Commandes :\n" );

    for( int i = 0; i < cmd_count; i++ )
		MESSAGE( client, "%s\t%s\t%s\n", 
			cmds[ i ].cmd, 
			cmds[ i ].usage, 
			cmds[ i ].description );
}

void cmd_kick( client_t* client, command_t* command, char* token )
{
   	char* pseudo = strtok( token, "\0" );

	if( pseudo == NULL )
	{
      	MESSAGE( client, "Utilisation : %s\n", command->usage );
		return;
	}

	client_t* target = clientFind( pseudo );
	
	if( target == NULL)
	{
		MESSAGE( client, "Cette utilisateur n'est pas connecté.\n" );
		return;
	}

	MESSAGE( target, "Vous avez été expulsé du serveur.\n" );

    pthread_cancel( target->thread );
    close( target->fd );

	BROADCAST( "%s a été expulsé du server.\n", target->pseudo );

    clientDestroy( target );
}

void cmd_mp( client_t* client, command_t* command, char* token )
{
   	char* pseudo = strtok( token, " " );
   	char* msg = strtok( NULL, "\0" );
	
	if( pseudo == NULL || msg == NULL )
	{
      	MESSAGE( client, "Utilisation : %s\n", command->usage );
		return;
	}
	
	client_t* target = clientFind( pseudo );
	
	if( target == NULL )
	{
		MESSAGE( client, "Cette utilisateur n'est pas connecté.\n" );
		return;
	}

	MESSAGE( client, "[To:%s] : %s\n", target->pseudo, msg );
	MESSAGE( target, "[From:%s] : %s\n", client->pseudo, msg );
}

void cmd_nickname( client_t* client, command_t* command, char* token )
{
   	char* pseudo = strtok( token, " " );
	   
	if( pseudo == NULL )
	{
      	MESSAGE( client, "Utilisation : %s\n", command->usage );
		return;
	}

	BROADCAST( "%s s'appelle maintenant %s.\n", client->pseudo, pseudo );

	clientPseudo( client, pseudo );
}

void cmd_shutdown( client_t* client, command_t* command, char* token )
{
	endSignal = 1;
}

void cmd_channels( client_t* client, command_t* command, char* token )
{
	MESSAGE( client, "Channel: \n" );

    for( int i = 0; i < MAX_CHANNEL; i++ )
		if ( channels[ i ].name != NULL )
			MESSAGE( client, "%s ", channels[ i ].name );

	MESSAGE( client, "\n" );
}

void cmd_users( client_t* client, command_t* command, char* token )
{
	MESSAGE( client, "Utilisateurs en ligne :\n" );

    for( int i = 0; i < MAX_CLIENT; i++ )
		if ( clients[ i ].pseudo != NULL )
			MESSAGE( client, "%s ", clients[ i ].pseudo );

	MESSAGE( client, "\n" );
}

void cmd_add_channel( client_t* client, command_t* command, char* token )
{
   	char* name = strtok( token, "\0" );

	if( name == NULL )
	{
      	MESSAGE( client, "Utilisation : %s\n", command->usage );
		return;
	}

	channel_t* channel = channelFind( name );

	if( channel != NULL )
	{
      	MESSAGE( client, "Ce channel existe déjà!\n" );
		return;
	}

	channel_t* chan = channelAdd( name );
	if( chan == NULL )
	{
      	MESSAGE( client, "Le nombre maximum de channel a été atteint.\n" );
		return;
	}

	BROADCAST( "Le channel %s a été crée.\n", chan->name );
}

void cmd_move_channel( client_t* client, command_t* command, char* token )
{
   	char* name = strtok( token, "\0" );

	if( name == NULL )
	{
      	MESSAGE( client, "Utilisation : %s\n", command->usage );
		return;
	}

	channel_t* channel = channelFind( name );

	if( channel == NULL )
	{
      	MESSAGE( client, "Ce channel existe pas!\n" );
		return;
	}

	BROADCAST_CHANNEL( client->channel, "%s vient de quitter le canal %s\n", client->pseudo, client->channel->name );

	clientChannel( client, channel );
}

void cmd_remove_channel( client_t* client, command_t* command, char* token )
{
   	char* name = strtok( token, "\0" );

	if( name == NULL )
	{
      	MESSAGE( client, "Utilisation : %s\n", command->usage );
		return;
	}

	channel_t* channel = channelFind( name );

	if( channel == NULL )
	{
      	MESSAGE( client, "Ce channel existe pas!\n" );
		return;
	}

	if( channel->home == 1 )
	{
      	MESSAGE( client, "Vous pouvez pas supprimer l'acceuil!\n" );
		return;
	}

	BROADCAST( "Le channel %s a été supprimé.\n", channel->name );

	for( int i = 0; i < MAX_CLIENT; i++ )
	{
		if( clients[ i ].channel == channel )
		{
			MESSAGE( &clients[ i ], "Vous avez été déplacé dans l'acceuil.\n" );
			clientChannel( &clients[ i ], &channels[ DEFAULT_CHANNEL ] );
		}
	}
	
	channelDestroy( channel );
}