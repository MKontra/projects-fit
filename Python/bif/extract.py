import sys
import re

intervalre = re.compile(r"(\d+)\.\.(\d+)", re.I | re.U)

seqre = re.compile(r"\s*\d+([gtac ]+)")

filename = sys.argv[1]
FILE = open(filename, "r")

NAME = FILE.readline()[5:].strip()

while True:
	curline = FILE.readline().strip()
	if curline.startswith("CDS"):
		break

curline = curline[3:].strip()
sequence = ""

if not curline[0:4] == "join":
	#parse single interval
	result = intervalre.match(curline)
	if not result:
		print "invalid interval"
		sys.exit(1)
	start=  int(result.group(1))
	end= int(result.group(2))
	while True:
		curline = FILE.readline().strip()
		if curline.startswith("ORIGIN"):
			break
	while True:
		reseq = seqre.match(FILE.readline().strip())	
		if reseq:
			sequence += reseq.group(1)
		else:
			break
	FILEOUT = open(NAME[0:8], "w")
	FILEOUT.write(">"+NAME+"\n")
	FILEOUT.write(re.sub(r"\s*", "", sequence)[start:end])
else:
	#parse multiple files
	FILE.seek(0)
	data = FILE.read()
	cdsre = re.compile(r"CDS\s*join\(")
	result = cdsre.search(data)
	if not result:
		print "invalid join"
		sys.exit(1)
	start = result.end(0)
	reblock = re.compile("(\w+)\.\d:(\d+)\.\.(\d+),")
	outstr = ""
	for res in reblock.finditer(data[start:]):
		SEKFIL = open(res.group(1) + ".txt", "r" )
		while True:
			curline = SEKFIL.readline().strip()
			if curline.startswith("ORIGIN"):
				break
		while True:
			reseq = seqre.match(SEKFIL.readline().strip())	
			if reseq:
				sequence += reseq.group(1)
			else:
				break
		outstr += re.sub(r"\s*", "", sequence)[int(res.group(2)):int(res.group(3))]
	FILEOUT = open(NAME[0:8], "w")
	FILEOUT.write(">"+NAME+"\n")
	FILEOUT.write(outstr)
		
	
