#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <math.h>


const double IZP_E = 2.7182818284590452354;	// e
const double IZP_PI = 3.14159265358979323846;	// pi
const double IZP_2PI = 6.28318530717958647692;	// 2*pi
const double IZP_PI_2 = 1.57079632679489661923;	// pi/2
const double IZP_PI_3_2 = 4.71238898038468985769;	// 3*pi/2
const double IZP_PI_4 = 0.78539816339744830962;	// pi/4
const double IZP_LN_2 = 0.69314718055994530941723212145818;	//ln2

typedef struct cstruc
{				//struktura pouzita na volanie
  int code;			//kod volanej funkcie vid nizsie
  double x;			//hodnota pre ktoru funkciu volame
  double eps;			//presnot s ktoru funkciu volame
} CStruct, *p_CStruct;

double integrate (double x, double y, double step, double (*fnc) (double));
double integratelch (double x, double y, double step, double (*fnc) (double));
int pre_vstup (p_CStruct pcs);
double sinus (p_CStruct pcs);
double cosinus (p_CStruct pcs);
double ln (p_CStruct pcs);
double priemer (p_CStruct pcs);
double stddev (p_CStruct pcs);
double errmsg (p_CStruct pcs);
double  tangens(p_CStruct pcs);
double tangens_rec(double x,double n,double pres);

typedef double (*mfncptr) (p_CStruct pcs);	//typ ukazatela na mat. funkcie

static const mfncptr FArray[] = {	//kazda funkcia je urcena jej poziciou v poli t.j ma svoju ciselnu hodnotu
  errmsg,			//F0 -chybne parametre
  sinus,			//F1 -sinus
  cosinus,			//F2 -cosinus
  ln,				//F3 -prirodzeny logaritmus
  priemer,			//F4 -priemer
  stddev,			//F5 -standardna odchylka
  tangens			//F6 -tangens
};


double
sinus (p_CStruct pcs)
{				//funkcia sinus
  double x = pcs->x;
  double sig = 1;
  
  if (x < 0)				//osetrenie zaporneho cisla na zaklade neparnosti funkcie sinus
    {
      x *= -1;
      sig *= -1;
    }	

    x = fmod (x, IZP_2PI);
//  x = x - IZP_2PI * (double) floor (x / IZP_2PI);	//prevedenie x do intervalu 0 - 2pi
  
  printf("NewX: %.10le\n",x);

  if (x > 0 && x < IZP_PI_2);	//prevedenie cisla do intervalu 0 - pi/2 a nalezita ymena znamienka
  if (x >= IZP_PI_2 && x < IZP_PI)
    {
      x = IZP_PI - x;
    }
  if (x >= IZP_PI && x < IZP_PI_3_2)
    {
      x = x - IZP_PI;
      sig *= -1;
    }
  if (x >= IZP_PI_3_2 && x < IZP_2PI)
    {
      x = IZP_2PI - x;
      sig *= -1;
    }

  double t = x;			// t - n-ty clen
  double s = t;			//s - sucet prvy clen postupnosti ma hodnotu x a rovno ho priradime
  const double x2 = x * x;	//x2 - 2 ha mocnina x aby sa nemuseli vypocitavat pri kazdej iteracii zavedieme ju ako contantu
  double k = 1;			//k - premenna pouzita na vypocet faktorialu
  while (fabs (t) > pcs->eps)
    {				//cyklus while podmienka urcujuca presnost
      t = (-t * (x2)) / ((k + 1) * (k + 2));	//vypocet n teho clena
      k += 2;
      s += t;
    }

  return sig * s;		//aplikovanie znamienka na cislo a navrat hodnoty
}

double
cosinus (p_CStruct pcs)
{
  double x = pcs->x;
  double sig = 1;
  if (x < 0)
    {
      x *= -1;
    }
  x = fmod (x, IZP_2PI);

  if (x > 0 && x < IZP_PI_2);
  if (x >= IZP_PI_2 && x < IZP_PI)
    {
      x = IZP_PI - x;
      sig *= -1;
    }
  if (x >= IZP_PI && x < IZP_PI_3_2)
    {
      x = x - IZP_PI;
      sig *= -1;
    }
  if (x >= IZP_PI_3_2 && x < IZP_2PI)
    {
      x = IZP_2PI - x;
    }
  double t = 1;
  double s = t;
  const double x2 = x * x;
  unsigned long long int k = 0;
  while (fabs (t) > pcs->eps)
    {
      t = (-t * (x2)) / ((k + 1) * (k + 2));
      k += 2;
      s += t;
    }

  return sig * s;
}

