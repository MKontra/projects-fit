import sys
import os.path
from array import *
from itertools import groupby
   
if len(sys.argv) == 1:
  print "Usage: get_tRNA.py dna.fasta [output file name]"
  exit()   
            
bufferpos = 0

done = False
fsize = None
fileh = None

try:
    fileh = open(sys.argv[1], "r")
    fsize = os.path.getsize(sys.argv[1])
except IOError:
    print "Cannot open input file"
    exit(1)

oname = "output.fasta"
if len(sys.argv) <= 2:
  print "Outputfile name not supplied using \"output.fasta\""
else:
  oname = sys.argv[2]
  
fileo = open(oname, "w")

fastacomment = fileh.readline()

symbols = [ ['a', 'g'], ['a'], ['*'], ['t', 'c'], ['c'], 
            ['*'], ['*'],['*'],['*'],['*'],['*'],['*'],['*'],['*'],['*'],['*'],['*'],
            ['c'], ['c'], ['a'] ]
    
symbolsl = len(symbols)

nbasedata = "".join( [x[0:-1] for x in fileh.readlines()])
nbasedatal = len(nbasedata)
out = []

i = 1
ffs = 0
#raw_input

while (bufferpos < nbasedatal):
    if ( nbasedata[bufferpos:bufferpos+4].lower() == "gttc" ):
      score = 0
      for p in range(0, symbolsl):
        if '*' in symbols[p]: continue
        if nbasedata[bufferpos+4+p].lower() in symbols[p]: score = score + 1
      if score > 6:
        for j in range(11, 32):
          if (nbasedata[bufferpos-j-1].lower() in ['a','g']) and\
             (nbasedata[bufferpos-j-5].lower() == 't') and\
             nbasedata[bufferpos-j-6].lower() != 'g':
            for k in range(11,15):
              if nbasedata[bufferpos-j-k-7].lower() == 'g':
                for l in range(1,4):
                  if nbasedata[bufferpos-j-k-l-8].lower() in ['a','g'] and\
                     nbasedata[bufferpos-j-k-l-9].lower() == 'a':
                    out.append(\
                      [">ecoli K.12 tRna - " +str(i) + " - " + str(bufferpos-j-k-l-21) + " - " + str(bufferpos+24)+ " "+ str(j)+" "+str(k)+" "+str(l) + "\n",\
                      nbasedata[bufferpos-j-k-l-22:bufferpos+24] + "\n",\
                      [bufferpos-j-k-l-21, bufferpos+24,j,k,l]] )
                    i =i + 1
      bufferpos = bufferpos + 4
    else:
      bufferpos = bufferpos + 1
      continue

groups = []
uniquekeys = []
data = sorted(out, key=lambda x: x[2][1])
for k, g in groupby(data, lambda x: x[2][1]):
    groups.append(list(g))
    uniquekeys.append(k)
    
uout = []
for g in groups:
  tmp = sorted(g, key=lambda x: -(x[2][1] - x[2][0]))
  od = tmp[0]
  uout.append(od)

      
def unique_items(L):
    founds = set()
    founde = set()
    for item in L:
        if item[2][0] not in founds or item[2][1] not in founde:
            yield item
            founds.add(item[2][0])  
            founde.add(item[2][1])      
      
#uout = list( unique_items (out)) 
for od in uout:
  fileo.write(od[0]+od[1])

        