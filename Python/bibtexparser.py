#!/usr/bin/env python
# -*- coding: utf-8 -*-

"""
This module represents utility for manipulating with data in BibTeX format.
"""

__modulename__ = "bibtexparser"
__author__ = "Matus Kontra"
__email__ = "xkontr00@stud.fit.vutbr.cz"
__date__ = "$24-Mai-2010 18:40:28$"
__edited__ = "$5-July-2010 17:01:29$"
__version__ = "$0.10.8.12$"


import re
import itertools
from itertools import ifilter, imap, chain
from sets import Set
from unicodedata import *
import unittest
import datetime

#mom nepotrebne
#import cProfile

import lxml
import lxml.html
from lxml import etree

from db.rrsdbdata import *
from extractors.entityextractor import EntityExtractor

#na testovanie
from extractors.crawler import GetHTMLPage, FileDownloader

_ee = EntityExtractor() #todo:opytat sa na loadovanie

#Active State Code Recipes
#Recipe 66061: Assign and test
class DataHolder:
    def __init__(self, value=None): self.value = value
    def set(self, value): self.value = value; return value
    def get(self): return self.value

#helper functions

#TODO: toto je nanic, alebo nie ?
#prvy podretazec, ktory sa zhoduje s vzorom
def _firstMatch( regex, strng, pos, *optargs):
    try:
        result = regex.finditer(strng[pos:], *optargs).next()
        return result
    except:
        pass # no match 
    return None 

#pozicie vyskytov daneho retazca  
def _indexiter(instr, whatstr):
    inx = 0
    while True:
        pos=instr.find(whatstr, inx)
        if pos == -1: break
        inx = pos+len(whatstr)
        yield pos

#funkcia fungujuca  ako operator in avsak vracia zhodny objekt
def _one_of(withlist, what):
    for i in withlist:
        if what == i: return i
    return None  

#startswith s navratom
def _starts_with_assign(withlist, what):
    for i in withlist:
        if what.startswith(i): return i
    return None

#enclosed string proc    
def _read_string(instr, optok, cltok, skipescaped = True):
    nesting = 1
    if optok == cltok:
        nesting = 0
    depth = 0
    pos = 0
    stpos = 0
    found = 0
    out = u""
    if nesting == 0:
        try:    
            while True:
                if skipescaped and instr[pos] == ur"\\":
                    pos += 1
                elif instr[pos] == optok:
                    if found == 0:
                        stpos = pos + 1
                        found = 1
                    else:
                        return (pos + 1, instr[stpos:pos])
                
                pos += 1

        except:
            return (0, u"")
            
    else:      
        try:    
            while True:
                if skipescaped and instr[pos] == ur"\\":
                    pos += 1
                elif instr[pos] == optok:
                    if found == 0:
                        stpos = pos + 1
                        found = 1
                        depth = 1
                    else:
                        depth = depth + 1
                        
                elif instr[pos] == cltok:
                    if depth == 0:
                        raise IndexError("ClTok befor OpTok")
                    elif depth == 1:
                        return (pos + 1, instr[stpos:pos])
                    else:
                        depth = depth - 1
                                                                       
                
                pos += 1

        except:
            return (0, u"")

#odstrani escape sequencie
def _unescape(instr):
    output = u""
    index = 0
    aggregate = 0
    try:
        while True:      
            if instr[index] == u"\\":   #backslash
                output = output + instr[aggregate:index]
                index += 1
                output = output + instr[index]
                index += 1                
                aggregate = index 
            else:
                index += 1
    except IndexError:
        pass #hotovo
        
    output = output + instr[aggregate:]
    return output 
    
def _skip_escaped(instr):
    output = u""
    index = 0
    aggregate = 0
    try:
        while True:      
            if instr[index] == u"\\":   #backslash
                output = output + instr[aggregate:index]
                index = index + 2
                aggregate = index 
            else:
                index += 1
    except IndexError:
        pass #hotovo
        
    output = output + instr[aggregate:]
    return output   

wspcs = re.compile(r'\s+', re.U)
def wsnormalize(instr):
    return wspcs.sub(u" ", instr)
    
def html2text(inputtree):
    if not isinstance(inputtree, etree._ElementTree):
        raise Exception("Bad input")
    
    textbuff = u""
    
    root = inputtree.getroot()     
    #0 optag
    #1 cltag
    stack = [(0,root)]
    
    depth = 0
    
    while stack:
        stop = stack.pop()
        curtp = stop[0]
        curel = stop[1]
        if curtp == 0:
            #textbuff += "op tag: " + str(curel.tag)            
            if curel.tag == "pre":
                textbuff += curel.text or u''
            elif curel.tag == "br":                 
                textbuff += u'\n'
            else:                 
                textbuff += (curel.text and wsnormalize(curel.text)) or u''
                    
            chset = curel.getchildren()
            chset.reverse()
            #textbuff += "tekst tag: ==>" + (curel.text or u'') +"<=="       
            if (curel.text is not None) or chset:
                #textbuff += "HAS text"
                stack.append( (1,curel) )
            if chset:
                #textbuff += "HAS children"             
                for ch in chset:
                    stack.append( (0,ch) )
            if curel.text is None and not chset:
                if curel.getparent() is not None and curel.getparent().tag == "pre":
                    textbuff += curel.tail or u''
                else:     
                    textbuff += (curel.tail and wsnormalize(curel.tail)) or u''                           
                
        elif curtp == 1:
            #textbuff += "closing tag: " + str(curel.tag)
            if curel.getparent() is not None and curel.getparent().tag == "pre":
                #textbuff += "closing tail: "
                textbuff += curel.tail or u''
            elif curel.tag in ('p','div'):
                #textbuff += "closing tail: "
                textbuff += (curel.tail and '\n'+wsnormalize(curel.tail)) or u''         
            else:     
                #textbuff += "closing tail: " 
                textbuff += (curel.tail and wsnormalize(curel.tail)) or u''          
  
    return textbuff

def tokenize(tape, table):
    index = 0
    while True:
        for i in table:
            result = i[0].match(tape[index:])
            if result:
                if i[1]: yield i[1]( *result.groups() ) 
                index += len(result.group(0))
                break
        if index == len (tape):
            break


class BibTeXParserError(Exception):
        pass

