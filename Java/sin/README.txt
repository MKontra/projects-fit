pouzite make (make run)
ak nefunguje skusit 'ant', 'ant run'
vystupy su bin/{CarGenStat, CarProcStat, KrizControllerStat}
prvy stlpec je vzdy cas. 
CarGenStat - informacie o nastaveni prestojov generatora.
CarProcStat - informacie o prechode aut systemom ( cas, id, zotrvany cas )
KrizControllerStat - informacie o casoch trvanie otvorenych stavov
pociatocne zrychlenie simulacie je nastavene na 100x zrychlenie
riadene premmenou kriz.SimulatorAgent.unit a kriz.KrizControllerAgent = 
nastavene na rovnako a hodonoty elementarnej jednotky v milisekundach ( 1000 = realtime )