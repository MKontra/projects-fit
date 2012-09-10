from random import uniform
from functools import reduce
from numpy import *
import numpy
import math

def makeWeightMatrix(x,y):
    return numpy.empty(x,y) #empty ndarray

#trieda reprezentujuca siet
class ffnet(object):

    def __init__( self,In, Hid, Out ):
    	self.In = In #+1 for bias

    	valid = False

    	if type(Hid) == int:
    		Hid = (Hid,)

      #kontrola parametrov
    	if ( type(Hid) == tuple or type(Hid) == list ):
    		valid = reduce(lambda x,y: x and (type(y) == int), Hid, True )
    	if ( not valid ):
    		print "Parameter mismatch"
    		raise "Parameter Mismatch"

    	self.Hid = Hid
    	self.Out = Out
    #vytvorrenie pola vah +1 = bias neuron
        self.HidWeights = []
    	self.HidWeights.append(((numpy.random.rand(Hid[0], self.In + 1)-1)*0.4))
    	self.HidWeights.extend([((numpy.random.rand(Hid[i], Hid[i-1] + 1)-1)*0.4) for i in range(1,len(Hid))])

        self.OutWeights = ((numpy.random.rand(Out, Hid[-1] + 1)-1)*1)

        self.InActvs = ones(self.In + 1)
    	self.HidActvs = [ones(Hid[i] + 1) for i in range(len(Hid))]
    	self.OutActvs = ones(Out)

      #funkcia vypocu vystupu
    def activate(self, inputs):
        if (len(inputs) != self.In):
            raise "Bad number of inputs"

        #vystup vstupu je vstupny vektor
        for i in range(self.In):
            self.InActvs[i] = inputs[i]

        #prva skryta vrstva - vahy x vystupy vstupu 
        numpy.dot(self.HidWeights[0], self.InActvs, out=self.HidActvs[0][0:-1])
        numpy.tanh(self.HidActvs[0][0:-1], out=self.HidActvs[0][0:-1])

        #ostatne skryte vrstvy - vahy x predchadzajuce vystupy
        for i in range(1, len(self.HidWeights)):
            numpy.dot(self.HidWeights[i],self.HidActvs[i-1], out=self.HidActvs[i][0:-1])
            numpy.tanh(self.HidActvs[i][0:-1], out=self.HidActvs[i][0:-1])

        numpy.dot(self.OutWeights, self.HidActvs[-1], out=self.OutActvs)
        numpy.tanh(self.OutActvs, out=self.OutActvs)

        return self.OutActvs

    def printWeights(self):
        for i in range(len(self.Hid)):
            print "Vrstva " + str(i) + ":"
            print self.HidWeights[i]

        print "Vystupna vrstva:"
        print self.OutWeights

    def getNetworkStats(self):
        return (self.In, self.Hid, self.Out)

    def getWeightsAsNumpyArray(self):
        return (numpy.array(self.HidWeights), numpy.array(self.OutWeights ))

    def setWeightsFromNumpyArray(self, HidWeights, OutWeights):
        self.HidWeights = HidWeights
        self.OutWeights = OutWeights

