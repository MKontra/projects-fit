#include <iostream>
#include <string>
#include <cstdio>
#include <cstdlib>
#include <ctime>

#include "msieve.h"
#include "getopt.h"
#include "gmp.h"

#define PROBPRIME 0
#define COMPOSITE 1

using namespace std;

gmp_randstate_t randstate;

//Modulo Exponentiation
//Zprava dol+lava bin=arna verzia
void mod_exp( mpz_t retval, mpz_t base, mpz_t exp, mpz_t mod )
{
  mpz_t result;
  mpz_init(result);
  mpz_set_ui(result, 1);
  
  mpz_t tmp;
  mpz_init( tmp );
  
  mpz_t tmpbase;
  mpz_init( tmpbase );
  mpz_set( tmpbase, base );
  
  size_t bitwidth = mpz_sizeinbase( exp, 2 );//bitova sirka cisla
  for ( unsigned long i = 0; i < bitwidth; i++ )
  {
    int bval = mpz_tstbit( exp, i );
    if ( bval == 1 )
    {
      mpz_mul( tmp, result, tmpbase );
      mpz_mod( tmp, tmp, mod );
      mpz_set( result, tmp ); 
    }
    mpz_mul( tmp, tmpbase, tmpbase );
    mpz_mod( tmp, tmp, mod );
    mpz_set( tmpbase, tmp );
  }
  mpz_set(retval, result);
  
  mpz_clear(result);  
  mpz_clear(tmp); 
  mpz_clear(tmpbase);    
}

//Miller Primality test
//vid wikipedia
int Miller_Rabin_test( mpz_t totest )
{
  int retval = PROBPRIME;
  
  mpz_t tmp; mpz_init(tmp);
  
  //najdi s, d z n-1 delenim 2
  mpz_t nm1; mpz_init(nm1);
  mpz_t s; mpz_init(s);
  mpz_t d; mpz_init(d);
  
  mpz_set_ui( s, 0 ); 
  mpz_sub_ui( nm1, totest, 1 ); //nm1 je sude
  mpz_set(tmp, nm1);
  while ( mpz_even_p(tmp) && (mpz_cmp_ui(tmp, 0) > 0) )
  {
    mpz_add_ui(s, s, 1);
    mpz_div_ui( tmp, tmp, 2 );
  }
  mpz_set( d, tmp );
 
  /**
  mpz_out_str( stdout, 10, s );cout << endl;
  mpz_out_str( stdout, 10, d );cout << endl;
  **/
  
  mpz_t a; mpz_init(a); 
  mpz_t x; mpz_init(x);
  mpz_t r; mpz_init(r);
  
  for ( int i = 0; i < 20; i++ )
  {
    //a - rozsah 2, n-2
    mpz_urandomm(a, randstate, nm1 );
    while ( mpz_cmp_ui(a, 2) < 0 )
      mpz_urandomm(a, randstate, nm1 );   
    
    mod_exp( x, a, d, totest );
    //ukonci a pokracuj
    if ( mpz_cmp_ui(x, 1) == 0 || mpz_cmp(x, nm1) == 0 ) continue;
    
    for ( mpz_set_ui(r, 1); mpz_cmp(r, s) <= 0; mpz_add_ui(r,r,1) )
    {       
      mpz_set(tmp, x);
      mpz_mul(tmp,tmp,tmp);
      mpz_mod(tmp, tmp, totest);
      mpz_set(x, tmp);
      
      if ( mpz_cmp_ui(x, 1) == 0 )
      {
        retval = COMPOSITE;
        break;
      }
      if ( mpz_cmp(x, nm1) == 0 ) 
      {
        break;     
      }
    }
    if ( retval == COMPOSITE ) break;
    if ( ! (mpz_cmp(r, s) <= 0) ) { retval = COMPOSITE; break; }
  
  }
  
  mpz_clear(nm1);
  mpz_clear(s);
  mpz_clear(d);
  mpz_clear(a);
  mpz_clear(x);
  mpz_clear(r);
  mpz_clear(tmp);
  
  return retval;
}

