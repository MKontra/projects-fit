#include <string>
#include <iostream>
#include <istream>
#include <ostream>
#include <iterator>
#include <limits>
#include <cctype>
#include <algorithm>
#include <string>
#include <vector> 
#include <cmath>
#include <cstring>

#include <set>
#include <map>

#define MINLEN 3 
#define MAXLEN 8                   
#define MAXPERLEN 100
#define MAXTOTAL 100
#define LENDIVISOR 0.8
#define MINALLOWED 40

using namespace std;

typedef pair<int, int> intint;

struct SecondComp {
    bool operator()(const intint &lhs, const intint &rhs) {
        return lhs.second > rhs.second;
    }
} SecondCompStruct;

typedef struct
{
        int len;
        int count;
        double IoC;
        double Mul;
} CResultSet;

struct CResultSetComp
{
    bool operator()(const CResultSet &lhs, const CResultSet &rhs) {
        return lhs.Mul > rhs.Mul;
    }
} CResultSetCompStruct;

double freqs[]  =  {0.08167, 0.01492, 0.02782, 0.04253, 0.12702, 0.02228, 0.02015, 0.06094, 0.06966, 0.00153, 0.00772, 0.04025, 0.02406,
	0.06749, 0.07507, 0.01929, 0.00095, 0.05987, 0.06327,0.09056, 0.02758, 0.00978, 0.02360, 0.0015, 0.01974,  0.00074 };          