#backprop trainer
class BackPropTrainer(object):
    def __init__(self, net, N):
        self.net = net
        self.N = N
        #polia delt
        self.odelta = numpy.empty_like(net.OutActvs)
        self.hdelta = [ numpy.empty_like(net.HidActvs[i]) for i in range(len(net.HidActvs)) ]

    def backPropagate(self, expectedValues):
        #vypocet delt - ideme odzadu - delty predchadzajucej vrstvy x transponovana matica vah
        self.odelta = (1.0 - (self.net.OutActvs*self.net.OutActvs))*(expectedValues - self.net.OutActvs)

        self.hdelta[-1] = (1.0 - (self.net.HidActvs[-1]*self.net.HidActvs[-1]))*( dot(numpy.transpose(self.net.OutWeights),self.odelta) )

        for i in reversed(xrange(len(self.net.HidWeights) - 1)):
            self.hdelta[i] = (1.0 - (self.net.HidActvs[i]*self.net.HidActvs[i]))*( dot(numpy.transpose(self.net.HidWeights[i]),self.hdelta[i+1][0:-1]) )

        #update weights
        for i in range(len(self.net.HidWeights[0])):
            numpy.add(self.net.HidWeights[0][i], (self.N*self.net.InActvs*self.hdelta[0][i]), out = self.net.HidWeights[0][i])

        for i in range(1, len(self.net.HidActvs)):
            for j in range(len(self.net.HidWeights[i])):
                numpy.add(self.net.HidWeights[i][j], (self.N*self.net.HidActvs[i-1]*self.hdelta[i]), out = self.net.HidWeights[i][j])

        for i in range(len(self.net.OutActvs)):
            numpy.add(self.net.OutWeights[i], (self.N*self.net.HidActvs[-1]*self.odelta[i]), out = self.net.OutWeights[i])

        return 0.5*numpy.add.reduce(((expectedValues - self.net.OutActvs)*(expectedValues - self.net.OutActvs)))

    #ucenie siete
    def train(self, data, iterations = 1000, maxerror = 0.001):
        error = 0.0
        #kym nie je vycerpany prideleny pocet iteracii
        for i in xrange(iterations):
            error = 0.0
            for d in data:
                inputs = d[0]
                expected = d[1]
                self.net.activate(inputs)
                error = error + self.backPropagate(expected)
            if error < maxerror: # ak je siet naucena skonci
                return (i, error)
        return ( -1, error )

class AnnealTrainer(object):
    def __init__(self, net):
        self.net = net #(self.In, self.Hid, self.Out)
        self.nIn = net.In
        self.nHid = net.Hid
        self.nOut = net.Out
        
        self.dirHidWeights = []
        for i in xrange(len(self.net.HidWeights)):
          self.dirHidWeights.append( numpy.zeros_like(self.net.HidWeights[i]) ) 
        self.dirOutWeights = numpy.zeros_like( self.net.OutWeights )       

    #uloz si teploty, vypocitaj faktor poklesu
    def setTemps(self, maxTemp, minTemp):
        self.maxTemp = maxTemp
        self.minTemp = minTemp
        self.currentTemp = maxTemp
        self.TempFactor = -math.log( float(maxTemp) / float(minTemp) )

    #perturbuj vahy maticou (-1,1)*weightStep, maxerror ignorovany
    def perturbeWeightSet(self, Weights, weightStep = 0.1, maxError = 0.1):
        HidWeights, OutWeights = Weights
        RetHidWeights = []
        RetOutWeights = None

        step = weightStep #float(self.currentTemp) / self.maxTemp

        for i in xrange(len(HidWeights)):
            randMat = numpy.random.rand(HidWeights[i].shape[0], HidWeights[i].shape[1])
            randMat = randMat - 0.5
            randMat = randMat * 2 * step
            numpy.add(HidWeights[i], randMat, out=randMat)
            RetHidWeights.append(randMat)

        randMatOut = numpy.random.rand(OutWeights.shape[0], OutWeights.shape[1])
        randMatOut = randMatOut - 0.5
        randMatOut = randMatOut * 2 * step
        numpy.add(OutWeights, randMatOut, out=randMatOut)
        RetOutWeights = randMatOut
        return (RetHidWeights, RetOutWeights)

    #funckcia prijatia
    def P(self, e, ei, t):
      if ( ei < e ): return 1.0
      return 0.5*float(t)/self.maxTemp
      #return (ei-e)/float(e)*t
            
    def anneal(self):
        #sucasne a najlepsie hodnoty
        bestNet = ( self.net.HidWeights, self.net.OutWeights )
        bestError = self.getErrorForDataSet()
        nextNet = None
        nextError = None
        
        #pocet teplotnych stupnov    
        for i in xrange(0, self.grades):                                                            
            currentNet = bestNet
            currentError = bestError
            #pocet vnutornych iteracii 
            for k in xrange(self.lvlmax):
                #novy stav
                nextNet = self.perturbeWeightSet(currentNet, float(self.currentTemp)/self.maxTemp*0.1)             
                self.net.HidWeights = nextNet[0]
                self.net.OutWeights = nextNet[1]
                #jeho chyba
                nextError = self.getErrorForDataSet()                 
                #print "tempdata currenterror:" +str(currentError)+" nexterror:"+str(nextError)+" diff:"+str(currentError-nextError)+" currentTemp:"+str(self.currentTemp)+" all:"+ str(self.P(currentError, nextError, self.currentTemp))
                #kriteriium prijatia
                if (numpy.random.uniform() < self.P(currentError, nextError, self.currentTemp)):
                    currentNet = nextNet
                    currentError = nextError
                else: #restore values
                    self.net.HidWeights = currentNet[0]
                    self.net.OutWeights = currentNet[1]

                if ( currentError < bestError ):
                    bestNet = currentNet
                    bestError = currentError

                if bestError <= self.maxerror:
                    self.net.HidWeights = bestNet[0]
                    self.net.OutWeights = bestNet[1]
                    return (i*self.lvlmax+k, bestError )
            #pokles teploty
            ratio = math.exp(math.log(float(self.minTemp)/self.maxTemp )/(self.grades-1));
            self.currentTemp = self.currentTemp * ratio;

        self.net.HidWeights = bestNet[0]
        self.net.OutWeights = bestNet[1]
        return (-1, bestError)

    def getErrorForDataSet(self):
        error = 0.0
        for d in self.data:
            d1npya = numpy.array(d[1])
            square = ( d1npya - self.net.activate(d[0]))*( d1npya - self.net.activate(d[0]))
            error = error + ( numpy.add.reduce(square) )
        return error*0.5

    def getAbsoluteError(self):
        error = 0.0
        for d in self.data:
            d1npya = numpy.array(d[1])
            square = ( d1npya - self.net.activate(d[0]))
            numpy.absolute(square, out = square)
            error = error + ( numpy.add.reduce(square) )
        return error*0.5    

    def getMaxNetworkErrorForDataset(self):
        forone = 0.0
        for d in self.data:
            d1npya = numpy.array(d[1])
            square = ( 1 - d1npya)*( 1 - d1npya )
            forone = forone + ( numpy.add.reduce(square) )
            
        forminusone = 0.0            
        for d in self.data:
            d1npya = numpy.array(d[1])
            square = ( -1 - d1npya)*( -1 - d1npya )
            forminusone = forminusone + ( numpy.add.reduce(square) )
            
        return 0.5*max(forone,forminusone)

    def train(self, data, maxTemp = 0.1, minTemp = 0.001, grades = 200, maxerror = 0.001,lvlmax = 50):
        self.data = data
        self.grades = grades
        self.lvlmax = lvlmax
        self.maxerror = maxerror
        #self.maxNetworkError = self.getMaxNetworkErrorForDataset()
        self.setTemps(float(maxTemp), float(minTemp))
        return self.anneal()

