// error.c
// Reseni IJC-DU1, priklad a), 14.3.2008
// Autor: Matus Kontra, FIT
// Verzia prekladaca: 4.2.3
// popis - implementacia funkcii modulu error...

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#include "error.h"

void
Error (const char *fmt, ...)
{
    va_list vl;
    
    va_start (vl,fmt);
    fprintf (stderr, "CHYBA: ");
    vfprintf (stderr, fmt, vl);
    va_end (vl); 

    exit(1);
}

