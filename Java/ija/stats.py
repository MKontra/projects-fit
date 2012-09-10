#!/usr/bin/env python
#
# Author: 	Ivan Homoliak
# Desription: 	Script for IPP course searching actual directory tree and all dived directory structures. Searching for .java files
# 

import os
import os.path as path
import re
import sys
import string

keywords = ['assert', 'abstract', 'boolean', 'break', 'byte', 'case', 'catch', 'char', 'class', 'const', 'continue', 'default', 'do', 'double', 'enum', 'else', 'extends', 'final', 'finally', 'float', 'for', 'goto', 'if', 'implements', 'import', 'instanceof', 'int', 'interface', 'long', 'native', 'new', 'package', 'private', 'protected', 'public', 'return', 'short', 'static', 'strictfp', 'super', 'switch', 'synchronized', 'this', 'throw', 'throws', 'transient', 'try', 'void', 'volatile', 'while']

operators = ['>>>=', '>>>', '<<=', '>>=', '++', '--', '<<', '>>', '<=', '>=', '==', '!=', '&&', '||', '+=', '-=', '*=', '/=', '%=', '&=', '^=', '|=', '+', '-', '~', '!', '*', '/', '%', '<', '>', '=', '&', '^' , '|' , '?']

operators_after_identifiers = ['>', '<', '+', '-', '=', '!', '&', '|', '*', '/', '%','^', '|' , ':']

digits = [ '0' ,'1' ,'2' ,'3' ,'4' ,'5' ,'6' ,'7' ,'8', '9']

def print_help():
	"""Funkcia zobrazuje napovedu"""
	print """Script finds out properties of java files which are located inside current directory tree. It recursively passes through whole directory structure and looks for "*.java". Every found java file is searched and requested operation is performed.
Usage:
	--help\t\tPrints this help.
	-k\t\tPrints count of keywords in java files.
	-o\t\tPrints count of operators in java files.
	-ik\t\tPrints count of indentificators including keywords.
	-i\t\tPrints count of indentificators.
	-w <pattern>\tPrints count of occurrences pattern in each file.
	-c\t\tPrints length of comments in java files using Bytes.
	-p\t\tWhen used together with one of previous, path is written relatively.\n"""

def get_params():
	"""Funkcia spracuje parametre z prikazoveho riadku"""
	scan_pattern = False
	pattern = ''
	arguments = []
	absolute_path = True
	do_keywords = False
	do_identifiers_keywords = False
	do_indentifiers = False
	do_pattern = False
	do_operators = False
	do_comments = False
	need_pattern = False
	
	if len(sys.argv) > 4:
		print 'Bad arguments.'
		sys.exit(0)
	
	for param in sys.argv[1:]:
		if scan_pattern:
			pattern = param
			scan_pattern = False
			need_pattern = False
			continue
		if param == '--help':
			print_help()
			sys.exit(0)
		elif param == '-k':
			arguments.append('-k')
			do_keywords = True
		elif param == '-o':
			arguments.append('-o')
			do_operators = True
		elif param == '-ik':
			arguments.append('-ik')
			do_identifiers_keywords = True
		elif param == '-i':
			arguments.append('-i')
			do_indentifiers = True
		elif param == '-w':
			arguments.append('-w')
			do_pattern = True
			scan_pattern = True
			need_pattern = True
		elif param == '-c':
			arguments.append('-c')
			do_comments = True
		elif param == '-p':
			arguments.append('-p')
			absolute_path = False
		else:
			print 'Bad arguments.'
			sys.exit(0)
	
	#analyza zoznamu naskenovanych argumentov
	if need_pattern:
		print 'Bad arguments.'
		sys.exit(0)
	
	if len(arguments) == 1 and not('-p' in arguments):
		return absolute_path, do_keywords, do_identifiers_keywords, do_indentifiers, do_pattern, do_operators, do_comments, pattern 
	
	elif len(arguments) == 2:
		if not('-p' in arguments):
			print 'Bad arguments.\n'
			sys.exit(0)
		return absolute_path, do_keywords, do_identifiers_keywords, do_indentifiers, do_pattern, do_operators, do_comments , pattern
	else:
		print 'Bad arguments.'
		sys.exit(0)