def getErrorForDataSet(network, data):
    error = 0.0
    for d in data:
        d1npya = numpy.array(d[1])
        square = ( d1npya - network.activate(d[0]))*( d1npya - network.activate(d[0]))
        error = error + ( numpy.add.reduce(square) )
    return error*0.5

def test_xor_bp():
    tst = ffnet(2,2,1)
    btt = BackPropTrainer(tst,0.65)

    pat = [
        [[0,0], [0]],
        [[0,1], [1]],
        [[1,0], [1]],
        [[1,1], [0]]
    ]

    print btt.train(pat,10000)

    #print tst.activate([0,0])
    #print tst.activate([1,0])
    #print tst.activate([0,1])
    #print tst.activate([1,1])

def test_xor_sa():
    tst = ffnet(2,2,1)
    btt = AnnealTrainer(tst)

    pat = [
        [[0,0], [0]],
        [[0,1], [1]],
        [[1,0], [1]],
        [[1,1], [0]]
    ]

    print btt.train(pat, 0.1, 0.001 ,200)

    #print tst.activate([0,0])
    #print tst.activate([1,0])
    #print tst.activate([0,1])
    #print tst.activate([1,1])

def test_addition_bp():
    scitc = numpy.random.rand(100,2)*0.5
    opt = numpy.add(scitc[:,0],scitc[:,1])
    testdata = zip(scitc,opt)

    tst = ffnet(2,6,1)
    btt = BackPropTrainer(tst,0.65)

    print btt.train(testdata,5000)

    scitc = numpy.random.rand(150,2)*0.5
    opt = numpy.add(scitc[:,0],scitc[:,1])
    verifydata = zip(scitc,opt)
    print getErrorForDataSet(tst, verifydata)

