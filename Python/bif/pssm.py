import re
import sys
from math import *
from string import ascii_lowercase

filename = sys.argv[1]
FILE = open(filename)

data = FILE.readlines()
dlen = len(data[0])-1
nseq = len(data)

pssm = []

for i in range(0, dlen):
	tmpdict = {}
	for k in ascii_lowercase:
		tmpdict[k] = 1.0
	pssm.append(tmpdict)

for l in data:
	for i in range(0, dlen):
		pssm[i][l[i].lower()] = pssm[i][l[i].lower()] + 1
		
for i in pssm:
	for k in i.iterkeys():
		i[k] = i[k]/nseq
		i[k] = i[k]/(1.0/4.0)
		i[k] = log(i[k], 2)
		
testdata = [ "VVALL", "LEAAL", "AASVI", "METVA" ]
for i in testdata:
	score = 0.0
	for c in range(0, len(testdata[0])):
		score = score + pssm[c][i[c].lower()]
	print "Sekvencia: " + i + " score: " + str(score)