def walk_through_tree(current_path):

	""" Funkia rekurzivne prechadza stromovou adresarovou strukturou a pridava subory do zoznamu"""
	try:
		content = os.listdir(current_path)
	except:
		print 'Permision denied: ' + current_path
		return []
	#print content
	java_files = []
	
	for f in content:
		if path.isdir(current_path + f):
			#print 'Openning: ' + current_path + f + '/'
			java_files +=  walk_through_tree(current_path + f + '/')
		else:
			end_index = string.find(f,'.java')
			if end_index == -1 or string.find(f,'.java~') != -1 or string.find(f,'.java.') != -1:
				continue
			#print 'Appending: ' + f
			java_files.append(current_path + f)	
	
	return java_files

def print_keywords(absolute_path):
	
	java_files = walk_through_tree('./')
	count_total_keywords = 0
	java_files.sort()
	list_for_relative = []
	
	
	#v java_files je zoznam vsetkych .java suborov
	for subor in java_files:
		#print 'Subor: ' + subor
		fd = open(subor)
		content = fd.read()
		state = 'start'
		token = ''
		keywords_count = 0
		
		for letter in content:
			#konecny stavovy automat pre obsah suboru
			if state == 'start':
				if letter == '"':
					state = 'literal'
				elif letter == '\n' or letter == '\t' or letter == ' ':
					continue
				elif letter == '/':
					state = 'maybe_commentar'
				else:
					token += letter
					state = 'lexem'
					
			elif state == 'block_commentar':
				if letter == '*':
					state = 'maybe_end'
			elif state == 'maybe_end':
				if letter == '/':
					state = 'start'
				else:
					state = 'block_commentar'	
			elif state == 'maybe_commentar':
				if letter == '/':
					state = 'commentar'
				elif letter == '*':
					state = 'block_commentar'
			elif state == 'commentar': #stav commentar hlada koniec komentaru
				if letter == '\n':
					state = 'start'
			elif state == 'literal': #stav literal, hlada koniec literalu
				if letter == '"':
					state = 'start'
			elif state == 'lexem':
				if letter in ['(' , ';' , ' ', '\t', '.' , '{']:
					#koniec tokenu, treba zistit ci je klucovym slovom
					if token in keywords:
						keywords_count += 1
						#print token
					state = 'start'
					token = ''
				else:
					token += letter
		
		count_total_keywords += keywords_count
		
		if absolute_path:
			sys.stdout.write('%-120s' % (path.abspath(subor))  + ' %20d\n' % (keywords_count))
			sys.stdout.flush()
		else:
			extracted_filename = subor[subor.rfind('/') + 1:] 
			list_for_relative.append((extracted_filename,keywords_count))
			#print 'Extracted filename: ' + extracted_filename
			#sys.stdout.write(subor + ' %50d\n' % (keywords_count))
		
	if not(absolute_path):
		#potrebujem spravit zoradenie podla mena suboru a vypis poctu vyskytov
		list_for_relative.sort()
		#print list_for_relative
		for s,c in list_for_relative:
			sys.stdout.write('%-30s' % (s)  + ' %20d\n' % (c))
			sys.stdout.flush()	
		sys.stdout.write( '%-30s' % ('CELKEM: ') + ' %20d\n' % (count_total_keywords))
	else:
		sys.stdout.write( '%-120s' % ('CELKEM: ') + ' %20d\n' % (count_total_keywords))
	sys.stdout.flush()
	
	
