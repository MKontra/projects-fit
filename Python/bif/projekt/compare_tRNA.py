import sys
import os.path
from array import *

if len(sys.argv) != 3:
  print "param1: file with get_tRNA.py output"
  print "param2: reference sequences"
  print "Usage: compare_tRNA.py param1 param2"
  exit()

def get_entries(lines):
  header = lines[0][0:-1]
  nextout = ""
  for l in lines[1:]:
    if l[0] == '>':
      if nextout != "": 
        yield [header,nextout]
        nextout = ""
        header = l[0:-1]
    else:
      if l != "" and l != "\n":
        nextout = nextout + l[0:-1]
  yield [header,nextout]
      
fsize = None
fileh = None

try:
    fileh = open(sys.argv[2], "r")
    fsize = os.path.getsize(sys.argv[1])
except IOError:
    print "Cannot open file 2"
    exit(1)
    
try:
    fileh2 = open(sys.argv[1], "r")
    fsize = os.path.getsize(sys.argv[1])
except IOError:
    print "Cannot open file 1"
    exit(1)
    
netlist = list( get_entries(fileh.readlines() ))
mylist =  list( get_entries(fileh2.readlines() ))

print "-"*80
print "Total entries in first file: " + str(len(mylist))   
print "Total entries in second file: " + str(len(netlist))  
print "-"*80  

count = 0
print "Entries not found in genome browser DB, but found by this algorithm:"
print
for l in mylist:
  found = 0
  for k in netlist:
    if ( l[1].endswith(k[1]) ): 
      found = 1
      break
  if found == 0:
    print l[0]
    print l[1]
    print 
    count = count + 1   

print "Total entries: " + str(count)    
print "-"*80   
print "-"*80  

count = 0
print "Entries not found by this algorithm, but present in genome browser DB:"
print
for l in netlist:
  found = 0
  for k in mylist:
    if ( k[1].endswith(l[1]) ): 
      found = 1
      break
  if found == 0:
    print l[0]
    print l[1]
    print
    count = count + 1 
    
print "Total entries: " + str(count)    
print "-"*80   
print "-"*80