//klasicky euklidov algoritmus
void cust_mpz_gcd( mpz_t result, mpz_t a, mpz_t b )
{
  mpz_t tmp; mpz_init(tmp);
  mpz_t tmpa; mpz_init(tmp); mpz_abs(tmpa, a);
  mpz_t tmpb; mpz_init(tmp); mpz_abs(tmpb, b);
  
  if ( mpz_cmp(tmpb, tmpa) > 0 )
    mpz_swap(tmpa, tmpb);
  
  while ( mpz_cmp_ui( tmpb, 0) > 0 )
  {
    mpz_set( tmp, tmpb);
    mpz_mod( tmpb, tmpa, tmpb );
    mpz_set( tmpa, tmp );
  }
  
  mpz_set(result, tmpa);
  mpz_clear(tmp);
  mpz_clear(tmpa);
  mpz_clear(tmpb);
}

//rozsireny euklidov algoritmus
//vid wikipedia
void ext_gcd( mpz_t resx, mpz_t resy, mpz_t a, mpz_t b )
{
  mpz_t x; mpz_init_set_ui(x, 0);
  mpz_t y; mpz_init_set_ui(y, 1);
    
  mpz_t lastx; mpz_init_set_ui(lastx, 1);
  mpz_t lasty; mpz_init_set_ui(lasty, 0);
  
  mpz_t tma; mpz_init_set(tma,a);
  mpz_t tmb; mpz_init_set(tmb,b); 
  
  mpz_t quot; mpz_init(quot);
  mpz_t tmp2; mpz_init(tmp2);
  
  while ( mpz_cmp_ui( tmb, 0 ) != 0 )
  {
    mpz_div( quot, tma, tmb );
    
    mpz_set( tmp2, tma );
    mpz_set( tma, tmb );
    mpz_mod( tmb, tmp2, tmb );
    
    mpz_set( tmp2, lastx );
    mpz_submul(tmp2, quot, x);
    mpz_set( lastx, x );
    mpz_set( x, tmp2 );
    
    mpz_set( tmp2, lasty );
    mpz_submul(tmp2, quot, y);
    mpz_set( lasty, y );
    mpz_set( y, tmp2 ); 
  }
  mpz_set(resx, lastx);
  mpz_set(resy, lasty);
  
  mpz_clear(x);
  mpz_clear(y);
  mpz_clear(lastx);
  mpz_clear(lasty);
  mpz_clear(tma);
  mpz_clear(tmb);
  mpz_clear(quot);
  mpz_clear(tmp2); 
}

//inverzne modulo
//pouzije sa rozsireny euklid
void inverse_mod( mpz_t result, mpz_t in, mpz_t mod )
{
  //ext_gcd
  mpz_t x,y;mpz_init(x);mpz_init(y);
  ext_gcd( x,y, in, mod );
  
  if ( mpz_cmp_ui( x, 0 ) < 0 )
    mpz_add( x, x, mod );
  
  mpz_set( result, x );
  
  mpz_clear(x);
  mpz_clear(y);
  
}

//funkcia generatora prvocisel
void get_prime_pair_of_width( mpz_t r1, mpz_t r2, int width )
{
  int hw = width / 2;
  mpz_t p; mpz_init(p); mpz_urandomb(p, randstate, hw);
  mpz_setbit(p, 0);
  mpz_setbit(p, hw-1);
  mpz_setbit(p, hw-2);
  while ( Miller_Rabin_test(p) == COMPOSITE )
    mpz_add_ui(p, p, 2);
  
  int hw2 = width - hw;
  mpz_t q; mpz_init(q); mpz_urandomb(q, randstate, hw2);
  mpz_setbit(q, 0);
  mpz_setbit(q, hw2-1);
  mpz_setbit(q, hw2-2);
  while ( Miller_Rabin_test(q) == COMPOSITE )
    mpz_add_ui(q, q, 2);  
  
  mpz_set(r1, p); mpz_set(r2, q);
  mpz_clear(p);mpz_clear(q);
  
}

//vypocet d
void get_d( mpz_t rd, mpz_t e, mpz_t phi)
{
  inverse_mod( rd, e, phi );
}