def print_identifiers(absolute_path):
	""" Funkcia vypise identifikatory - resp ich pocet"""
	java_files = walk_through_tree('./')
	count_total_identifiers = 0
	java_files.sort()
	list_for_relative = []
	
	#v java_files je zoznam vsetkych .java suborov
	for subor in java_files:
		#print 'Subor: ' + subor
		fd = open(subor)
		content = fd.read()
		state = 'start'
		token = ''
		identifiers_count = 0
		
		for letter in content:
			#konecny stavovy automat pre obsah suboru
			if state == 'start':
				if letter == '"':
					state = 'literal'
				elif letter in [' ' , '\t' , '\n', '}' , ')' , '{', ';', '['] or letter in digits:
					continue
				elif letter == '/':
					state = 'maybe_commentar'
				else:
					token += letter
					state = 'lexem'
					
			elif state == 'block_commentar':
				if letter == '*':
					state = 'maybe_end'
			elif state == 'maybe_end':
				if letter == '/':
					state = 'start'
				else:
					state = 'block_commentar'	
			elif state == 'maybe_commentar':
				if letter == '/':
					state = 'commentar'
				elif letter == '*':
					state = 'block_commentar'
			elif state == 'commentar': #stav commentar hlada koniec komentaru
				if letter == '\n':
					state = 'start'
			elif state == 'literal': #stav literal, hlada koniec literalu
				if letter == '"':
					state = 'start'
			elif state == 'lexem':
				if letter in [' ' , '(' , ')' , '\t' , '.' , '{' , ',' , ';' , '[',']'] or letter in operators_after_identifiers:
					#koniec tokenu, treba zistit ci je identifikatorom
					if not(token in keywords) and not(token in operators) and not (token in ['}',')', ']',';']):
						#print '"' + token + '"'
						identifiers_count += 1
						#print token
					state = 'start'
					token = ''
				else:
					token += letter
		
		count_total_identifiers += identifiers_count
		
		if absolute_path:
			sys.stdout.write('%-120s' % (path.abspath(subor))  + ' %20d\n' % (identifiers_count))
			sys.stdout.flush()
		else:
			extracted_filename = subor[subor.rfind('/') + 1:] 
			list_for_relative.append((extracted_filename,identifiers_count))
			#print 'Extracted filename: ' + extracted_filename
			#sys.stdout.write(subor + ' %50d\n' % (keywords_count))
		
	if not(absolute_path):
		#potrebujem spravit zoradenie podla mena suboru a vypis poctu vyskytov
		list_for_relative.sort()
		#print list_for_relative
		for s,c in list_for_relative:
			sys.stdout.write('%-30s' % (s) + ' %20d\n' % (c))
			sys.stdout.flush()
		sys.stdout.write( '%-30s' % ('CELKEM: ') + ' %20d\n' % (count_total_identifiers))
	else:
		sys.stdout.write( '%-120s' % ('CELKEM: ') + ' %20d\n' % (count_total_identifiers))
	sys.stdout.flush()	


def print_identifiers_keywords(absolute_path):
	""" Funkcia vypise identifikatory a klucove slova - resp ich pocet"""
	java_files = walk_through_tree('./')
	count_total_idenkeywords = 0
	java_files.sort()
	list_for_relative = []
	
	#v java_files je zoznam vsetkych .java suborov
	for subor in java_files:
		#print 'Subor: ' + subor
		fd = open(subor)
		content = fd.read()
		state = 'start'
		token = ''
		idenkeywords_count = 0
		
		for letter in content:
			#konecny stavovy automat pre obsah suboru
			if state == 'start':
				if letter == '"':
					state = 'literal'
				elif letter in [' ' , '\t' , '\n', '}' , ')' , '{', ';', '['] or letter in digits:
					continue
				elif letter == '/':
					state = 'maybe_commentar'
				else:
					token += letter
					state = 'lexem'
					
			elif state == 'maybe_commentar':
				if letter == '/':
					state = 'commentar'
				elif letter == '*':
					state = 'block_commentar'
			elif state == 'block_commentar':
				if letter == '*':
					state = 'maybe_end'
			elif state == 'maybe_end':
				if letter == '/':
					state = 'start'
				else:
					state = 'block_commentar'	
			elif state == 'commentar': #stav commentar hlada koniec komentaru
				if letter == '\n':
					state = 'start'
			elif state == 'literal': #stav literal, hlada koniec literalu
				if letter == '"':
					state = 'start'
			elif state == 'lexem':
				if letter in [' ' , '(' , ')' , '\t' , '.' , '{' , ',' , ';' , '[',']'] or letter in operators_after_identifiers:
					#koniec tokenu, treba zistit ci je identifikatorom
					if  not(token in operators) and not (token in ['}',')', ']',';']):
						#print '"' + token + '"'
						idenkeywords_count  += 1
						#print token
					state = 'start'
					token = ''
				else:
					token += letter
		
		count_total_idenkeywords += idenkeywords_count 
		
		if absolute_path:
			sys.stdout.write('%-120s' % (path.abspath(subor))  + ' %20d\n' % (idenkeywords_count))
			sys.stdout.flush()
		else:
			extracted_filename = subor[subor.rfind('/') + 1:] 
			list_for_relative.append((extracted_filename,idenkeywords_count ))
			#print 'Extracted filename: ' + extracted_filename
		
		
	if not(absolute_path):
		#potrebujem spravit zoradenie podla mena suboru a vypis poctu vyskytov
		list_for_relative.sort()
		#print list_for_relative
		for s,c in list_for_relative:
			sys.stdout.write('%-30s' % (s) + ' %20d\n' % (c))
			sys.stdout.flush()	
		sys.stdout.write( '%-30s' % ('CELKEM: ') + ' %20d\n' % (count_total_idenkeywords))
	else:
		sys.stdout.write( '%-120s' % ('CELKEM: ') + ' %20d\n' % (count_total_idenkeywords))
	sys.stdout.flush()	
	
