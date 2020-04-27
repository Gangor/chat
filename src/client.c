#include <client.h>

void client_init( client_t* p, int fd) 
{
    p->pseudo = NULL;
    p->fd = fd;
}

int client_pseudo( client_t* client, char* buffer )
{
    int len = strlen( buffer );

    if( client->pseudo == NULL )
        client->pseudo = calloc( len, sizeof(char) );
    else
        client->pseudo = realloc( client->pseudo, len * sizeof(char) );

    if( client->pseudo == NULL )
        return EXIT_FAILURE;

    strcpy( client->pseudo, buffer );
    return EXIT_SUCCESS;
}

void client_destroy(client_t* p)
{
    p->fd = UNUSED;
    free(p->pseudo);
}