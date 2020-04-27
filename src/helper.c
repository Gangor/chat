#include <helper.h>

int stricmp (const char *p1, const char *p2)
{
    char c1, c2;
    do
    {
        c1 = toupper( *p1 );
        c2 = toupper( *p2 );

        if (c1 == '\0')
            return c1 - c2;

        if ( c1 != c2 )
            return c1 - c2;
    }
    while( *p1 == *p2 );

    return c1 - c2;
}