#-------------------------------------------------------------------------------
# end of class BibTeXParserError
#-------------------------------------------------------------------------------
class ObjList2RRSObject(object):
    #entry types regexes and key values (their hashes = addresses)
    article         = re.compile(r"article", re.IGNORECASE)
    book            = re.compile(r"book", re.IGNORECASE)
    booklet         = re.compile(r"booklet", re.IGNORECASE)      
    inbook          = re.compile(r"inbook", re.IGNORECASE)       
    incollection    = re.compile(r"incollection", re.IGNORECASE) 
    inproceedings   = re.compile(r"inproceedings", re.IGNORECASE)
    conference      = re.compile(r"conference", re.IGNORECASE)   
    manual          = re.compile(r"manual", re.IGNORECASE)       
    mastersthesis   = re.compile(r"mastersthesis", re.IGNORECASE)
    misc            = re.compile(r"misc", re.IGNORECASE)         
    phdthesis       = re.compile(r"phdthesis", re.IGNORECASE)    
    proceedings     = re.compile(r"proceedings", re.IGNORECASE)  
    techreport      = re.compile(r"techreport", re.IGNORECASE)   
    unpublished     = re.compile(r"unpublished", re.IGNORECASE)  
    
    #attribute regexes and key values (their hashes = addresses)
    crossref        = re.compile(r"crossref", re.IGNORECASE)  
    author          = re.compile(r"author", re.IGNORECASE)
    editor          = re.compile(r"editor", re.IGNORECASE)
    authedit        = re.compile(r"(author)|(editor)", re.IGNORECASE)
    title           = re.compile(r"title", re.IGNORECASE)
    booktitle       = re.compile(r"booktitle", re.IGNORECASE)
    edition         = re.compile(r"edition", re.IGNORECASE)  
    chapter         = re.compile(r"chapter", re.IGNORECASE)
    chappages       = re.compile(r"(chapter)|(pages)", re.IGNORECASE)
    journal         = re.compile(r"journal", re.IGNORECASE)
    eprint          = re.compile(r"eprint", re.IGNORECASE) 
    publisher       = re.compile(r"publisher", re.IGNORECASE)
    address         = re.compile(r"address", re.IGNORECASE)
    organization    = re.compile(r"organization", re.IGNORECASE)
    institution     = re.compile(r"institution", re.IGNORECASE)
    school          = re.compile(r"school", re.IGNORECASE)
    year            = re.compile(r"year", re.IGNORECASE)
    volume          = re.compile(r"volume", re.IGNORECASE)
    number          = re.compile(r"number", re.IGNORECASE)
    series          = re.compile(r"series", re.IGNORECASE)
    pages           = re.compile(r"pages", re.IGNORECASE)
    address         = re.compile(r"address", re.IGNORECASE)
    month           = re.compile(r"month", re.IGNORECASE)
    edition         = re.compile(r"edition", re.IGNORECASE)
    note            = re.compile(r"note", re.IGNORECASE)
    tech_report_tpy = re.compile(r"type", re.IGNORECASE) 
    annote          = re.compile(r"annote", re.IGNORECASE)
    url             = re.compile(r"url", re.IGNORECASE)    
    key             = re.compile(r"key", re.IGNORECASE)
    howpublished    = re.compile(r"howpublished", re.IGNORECASE)

    #entrytype - attribute relation
    entrytypesreq = \
    {
        article       : ( (author, title, journal, year), (volume, number, pages, month, note, key) ),
        book          : ( (authedit, title, publisher, year), (volume, series, address, edition, month, note, key) ),
        booklet       : ( (title), (author, howpublished, address, month, year, note, key) ),
        inbook        : ( (authedit, title, chappages, publisher, year), (volume, series, address, edition, month, note, key) ),
        incollection  : ( (author, title, booktitle, year), (editor, pages, organization, publisher, address, month, note, key) ), 
        inproceedings : ( (author, title, booktitle, year), (editor, series, pages, organization, publisher, address, month, note, key) ), 
        conference    : ( (author, title, booktitle, year), (editor, series, pages, organization, publisher, address, month, note, key) ),
        manual        : ( (title), (author, organization, address, edition, month, year, note, key) ),
        mastersthesis : ( (author, title, school, year), (address, month, note, key) ),
        misc          : ( (), (author, title, howpublished, month, year, note, key) ),
        phdthesis     : ( (author, title, school, year), (address, month, note, key) ),
        proceedings   : ( (title, year), (editor, publisher, organization, address, month, note, key) ),
        techreport    : ( (author, title, institution, year), (tech_report_tpy, number, address, month, note, key) ),
        unpublished   : ( (author, title, note), (month, year, key) )     
    }
    
    monfull = Set(['january', 'february', 'march', 'april',
                   'may', 'june', "july", "august", "september", 
                   'october', 'november', 'december'])
    
    monabbr = Set(['jan', 'feb', 'mar', 'apr',
                   'may', 'jun', "jul", "aug", "sep", 
                   'oct', 'nov', 'dec'])
    
    PType = object() #dictionary key for publication type
    
    numrange    = re.compile(r"\D*(?P<startnum>\d+)\D*(?P<endnum>\d*)\D*", re.I | re.U )
    textrange   = re.compile(r"([^_\d\W]+)", re.I | re.U )        

    whiteSpace  = re.compile(r"\s", re.I | re.U )
    letter      = re.compile(r"\w", re.I | re.U )
    lowercase   = re.compile(r"[a-z]", re.U )
    leftbr      = re.compile(r"[{]([^}]+?)(?=\w)[}]", re.I | re.U )
    rightbr     = re.compile(r"(?<=\w)[{]([^}]+?)[}]", re.I | re.U )

    (NAME, COMMA, VONP, JRP, AND) = (1, 2, 3, 4, 5)

    @staticmethod
    def clasiftok(x):
        x = x.strip()
        if x == "and":
            return (ObjList2RRSObject.AND,) 
        if x == ",":
            return (ObjList2RRSObject.COMMA,)           
        if x in ( "von", "Von", "de la" ):
            return (ObjList2RRSObject.VONP, x)
        if x in ( "JR","Jr","jr","jr.", "Jr.", "JR.", "junior" ):
            return (ObjList2RRSObject.JRP, x)
        if ObjList2RRSObject.lowercase.match(x[0]):
            return (ObjList2RRSObject.VONP, x)
        return (ObjList2RRSObject.NAME, "".join(i for i in x if i <> "}" ) )       
        
    
    regextabTab = [\
                    #regex                                                           #callable
                    [re.compile(ur"\s+", re.U),                                      None], 
                    [re.compile(ur"([,])", re.U),                                    lambda x: (ObjList2RRSObject.COMMA,)],
                    [re.compile(ur"(de la)", re.U),                                  lambda x: (ObjList2RRSObject.VONP, x)],
                    [re.compile(ur"([^{\s][^,\s]*)", re.U),                          lambda x: ObjList2RRSObject.clasiftok(x)],
                    [re.compile(ur"[{]([^,\s]*[}][^,\s]*)", re.U),                    lambda x: ObjList2RRSObject.clasiftok(x)],                    

                    [re.compile(ur"[^\w][{]([^}]{2,})[}][^\w]", re.U),               lambda x: (ObjList2RRSObject.NAME, x)],                 
                    [re.compile(ur"[{]([^}]{2,})[}]", re.U),                         lambda x: (ObjList2RRSObject.NAME, x)],                    
                    [re.compile(ur"\A[{]([^}]{2,})[}][^\w]", re.U),                  lambda x: (ObjList2RRSObject.NAME, x)],
                    [re.compile(ur"[^\w][{]([^}]{2,})[}]\Z", re.U),                  lambda x: (ObjList2RRSObject.NAME, x)],
                    

                    
                  ]   
    
    def _rel_inbrackets(self,instr):
        return ObjList2RRSObject.rightbr.sub(r"\1",ObjList2RRSObject.leftbr.sub(r"\1", instr))
          
    
    def separate_author_fields(self):  
        (FORMONE, FORMTWO, FORMTHREE) = (1, 2, 3)    
        (NAME, COMMA, VONP, JRP, AND) = (1, 2, 3, 4, 5)
        (FIRST, LAST, VONP, JRP) = (1, 2, 3, 4)        
        whiteSpace = ObjList2RRSObject.whiteSpace
        lowercase  = ObjList2RRSObject.lowercase
        #separate tokens
        #for i in (x for x in itertools.chain.from_iterable( (z.iteritems() for z in self.pubdict.itervalues() ) ) if x[0] in ("author", "editor", "authors")) :
        #  i[1] = "bambus"
        for (k,v) in self.pubdict:
            for kk,vv in v.iteritems():
                if kk in ("author", "editor", "authors"):
                    state = FORMONE
                    nextname = FIRST
                    names = []
                    stack = [] 
                    for i in tokenize(self._rel_inbrackets(vv.strip()), ObjList2RRSObject.regextabTab):
                        if i[0] == AND:
                            if len(stack) > 0:
                                if state == FORMONE:
                                    stack[-1][0] = LAST
                                names.append(stack)
                            stack = []
                            state = FORMONE
                            nextname = FIRST              
                        if i[0] == NAME:
                            stack.append( [nextname,i[1]] )
                        if i[0] == COMMA:
                            if state == FORMONE:
                                for x in (y for y in stack if y[0] == FIRST):
                                    x[0] = LAST
                                nextname = FIRST
                                state = FORMTWO
                            if state == FORMTWO:
                                state = FORMTHREE
                        if i[0] == VONP and state == FORMONE:
                            nextname = LAST
                            stack.append( list(i) )
                        if i[0] == VONP and state == FORMTWO:
                            nextname = FIRST
                            stack.append( list(i) )
                        if i[0] == JRP:
                            stack.append( list(i) )            
                            pass #wat ?                               
                    if state == FORMONE:
                        if len(stack) > 0:
                            stack[-1][0] = LAST
                        
                    if len(stack) > 0:             
                        names.append(stack) 
                    #print names             
                    v[kk] = [{ "first_name": " ".join([wsnormalize(x[1]).strip() for x in name if x[0] == FIRST]) , "last_name": " ".join([wsnormalize(x[1]).strip() for x in name if x[0] in (LAST, VONP)]), "is_jr": JRP in imap(lambda x: x[0], name) } for name in names]   
                    #print str(self.pubdict[k][kk]).encode("utf-8")
                    #finish last name
                    
        pass

    def prepare_dict(self):
        self.pubdict = []
        for i in self.data:
            self.pubdict.append( [i[1], dict( [ [ObjList2RRSObject.PType , i[0]] ] + [ [x[0],x[1]] for x in i[2:] ] )] ) 

    def do_map(self):
        #print "Creating and mappings"
        self.xmllist = []
        for (k,v) in self.pubdict:
            #print k
            #print v    
            res = RRSPublication()
            par = None
            for kk,vv in v.iteritems():
                
                #nastavenie typu publikacie
                if kk is ObjList2RRSObject.PType:
                    try:
                        res.set('type', vv.lower() )
                    except RRSDatabaseValueError:
                        res.set('type', "article" )
                #pridanie autorov
                #dalsie spracovanie ?
                elif kk.lower() in ("author", "editor"):
                    for name in vv:
                        per = RRSPerson()
                        if name["first_name"]:
                            try:
                                per.set('first_name', name["first_name"])
                            except RRSDatabaseValueError:
                                pass #nejaky default ?
                        if name["last_name"]:
                            try:
                                per.set('last_name', name["last_name"])
                            except RRSDatabaseValueError:
                                pass #nejaky default ?                                              
                        res.set("person_" + kk, per )
                        
                #
                elif kk.lower() in ("institution", "organization", "school"):
                    org = _ee.find_organization(vv)
                    
                    try:
                        res.set('organization', org[0][0])
                    except :
                        res.set('organization', RRSOrganization(type="misc", title=vv)  )
                
                elif kk.lower() == "year": #teor. viac   "month" ,
                    dty = res.get('published_date') or RRSDateTime()
                    try:
                        dty.set(kk, int(vv))
                    except:
                        pass
                    res.set('published_date', dty)
                    
                elif kk.lower() == "month": #teor. viac   
                    dty = res.get('published_date') or RRSDateTime()
                    while True:
                        #mesiac cislo
                        try:
                            dty.set(kk, int(vv))
                            break              
                        except ValueError:
                            pass
                        
                        #mesiac retazec
                        for i, j in BibtexGrammarParser.months.iteritems():
                            if vv in (i, j[0]):
                                dty.set(kk, j[1])
                                break
                                
                        #mesiac lokalizovany retazec
                        try:
                            dto = datetime.datetime.strptime(vv, "%B")
                            dty.set(kk, dto.month )              
                            break
                        except:
                            pass
                        
                        try:
                            dto = datetime.datetime.strptime(vv, "%b")
                            dty.set(kk, dto.month )
                            break 
                        except:
                            pass            

                        month = Set(ObjList2RRSObject.textrange.findall(vv.lower()))
                        dayrn = ObjList2RRSObject.numrange.search(vv.lower())
                        if month is None:
                                break
                        
                        monthlong = month & ObjList2RRSObject.monfull
                        monthshrt = month & ObjList2RRSObject.monabbr
                        if len(monthlong) == 0 and len(monthshrt) == 0:
                                break

                        #print monthlong
                        
                        if len(monthlong) > 0:
                            setel = monthlong.pop()          
                            for i, j in BibtexGrammarParser.months.iteritems():
                                if setel == j[0].lower():
                                    dty.set(kk, j[1])
                                    break
                        else:
                            setelsh = monthshrt.pop()         
                            for i, j in BibtexGrammarParser.months.iteritems():
                                if setelsh == i:
                                    dty.set(kk, j[1])
                                    break
      
                        if dayrn is not None:
                            try:
                                    day = dayrn.group('startnum')
                                    day = int(day)
                                    dty.set('day', day)
                            except IndexError:
                                    pass
                                                                                    
                        break
                        
                    res.set('published_date', dty)
                    
                elif kk.lower() == "booktitle":  #"series",
                    par = RRSPublication()
                    par.set('title', _unescape(vv) ) 
                    pass
                    
                elif kk.lower() == "address":
                    try:
                        res.set('address', _unescape(vv) )           
                    except:
                        pass

                elif kk.lower() == "title":
                    res.set('title', _unescape(vv) )
                    
                elif kk.lower() in ("number", "volume"):
                    val = 0
                    try:
                        val = int(vv)
                    except:
                        pass
                    if res.get('number') is None:
                        res.set('number', val )
                    else:
                        if kk == 'number':
                            res.set('number', val )                  

                elif kk.lower() == "pages":
                    mres = ObjList2RRSObject.numrange.search(vv)
                    if mres:
                        #print mres.groups()
                        try:  
                            #pagecount   = re.compile(r"\D*((?P<startnum\d+)\D*((?P<endnum\d*)\D*", re.I | re.U )        
                            snum = mres.group('startnum')
                            if snum == u'':
                                raise IndexError
                            try:
                                enum = mres.group('endnum')
                                if enum == u'':
                                    raise IndexError
                                val = int(enum)-int(snum)
                                if val == 0:
                                    val = 1
                                res.set('page_count', val)
                            except IndexError:
                                res.set('page_count', int(snum))                        
                        
                        except IndexError:
                            pass

    
                elif kk.lower() == "url":
                    try:
                        urls = _ee.find_url(vv)[0]
                        for i in urls:
                            try:
                                res.set('url', i )
                            except:
                                pass
                                                        
                    except:
                        pass
                      
                        
                else:
                    try:
                        res.set(kk.lower(), _unescape(vv) )
                    except:
                        pass              
                    
            if par is not None:
                par.set('publication', res)
                res = par      
            self.xmllist.append(res)

        return self.xmllist
        
    def transform(self, objectlist):
        self.data = objectlist
        self.prepare_dict()
        self.separate_author_fields()
        return self.do_map()
                        
    def __init__(self, ):
        pass
        
