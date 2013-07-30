#include <stdio.h>

int some_function(int a)
{
    a *= 5;     // a is not really needed.
    return 42;
}

int main(int argc, char* argv[])
{
    int variableA = 10;

    printf("This is a test program.\n");

    variableA += 5;
    variableA += some_function(20);
    
    return variableA;
}