double
x_1 (p_CStruct pcs)
{
  return 1 / pcs->x;
}

double  tangens(p_CStruct pcs){
	
	double x = pcs->x;
	double sig = 1;
	if (x < 0)		//osetrenie zaporneho cisla
	{
		x *= -1;
		sig*= -1;
	}
	x = fmod (x, IZP_PI);	//interval 0 - PI
	
	if (pcs->eps+IZP_PI_2>x && IZP_PI_2-pcs->eps<x) return NAN; //Zistim ci cislo patri do def intervalu
								    //tan nie je definovany na PI/2+k*PI
	if (x>IZP_PI_2) x-=IZP_PI_2;		//interval 0-> PI/2

	
	if(x==0) return 0;		//hodnota tan(0) je 0
	
	return 1/tangens_rec(pcs->x,1,pcs->eps);	//zavolanie rekurzivnej funkcie
	
}

double tangens_rec(double x,double n,double pres){
	double tmp = n/x;
	if (1/tmp<pres) return tmp; else return tmp-1/tangens_rec(x,n+2,pres);
}
				
				
double
ln (p_CStruct pcs)
{
  double x = pcs->x;
  if (x < 0)
    return NAN;			//vstup mimo def obor
  if (x == 0)
    return -INFINITY;		// vstup 0
  if (x == INFINITY)
    return INFINITY;		// vstup +nekonecno

  double result = 0;

  if (x >= 2)
    {				//redukcia argumentov ak x>2 delime 2 kym sa nedostaneme do intervalu 0-2 a 
      while (x >= 2)
	{
	  x /= 2;
	  result += IZP_LN_2;	//do hodnoty result pricitavame konstantu ln(2)
	}
    }
  else				
  if (x < 1)
    {
      while (x < 1)
	{
	  x *= 2;
	  result -= IZP_LN_2;	//pre cisla mensie ako 2 odcitavame
	}
    }				

  double t = x - 1;		//n - ty clen pociatocna hodnota je hodnota prveho clena
  double s = t;			//sucet
  double k = 2;			//konstanta
  double tpow = t;		//pomocna premenna na uchovanie mocniny clena t
  const double t1 = x - 1;	//konstanta ktoru sa nasoby clen v kazdj iteracii
  while (fabs (t) > (pcs->eps))
    {				//podmienka ukoncenia vzhladom na pozadovanu presnost
      tpow = -tpow * t1;	//vypocitanie pomocnej premennej -t*(x-1)
      t = tpow / k;		//vydelime k a dostaneme n ty clen
      s += t;			//pricitanie
      k++;
    }

  return s + result;		//aplikacia vypocitanej kompenzaciee a navrat hodnoty


}

double
integrate (double x, double y, double step, double (*fnc) (double))
{
  double s = 0;
  while (x < y)		//vypocet inegralu obdlznikovou metodou
    {
      s += fabs (((*fnc) (x)) * step);
      x += step;
    }
  return s;

}

double
integratelch (double x, double y, double step, double (*fnc) (double))
{
  double s = 0;
  double y1;
  while (x < y)
    {
      y1 = (*fnc) (x);
      s += fabs(y1 * step);
      x += step;
      s += fabs(((*fnc) (x) - y1) * step / 2);
    }
  return s;

}


double
priemer (p_CStruct pcs)
{				//priemer
  static unsigned long long count;	//staticka lokalna premenna na uchovanie poctu prvkov
  static double suc;		//staticka premenna na uchovanie doterajsieho suctu hodnot
  return (suc += pcs->x) / ((double) (++count));	//vypocet priemeru - pripocitame nove cislo k doterajsiemu suctu a vyelime poctom zvysenim o jednotku

}

