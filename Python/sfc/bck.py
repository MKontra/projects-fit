
        
        

    def autoTemp(self, data, startTemp, initialAcceptancePct = 0.98, terminalImprovementPct = 0.0, step = 1000):
        self.data = data
        self.setTemps(startTemp, 1)

        E, imp, acc = self.annealForTemp(step)
        #print "durrr: " + str(E)+" "+str(imp)+" "+str(acc)
        while ( acc > initialAcceptancePct ):
            self.setTemps( self.maxTemp/1.5, 1 )
            E, imp, acc = self.annealForTemp(step)

        #print "hurr: " + str(E)+" "+str(imp)+" "+str(acc)

        while ( acc < initialAcceptancePct ):
            self.setTemps( self.maxTemp*1.5, 1 )
            E, imp, acc = self.annealForTemp(step)

        #print "furrr: " +str(self.maxTemp)+" " + str(E)+" "+str(imp)+" "+str(acc)

        while imp > 0.01:
            self.currentTemp = self.currentTemp/1.5
            E, imp, acc = self.annealForTemp(step)
            #print imp

        return ( self.maxTemp, self.currentTemp)
        

    def annealForTemp(self, steps):
        bestNet = ( self.net.HidWeights, self.net.OutWeights )
        bestError = self.getErrorForDataSet()
        currentNet = bestNet
        currentError = bestError

        improves = 0
        accepts = 0

        for step in range(steps):
            nextNet = self.perturbeWeightSet(currentNet)
            self.net.HidWeights = nextNet[0]
            self.net.OutWeights = nextNet[1]
            nextError = self.getErrorForDataSet()
            if nextError < currentError:
                improves = improves + 1
                accepts = accepts + 1
                currentNet = nextNet
                currentError = nextError
            elif (nextError > currentError) and numpy.random.uniform() < math.exp((currentError-nextError)/self.currentTemp):
                accepts = accepts + 1
                currentNet = nextNet
                currentError = nextError
            else: #restore values
                self.net.HidWeights = currentNet[0]
                self.net.OutWeights = currentNet[1]

        return (currentError, float(improves)/steps, float(accepts)/steps)  
        
            def anneal(self):
        bestNet = ( self.net.HidWeights, self.net.OutWeights )
        bestError = self.getErrorForDataSet()
        currentNet = bestNet
        currentError = bestError

        for i in xrange(1, self.iterations+1):

            self.currentTemp = self.maxTemp * math.exp( self.TempFactor * (float(i) / self.iterations) )

            nextNet = self.perturbeWeightSet(currentNet)
            self.net.HidWeights = nextNet[0]
            self.net.OutWeights = nextNet[1]
            nextError = self.getErrorForDataSet()
            print "tempdata currenterror:" +str(currentError)+" nexterror:"+str(nextError)+" currentTemp:"+str(self.currentTemp)+" all:"+ str(math.exp((currentError-nextError)/self.currentTemp))
            if nextError <= currentError:
                currentNet = nextNet
                currentError = nextError
            elif (nextError > currentError) and numpy.random.uniform() < math.exp((currentError-nextError)/self.currentTemp):
                currentNet = nextNet
                currentError = nextError
            else: #restore values
                self.net.HidWeights = currentNet[0]
                self.net.OutWeights = currentNet[1]

            if ( currentError < bestError ):
                bestNet = currentNet
                bestError = currentError



        self.net.HidWeights = bestNet[0]
        self.net.OutWeights = bestNet[1]
        return bestError      