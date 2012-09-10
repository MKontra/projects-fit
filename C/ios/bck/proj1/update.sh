#! /bin/bash

#defaultne hodnoty premennych

INRANK=hodnoceni.txt
INSTUD=students.txt
RANGE=A-Z

# param parsing

while [ $# -gt 0 ]; do
    case "$1" in
	-h )
	    #-h zmen vstupny subor hodnoteni
	    if [ -z "$2" ]; then
		echo "Error: Nezadany parameter !!!"
		exit 1
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
		exit 1
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
		exit 1
	    fi
	    #-v vstupny pattern
	    RANGE="$2"
	    shift 
	    ;;    
    esac
    shift
done

OLDIFS=$IFS
NEWLINE=$'\012'
IFS=$NEWLINE

for line in $( cat $INSTUD )
do

#naplnenie pola daneho studenta povodnymi hodnotami
    logname=`echo $line | cut -d ';' -f2 | sed 's/[ \t]*//g'`
    points=( `echo $line | awk 'BEGIN{FS=";";} { for (i=3;i<10;i++) print $i;}' | sed 's/[ \t]*//g'` )

#odfiltrovanie variant z parametra a naplnenie premennej platnymi zaznamami pre studenta
    hodnotenia=`grep -i "^[ \t]*$logname[ \t]*;[ \t]*[$RANGE][ \t]*;.*" $INRANK | sort -t ';' -k2 | sed 's/ //g'`

i=0
lowerA=97
# loop nahradzovania zaznamov v poli
while [[ $i -lt 5 ]]
do
    #pomocny znak 
    hlpchar=`awk 'BEGIN{ printf("%c",'$((i+lowerA))');}'`
    newpts=`echo -e "$hodnotenia" | grep -i "^$logname;$hlpchar;[0-9]*;.*$" | awk 'BEGIN{ FS=";";} {print $3}'`
    
    #ak sa medzi zaznamami najde platny variant ktory ma byt zmeneny updatne sa pole..
    if [[ -n $newpts ]]
    then
	points[$i]=$newpts
	#echo " ${points[$i]}" 
    fi

let "i++"
done 

#nova suma
hlpval=${points[5]}
points[5]=$((`echo ${points[@]} | sed 's/ /+/g'`-hlpval))

#patch studentov
nom="`echo ${points[@]} | sed 's/ /;/g'`"
sed -i -e 's/^\([ \t]*[^;]\+[ \t]*;[ \t]*'$logname'[ \t]*;\).*$/\1'$nom'/' $INSTUD

done

IFS=$OLDIFS