double
stddev (p_CStruct pcs)
{				//vypocet standardnej odchylky
  static unsigned long long count;	//pocet prvkov
  static double sumx;		//suma 2hych mocnin
  double avg = priemer (pcs);	//priebezny priemer - nepocita sa vzdy znova len sa pripocita nova hodnota  vydeli poctom zvysenim o jeden
  count++;
  sumx += pcs->x * pcs->x;	//prpocitanie dalsej druhej mocniny
  return count ==
    1 ? 0 : sqrt ((1 / ((double) count-1)) *
		  (sumx - (double) count * avg * avg));
  /* samotny vypocet pri 1. prvku vrati 0
     inak na zaklade priebezneho priemeru a priebezneho suctu 2 mocnin vypocita odchylku */

}

double
errmsg (p_CStruct pcs)
{				//vypise chybovu hlasku pri zlych parametroch

	printf(	  "Program na vypocet roznych matematickych funkcii\n"
			"Program vykonava vypocty operacii: sin (x), cos (x), ln (x), tan (x), standardna odchylka, priemer\n"
		  "      , ... kde pri goniometrickych funkciach je x v radianoch.\n"
		  "        Cisla sa nacitavaju zo standardneho vstupu\n"
		  "        a to az do prveho vyskytu EOF(koniec vstupu/suboru)\n"
		  "        DOS/WIN - CTRL+Z, UNIX - CTRL+D).\n"
		  "        chybny vstup sa preskakuje a vrati sa NaN\n"
	
		  "Pouzitie: proj1 [jeden z -sin,-cos,-ln,-tan] presnost \n"
		  "          proj1 -mv \n"    // priemer
		  "          proj1 -stdev \n" // smerodatna odchylka
			
		  "Popis parametrov:\n"
		  
		  "  operacia   Operacia, ktora sa ma vykonat na nacitanych cislach (-sqrt, -cos,\n"
		  "              -ex, -ln, -tan, -cotg).\n"
		  "  presnost   Povolena odchylka od skutocnej hodnoty.\n"
		  "             Presnost je povolena absolutna (0.0), ale nie zaporna.\n"
		  "             Ak parametrom nie je cislo, pocita sa s absolutnou presnostou.\n"
		  "Definicne obory funkcii:\n"
		  "   -cos   - (-INFINITY;INFINITY)\n"
		  "   -sin   - (-INFINITY;INFINITY)\n"
		  "   -ln    - (0;INFINITY)\n"
		  "   -tan   - (-INFINITY;INFINITY)-{PI/2 + k*PI; k je zo Z}\n");
		  
		  
  return 1;
}

int
pre_vstup (p_CStruct pcs)
{				//zavola zvolenu funkciu pre kazdu hodnotu na vstupe
  int i;
  while ((i = scanf ("%lf", &pcs->x)) != EOF)	//ak eof koniec
    if (i == 1)
      {
	printf ("%.10le\n", (*FArray[pcs->code]) (pcs));//vybrie funkciu z tabulky a zavola ju
      }
    else
      {
	printf ("nan\n");
	scanf ("%*[^\n\t ]");//preskocenie zleho vstupu
      };
  return 0;
}

CStruct
processArgs (int count, char *vector[]) //funkcia vracajuca strukturu naplnenu kodom funkcie a presnostou
{

  CStruct cs;
  cs.code = 0;
  cs.eps = 0;

  if (count == 3 && strcmp (vector[1], "-sin") == 0
      && (sscanf (vector[2], "%lf", &(cs.eps)) == 1))
    cs.code = 1;
  
  if (count == 3 && strcmp (vector[1], "-cos") == 0
      && (sscanf (vector[2], "%lf", &(cs.eps)) == 1))
    cs.code = 2;
  
  if (count == 3 && strcmp (vector[1], "-ln") == 0
      && (sscanf (vector[2], "%lf", &(cs.eps)) == 1))
    cs.code = 3;
  
  if (count == 2 && strcmp (vector[1], "-mv") == 0)
    cs.code = 4;
  
  if (count == 2 && strcmp (vector[1], "-stddev") == 0)
    cs.code = 5;
  
  if (count == 3 && strcmp (vector[1], "-tan") == 0
      && (sscanf (vector[2], "%lf", &(cs.eps)) == 1))
	  cs.code = 6;

  if (cs.eps < 0)
    cs.code = 0;
  
  return cs;
}


int
main (int argc, char *argv[])
{
  CStruct cs = processArgs (argc, argv);
  if (cs.code == 0) errmsg(&cs);
	  else pre_vstup (&cs);
}