//vypocet e
void get_e(mpz_t re, mpz_t phi)
{
  mpz_t gcdres; mpz_init(gcdres);
  
  if ( mpz_cmp_ui(phi, 3) >= 0 )
  {
    mpz_gcd_ui( gcdres, phi, 3 );
    if ( mpz_cmp_ui( gcdres, 1 ) == 0 )
    {
      mpz_set_ui(re, 3 );
      mpz_clear(gcdres); 
      return;
    } 
  } 
  
  if (  mpz_cmp_ui(phi, 17) >= 0 )
  {
    mpz_gcd_ui( gcdres, phi, 17 );
    if ( mpz_cmp_ui( gcdres, 1 ) == 0 )
    {
      mpz_set_ui(re, 17 );
      mpz_clear(gcdres); 
      return;
    } 
  }
     
  if (  mpz_cmp_ui(phi, 65537) >= 0 )
  {
    mpz_gcd_ui( gcdres, phi, 65537 );
    if ( mpz_cmp_ui( gcdres, 1 ) == 0 )
    {
      mpz_set_ui(re, 65537 );
      mpz_clear(gcdres); 
      return;
    } 
  } 

  mpz_t tmp2; mpz_init(tmp2);

  do
  {
    mpz_urandomm(tmp2, randstate, phi);
    if ( mpz_cmp_ui(tmp2, 1) <=0 ) continue;
    mpz_gcd( gcdres, tmp2, phi);
    
  } while ( mpz_cmp_ui( gcdres, 1 ) != 1 );
  
  mpz_set(re, tmp2 );
  
  mpz_clear(gcdres); 
  mpz_clear(tmp2); 
}

void genkeys( int keywidth )
{
  //cout <<"generating keys" <<endl;
  mpz_t p;mpz_init(p);
  mpz_t q;mpz_init(q);

  get_prime_pair_of_width(p,q, keywidth);
  
  mpz_t n; mpz_init(n);
  mpz_t phi; mpz_init(phi);
  
  mpz_t tmp; mpz_init(tmp);
  
  mpz_mul(n, p, q);
  
  mpz_sub_ui(phi, p, 1);
  mpz_sub_ui(tmp, q, 1);
  mpz_mul( phi, phi, tmp);
  
  //calculate e
  mpz_t e; mpz_init(e);
  get_e( e, phi);
  
  //calculate d
  mpz_t d; mpz_init(d);
  get_d( d, e, phi);
  
  cout << "0x"; mpz_out_str( stdout, 16, p ); cout << " ";  
  cout << "0x"; mpz_out_str( stdout, 16, q ); cout << " "; 
  cout << "0x"; mpz_out_str( stdout, 16, n ); cout << " "; 
  cout << "0x"; mpz_out_str( stdout, 16, e ); cout << " "; 
  cout << "0x"; mpz_out_str( stdout, 16, d ); cout << endl; 
  
  mpz_clear(p);
  mpz_clear(q);
  mpz_clear(n);
  mpz_clear(phi);
  mpz_clear(e);
  mpz_clear(d);
  mpz_clear(tmp);
}

void encrypt( mpz_t E, mpz_t N, mpz_t M)
{
  //cout <<"encrypting" <<endl;
  
  mpz_t result;
  mpz_init(result);
  mpz_set_ui(result, 0);
  
  mod_exp( result, M, E, N );
  cout << "0x"; mpz_out_str( stdout, 16, result ); cout << endl; 
  mpz_clear(result);
}

void decrypt( mpz_t D, mpz_t N, mpz_t C)
{
  //cout <<"decrypting" <<endl;
  mpz_t result;
  mpz_init(result);
  mpz_set_ui(result, 0);
  
  mod_exp( result, C, D, N );
  cout << "0x"; mpz_out_str( stdout, 16, result ); cout << endl; 
  mpz_clear(result);
}

