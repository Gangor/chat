#include <command.h>

command_t cmds[] = 
{
	// cmd			usage						description						fonction
	{ "/exit",		"/exit",					"Permet de vous déconnecter.",	&cmd_exit },
	{ "/help",		"/help",					"Affiche toute les commandes.",	&cmd_help },
	{ "/kick",		"/kick [pseudo]",			"Déconnecter un utilisateur.",	&cmd_kick },
	{ "/mp",		"/mp [pseudo] [message]",	"Envoyer un message privé.",	&cmd_mp },
	{ "/nickname",	"/nickname [pseudo]",		"Modifier votre pseudo.",		&cmd_nickname },
	{ "/shutdown",	"/shutdown",				"Arrêter le serveur.",			&cmd_shutdown },
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
            command->func(client, args );
			break;
        }
    }

	free( copy );
    return EXIT_SUCCESS;
}

void cmd_exit( client_t* client, char* token )
{
    MESSAGE( client, "Aurevoir !\n");
	
    close( client->fd );
    client_destroy( client );

    pthread_cancel( client->thread );
}

void cmd_help( client_t* client, char* token )
{
	MESSAGE( client, "Commandes :\n" );

    for( int i = 0; i < cmd_count; i++ )
		MESSAGE( client, "%s\t%s\t%s\n", 
			cmds[ i ].cmd, 
			cmds[ i ].usage, 
			cmds[ i ].description );
}

void cmd_kick( client_t* client, char* token )
{
   	char* pseudo = strtok( token, " " );

	if( pseudo == NULL )
	{
      	MESSAGE( client, "Utilisation : /kick [pseudo]\n" );
		return;
	}

	client_t* target = findClient( pseudo );
	
	if( target == NULL)
	{
		MESSAGE( client, "Cette utilisateur n'est pas connecté.\n" );
		return;
	}

	MESSAGE( target, "Vous avez été expulsé du serveur.\n" );
    pthread_cancel( target->thread );
    close( target->fd );
    client_destroy( target );
}

void cmd_mp( client_t* client, char* token )
{
   	char* pseudo = strtok( token, " " );
   	char* msg = strtok( NULL, "\0" );
	
	if( pseudo == NULL || msg == NULL )
	{
      	MESSAGE( client, "Utilisation : /mp [pseudo] [message]\n" );
		return;
	}
	
	client_t* target = findClient( pseudo );
	
	if( target == NULL )
	{
		MESSAGE( client, "Cette utilisateur n'est pas connecté.\n" );
		return;
	}

	MESSAGE( client, "[To:%s] : %s\n", target->pseudo, msg );
	MESSAGE( target, "[From:%s] : %s\n", client->pseudo, msg );
}

void cmd_nickname( client_t* client, char* token )
{
   	char* pseudo = strtok( token, " " );
	   
	if( pseudo == NULL )
	{
      	MESSAGE( client, "Utilisation : /nickname [pseudo]\n" );
		return;
	}

	BROADCAST( "%s s'appelle maintenant %s.\n", client->pseudo, pseudo );

	client_pseudo( client, pseudo );
}

void cmd_shutdown( client_t* client, char* token )
{
	endSignal = 1;
}