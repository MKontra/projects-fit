#! /bin/bash

#defaultne hodnoty premennych

ODIR=./comments
INRANK=hodnoceni.txt
OFORM=def
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
	-g )
	    #-g prepni vystup na alternativnu formu
	    OFORM="alt"
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



#cyklus variant
for var in $( sed "s/[ \t]*//g" $INRANK | grep -i "^[^;]\+;[$RANGE];.*" | cut -d ';' -f2 | sort | uniq )
do
#premenna var obsahuje sucasnu variantu

    #vytvorenie mena suboru - varianta malym pismenom
    filename=`echo $var | sed "s/\($var\)/\l\1/"`.txt
    
    echo -n > $ODIR/$filename
    
    #pole hodnot danej varianty    
    values=( `grep -i "^[^;]\+;$var;[0-9]\+;.*" $INRANK | awk 'BEGIN{ FS=";";} { print $3 }' | sort -n` )
    #pomocna premenna s dlzkou daneho pola
    valueslen=${#values[*]}
    i=0
    inx=0
    while [ "$i" -le ${values[$valueslen-1]} ]
    do
	echo -n "$i-$((i+4)):" >> $ODIR/$filename
	count=0
	
	while [[ -n ${values[$inx]} && "${values[$inx]}" -lt "$((i+5))" ]]
	do
	    let "count+=1"
	    let "inx+=1"
	done
	
	if [ "$OFORM" = "alt" ]
	then
	    echo $count | awk '{ for (i=0; i<$1; i++) printf "=" ; }' >>$ODIR/$filename
	else
	    echo -n "$count" >> $ODIR/$filename
	fi 
    echo >> $ODIR/$filename
    let "i+=5"
    done
    echo "Suma: $((`echo ${values[@]} | sed 's/ /+/g'`)) / $valueslen" >> $ODIR/$filename
done