class BibtexGrammarParser(object):

    combiningCharactersCodes = { \
                                  "\"" : unichr(0x0308),#.encode('utf-16'),       #Diaresis
                                  "`" : unichr(0x0300),#.encode('utf-8'),        #Grave
                                  "'" : unichr(0x0301),#.encode('utf-8'),        #Acute
                                  "^" : unichr(0x0302),#.encode('utf-8'),        #Circum
                                  "~" : unichr(0x0303),#.encode('utf-8'),        #Tilde 
                                  "=" : unichr(0x0304),#.encode('utf-8'),        #Macron
                                  "." : unichr(0x0307),#.encode('utf-8'),        #Dot Above
                                  "u" : unichr(0x0306),#.encode('utf-8'),        #Breve
                                  "v" : unichr(0x030C),#.encode('utf-8'),        #Caron  
                                  "r" : unichr(0x030A),#.encode('utf-8'),        #Ring Above  
                                  "H" : unichr(0x030B),#.encode('utf-8'),        #Double Acute 
                                  "c" : unichr(0x0327),#.encode('utf-8'),        #Cedilla  
                                  "t" : unichr(0x0361),#.encode('utf-8'),        #Inverse u over two
                                  "d" : unichr(0x0323),#.encode('utf-8'),        #Dot bellow  
                                  "b" : unichr(0x0331),#.encode('utf-8'),        #Macron bellow                                                                                                                                                                                                                                                                                                       
    }
    
    digraphCharactersCodes = {\
                                  "ae" : unichr(0x00E6),#.encode('utf-8'),
                                  "AE" : unichr(0x00C6),#.encode('utf-8'), 
                                  "oe" : unichr(0x0153),#.encode('utf-8'), 
                                  "OE" : unichr(0x0152),#.encode('utf-8'),
                                  "aa" : u'\u0061\u030A',#.encode('utf-8'), 
                                  "AA" : u'\u0041\u030A',#.encode('utf-8'),
                                  "ss" : unichr(0x00DF),#.encode('utf-8'),
                                  "SS" : u"SS",#.encode('utf-8'),       
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                          
    }
    
    otherCharactersCodes = { \
                                  "o" : unichr(0x00F8),#.encode('utf-8'),        #striked o
                                  "O" : unichr(0x00D8),#.encode('utf-8'),        #same
                                  "i" : unichr(0x0131),#.encode('utf-8'),        #i without dot
                                  "I" : unichr(0x0130),#.encode('utf-8'),        #Circum
                                  "l" : unichr(0x0142),#.encode('utf-8'),        #Tilde 
                                  "L" : unichr(0x0141),#.encode('utf-8'),        #Macron
                                  "!" : unichr(0x00A1),#.encode('utf-8'),        #Dot Above
                                  "?" : unichr(0x00BF),#.encode('utf-8'),        #Breve
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                          
    }
    #brackets
    
    ( PARENTHESES, CURLY, SQUARE, CHEVRONS, HALF ) = (1,2,3,4,5)
    
    tokmap = {\
                  1:(u"(", u")"),
                  2:(u"{", u"}"),
                  3:(u"[", u"]"),
                  4:(u"\u27E8", u"\27E9"),
                  5:(u"(", u")"),                                                                        
    }
      
    latexCommandGrid = {  #name                #params      #delims             #callable
                          u"latex"             : (0,        (),                 lambda: "LaTeX"),        
                          u"texttt"            : (1,        (CURLY,),           lambda x: x    ),  
                          u"emph"              : (1,        (CURLY,),           lambda x: x    ), 
                          u"em"                : (1,        (CURLY,),           lambda x: x    ), 
                          u"verb"              : (1,        (CURLY,),           lambda x: x    ),
                          u"coshit"            : (2,        (CURLY,CURLY),      lambda x,y : y+x  ),
                          u"unichar"           : (1,        (CURLY,),           lambda x: unichr(int(x))  ),               
    }
    
    months =  { \
                "jan": (u"January", 1),
                "feb": (u"February", 2),
                "mar": (u"March", 3),
                "apr": (u"April", 4),
                "may": (u"May", 5),
                "jun": (u"June", 6),
                "jul": (u"July", 7),
                "aug": (u"August", 8),
                "sep": (u"September", 9),
                "oct": (u"October", 10),
                "nov": (u"November", 11),
                "dec": (u"December", 12),
                }  

    publicationTypeList = ['article',  'book',  'booklet',  'inbook',  'incollection',
                           'inproceedings',  'conference',  'manual', 'mastersthesis',
                           'misc',  'phdthesis',  'proceedings',  'techreport',
                           'unpublished', 'periodical' ]
    #relaxed parser regexes
    blockstart  = re.compile(r"@\s*([^{]*?)[{]", re.I | re.U )
    keystart    = re.compile(r"\s*(\S+)\s*,", re.I | re.U )
    asstart     = re.compile(r"(\w+)\s*=" , re.I | re.U )
    valstart    = re.compile(r"\s*(\w+)\s*", re.I | re.U )
    digitals    = re.compile(r"\d+", re.I | re.U )

    ccs         = re.compile(r"""(?<![\\])(?:([\\]{1})(["`'^~=\.uvrHctdb]{1})([{])?(?P<let>\w){1}(?(3)[}]))""", re.U )          
    linb        = re.compile(r"^([{])?((?(1)\s*)(?P<let>\w){1})(?(1)\s*[}])", re.I | re.U )

    whiteSpace  = re.compile(r"([\s])", re.I | re.U )  
    whiteSpaces = re.compile(r"^([\s]+)", re.I | re.U )
    spacesWE    = re.compile(r"([\s]+)", re.I | re.U )  
    pubMatchRE  = re.compile(r"^(\S+?)[\s{]", re.I | re.U )
    fieldabb    = re.compile(r"^([^#=,]+?)[#=,]", re.I | re.U )
    digits      = re.compile(r"^([\d]+?)[\s,}]", re.I | re.U )
    letters     = re.compile(r"^([^,}#\s]+?)[\s,}]", re.I | re.U )
    
    mathmode    = re.compile(r"(?<=\\)\$(.*?)(?<=\\)\$", re.I | re.U)
    

    
    QSTR = 1
    BSTR = 2
    DIGS = 3
    USTR = 4
                                            
    def __init__(self):
        """                  
        Constructor.         
        """ 
        pass
                                                  
    def startParse(self, inputdata):
        self.istream = inputdata
        self.stpos = 0 
        
        self.strSubTable = []
        self.PublicationTable = []      
        self.currentPub = None                 
        lastpos = 0
        lastmatch = DataHolder() 
        while lastmatch.set( BibtexGrammarParser.blockstart.search(self.istream, lastpos) ):
            blck = lastmatch.get()
            self.stpos = blck.start(0)
            lastpos = self.stpos + 1
            if not self._parseBlock():
                self.stpos = blck.start(0)
                self._parseBlockRelaxed(blck)                    
        self._postprocess()
           
        return self.PublicationTable                  

    def _parseBlockRelaxed(self, block):
        #typ extrahuj z bloku
        pubtype = block.group(1).strip()
        self.stpos = block.end(0)
   
        #citation key by regex             
        ckey = BibtexGrammarParser.keystart.match(self.istream, self.stpos )
        citate = None if ckey is None else ckey.group(1)
        self.stpos = self.stpos if ckey is None else ckey.end(0)
        
        #dvojice assigment key = value
        assigs = []       
        while True:
            lastmatch = BibtexGrammarParser.asstart.search(self.istream, self.stpos )
            if not lastmatch: break          
            if "}" in self.istream[self.stpos : lastmatch.start(0)]: break
            key = lastmatch.group(1)
            self.stpos = lastmatch.end(0)
            val = []
            while True:
                try:
                    while (self.istream[self.stpos] == " " or
                           self.istream[self.stpos] == "\t" or
                           self.istream[self.stpos] == "\n"):
                        self.stpos += 1
                except IndexError:
                    pass  
                if self.istream[self.stpos] == "{":
                    res = _read_string(self.istream[self.stpos:], "{", "}",skipescaped = False )
                    self.stpos += res[0]
                    val.append( [BibtexGrammarParser.BSTR,res[1]] )                          
                elif self.istream[self.stpos] == '"':
                    res = _read_string(self.istream[self.stpos:], '"', '"',skipescaped = False )
                    self.stpos += res[0]
                    val.append( [BibtexGrammarParser.QSTR,res[1]] )                
                else:
                    strmatch = BibtexGrammarParser.valstart.match(self.istream, self.stpos )
                    if not strmatch: continue                     
                    self.stpos += (strmatch.end(0)- strmatch.start(0))
                    if ( BibtexGrammarParser.digitals.match(strmatch.group(1)) ):
                        val.append( [BibtexGrammarParser.DIGS, strmatch.group(1)] )
                    else:    
                        val.append( [BibtexGrammarParser.USTR, strmatch.group(1)] )

                try:
                    while (self.istream[self.stpos] == " " or
                           self.istream[self.stpos] == "\t" or
                           self.istream[self.stpos] == "\n"):
                        self.stpos += 1
                except IndexError:
                    break
                               
                if self.istream[self.stpos] == "#":
                    self.stpos += 1 
                    continue
                break
            fass = [key]
            fass.extend(val)
            assigs.append( fass )
           
        if pubtype.lower() == "string":
            fval = [pubtype.lower()]
            fval.extend( assigs )
            self.strSubTable.append(fval)
        else:
            fval = [pubtype.lower(), citate ]
            fval.extend( assigs )
            self.PublicationTable.append(fval)
        
        pass
    
    def _expectTokenAsStr(self, token):
        substr = self.istream[self.stpos : (self.stpos+len(token)) ]
        if substr.lower() == token.lower():
            return True
        return False 

    def _expectTokenAsOneofStr(self, tokens):
        for tok in tokens:
            substr = self.istream[self.stpos : (self.stpos+len(tok)) ]
            if substr.lower() == tok.lower():
                return True
        return False 
        
    def _expectTokenAsRegex(self, tokenre):
        return _firstMatch(tokenre, self.istream, self.stpos)

                            
    def _advanceStream(self, value=1):
        self.stpos += value    

    
    
    def _eatOptionalSpaces(self):
        result = _firstMatch(BibtexGrammarParser.whiteSpaces, self.istream, self.stpos)
        if result == None: return
        self.stpos += result.end(1) - result.start(1)
        
        
    def _parseBlock(self): 
        self._eatOptionalSpaces() 
        if (not self._expectTokenAsStr('@')):
            print "STRICT PARSER ERROR: Not Valid Start of Command block"
            return False
        
        self._advanceStream()

        self._eatOptionalSpaces()
        if (self._expectTokenAsStr('string')):
            self._advanceStream( 6 )    
            return self._parseStringBlock()
        elif (self._expectTokenAsOneofStr(BibtexGrammarParser.publicationTypeList)):
            return self._parsePublicationBlock()
        else:
            print "STRICT PARSER ERROR: Unknown or not informative block"     
      
    def _parseStringBlock(self):
        self._eatOptionalSpaces()
        if (not self._expectTokenAsStr('{') and not self._expectTokenAsStr('(') ):
            print "STRICT PARSER ERROR: Not Valid Start of STRING block"
            return False        
        self._advanceStream()
        self._eatOptionalSpaces()
        self.currentPub = ["string"]
        if not self._parseAssignBlock():
            return False
        
        self._eatOptionalSpaces()             
        if (not self._expectTokenAsStr('}') and not self._expectTokenAsStr(')') ):
            print "STRICT PARSER ERROR: Missing }"
            return False
        self._advanceStream()
        self.strSubTable.append(self.currentPub)    
        return True
        
    def _parsePublicationBlock(self):

        #read publication type
        result = _firstMatch(BibtexGrammarParser.pubMatchRE, self.istream, self.stpos)
        if result == None:
            print "STRICT PARSER ERROR: Cannot find publication type"
            return False
            
            
        self.currentPub = [result.group(1)]
        self._advanceStream(result.end(1) - result.start(1))
            
        self._eatOptionalSpaces()
        
        if (not self._expectTokenAsStr('{')):
            print "STRICT PARSER ERROR: Not Valid Start of Publication block"
            return False        
        self._advanceStream()
        self._eatOptionalSpaces()
        
        result = _firstMatch(BibtexGrammarParser.fieldabb, self.istream, self.stpos)
        if result == None:
            print "STRICT PARSER ERROR: Cannot find citation key"
            return False
            
        self.currentPub.append(re.sub(r'\s', '', result.group(1)))
        self._advanceStream(result.end(1) - result.start(1))
        self._eatOptionalSpaces()           
        if self._expectTokenAsStr(','):
            self._advanceStream()       
        else:
            print "STRICT PARSER ERROR: Missing colon"
            return False
                      
        if not self._parseAssignList():
            return False
        
        self._eatOptionalSpaces()             
        if (not self._expectTokenAsStr('}')):
            print "STRICT PARSER ERROR: Missing }"
            return False
        self._advanceStream()
        self.PublicationTable.append(self.currentPub)
        return True
        
    def _parseAssignList(self):
        self._eatOptionalSpaces()  
        if self._expectTokenAsStr('}'):
            return True
                  
        if not self._parseAssignBlock():
            return False
        self._eatOptionalSpaces() 
        
        if self._expectTokenAsStr(','):
            self._advanceStream()   
        return self._parseAssignList()

            
    def _parseAssignBlock(self):
        result = _firstMatch(BibtexGrammarParser.fieldabb, self.istream, self.stpos)
        if result == None:
            print "STRICT PARSER ERROR: Cannot find assigment key"
            #print self.istream[self.stpos-10:].encode('utf-8')
            return False
        self._advanceStream(result.end(1) - result.start(1))    
        key = re.sub(r'\s', '', result.group(1))
        self.currentPub.append([key])

        if self._expectTokenAsStr('='):
            self._advanceStream()       
        else:
            print "STRICT PARSER ERROR: Missing EQUAL sign"
            return False


        
        if not self._parseValue():
            return False
        else:
            return True
                
    def _parseValue(self):
        self._eatOptionalSpaces()  
        if self._expectTokenAsStr('"'):
            if not self._parseQuotedString():
                return False
        elif self._expectTokenAsStr('{'):
            if not self._parseBracketedString():
                return False 
        elif self._expectTokenAsRegex(BibtexGrammarParser.digits):
            if not self._parseDigits():
                return False
        elif self._expectTokenAsRegex(BibtexGrammarParser.letters):
            if not self._parseUnqotedText():
                return False                
      
        self._eatOptionalSpaces()          
        if self._expectTokenAsStr('#'):
            self._advanceStream()    
            self._eatOptionalSpaces()
            return self._parseValue()
        return True

    def _parseQuotedString(self):
        self._advanceStream()
        tmppos = self.stpos
        depth = 0
        try:    
            while True:
                if self.istream[tmppos] == u"{":
                    depth += 1
                elif self.istream[tmppos] == u"}":
                    depth -= 1
                elif self.istream[tmppos] == u"\\":
                    tmppos += 1
                elif self.istream[tmppos] == u'"':
                    if depth == 0: break
                tmppos += 1
        except:
            print "STRICT PARSER ERROR: Missing terminal quote"
            return False

        self.currentPub[-1].append([BibtexGrammarParser.QSTR, self.istream[self.stpos:tmppos]])
        self._advanceStream(tmppos-self.stpos)
        self._advanceStream()
        return True
        
    def _parseBracketedString(self):
        self._advanceStream()
        tmppos = self.stpos
        depth = 1
        try:                      
            while True:
                if self.istream[tmppos] == u"\\":
                    tmppos += 1      
                elif self.istream[tmppos] == u"{":
                    depth += 1
                elif self.istream[tmppos] == u"}":
                    depth -= 1
                    if depth == 0: break
                tmppos += 1
        except IndexError:
            print "STRICT PARSER ERROR: Missing terminal bracket"
            return False      
        self.currentPub[-1].append([BibtexGrammarParser.BSTR,self.istream[self.stpos:tmppos]])
        self._advanceStream(tmppos-self.stpos)
        self._advanceStream()
        return True
        
    def _parseDigits(self):
        result = _firstMatch(BibtexGrammarParser.digits, self.istream, self.stpos)
        if result == None:
            print "STRICT PARSER ERROR: Should not happen"
            return False
        self.currentPub[-1].append([BibtexGrammarParser.DIGS,self.istream[self.stpos:(self.stpos+(result.end(1) - result.start(1)))]])    
        self._advanceStream(result.end(1) - result.start(1))

        return True
        
    def _parseUnqotedText(self):
        result = _firstMatch(BibtexGrammarParser.letters, self.istream, self.stpos)
        if result == None:
            print "STRICT PARSER ERROR: Should not happen"
            return False
        self.currentPub[-1].append([BibtexGrammarParser.USTR,self.istream[self.stpos:(self.stpos+(result.end(1) - result.start(1)))]])    
        self._advanceStream(result.end(1) - result.start(1))

        return True
    
    def _postprocess(self):
        #print self.strSubTable
        for l in self.strSubTable:
            for k in ( vk for vk in l if isinstance(vk, list)):
                for j in ( vj for vj in k if isinstance(vj, list) ):
                    if j[0] in (1,2): 
                        #print "vstup  " + j[1].encode("utf-8")      
                        (j[1], tmp) = self.string_parser(j[1])     
                        #print "vystup " + j[1].encode("utf-8")
                    self._perform_string_sym_substitutions(j)
                                              
                self._merge_strings(k)        
                
        #print self.PublicationTable                  
        for l in self.PublicationTable:
            for k in ( vk for vk in l if isinstance(vk, list)):
                for j in ( vj for vj in k if isinstance(vj, list) ): 
                    if j[0] in (1,2): 
                        #print "vstup  " + j[1].encode("utf-8")
                        (j[1], tmp) = self.string_parser(j[1], level = (k[0] not in ("author","editor")) )      
                        #print "vystup " + j[1].encode("utf-8")
                    self._perform_string_sym_substitutions(j)
                                            
                self._merge_strings(k)

            
    def string_parser(self,input, level=0, opclosetoks = CURLY,keepescaping = True):
        #print "Entering string parser level: " + str(level)
        index = 0
        aggregate = 0
        content = input
        output = u""
        data = DataHolder()
        data2 = DataHolder()
        (optoken, cltoken) = BibtexGrammarParser.tokmap[opclosetoks] 
        try:
            while True:      
                if content[index] == u"\\":   #backslash
                    output = output + content[aggregate:index]
                    index = index + 1
                    #latex commands ver 2
                    #author={Chris Lat\latextner and {Vikr{\.am } Adve} },
                    tmpcon = content[index:].lower()
                    if data.set( (lambda x: [mat for mat in BibtexGrammarParser.latexCommandGrid.iterkeys() if x.startswith(mat.lower())]) (tmpcon) ):
                        result = sorted(data.get(), key=lambda x:len(x), reverse = True )[0]
                        index +=  len(result)
                        paramlist = []
                        
                        #print "Command FOUND"
                        #print content[index:]
                        #print result         
                        
                        for i in xrange(0, BibtexGrammarParser.latexCommandGrid[result][0]):
                            #print BibtexGrammarParser.tokmap[BibtexGrammarParser.latexCommandGrid[result][1][i]][0]
                            if content[index] != BibtexGrammarParser.tokmap[BibtexGrammarParser.latexCommandGrid[result][1][i]][0]:
                                print "STRING PARSER ERROR: Wrong opening token"
                            index += 1
                            (subres, subval) = self.string_parser( content[index:], level+1 )
                            #print "PARAM " + subres
                            index += subval + 1
                            paramlist.append(subres)              
                            #print "param FOUND"
                            #print content[index:]       
                        
                        try:                              
                            res = BibtexGrammarParser.latexCommandGrid[result][2](*paramlist)
                        except:
                            pass # log error in command execution
                        #print res
                        output += res          
    
                        """     
                        output = output + content[aggregate:index]
                        if level == 0: output += optoken
                        index = index + 1          
                        (newstr, count) = self.string_parser( content[index:], level+1 )           
                        output = output + newstr
                        if level == 0: output += cltoken                              
                        index += count + 1
                        aggregate = index              
                        """
                                                                                    
                    #combining characters
                    elif content[index] in BibtexGrammarParser.combiningCharactersCodes.keys():               
                        ccc = content[index]
                        res = BibtexGrammarParser.linb.match(content[index+1:])
                        if res:
                            output = output + res.group('let') + BibtexGrammarParser.combiningCharactersCodes[ccc]
                            index = index + len(res.group(0)) 
                            index = index + 1
                        else:
                            output = output + ccc
                            index = index + 1
                    #digraphs
                    elif content[index:index+2] in BibtexGrammarParser.digraphCharactersCodes.keys(): 
                            ccc = content[index:index+2]
                            output = output + BibtexGrammarParser.digraphCharactersCodes[ccc]
                            index = index + 2
                    #slashes
                    elif content[index] in BibtexGrammarParser.otherCharactersCodes.keys(): 
                            ccc = content[index]
                            output = output + BibtexGrammarParser.otherCharactersCodes[ccc]
                            index = index + 1
                    else:    #backslashed symbols or quotes
                            ccc = content[index]
                            output = output + u"\\" + ccc            
                            index = index + 1
                                                
                    aggregate = index
                elif content[index] == optoken:   # lbrace
                    output = output + content[aggregate:index]
                    if level == 0: output += optoken
                    index = index + 1          
                    (newstr, count) = self.string_parser( content[index:], level+1 )           
                    output = output + (newstr)  #.strip()
                    if level == 0: output += cltoken                              
                    index += count + 1
                    aggregate = index 
                elif content[index] == cltoken:   # rbrace
                    output = output + content[aggregate:index]         
                    if level > 0 : return (output, index)          
                    index = index + 1            
                    aggregate = index             
                elif content[index] == u"$":   # dollar
                    output = output + content[aggregate:index]
                    index = index + 1            
                    aggregate = index                                     
                else: #other
                    index = index + 1                
                """    
    
                """      
        except IndexError:
            pass #hotovo
        output = output + content[aggregate:index]
        return (output, index)
        
    def _perform_string_sym_substitutions(self, inlst):
        data = DataHolder()
        #transformacia skratiek mesiacov ?
        if data.set(_one_of(BibtexGrammarParser.months.keys(), inlst[1].lower() )):
            inlst[1] = BibtexGrammarParser.months[data.get()][0]
            return
                  
        if data.set(_one_of([x[1][0] for x in self.strSubTable], inlst[1])):
            #FIX THIS
            inlst[1] = [ x[1][1] for x in self.strSubTable if x[1][0] == data.get() ][0]
            return 

    def _merge_strings(self, strlist):
        outstr = ""
        for i in strlist[1:]:
            outstr = outstr + i[1]
        strlist[1:] = [outstr]
        pass
        
    def _normalize_whitespaces(self, inlst):
        if inlst[0] in (1,2):
            inlst[1] = BibtexGrammarParser.spacesWE.sub(" ", inlst[1] )       
                        
