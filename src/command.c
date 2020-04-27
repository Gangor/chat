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
	int result = EXIT_FAILURE;

	if ( *buff == '\0' )
		return result;

	char* copy = calloc( strlen( buff ), sizeof( char ) );

	strcpy( copy, buff );

	char* pcmd = strtok( copy, " " );
	char* pargs = strtok( NULL, "\0" );

    for (int i = 0; i < cmd_count; i++)
    {
        command_t* cmd = &cmds[i];
        int len = strlen( cmd->cmd );

        if ( strcmp( pcmd, cmd->cmd ) == 0 )
        {
            LOG( "User %s executed the command : %s.", client->pseudo, buff );

            cmd->func(client, pargs );
            result = EXIT_SUCCESS;
			break;
        }
    }
    
	free( copy );
    return result;
}

void cmd_exit( client_t* client, char* token )
{
    MESSAGE( client->fd, "Aurevoir !\n");
	
    close( client->fd );
    client_destroy( client );

    pthread_cancel( client->thread );
}

void cmd_help( client_t* client, char* token )
{
	MESSAGE( client->fd, "Commandes :\n" );

    for( int i = 0; i < cmd_count; i++ )
		MESSAGE( client->fd, "%s\t%s\t%s\n", 
			cmds[ i ].cmd, 
			cmds[ i ].usage, 
			cmds[ i ].description );
}

void cmd_kick( client_t* client, char* token )
{
   	char* pseudo = strtok( token, " " );

	if( pseudo == NULL )
	{
      	MESSAGE( client->fd, "Utilisation : /kick [pseudo]\n" );
		return;
	}

	client_t* target = findClient( pseudo );
	
	if( target == NULL)
	{
		MESSAGE( client->fd, "Cette utilisateur n'est pas connecté.\n" );
		return;
	}

	MESSAGE( target->fd, "Vous avez été expulsé du serveur.\n" );
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
      	MESSAGE( client->fd, "Utilisation : /mp [pseudo] [message]\n" );
		return;
	}
	
	client_t* target = findClient( pseudo );
	
	if( target == NULL )
	{
		MESSAGE( client->fd, "Cette utilisateur n'est pas connecté.\n" );
		return;
	}

	MESSAGE( client->fd, "[To:%s] : %s\n", target->pseudo, msg );
	MESSAGE( target->fd, "[From:%s] : %s\n", client->pseudo, msg );
}

void cmd_nickname( client_t* client, char* token )
{
   	char* pseudo = strtok( token, " " );
	   
	if( pseudo == NULL )
	{
      	MESSAGE( client->fd, "Utilisation : /nickname [pseudo]\n" );
		return;
	}

	BROADCAST( "%s s'appelle maintenant %s.\n", client->pseudo, pseudo );

	client_pseudo( client, pseudo );
}

void cmd_shutdown( client_t* client, char* token )
{
	endSignal = 1;
}