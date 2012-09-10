#!/bin/sh

echo "Running tests..."
echo "Test 1: Download an image with file name specified, no redirect"
echo "Retrieving using wget"
wget -q -O pic1.gif --user-agent="Mozilla/4.0 (compatible; MSIE 8.0; Windows NT 6.1; WOW64; Trident/4.0; SLCC2; Media Center PC 6.0; InfoPath.2; MS-RTC LM 8)" "http://www.fit.vutbr.cz:80/common/img/fit_logo_cz.gif"
echo "Using webclient"
./webclient http://www.fit.vutbr.cz:80/common/img/fit_logo_cz.gif

cmp -s pic1.gif fit_logo_cz.gif

if [ $? -ne 0 ]; then
	echo "Files are not equal, they probably changed between calls..."
	else
	echo "Subory zhodne"
fi

echo ""
echo "Test 2: Download web page no name, no redirect"
echo "Retrieving using wget"
wget -q -O page1.html --user-agent="Mozilla/5.0 (X11; U; Linux x86_64; en-US; rv:1.9.0.7) Gecko/2009022800 SUSE/3.0.7-1.1.6 Firefox/3.0.7" --header='Accept-Charset: utf-8' "http://www.fit.vutbr.cz/"
echo "Using webclient"
./webclient http://www.fit.vutbr.cz/

cmp -s page1.html index.html
cp index.html index-test2-save.html

if [ $? -ne 0 ]; then
	echo "Files are not equal, they probably changed between calls..."
	else
	echo "Subory zhodne"
fi

echo ""
echo "Test 3: Download web page no name, expected one redirect"
echo "Retrieving using wget"
wget -q -O page2.html --user-agent="Mozilla/5.0 (X11; U; Linux x86_64; en-US; rv:1.9.0.7) Gecko/2009022800 SUSE/3.0.7-1.1.6 Firefox/3.0.7" --header='Accept-Charset: utf-8' "http://www.google.com/"
echo "Using webclient"
./webclient http://www.google.com/

cmp -s page2.html index.html

if [ $? -ne 0 ]; then
	echo "Files are not equal, they probably changed between calls..."
	echo "Sposobene tym  ze google posiela unikatny kluc relacie pri kazdom pripojeni..."
	else
	echo "Subory zhodne"
fi