class BibTeXParser(object):
                
        def __init__(self):
                """
                Constructor.    
                """
                self.btgp = BibtexGrammarParser()
                self.mapr = ObjList2RRSObject()

        def parse(self, inputdata):
                """
                Parses lxml.etree._ElementTree object or plain text and harvests all
                data from BibTeX-formatted text. Returns hierarchical model of rrs db.
                Top-level entity is RRSPublication (because BibTeX is format for 
                representing publications).
                """                
                if isinstance(inputdata, etree._ElementTree):
                    inputdata =  html2text( inputdata )
                elif isinstance(inputdata, str):
                    pass # decode
                elif isinstance(inputdata, unicode):
                    pass # donothing
                else: 
                    raise BibTeXParserError("Input of BibTeXParser.parse() has to be "\
                                            "either type lxml.etree._ElementTree or "\
                                            "string. Got: " + str(type(inputdata)))
                                                                                                                                    
                objlist = self.btgp.startParse(inputdata.decode("utf-8"))
                xmls = self.mapr.transform(objlist)
                
                return xmls

#-------------------------------------------------------------------------------
# end of class BibTeXParser
#-------------------------------------------------------------------------------

class BibtexGrammarParserTestCases(unittest.TestCase):
    def setUp(self):
        self.btgp = BibtexGrammarParser()        
    
    def testBlockLookup(self):
        print "Testing block lookup..."
        testdata = """
                    @Book{     Lamport:LDP85   ,
                               title =  "kniha1", 
                               author = "Matusko Kontra""
                               institution = "fit vutbr"
                    }
                    @Article{  Lamport:LDP86   ,
                               title =  "kniha1", 
                               author = "Matusko Kontra""
                               institution = "fit vutbr"
                    }
                    @Article}  Lamport:LDP87   ,
                               title =  "kniha1", 
                               author = "Matusko Kontra""
                               institution = "fit vutbr"
                    }                                           
        """
        
        result = self.btgp.startParse(testdata)
        supposedresult = [['Book', 'Lamport:LDP85', ['title', u'kniha1'], ['author', u'Matusko Kontra'], ['"institution', u'fit vutbr']], ['Article', 'Lamport:LDP86', ['title', u'kniha1'], ['author', u'Matusko Kontra'], ['"institution', u'fit vutbr']]]
        self.assertEqual(result, supposedresult)
        pass
        
    def testStringLoc(self):
        print "Testing String Block Identification..."
        testdata = """
                    @String{   Lamport:LDP85   ,
                               title =  "kniha1", 
                               author = "Matusko Kontra""
                               institution = "fit vutbr"
                    }
                    @String{ title =  "kniha1" }                                       
        """

        result = self.btgp.startParse(testdata)
        strtab = self.btgp.strSubTable
        supposedresult = [['string', ['title', u'kniha1'], ['author', u'Matusko Kontra'], ['institution', u'fit vutbr']], ['string', ['title', u'kniha1']]]
        self.assertEqual(strtab, supposedresult)
        pass
    
    def testPublicLoc(self):
        print "Testing Publication Block Identification..."
        testdata = """
                    @    Book     {     Lamport:LDP85   ,
                               title =  "kniha1", 
                               author = "Matusko Kontra""
                               institution = "fit vutbr"
                    }
                    @     INProCeedINgs      {  Lamport:LDP86   ,
                               title =  "kniha1", 
                               author = "Matusko Kontra""
                               institution = "fit vutbr"
                    }
                    @Article}  Lamport:LDP87   ,
                               title =  "kniha1", 
                               author = "Matusko Kontra""
                               institution = "fit vutbr"
                    }                                           
        """
        
        result = self.btgp.startParse(testdata)
        supposedresult = [['Book', 'Lamport:LDP85', ['title', u'kniha1'], ['author', u'Matusko Kontra'], ['"institution', u'fit vutbr']], ['INProCeedINgs', 'Lamport:LDP86', ['title', u'kniha1'], ['author', u'Matusko Kontra'], ['"institution', u'fit vutbr']]]
        self.assertEqual(result, supposedresult)
        pass
        
    def testCitKeyLoc(self):
        print "Testing Citation Key Identification..."
        testdata = """
                    @    Book                  {     KUSTE_KSTU::546:54,,,   ,
                               title =  "kniha1", 
                               author = "Matusko Kontra""
                               institution = "fit vutbr"
                    }
                    @     INProCeedINgs {  LAMPORT   ,
                               title =  "kniha1", 
                               author = "Matusko Kontra""
                               institution = "fit vutbr"
                    }
                    @Article}  Lamport:LDP87   ,
                               title =  "kniha1", 
                               author = "Matusko Kontra""
                               institution = "fit vutbr"
                    }                                           
        """
        
        result = self.btgp.startParse(testdata)
        supposedresult = [['book', 'KUSTE_KSTU::546:54,,,', ['title', u'kniha1'], ['author', u'Matusko Kontra'], ['institution', u'fit vutbr']], ['INProCeedINgs', 'LAMPORT', ['title', u'kniha1'], ['author', u'Matusko Kontra'], ['"institution', u'fit vutbr']]]
        self.assertEqual(result, supposedresult)
        pass
        
    def testSingleAssign(self):
        print "Testing Single Assign..."
        testdata = """
                    @    Book  { citkey,
                               title =  "kniha1"
                            }
                    @    Book  { citkey2,
                               title =  {{kniha}}
                            } 
                    @    Book  { citkey3,
                               title = kniha
                            } 
                    @    Book  { citkey4,
                               title =  123
                            }                                                                                                                                
        """
        
        result = self.btgp.startParse(testdata)
        supposedresult = [['Book', 'citkey', ['title', u'kniha1']], ['Book', 'citkey2', ['title', u'kniha']], ['Book', 'citkey3', ['title', 'kniha']], ['Book', 'citkey4', ['title', '123']]]
        self.assertEqual(result, supposedresult)
        pass
        
    def testSingleAssignMulti(self):
        print "Testing Single Assign Multiple Parts..."
        testdata = """
                    @    Book  { citkey,,,
                               title =  "kniha1" # "kniha2"
                            }
                    @    Book  { citkey2,
                               title =  {{kniha}} # 123
                            } 
                    @    Book  { citkey3,
                               title = kniha # {nicota}
                            } 
                    @    Book  { citkey4,   
                               title =  123 # 321 # "ant" # {{last}}
                            }                                                                                                                                
        """
        
        result = self.btgp.startParse(testdata)
        supposedresult = [['book', 'citkey,,', ['title', u'kniha1kniha2']], ['Book', 'citkey2', ['title', u'kniha123']], ['Book', 'citkey3', ['title', u'knihanicota']], ['Book', 'citkey4', ['title', u'123321antlast']]]
        self.assertEqual(result, supposedresult)
        pass
        
    def testMultiAssigns(self):
        print "Testing Single Assign..."
        testdata = """
                    @    Book  { citkey,
                               title =  "kniha1",,author =  "matus",
                               title =  "kniha1",,author =  "matus",
                            }
                    @    Book  { citkey2,
                               title =  {{kniha}},
                               organization = ,
                               published = 12212010
                            } 
                                                                                                                           
        """
        
        result = self.btgp.startParse(testdata)
        supposedresult = [['book', 'citkey', ['title', u'kniha1'], ['author', u'matus'], ['title', u'kniha1'], ['author', u'matus']], ['Book', 'citkey2', ['title', u'kniha'], ['organization', ''], ['published', '12212010']]]
        self.assertEqual(result, supposedresult)
        pass
        
    def testPostProcStringParse(self):
        print "Testing Postprocesing String Parsing Operations..."
        testdata = r"""
                    @    Book  { citkey,
                               title = {\unichar{65}lfa, \"o \"u \ae \AE \l \!},
                            }

                                                                                                                           
        """
        
        result = self.btgp.startParse(testdata)[0][2][1]
        supposedresult = u"Alfa, ö ü æ Æ ł ¡"
        self.assertEqual(result, supposedresult)
        pass
        
    def testPostProcMergeandSub(self):
        print "Testing String Mergings..."
        testdata = """
                    @    String  { CITKEY = "Sometext"
                            }
                    @    Book  { citkey2,
                               title =  {{kniha}} # CITKEY,
                               month = jan
                            } 
                                                                                                                           
        """
        
        result = self.btgp.startParse(testdata)
        supposedresult = [['Book', 'citkey2', ['title', u'knihaSometext'], ['month', u'January']]]
        self.assertEqual(result, supposedresult)
        pass



