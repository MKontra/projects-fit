/*
	filename: tail2.cc
	meno: Kontra Matus	
	fakulta: FIT
	uloha: 2 - B
	17.4.2008
	compiler: G++-4.2.3
*/

#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <queue>
#include <cstdlib>
#include <cstdarg>

typedef std::queue<std::string>  queue_t; // typ fronty

int from_line(int num = 10, std::istream& fs = std::cin)
{
	if (fs.fail())
	{ printf("Zlyhalo otvorenie suboru\n");
	return 1;
	}

	std::string line;
	int inx = 0;
	queue_t fronta;
	while( getline(fs,line) && (!fs.eof() || line != "") ) {
		if (inx >= num)
			{
			fronta.push(line);
			}
        	inx++;	

	}


	while (!fronta.empty()) {
		std::cout<<fronta.front()<<std::endl;
		fronta.pop();
	}
	return 0;
}

int last_lines(int num = 10, std::istream& fs = std::cin)
{
	if (fs.fail())
	{ printf("Zlyhalo otvorenie suboru\n");
	return 1;
	}
	
	std::string line;
	int inx = 0;
	queue_t fronta;
	while( getline(fs,line) && (!fs.eof() || line != "") ) {
		
		fronta.push(line);
		if (inx >= num)
			{
	  		fronta.pop();
			}
	      inx++;

	}


	while (!fronta.empty()) {
		std::cout<<fronta.front()<<std::endl;
		fronta.pop();
	}
	
	return 0;
}

int main(int argc, char *argv[])
{

	int i = 1;
	int errorcode = 0;
	char c;
	std::ifstream * tmpptr = NULL;
	std::string allargs;

	while (i < argc)
	{
    		allargs.append(argv[i]);
    		allargs.append(" ");
		i++;
	}

	i=0;
	
	std::istringstream vstup (allargs , std::istringstream::in);

	if ( argc > 3 ){
		std::cout<<"Zla prikazova riadka"<<std::endl;
		errorcode = 1;
		}
	else if ( argc == 1 )
	{
	errorcode = last_lines();
	}
	else if ( vstup>>c>>i && (c =='+' || c == '-') && argc < 4)
	{
		if ( c == '+' )
			errorcode = from_line ( i , argc > 2 ? *(tmpptr = new std::ifstream ( argv[2] )) : std::cin );
		else
			errorcode = last_lines ( i , argc > 2 ? *(tmpptr = new std::ifstream ( argv[2] )) : std::cin );
	} 
	else if ( argc == 2 )
	{
	errorcode = last_lines ( 10 , *(tmpptr = new std::ifstream ( argv[1] )) );

	} else	{
		std::cout<<"Zla prikazova riadka"<<std::endl;
		errorcode = 1;
		}

	if ( tmpptr != NULL )
	{
	tmpptr->close();
	delete tmpptr; 
	}

return errorcode;
}

