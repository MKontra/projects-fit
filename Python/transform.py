import sys, csv, math, numpy
from itertools import *

def open_csvs(filelist):
  retval = []
  for f in filelist:
    retval.append( csv.reader(open(f, 'r'), delimiter=';', quotechar='"') )  
  return retval

def get_colnames(csvfiles):
  retval = []
  for f in csvfiles:
    retval.append( f.next() )  
  return retval 
  
def csvdicts( csvs, colnames ):
  retval = []
  diter = zip(colnames, csvs)
  for cd in diter:
    toadd = {}
    for j in xrange(len(cd[0])):
      toadd[cd[0][j]] = []       
    for row in cd[1]:
      for i in xrange(len(cd[0])):
        toadd[cd[0][i]].append( row[i])
    retval.append(toadd)
  return retval
    

fnames = [ "account.csv",
           "card.csv",
           "client.csv",
           "disp.csv",
           "loan.csv",
           "order.csv",
           "trans.csv",
           "district.csv" ]      # 

csvs = open_csvs(fnames)           
account, card, client, disp, loan, order, trans, district = csvs   #

colnames = get_colnames(csvs)

coldicts = csvdicts(csvs, colnames)
accountdict, carddict, clientdict, dispdict, loandict, orderdict, transdict, districtdict = coldicts  #

tdict = {}
for c in accountdict["account_id"]:
  tdict[c] = [[0],[0.0,0]]
  
for t in izip(transdict["account_id"],transdict["trans_id"], transdict["balance"],transdict["k_symbol"]):    
  if t[3] == "UVER":
    tdict[t[0]][0][0] = tdict[t[0]][0][0] + 1   

  tdict[t[0]][1][0] = tdict[t[0]][1][0] + float(t[2]) 
  tdict[t[0]][1][1] = tdict[t[0]][1][1] + 1
  
for v in tdict.itervalues():
  v[1] =  v[1][0]/float(v[1][1])     
                                                                                                          
print '''"client_id";"age";"sex";"leasings";"uvers";"balance_avg";"trans_uvers";"card_type";"district_id";"A3";"A4";"A5";"A6";"A7";"A8";"A9";"A10";"A11";"A12";"A13";"A14";"A15";"A16";"loan_status"'''
  
for clientset in izip(clientdict["client_id"], clientdict["birth_number"], clientdict["district_id"]):
  client = clientset[0]
  dispacc =[(x[0], x[2])\
            for x 
            in izip(dispdict["disp_id"], dispdict["client_id"],dispdict["account_id"],dispdict["type"]) 
            if x[3] == "OWNER" and x[1] == client ]

  if len(dispacc) == 0:
    continue
                
  cards =  [ x[1] \
            for x in izip(carddict["disp_id"], carddict["type"]) if x[0] == dispacc[0][0] ]
            
  orders =[ x[1] \
            for x in izip(orderdict["account_id"],orderdict["k_symbol"]) if x[0] == dispacc[0][1] ]

  
            
  trans = tdict[dispacc[0][1]]
  
  loans = [ x[1] \
            for x in izip(loandict["account_id"],loandict["status"]) if x[0] == dispacc[0][1] ]

  number = int(clientset[1])
  sex = "male"
  if number/100 % 100 >= 50:
    sex = "female"
    
  yearofbirth = number/10000   
  
  district = clientset[2]   

  leasecount = len( [x for x in orders if x == "LEASING"] )
  uvercount  = len( [x for x in orders if x == "UVER"] )  
  
  balanceavg = trans[1] 
  trans_uvers = trans[0][0]

    
  card = "none"
  if len(cards) != 0:
    card = cards[0] 
     
  if len(loans) == 0 or loans[0] == "":
    continue
      
  loanstatus = ""
  if len(loans) != 0:
    loanstatus = loans[0]    
    

  
  strset = client + ";" + str(yearofbirth) + ";" + '"' + sex + '";'
  strset = strset + str(leasecount) + ";" + str(uvercount) + ";" + str(balanceavg) + ";" + str(trans_uvers)+ ";" 
  strset = strset + '"' + card + '";' + district + ";" 

  districtrow = districtdict["A1"].index(district)
  strset = strset +  '"' + districtdict["A3"][districtrow] + '";'
    
  for i in ["A4","A5","A6","A7","A8","A9","A10","A11","A12","A13","A14","A15","A16"]:
    strset = strset + str(districtdict[i][districtrow]) + ";"
  
  strset = strset +  '"' + loanstatus + '"'
         
  print strset 
         