#include <stdio.h>

#define N 1000
#define STARTN 80000000
double pi( long n )
{
     double pi = 4.0;
     double decimal = 1.0;
     while( n > 2 )
    {
        decimal = decimal - ( 1.0 / ( 2.0 * n + 1 ) );
        --n;
        decimal = decimal +( 1.0 / ( 2.0 * n + 1 ) );
        --n;
    }
    if( n > 0 )
        decimal = decimal - ( 1.0 / ( 2.0 * n + 1 ) );
    return( pi * decimal );
}

int main()
{
    long i = 0; 
    for (i = 1; i <= N; i++)
        printf("Loop %d: Pi is %20.18f\n", i, pi(i*2+1+STARTN));
    return 0; 
}
