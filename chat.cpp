#include "chat.h"

void signal_callback_handler( int signum )
{
   exit( 0 );
}

void sendMessage( int fd, const char* format, ... )
{
   if ( fd <= 0 )
      return;

   char str[256];
   va_list arglist;

   va_start( arglist, format );
   vsprintf( str, format, arglist );
   va_end( arglist );
   
   send( fd, str, strlen( str ), 0 );
}

void sendAllMessage( const char* format, ... )
{
   char str[256];
   va_list arglist;

   va_start( arglist, format );
   vsprintf( str, format, arglist );
   va_end( arglist );
   
    for (int i = 0; i < Clients.size(); i++) 
      send( Clients[i]->fd, str, strlen( str ), 0 );
}

void* networkProcess( void* ptr )
{
   if ( !ptr )
      pthread_exit( NULL );

   char buffer[ BUFFER ];
   connection_t * connection = (connection_t *)ptr;

   printf( "guest#%i join.\n", connection->fd );
   sendMessage( connection->fd, "Bienvenue sur le chat EPSI v1.\n" );
   
   while( 1 )
   {
      bzero( buffer, BUFFER ); 
      int len = read( connection->fd, buffer, sizeof( buffer ) );

      if ( len <= 0 )
         break;

      if ( strcmp( buffer, "/exit\r\n" ) == 0 )
         break;

      // show console
      printf( "guest#%i: %s", connection->fd, buffer );

      // send everyone else
      sendMessage( connection->fd, "\033[A\r" ); // delete the traft line
      sendAllMessage( "\rguest#%i: %s", connection->fd, buffer );
   }

   printf( "guest%i left.\n", connection->fd );
   close( connection->fd );
   free( connection );
   pthread_exit( NULL );
}

int main(int argc, char *argv[])
{
   //daemon( 0, 0 );

   signal( SIGINT, signal_callback_handler );
   signal( SIGTERM, signal_callback_handler );

   connection_t clients[ MAX_CLIENT ];

   if ( argc < 2 )
   {
      printf( "Utilisation : %s port\n", argv[0] );
      exit( EXIT_FAILURE );
   }
   
   if (( fd = socket( AF_INET, SOCK_STREAM, IPPROTO_TCP )) <= 0 )
   {
      perror( "socket failed.\n" );
      exit( EXIT_FAILURE );
   }

   int port = atoi( argv[1] );

   Server.sin_family = AF_INET;
   Server.sin_addr.s_addr = INADDR_ANY;
   Server.sin_port = htons( port );

   if ( bind( fd, ( struct sockaddr * ) &Server, sizeof( Server ) ) < 0 )
   {
      perror("Echec de la liaison du port d'écoute.\n");
      exit( EXIT_FAILURE );
   }

   if ( listen( fd, 5 ) )
   {
      perror( "Echec de l'écoute.\n" );
      exit( EXIT_FAILURE );
   }

   socklen_t addrlen = sizeof( Server );

   printf( "Ecoute du port : %d \n", port );
   printf("En attente de connexion ...\n");

   while ( !End )
   {
      connection_t * client = (connection_t*)malloc( sizeof( connection_t ) );

      if (( client->fd = accept( fd, (struct sockaddr *)&Server, (socklen_t*)&addrlen )) < 0 )
      {
         perror("Echec de l'acceptation de la connexion entrante.\n");
      }

      if ( client->fd > 0 )
      {
         if( pthread_create( &client->thread, NULL, networkProcess, (void *)client ) == 0 )
         {
            Clients.push_back( client );
            pthread_detach( client->thread );
         }
         else perror("Echec de la création du thread client pour la connexion entrante.\n");
      }
      else free( client );
   }

   printf("Cleaning...\n");
   Clients.clear();
   return EXIT_SUCCESS;
}