char alpha_low[] = "abcdefghijklmnopqrstuvwxyz";
char alpha_upp[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";

const long charlen = sizeof freqs / sizeof freqs[0];

//povolenie fixu repetici pri fallbacku
bool fixRepet = false;

long gcd(long a, long b)
{
	long c = a % b;
	while(c != 0)
	{
		a = b;
		b = c;
		c = a % b;
	}
	return b;
}

long gcdr( long a, long b )
{
     if ( b == 0 ) return a;
     else return gcdr( b, a%b );
}

//vypocet index of coincidence jedneho stlpca
//vypocitaj pocet vyskytov a aplikuj vzorec
double ColumnIOC( string * indata, unsigned long column, unsigned long maxcol )
{
	const char * plainarr = indata->c_str(); 
	unsigned long occur[charlen] = {0};

	for ( unsigned long i = 0; i < indata->size(); i++ )
		if ( i % maxcol == column )
			occur[plainarr[i]-'a']++;
			
	unsigned long csum =0;
	for ( long i = 0; i < charlen; i++ )
	{
		csum += occur[i] ;
	}		
  double sum = csum;	
	double ko = 0.0;
	for ( long i = 0; i < charlen; i++ )
	{
		ko = ko + ( (double)occur[i]*(occur[i]-1));
	}  	
	ko = ko / (  (double) sum*(sum-1));
	
	return ko;
}

//vypocitaj index of coincidenece pre zvolenu dlzku hesla
//aplikacia fukcie nad stlpcami
double IoCForLen(string * indata, int pwlen)
{
  double currentIOC = 0.0;
  for ( long j = 0; j < pwlen; j++ )
  {
    double tmp = ColumnIOC(indata, j, pwlen); 
  	currentIOC += tmp;
  } 
  currentIOC /= (double)pwlen;
  return currentIOC;
}

//Friedman - vypocitaj pocet vyskytov pismen aplikuj vzorec
double Friedman( string * indata )
{
	const char * plainarr = indata->c_str(); 
	long occur[charlen] = {0};

	for ( unsigned long i = 0; i < indata->size(); i++ )
			occur[plainarr[i]-'a']++; 
  	
	double sum = indata->size();

  
	double ko = 0.0;
	for ( long i = 0; i < charlen; i++ )
	{
		ko = ko + (occur[i]*(occur[i]-1));
	}  	

	ko = ko / ((double) sum * (sum-1));
  //cout << "----------" << ko << endl;
  //return (0.0265*sum)/(  (sum-1)*ko - 0.038*sum + 0.065  );
  //return (0.0265*sum)/(  (0.065 - ko )+ sum*(ko-0.0385)  );
	return (0.065-(1.0/26.0))/(ko-(1.0/26.0));    
}


//kasiskeho vysetrenie - vrat zoznam parov delitel - kvalitativny pocet vyskytov
//hladaj 8 - 3 gramy, maximum klesajuce
//ukladaj gcd ntic, a dlzku vzdial dvojic
//najdi delitele
//usporiadaj podla vhodnost - pocet vyskytov a vrat vysledok
vector<intint> * Kasiski( string * indata )
{
	const char * plainarr = indata->c_str(); 
  unsigned long strl = (unsigned long)indata->size();

  set<string> finds;
  set<string>::iterator sit;
  
  map<int, int> distances;
  map<int, int>::iterator dit;
  
  int maxcount = MAXPERLEN;
  
  //MAXLEN az MINLEN -gramy
  for ( int len = MAXLEN; len >= MINLEN; len-- )
  {                                                                
      int c = 0; //distcount
      for ( unsigned long i = 0; i < strl-len; i++ )
      {
      
          //sucasne hladany retazec
          string current(plainarr+i, len);
          
          //ak bol hladany n gram zahrnuty uz aj v inych hladaniach nehladaj
          bool containssub = false;
          for ( sit = finds.begin(); sit != finds.end(); ++sit )
          {
              if ( (*sit).find(current) != string::npos )
              {
                  containssub = true;
                  break;
              }
          }
          
          if ( containssub )
          {
             continue;
          } else
          {
            finds.insert(current);
          }
          
          //hladaj repeticie
          long lastdst = i;
          long lastdiff = 0; 
          long lastgcd = 0;         
          for ( unsigned long j = i+len; j < strl-len; )
          {
              //ak najdena
              if ( strncmp( current.c_str(), plainarr+j, len) == 0 )
              { 
                 if ( lastdiff != 0 )
                 {
                    lastgcd = gcd(j-lastdst, lastdiff);
                    distances[lastgcd]+=len;;  
                 }
                 lastdiff = j-lastdst;
                 lastdst = j;
                 c++;
                 if ( c > maxcount )
                    break;
                 j+= len;
              } else
              {
               j++;
              }
                     
          }
          //v retazci sa nachadza len 1 par repeticii daneho slova
          if ( lastgcd == 0 && lastdiff != 0)
          {  
             distances[lastdst-i]+= len;
          }  
          if ( c > maxcount )
             break;
      }
      //maximalny pocet kratsich len-1 gramov
      maxcount = max( MINALLOWED,(int)((double) maxcount * LENDIVISOR ));
  }

  map<int, int>::iterator gci;
  map<int, int>::iterator gco;

  map<int,int> gcds;
  
  //najdi delitele
	for ( gco=distances.begin() ; gco != distances.end(); gco++ )
	{  
  	for ( gci=gco ; gci != distances.end(); gci++ )
  	{
          gcds[ gcdr((*gco).first,(*gci).first) ]+= (*gco).second+(*gci).second;  //skore delitela urcuje pocet vyskytov paru
    }  
  }
  

  //zorad pole delitelov
  vector<intint> * gcdsorted = new vector<intint>();
	for ( dit=gcds.begin() ; dit != gcds.end(); dit++ )
	{
        gcdsorted->push_back(make_pair((*dit).first, (*dit).second));
  }
  
  sort (gcdsorted->begin(), gcdsorted->end(),  SecondCompStruct );


	return gcdsorted;
}

//vypocet dlzky hesla - vstup slovnik delitelov z kasiskeho
//pre prvych pat delitelov sa vypocita IoC 
//filtruju sa hesla dlhsie ako 10000 & IoC < 0.06
//prekadzy delitel sa spocita  log10(vyskytdelitela) + 6*IoC+ vyskytdelitela/vyskytbestdelitela
//vysledkom je najlepsi
//v pripade ze kasisky vrati 0 delitelov vrat dlzku s najlepsim IoC z rozsahu 1 - 200
long ExtrapolatePasswordLength( vector<intint> * kasiski, double friedman, string * indata)
{

  bool triggerFallback = false;

  vector<intint>:: iterator vit;
  vector<CResultSet>cumuldta;
  int i = 0;  
  double scale = (*kasiski)[0].second / (double) 0.06;
  for ( i = 0, vit = kasiski->begin(); vit != kasiski->end() && i < 5; ++vit, ++i )
  {
      if ( (*vit).first > 10000 ) continue; //prilis dlhe heslo - casovo narocne pocitat IoC
      double curr = IoCForLen(indata, (*vit).first );
      if ( curr < 0.06 || curr > 0.09 ) continue;         //nizka uroven IoC nepravdepodobne
      //cout << "len: " << (*vit).first << " count: " << (*vit).second << " IoC: " << curr << " mul:" << log10((*vit).second) + 6*curr+(*vit).second/scale  << endl;
      CResultSet tmp = {(*vit).first, (*vit).second, curr, log10((*vit).second) + 6*curr+(*vit).second/scale };
      cumuldta.push_back(tmp);  
  }
  sort (cumuldta.begin(), cumuldta.end(),  CResultSetCompStruct );
  
  double IoC1 = IoCForLen(indata, 1 );
  double IoC2 = IoCForLen(indata, 2 );
  if ( cumuldta.size() > 0 && (IoC1 > cumuldta[0].len || IoC2 > cumuldta[0].len) )
     return max(IoC1, IoC2);
 
  //fallback pri zlyhani kasiskeho vystupu - vypocitaj iterativne ioc a vrat 
  if ( triggerFallback || cumuldta.size() == 0)
  {
    //cout << "Fallback active..."  << endl;
    fixRepet = true;
    double bestIOC = 0.0;
    int bestPlen = 1;
  	for ( int i = 1; i < 200; i++ )
  	{
        double currentIOC = 0.0;
      	for ( long j = 0; j < i; j++ )
      	{
      		currentIOC += ColumnIOC(indata, j, i);
      	} 
        currentIOC /= i;
  
        if ( (currentIOC > bestIOC) ) 
        {
           bestIOC = currentIOC;
           bestPlen = i;
        }
    }  
    return bestPlen;
  } 
   
  return cumuldta[0].len;
}

//vrat pismeno reprezentujuce posun jedneho stlpca
char BreakColumn(string * indata, unsigned long column, unsigned long maxcol)
{

  //pocet vyskytov pismen v stlpci
	const char * plainarr = indata->c_str(); 
	long occur[charlen] = {0};

	for ( unsigned long i = 0; i < indata->size(); i++ )
		if ( i % maxcol == column )
			occur[tolower(plainarr[i])-'a']++;


	long sum =0;
	for ( long i = 0; i < charlen; i++ )
	{
		sum += occur[i] ;
	}  

	double occur_rel[charlen];
	for ( long i = 0; i < charlen; i++ )
	{
		occur_rel[i] = ((double)occur[i]) / (double) sum;
	}

  
  //Petersons chi sqare fit test
  //vypocet posunu s najmensim stvorcom chyby od standardnych pravdepodobnosti
	double minsqaredsum = numeric_limits<double>::max();
	char shift = 0;
	for (char c = 'a'; c <= 'z'; c++)
	{
		double sqsum = 0.0;
		for ( long i = 0; i < charlen; i++ )
		{
			long shifind = (i + (c - 'a')) % charlen;
			double diff = occur_rel[shifind];
			sqsum += ((diff*diff)/freqs[i]);
		}
		if ( sqsum < minsqaredsum )
		{
			minsqaredsum = sqsum;
			shift = c;
		}      
	}
 
	return shift;
}

//odhadni heslo
//iterativne volanie metody stlpca
string * ExtrapolatePassword( vector<intint> * kasiski, double friedman, long passlen, string * indata)
{

	char shifts[passlen+1];
	shifts[passlen] = 0;
	for ( long i = 0; i < passlen; i++ )
	{
		shifts[i] = BreakColumn(indata, i, passlen);
	}

	return new string(shifts);
}

//oprav repeticie pri fallback mechanizme
// --NEVOLA SA PRI NORMALNOM VYPOCTE - LEN PRI ZLYHANI KASISKEHO
void FixPlenPword( long * plen, string * pword )
{   
     long i = 2;
     bool done = false;
     for ( i = 2; (i < *plen/2 + 1) && !done; i++ )
     {
         if ( *plen % i != 0 )
            continue;
         
         string sub = pword->substr(0, i);
         int pos = i;
         while ( pos < *plen && pword->substr(pos, i) == sub )
               pos += i;
               
         if ( pos == *plen ) done = true;
   
     }
     
     if ( done == true )
     {
        *plen = --i;
        string sw = pword->substr(0, i);
        pword->swap(sw);
     }
     
}


bool not_alnum(char chr)
{
 return !isalpha(chr);
}

int main(int argc, char * argv[])
{
	//std::cin >> std::noskipws; //--ignorujeme whitespaces


  //nacitaj data zo vstupu do retazca
	istream_iterator<char> it(cin);
	istream_iterator<char> end;
	string * indata = new string(it, end);

  //vymaz nie pismena
	indata->erase(std::remove_if(indata->begin(), indata->end(), ::not_alnum), indata->end());
	//all -> tolower
	std::transform(indata->begin(), indata->end(), indata->begin(), ::tolower);

	if (indata->size() < MAXLEN*2) //proti segfaulty pri kratkom vstupe
	    return 0;

	//cout << *indata <<endl;

	vector<intint> * kasiski = Kasiski(indata);
	double friedman =          Friedman(indata);

	long passlen =             ExtrapolatePasswordLength(kasiski, friedman, indata);
	string * pass =            ExtrapolatePassword(kasiski, friedman, passlen, indata);
	
	//aktivovane pri zlom vysledku kasiskeho
  if (fixRepet) FixPlenPword( &passlen, pass);
  
  //vypis vysledok
	cout << friedman<< ";" << (*kasiski)[0].first << ";" << passlen << ";" << *pass << endl;

	return 0;
}