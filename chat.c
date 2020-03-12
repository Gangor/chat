#include "chat.h"

void signal_callback_handler( int signum )
{
   close( Socket );
   end = 1;
}

void* networkProcess( void* args )
{
   int newSocket = *((int *)args);
   
   pthread_exit( NULL );
}

int main(int argc, char *argv[])
{
   //daemon( 0, 0 );

   signal( SIGINT, signal_callback_handler );
   signal( SIGTERM, signal_callback_handler );

   int Socket, i = 0, opt = 1;
   pthread_t threads[ MAX_CLIENT ];
   //struct sockaddr_in client;

   if ( argc < 2 )
   {
      printf( "Utilisation : %s port\n", argv[0] );
      exit( EXIT_FAILURE );
   }
   
   if (( Socket = socket( AF_INET, SOCK_STREAM, 0 )) < 0 )
   {
      perror( "socket failed" );
      exit( EXIT_FAILURE );
   }

   if( setsockopt( Socket, SOL_SOCKET, SO_REUSEADDR, (char *)&opt, sizeof( opt ) ) < 0 )   
   {
      perror( "Echec de l'application des options socket." );
      exit( EXIT_FAILURE );
   }

   int port = atoi( argv[1] );

   server.sin_family = AF_INET;
   server.sin_addr.s_addr = INADDR_ANY;
   server.sin_port = htons( port );

   if ( bind( Socket, ( struct sockaddr * ) &server, sizeof( server ) ) < 0 )
   {
      perror("Echec de la liaison du port d'écoute.");
      exit( EXIT_FAILURE );
   }

   if ( listen( Socket, 5 ) )
   {
      perror( "Echec de l'écoute." );
      exit( EXIT_FAILURE );
   }

   socklen_t addrlen = sizeof( server );

   printf( "Ecoute du port : %d \n", port );
   puts("En attente de connexion ...");

   while ( !end )
   {
      int fdclient;

      if (( fdclient = accept( Socket, (struct sockaddr *)&server, (socklen_t*)&addrlen )) < 0 )
      {
         perror("Echec de l'acceptation de la connexion entrante");
         exit( EXIT_FAILURE );
      }

      printf( "New connection , socket fd is %d, ip is : %s, port : %d\n", 
         fdclient,
         inet_ntoa( server.sin_addr ),
         ntohs( server.sin_port ));

      if( pthread_create( &threads[i], NULL, networkProcess, &fdclient) != 0 )
      {
         perror("Echec de la création du thread client pour la connexion entrante.");
      }

      i++;
   }

   return EXIT_SUCCESS;
}
