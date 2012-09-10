IPK projekt 3
xkontr00

Protokol a funkcnost programov
Na strane klienta sa v odosielacom okne udrzuje stav odoslanych paketov. Okno je reprezentovane frontou.
Po prijati acknowledgu sa zisti ci je potvrdeny paket z okna a oznaci sa za prijaty. Ak je prijaty paket zo zaciatku okno sa posuva doprava.
Preposielanie je riesene casovacom ktory za zopina kazdych 200 ms a kontroluje ci su v okne nepotvrdene pakety, ak take najde preposle znova.
Na vstupne data sa caka volanim selectu. Pokialu su data na stdin a v okne je miesto data sa tam vlozia a poslu. okial prisiel paket od servera najde sa dane sekvencne cislo a paket sa potvrdi. Ukoncenie spojenia sa indikuje severu specialnym paketom obsahujucim cislo poslednej sekvencie, ten sa preposiela kym nieje potvrdeny.

Na strane servra sa caka na data ok klienta. Velkost receive okna je 4 a server umoznuje out-of-order delivery. Po prijati paketu server skontroluje sekvencvne cislo
a vlozi ho do fronty ktora ho umiestni na spravnu poziciu. Po tomto sa kontroluje ci mozme primacie okno posunut. Skoncenie je indikovane specialnym paketom od klienta ktory sa potom potvrdi.

Paket ma jednoduchu strukturu obsahuje operacny kod, chceksum a cislo sekvencie, za nimi nasleduje data.