def test_addition_sa():
    scitc = numpy.random.rand(100,2)*0.5
    opt = numpy.add(scitc[:,0],scitc[:,1])
    testdata = zip(scitc,opt)

    tst = ffnet(2,6,1)
    btt = AnnealTrainer(tst)

    print btt.train(testdata, 0.1, 0.001 ,200)

    scitc = numpy.random.rand(150,2)*0.5
    opt = numpy.add(scitc[:,0],scitc[:,1])
    verifydata = zip(scitc,opt)
    print getErrorForDataSet(tst, verifydata)

def test_multiplication_bp():
    scitc = numpy.random.rand(100,2)*0.5
    opt = numpy.multiply(scitc[:,0],scitc[:,1])
    testdata = zip(scitc,opt)

    tst = ffnet(2,6,1)
    btt = BackPropTrainer(tst,0.65)

    print btt.train(testdata,5000)
    scitc = numpy.random.rand(150,2)*0.5
    opt = numpy.multiply(scitc[:,0],scitc[:,1])
    verifydata = zip(scitc,opt)
    print getErrorForDataSet(tst, verifydata)

def test_multiplication_sa():
    scitc = numpy.random.rand(100,2)*0.5
    opt = numpy.multiply(scitc[:,0],scitc[:,1])
    testdata = zip(scitc,opt)

    tst = ffnet(2,6,1)
    btt = AnnealTrainer(tst)

    print btt.train(testdata, 0.1, 0.001 ,200)
    scitc = numpy.random.rand(150,2)*0.5
    opt = numpy.multiply(scitc[:,0],scitc[:,1])
    verifydata = zip(scitc,opt)
    print getErrorForDataSet(tst, verifydata)

def test_division_bp():
    scitc = numpy.random.rand(100,2)
    one = numpy.ones(100)
    numpy.add(scitc[:,1], one ,out=scitc[:,1])
    opt = numpy.divide(scitc[:,0],scitc[:,1])
    testdata = zip(scitc,opt)

    tst = ffnet(2,6,1)
    btt = BackPropTrainer(tst,0.65)

    print btt.train(testdata,5000)
    scitc = numpy.random.rand(150,2)
    one = numpy.ones(150)
    numpy.add(scitc[:,1], one ,out=scitc[:,1])
    opt = numpy.divide(scitc[:,0],scitc[:,1])
    verifydata = zip(scitc,opt)
    print getErrorForDataSet(tst, verifydata)
    

def test_division_sa():
    scitc = numpy.random.rand(100,2)
    one = numpy.ones(100)
    numpy.add(scitc[:,1], one ,out=scitc[:,1])
    opt = numpy.divide(scitc[:,0],scitc[:,1])
    testdata = zip(scitc,opt)

    tst = ffnet(2,6,1)
    btt = AnnealTrainer(tst)

    print btt.train(testdata, 0.1, 0.001 ,200)
    scitc = numpy.random.rand(150,2)
    one = numpy.ones(150)
    numpy.add(scitc[:,1], one ,out=scitc[:,1])
    opt = numpy.divide(scitc[:,0],scitc[:,1])
    verifydata = zip(scitc,opt)
    print getErrorForDataSet(tst, verifydata)

def main():
    #for i in xrange( 100 ):
    #  test_xor_bp()
    #  test_xor_sa()
    #  print "-----------------"
    
    import time  
    test_addition_bp()
    print time.strftime('%X %x %Z')    
    test_addition_sa()
    print time.strftime('%X %x %Z')
    print "--------"
    
    test_multiplication_bp()
    print time.strftime('%X %x %Z')
    test_multiplication_sa()
    print time.strftime('%X %x %Z')
    print "--------"
        
    test_division_bp()
    print time.strftime('%X %x %Z')
    test_division_sa()
    print time.strftime('%X %x %Z')
    print "--------"
    
if __name__ == '__main__':
    main()
