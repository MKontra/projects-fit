#! /bin/bash

#defaultne hodnoty premennych

ODIR=./comments
INRANK=hodnoceni.txt
INSTUD=students.txt
RANGE=A-Z

# param parsing

while [ $# -gt 0 ]; do
    case "$1" in
	-o )
	    if [ -z "$2" ]; then
		echo "Error: Nezadany parameter !!!"
		exit 
	    fi
	    #-o zmen vystupny adresar
	    ODIR="$2"
	    shift 
	    ;;
	-h )
	    #-h zmen vstupny subor hodnoteni
	    if [ -z "$2" ]; then
		echo "Error: Nezadany parameter !!!"
		exit 
	    fi
	    INRANK="$2"
	    shift
	    if [ ! -e $INRANK ]; then
		echo "Error: Subor vstupnych hodnoteni neexistuje !!!"
		exit 1
	    fi
	    ;;    
	-s )
	    if [ -z "$2" ]; then
		echo "Error: Nezadany parameter !!!"
		exit 
	    fi
	    #-s zmen vstupny subor studentov
	    INSTUD="$2"
	    shift
	    if [ ! -e $INSTUD ]; then
		echo "Error: Subor zoznamu studentov neexistuje !!!"
		exit 1
	    fi
	    ;;
	-v )
	    if [ -z "$2" ]; then
		echo "Error: Nezadany parameter !!!"
		exit 
	    fi
	    #-v vstupny pattern
	    RANGE="$2"
	    shift 
	    ;;    
    esac
    shift
done

mkdir $ODIR 2>/dev/null

#zmena oddelovaca zaznamov bashu na novy riadok koli prikazu for
OLDIFS=$IFS
NEWLINE=$'\012'
IFS=$NEWLINE

for line in $( cat $INSTUD )
do
usrname=`echo $line | cut -d ';' -f1`
logname=`echo $line | cut -d ';' -f2 | sed 's/[ \t\n]*//g'`

echo "$usrname ($logname) "> $ODIR/$logname.txt

grep -i '^[ \t]*'$logname'[ \t]*;[ \t]*['$RANGE'][ \t]*;.*$' $INRANK | sort -t ';' -k2 |
 sed "s/^[ \t]*[^;]\+[ \t]*;[ \t]*\(.\)[ \t]*;[ \t]*\([[:digit:]]\+\)[ \t]*;\(.*\)/\1 : \2\x0aKomentar \1 : \3/g" >> $ODIR/$logname.txt
 
tmp=`grep -i "^[ \t]*$logname[ \t]*;[ \t]*[$RANGE][ \t]*;.*" $INRANK | cut -d ';' -f3 | sed 's/[ \t]* //g'`
sum=0
for num in $tmp
do
let sum=($sum+$num)
done
echo "Suma : $sum" >> $ODIR/$logname.txt
done

IFS=$OLDIFS

