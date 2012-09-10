#!/bin/sh

echo "Running tests..."
echo "recompile sources"
make clean
make
echo "Step one: Starting server on port 11111"
./server -p 11111 &

SERVERPID=`ps axo pid,command | grep -e "./server -p 11111$" | sed 's/^[ ]*\([0-9]*\)\(.*\)$/\1/'`

if [ -z SERVERPID ]; then
	echo "Server not running ?"
	exit
fi

echo "Start successfull. Server PID: $SERVERPID"
echo
echo "Spustam klienta s roznimy dotazmi..."
TEST1=`./client -h localhost -p 11111 -NSL -l xkontr00`
TEST2=`./client -h localhost -p 11111 -LF -f FEKT`
TEST3=`./client -h localhost -p 11111 -NS -n Jan -f FIT`
echo -e "Moje meno, priezvisko, login: $TEST1"
echo
echo "Ziaci z FEKTU"
echo -e "$TEST2"
echo
echo "Janovia z FITu"
echo -e "$TEST3"

echo "Ukoncujem server..."
kill -9 $SERVERPID