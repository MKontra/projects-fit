import re
import sys

filename = sys.argv[1]
FILE = open(filename)

gencode = \
{
	"aaa" : "aminoacid1",
	"aac" : "aminoacid2",
}

header = FILE.readline()
data = FILE.read()
while True:
	if data == '':
		break
	triplet = data[0:3]
	data = data[3:]
	print triplet