void factor( mpz_t E, mpz_t N, mpz_t C)
{
    //cout <<"breaking keys" <<endl;
    
    char *n = mpz_get_str( NULL, 10, N );
    
    srand ( time(NULL) );
    uint seed1 = rand();
    uint seed2 = rand();
    
    enum cpu_type cpu;
    uint cache_size1;
    uint cache_size2;
    
    get_cache_sizes(&cache_size1, &cache_size2 );
    cpu = get_cpu_type();
    
    uint flags = 0;
    /**
    flags |= MSIEVE_FLAG_USE_LOGFILE;
    flags |= MSIEVE_FLAG_NFS_POLY1 | MSIEVE_FLAG_NFS_POLY2;
    flags |= MSIEVE_FLAG_NFS_SIEVE;
    flags |= MSIEVE_FLAG_NFS_FILTER;
    flags |= MSIEVE_FLAG_NFS_LA;
    flags |= MSIEVE_FLAG_NFS_SQRT;
    **/   
    
    msieve_obj *current_factorization = //pre popis vid msieve.h
        msieve_obj_new( 
          n,
          flags,
          NULL,
          NULL,
          NULL,
          seed1,seed2,
          0,
          0,
          0,
          cpu,
          cache_size1,
          cache_size2,
          0,
          0,
          0,
          0.0);
    
    msieve_run( current_factorization );
    
    mpz_t p; mpz_init(p);
    mpz_t q; mpz_init(q);
    
    int factcount = 0;
    
    msieve_factor *factor = current_factorization->factors;
    while ( factor != NULL )
    {
      if (factcount == 0) mpz_set_str(p, factor->number, 10);
      if (factcount == 1) mpz_set_str(q, factor->number, 10);
      factor = factor->next;
      factcount++;
    }
    
    cout << "0x"; mpz_out_str( stdout, 16, p ); cout << " "; 
    cout << "0x"; mpz_out_str( stdout, 16, q ); cout << " "; 
    
    if ( factcount > 2 ) 
    { 
      cout<< "Too many factors\n";     
      mpz_clear(p);
      mpz_clear(q);
      return; 
    }
    
    mpz_t phi; mpz_init(phi);
    mpz_t tmp; mpz_init(tmp);

    mpz_sub_ui(phi, p, 1);
    mpz_sub_ui(tmp, q, 1);
    mpz_mul( phi, phi, tmp);
    
    //calculate d
    mpz_t d; mpz_init(d);
    get_d( d, E, phi);
    
    //decrypt
    mpz_t result;
    mpz_init(result);
    mpz_set_ui(result, 0);
  
    mod_exp( result, C, d, N );
    
    cout << "0x"; mpz_out_str( stdout, 16, result ); cout << endl; 

    mpz_clear(p);
    mpz_clear(q);
    mpz_clear(result);
    mpz_clear(phi);
    mpz_clear(d);
    mpz_clear(tmp);
}

int main( int argc, char* argv[] )
{
  gmp_randinit_default(randstate);
  gmp_randseed_ui( randstate, (unsigned long) time(NULL) );
  
  if ( argc < 2 )
  {
    cout << "Missing operation parameter\n";
    return -1;
  }
  
  string param1(argv[1]);
  if ( (param1[0] != '-') || (param1.size() != 2) )
  {
    cout << "Parameter mismatch\n";
    return -1;
  }
    
  int op = 0;
  switch (param1[1])
  {
    case 'g': op = 1;break;
    case 'e': op = 2;break;
    case 'd': op = 3;break;
    case 'b': op = 4;break;
    default: cout << "Bad operation parameter\n"; return -1;
  }
  
  if ( op == 1  && argc == 3 )
  {
    int genwidth = atoi( argv[2] );
    if ( genwidth == 0 ) { cout  << "Bad bitwidth specifier\n"; return -1; }
    genkeys (genwidth);
  } else
  {
    if ( argc != 5 )
    {
      cout << "Parameter mismatch\n";
      return -1;
    }
    
    mpz_t in1; mpz_init(in1);
    mpz_t in2; mpz_init(in2);
    mpz_t in3; mpz_init(in3);
    
    if ( mpz_set_str(in1, argv[2], 0) == -1 )
    {
      cout << "Error parsing input numerals position 1\n";
      return -1;
    }
    
    if ( mpz_set_str(in2, argv[3], 0) == -1  )
    {
      cout << "Error parsing input numerals position 2\n";
      return -1;
    }

    if ( mpz_set_str(in3, argv[4], 0) == -1)
    {
      cout << "Error parsing input numerals position 3\n";
      return -1;
    }
       
          
    switch (op)
    {
      case 2: encrypt( in1, in2, in3 ); break;
      case 3: decrypt( in1, in2, in3 ); break;
      case 4: factor ( in1, in2, in3 ); break;    
      default: cout << "Unreachable?\n";
    }  
    
    mpz_clear(in3);  
    mpz_clear(in2); 
    mpz_clear(in1);     
  } 

}