def print_operators(absolute_path):	
	
	""" Funkcia vypise operatory - resp ich pocet"""
	java_files = walk_through_tree('./')
	count_total_operators = 0
	java_files.sort()
	list_for_relative = []
	
	#v java_files je zoznam vsetkych .java suborov
	for subor in java_files:
		#print 'Subor: ' + subor
		fd = open(subor)
		content = fd.read()
		state = 'start'
		token = ''
		operators_count = 0
		i = 0
		
		while i < len(content):
			#konecny stavovy automat pre obsah suboru
			letter = content[i]
			if state == 'start':
				if letter == '"':			 #String nieco = (false) ? "blabla" : "lalal' ; 
					state = 'literal'
				elif letter in [' ' , '\t' , '\n', '}' , ')' , '{', ';', '['] or letter in digits:
					i += 1
					continue
				elif letter == '/':
					state = 'maybe_commentar'
				elif letter in operators_after_identifiers + ['?']:
					token += letter
					state = 'operator'
				elif letter == '.' and content[i + 1] == '*':
					i += 2
					token = ''
					continue
				else:
					state = 'start'
					token = ''
			elif state == 'block_commentar':
				if letter == '*':
					state = 'maybe_end'
			elif state == 'maybe_end':
				if letter == '/':
					state = 'start'
				else:
					state = 'block_commentar'	
			elif state == 'maybe_commentar':
				if letter == '/':
					state = 'commentar'
				elif letter == '*':
					state = 'block_commentar'
			elif state == 'commentar': #stav commentar hlada koniec komentaru
				if letter == '\n':
					state = 'start'
			elif state == 'literal': #stav literal, hlada koniec literalu
				if letter == '"':
					state = 'start'
			elif state == 'operator':
				if letter in [' ' ,'(' , '\t' , ';', '"',')' ] or letter in digits or str.isalpha(letter):
					if  token in operators and not (token in ['}',')',']',';']):
						#print '"' + token + '"'
						operators_count  += 1
						#print token
					state = 'start'
					token = ''
				else:
					token += letter
			i += 1
		
		count_total_operators += operators_count 
		
		if absolute_path:
			sys.stdout.write('%-120s' % (path.abspath(subor))  + ' %20d\n' % (operators_count))
			sys.stdout.flush()
		else:
			extracted_filename = subor[subor.rfind('/') + 1:] 
			list_for_relative.append((extracted_filename,operators_count))
			#print 'Extracted filename: ' + extracted_filename

	if not(absolute_path):
		#potrebujem spravit zoradenie podla mena suboru a vypis poctu vyskytov
		list_for_relative.sort()
		#print list_for_relative
		for s,c in list_for_relative:
			sys.stdout.write('%-30s' % (s)  + ' %20d\n' % (c))
			sys.stdout.flush()	
		sys.stdout.write( '%-30s' % ('CELKEM: ') + ' %20d\n' % (count_total_operators))
	else:
		sys.stdout.write( '%-120s' % ('CELKEM: ') + ' %20d\n' % (count_total_operators))
	sys.stdout.flush()	
	
