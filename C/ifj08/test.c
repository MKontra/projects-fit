#include <stdio.h>
#include "str.h"

int main()
{

    string * s1 = strNewFromConstChar("abababcb");
    int * tab = pm_table(s1);
    for (int i = 0;i<strGetLength(s1);i++)
	printf("%d ", tab[i]);

}
