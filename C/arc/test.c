#include<stdio.h>

int main(int argc, char * argv[])
{
	volatile int i = 0;
	volatile int j = 1;
	volatile double var = 55.0;
	printf("%d\n", (i == j)*var);
}
