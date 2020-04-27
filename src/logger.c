#include <logger.h>

void logger( char* level, char* file, int line, char* format, ... )
{
	va_list arglist;

	FILE* fp = fopen( LOG_FILE, "a+");

	time_t t = time( NULL );
	struct tm *lt = localtime( &t );
	char buf[ 32 ];

	buf[ strftime( buf, sizeof( buf ), "%Y-%m-%d %H:%M:%S", lt ) ] = '\0';
	
	if( strcmp( level, "error" ) == 0 )
		fprintf( fp, "%s %s [%s:%i] : ", buf, level, file, line );
	else
		fprintf( fp, "%s %s : ", buf, level );

	va_start( arglist, format );
	vfprintf( fp, format, arglist );
	va_end( arglist );

	fprintf( fp, "\n");
	fflush( fp );

	fclose( fp );
}