def print_comments(absolute_path):
	""" Fukcia vypise kolko Bytov komentaru sa nachadza v java suboroch"""
	java_files = walk_through_tree('./')
	count_total_Bytes = 0
	java_files.sort()
	list_for_relative = []
	
	
	#v java_files je zoznam vsetkych .java suborov
	for subor in java_files:
		fd = open(subor)
		content = fd.read()
		state = 'start'
		token = ''
		Bytes_count = 0
		
		for letter in content:
			#konecny stavovy automat pre obsah suboru
			if state == 'start':
				if letter == '/':
					state = 'maybe_commentar'
				else:
					token = ''
					
			elif state == 'maybe_commentar':
				if letter == '/':
					state = 'commentar'
				elif letter == '*':
					state = 'block_commentar'
				else:
					token = ''
					state = 'start'
			
			elif state == 'block_commentar':
				if letter == '*':
					token += letter
					state = 'maybe_end'
				else:
					token += letter
			elif state == 'maybe_end':
				if letter == '/':
					state = 'start'
					Bytes_count += len(token) - 1
					print '"' + token +'"'
					token = ''
				else:
					token += letter
					state = 'block_commentar'
			elif state == 'commentar': #stav commentar hlada koniec komentaru
				token += letter
				if letter == '\n':
					Bytes_count += len(token) #zapocitany aj koniec riadku
					token = ''
					state = 'start'
			
		count_total_Bytes += Bytes_count
		
		if absolute_path:
			sys.stdout.write('%-120s' % (path.abspath(subor))  + ' %20d\n' % (Bytes_count))
			sys.stdout.flush()
		else:
			extracted_filename = subor[subor.rfind('/') + 1:] 
			list_for_relative.append((extracted_filename,Bytes_count))
			#print 'Extracted filename: ' + extracted_filename
			#sys.stdout.write(subor + ' %50d\n' % (keywords_count))
		
	if not(absolute_path):
		#potrebujem spravit zoradenie podla mena suboru a vypis poctu vyskytov
		list_for_relative.sort()
		#print list_for_relative
		for s,c in list_for_relative:
			sys.stdout.write('%-30s' % (s) + ' %20d\n' % (c))
			sys.stdout.flush()	

		sys.stdout.write( '%-30s' % ('CELKEM: ') + ' %20d\n' % (count_total_Bytes))
	else:
		sys.stdout.write( '%-120s' % ('CELKEM: ') + ' %20d\n' % (count_total_Bytes))
	sys.stdout.flush()
	

def print_pattern(absolute_path,pattern):
	""" Fukcia vypise kolko Bytov komentaru sa nachadza v java suboroch"""
	java_files = walk_through_tree('./')
	count_total = 0
	java_files.sort()
	list_for_relative = []
	
	
	#v java_files je zoznam vsetkych .java suborov
	for subor in java_files:
		fd = open(subor)
		content = fd.read()
		state = 'start'
		token = ''
		count = 0
		
		#budem hladat v celom subore
		p = re.compile(pattern,re.M | re.S)
		occurencies = p.findall(content)
		
		for o in occurencies:
			count += 1
		
		count_total += count
		
		if absolute_path:
			sys.stdout.write('%-120s' % (path.abspath(subor))  + ' %20d\n' % (count))
			sys.stdout.flush()
		else:
			extracted_filename = subor[subor.rfind('/') + 1:] 
			list_for_relative.append((extracted_filename,count))
			#print 'Extracted filename: ' + extracted_filename
			#sys.stdout.write(subor + ' %50d\n' % (keywords_count))
		
	
	
	if not(absolute_path):
		#potrebujem spravit zoradenie podla mena suboru a vypis poctu vyskytov
		list_for_relative.sort()
		#print list_for_relative
		for s,c in list_for_relative:
			sys.stdout.write('%-30s' % (s) + ' %20d\n' % (c))
			sys.stdout.flush()	

		sys.stdout.write( '%-30s' % ('CELKEM: ') + ' %20d\n' % (count_total))
	else:
		sys.stdout.write( '%-120s' % ('CELKEM: ') + ' %20d\n' % (count_total))
	sys.stdout.flush()

def main():
	absolute_path, do_keywords, do_identifiers_keywords, do_indentifiers, do_pattern, do_operators, do_comments, pattern  = get_params()
	if do_keywords:
		print_keywords(absolute_path)
	elif do_identifiers_keywords:
		print_identifiers_keywords(absolute_path)
	elif do_indentifiers:
		print_identifiers(absolute_path)
	elif do_pattern:
		print_pattern(absolute_path,pattern)
	elif do_operators:
		print_operators(absolute_path)
	elif do_comments:
		print_comments(absolute_path)

if __name__ == '__main__':
	main()
