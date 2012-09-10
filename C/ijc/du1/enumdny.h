// enumdny.h
// Reseni IJC-DU1, priklad a), 14.3.2008
// Autor: Matus Kontra, FIT
// Verzia prekladaca: 4.2.3
// popis - modul enumdny

#ifndef ENUMDNY_H
#define ENUMDNY_H

enum dny { Po=1, Ut, St, Ct, Pa, So, Ne };

void PrintDayShort(enum dny den);
void PrintDayShortASCII(enum dny den);

void PrintDayLong(enum dny den);
void PrintDayLongASCII(enum dny den);

enum dny ReadDay(void);

#endif // ENUMDNY_H