#-------------------------------------------------------------------------------
# end of class BibTeXParserTestSuite
#-------------------------------------------------------------------------------

class ObjList2RRSObjectTestCases(unittest.TestCase):

    def setUp(self):
        self.btpr = BibTeXParser()

    def testRRSObjCreation(self):
        print "Testing General Functionality..."
        testdata = """
                    @    String  { CITKEY = "Sometext"
                            }
                    @    Book  { citkey2,
                               title =  {{kniha}} # CITKEY,
                               month = jan,
                               year = 1933
                            } 
                                                                                                                           
        """    
        result = self.btpr.parse(testdata)[0]
        respubtype = result.get('type')
        restitle = result.get('title')
        respubdateyr = result.get('published_date').get('year')
        respubdatemn = result.get('published_date').get('month')        
        self.assertEqual(respubtype, "book")
        self.assertEqual(restitle, "knihaSometext")
        self.assertEqual(respubdateyr, 1933)
        self.assertEqual(respubdatemn, 1)
        
    def testPubTypeExtraction(self):
        print "Testing Mappings of Publication type..."
        testdata = """
                    @    Book  { citkey2,
                               title =  {{kniha}} # CITKEY ,
                               month = jan,
                               year = 1933
                            } 
                    @    InPrOceedInGs  { citkey3,
                               title =  {{kniha}} ,
                               fag = "tar" ,
                               year = 1933 ,
                            } 
                    @    UnKnownValue  { citkey2,
                               title =  {{kniha}} # CITKEY,
                               month = jan,
                               year = 1933
                            }                             
                                                                                                                           
        """ 
        presult = self.btpr.parse(testdata)   
        result = (  presult[0].get('type'),
                    presult[1].get('type'),
                    presult[2].get('type') )

        suppoosedresult = ('book', 'inproceedings', 'article')       
        self.assertEqual(result, suppoosedresult)
                                
    def testNameSeparationAndProcessing(self):
        print "Testing Name fields processing..."
        testdata = r"""
                    @Book{ citkey2,
                           author = "Fred de la Ubermann and Beethoven, jr.,Ludwig",                           
                         } 
                                                                                     
        """ 
        presult = self.btpr.parse(testdata)
        auth1 = presult[0].get('person_author')[0]
        auth2 = presult[0].get('person_author')[1]            
        a1names = ( auth1.get('first_name'), auth1.get('last_name') )
        a2names = ( auth2.get('first_name'), auth2.get('last_name') )
        result = [a1names, a2names]
        supposedresults = [("Fred","de la Ubermann"), ("Ludwig","Beethoven")]       
        self.assertEqual(result, supposedresults)
        
    def testOrganizationExtraction(self):
        print "Testing Organization Extraction..."
        testdata = r"""
                    @Book{ citkey2,
                           organization = "University of South California",                           
                         } 
                                                                                     
        """ 
        presult = self.btpr.parse(testdata)[0].get('organization')[0]
        result  = ( presult.get('type'), presult.get('title') )
        supposedresults = ("university","University of South California") 
        self.assertEqual(result, supposedresults)

    def testDateTimeExtraction(self):
        print "Testing Date and Time Extraction..."
        testdata = r"""
                    @Book{ citkey2,
                           month = "January",
                           year = 1950                           
                         } 
                    @Book{ citkey5,
                           month = 11,
                           year = 1950                           
                         }                                                                                      
        """ 
        presult = self.btpr.parse(testdata)
        dt1 = presult[0].get('published_date')
        dt2 = presult[1].get('published_date')        
        respubdateyr1 = dt1.get('year')
        respubdatemn1 = dt1.get('month')
        respubdateyr2 = dt2.get('year')
        respubdatemn2 = dt2.get('month')
        result = ( respubdateyr1, respubdatemn1, respubdateyr2, respubdatemn2 )
        supposedresults = ( 1950, 1, 1950, 11 )        
        self.assertEqual(result, supposedresults)
        
    def testBookititle(self):
        print "Testing Booktitle Extraction..."
        testdata = r"""
                    @Book{ citkey2,
                            title = "Child Text",
                           booktitle = "Mother Volume",                           
                         } 
                                                                                     
        """ 
        presult = self.btpr.parse(testdata)[0]
        result  = ( presult.get('title'), presult.get('publication')[0].get('title') )
        supposedresults = ("Mother Volume","Child Text") 
        self.assertEqual(result, supposedresults)        

    def testPages(self):
        print "Testing Page Ranges Extraction..."
        testdata = r"""
                    @Book{ citkey2,
                           pages = "27-36",                           
                         } 
                                                                                     
        """ 
        presult = self.btpr.parse(testdata)[0].get('page_count')
        result  = int(presult)
        supposedresults = 9
        self.assertEqual(result, supposedresults)

    def testNumberVolumes(self):
        print "Testing Volume Numbers Extraction..."
        testdata = r"""
                    @Book{ citkey2,
                           number = 6,
                           volume = 4                           
                         } 
                         
                    @Book{ citkey3,
                           volume = 5                           
                         }                          
                                                                                     
        """ 
        presult = self.btpr.parse(testdata)
        pub1 = presult[0].get('number')
        pub2 = presult[1].get('number')
        result  = ( pub1, pub2 )
        supposedresults = ( 6,5 )  
        self.assertEqual(result, supposedresults)

    def testURL(self):
        print "Testing URL Extraction..."
        testdata = r"""
                    @Book{ citkey2,
                           url = "http://www.lokacia/subloc/file.php?query=data",                           
                         } 
                                                                                     
        """ 
        presult = self.btpr.parse(testdata)[0]
        result  = presult.get('url')[0].get('link')
        supposedresults = "http://www.lokacia/subloc/file.php" 
        self.assertEqual(result, supposedresults)
        
class BibtexParserTestCases(unittest.TestCase):
    def setUp(self):
        pass
        
    def testBigSampleInputTest(self):
        pass       
        
################################################################################

# main
if __name__ == '__main__':
        unittest.main()

