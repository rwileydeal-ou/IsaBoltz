import json
from math import e
import sqlite3
import matplotlib
from matplotlib import scale
from matplotlib.units import AxisInfo
import numpy as np
from cgitb import grey
import matplotlib.pyplot as plt
#matplotlib.rcParams['text.usetex'] = True
#matplotlib.rcParams['text.latex.preamble'] = r'\usepackage{amsmath}'
from pyparsing import alphas
#for \text command#from scripts import plotter
#from scripts import dbconnection

def unique(list1):

    # initialize a null list
    unique_list = []

    # traverse for all elements
    for x in list1:
        # check if exists in unique_list or not
        if x not in unique_list:
            unique_list.append(x)
    # print list
    return unique_list

def addReadIndicesToDatabase(fullPath):
    db = openDatabase(fullPath)
    db.execute('''CREATE INDEX idx_particleEvolution_ID ON ParticleEvolution(ID);''')
    db.execute('''CREATE INDEX idx_particleEvolution_InputID ON ParticleEvolution(InputID);''')
    db.execute('''CREATE INDEX idx_particleEvolution_ParticleKey ON ParticleEvolution(ParticleKey);''')
    db.execute('''CREATE INDEX idx_particleEvolution_Production ON ParticleEvolution(Production);''')
    db.execute('''CREATE INDEX idx_relicDensity_OmegaHSqr ON RelicDensity(OmegaHSqr);''')
    db.execute('''CREATE INDEX idx_relicDensity_ParticleEvoID ON RelicDensity(ParticleEvolutionID);''')
    db.execute('''CREATE INDEX idx_relicDensity_ParticleID ON RelicDensity(ParticleID);''')
    db.execute('''CREATE INDEX idx_deltaNeff_DeltaNeff ON DeltaNeff(DeltaNeff);''')
    db.execute('''CREATE INDEX idx_deltaNeff_ParticleEvoID ON DeltaNeff(ParticleEvolutionID);''')
    db.execute('''CREATE INDEX idx_checkBBN_ConstraintSatisfied ON CheckBBN(ConstraintSatisfied);''')
    db.execute('''CREATE INDEX idx_checkBBN_ParticleEvoID ON CheckBBN(ParticleEvolutionID);''')
    db.execute('''ANALYZE;''')

def openDatabase(fullPath):
    con = sqlite3.connect(fullPath)
    cur = con.cursor()
    return cur

def pullInputs(db):
    db.execute('''SELECT
        *
    FROM Inputs
    ORDER BY ID DESC;
    ''')
    dataRaw = db.fetchall()
    return dataRaw


def pullRelicDensityForInputId(inputId, particleKey, productionMode, db):
    db.execute('''SELECT
        RelicDensity.OmegaHSqr,
        RelicDensity.ScaleFactorID,
        ParticleEvolution.ParticleKey,
        ParticleEvolution.Production, 
        Particle.Mass
    FROM 
        RelicDensity
    LEFT JOIN 
        ParticleEvolution 
    ON 
        RelicDensity.ParticleEvolutionID = ParticleEvolution.ID
    LEFT JOIN 
        Particle
    ON 
        RelicDensity.ParticleID=Particle.ID
    WHERE 
        ParticleEvolution.InputId=''' + "'" +str(inputId) + "'" +'''
        AND
        ParticleEvolution.ParticleKey=''' + "'" +str(particleKey)+ "'" +'''
        AND
        ParticleEvolution.Production ='''+str(productionMode)+'''
        AND 
        RelicDensity.OmegaHSqr IS NOT NULL
    LIMIT 1;''')
    dataRaw = db.fetchall()
    if len(dataRaw) == 0:
        print('Missing Relic Density from input: ', inputId)
        return None
    return dataRaw[0]

def pullDeltaNeffForInputId(inputId, particleKey, productionMode, db):
    db.execute('''SELECT
        DeltaNeff.DeltaNeff,
        DeltaNeff.ScaleFactorID,
        ParticleEvolution.ParticleKey,
        ParticleEvolution.Production
    FROM 
        DeltaNeff
    LEFT JOIN 
        ParticleEvolution 
    ON 
        DeltaNeff.ParticleEvolutionID = ParticleEvolution.ID
    WHERE 
        ParticleEvolution.InputId=''' + "'" +str(inputId) + "'" +'''
        AND
        ParticleEvolution.ParticleKey=''' + "'" +str(particleKey)+ "'" +'''
        AND
        ParticleEvolution.Production ='''+str(productionMode)+'''
        AND 
        DeltaNeff.DeltaNeff IS NOT NULL
    LIMIT 1;''')
    dataRaw = db.fetchall()
    if len(dataRaw) == 0:
        print('Missing Delta Neff from input: ', inputId)
        return None
    return dataRaw[0]

def pullBbnCheckForInputId(inputId, particleKey, productionMode, db):
    db.execute('''SELECT
        CheckBBN.ConstraintSatisfied,
        CheckBBN.ScaleFactorID,
        ParticleEvolution.ParticleKey,
        ParticleEvolution.Production
    FROM 
        CheckBBN
    LEFT JOIN 
        ParticleEvolution 
    ON 
        CheckBBN.ParticleEvolutionID = ParticleEvolution.ID
    WHERE 
        ParticleEvolution.InputId=''' + "'" +str(inputId) + "'" +'''
        AND
        ParticleEvolution.ParticleKey=''' + "'" +str(particleKey)+ "'" +'''
        AND
        ParticleEvolution.Production ='''+str(productionMode)+'''
        AND 
        CheckBBN.ConstraintSatisfied IS NOT NULL
    LIMIT 1;''')
    dataRaw = db.fetchall()
    if len(dataRaw) == 0:
        print('Missing CheckBBN from input: ', inputId)
        return None
    return dataRaw[0]

def pullEnergyData(inputId, particleKey, productionMode, db):
    db.execute('''SELECT
        ParticleEvolution.ID,
        ParticleEvolution.ParticleKey,
        ParticleEvolution.Production,
        ParticleEvolution.ScaleFactorID,
        ParticleEvolution.EnergyDensity,
        ParticleEvolution.NumberDensity,
        ParticleEvolution.EqNumberDensity,
        ScaleFactor.ScaleFactor
    FROM 
        ParticleEvolution
    LEFT JOIN 
        ScaleFactor
    ON 
        ParticleEvolution.ScaleFactorID = ScaleFactor.ID
    WHERE 
        ParticleEvolution.InputId=''' + "'" +str(inputId) + "'" +'''
        AND
        ParticleEvolution.ParticleKey=''' + "'" +str(particleKey)+ "'" +'''
        AND
        ParticleEvolution.Production ='''+str(productionMode)+''';''')
    dataRaw = db.fetchall()
    return dataRaw

def getEnergyDensityData(fullPath, particleKey, productionMode, inputId, energyDensities=[], numberDensities=[], eqNumberDensities=[], scaleFactors=[]):
    print("Fetching energy density data")
    db = openDatabase(fullPath)
    try:
        densities = pullEnergyData(inputId, particleKey, productionMode, db)
        for density in densities:
            energyDensities.append(float(density[4]))
            numberDensities.append(float(density[5]))
            eqNumberDensities.append(float(density[6]))
            scaleFactors.append(float(density[7]))
    except Exception as e:
        print(f"Error fetching energy density data: {e}")
    finally:
        db.close()

def getRelicData(fullPath, particleKey, productionMode=0, inputIds=[], couplings=[], fas=[], masses=[], relicDensities=[], deltaNeffs=[], bbnChecks=[]):
    print("Fetching data")
    db = openDatabase(fullPath)

    try:
        inputs = pullInputs(db)
        print("Pulling data from ",len(inputs), " inputs")
        for input in inputs:
            modelData = json.loads(input[1])
            cosmologyData = json.loads(input[2])
            coupling = float( modelData['Model']['ModulusParams']['LambdaU1'] )
            fa = float( modelData['Model']['DfszParams']['Fa'] ) 
            
            relicDensity = pullRelicDensityForInputId(input[0], particleKey, productionMode, db)
            if relicDensity is None:
                continue

            deltaNeff = pullDeltaNeffForInputId(input[0], particleKey, productionMode, db)            
            if deltaNeff is None:
                continue

            bbnCheck = pullBbnCheckForInputId(input[0], particleKey, productionMode, db)            
            if bbnCheck is None:
                continue

            inputIds.append(input[0])
            couplings.append(coupling)
            fas.append(fa)
            masses.append(float(relicDensity[-1]))
            relicDensities.append(float(relicDensity[0]))
            deltaNeffs.append(float(deltaNeff[0]))
            bbnChecks.append(int(bbnCheck[0]))

    except Exception as e:
        print(f"Error fetching inputs: {e}")
    finally:
        db.close()
    print("Relic density count: ", len(relicDensities))
    print("Delta Neff count: ", len(deltaNeffs))
    print("BBN Check count: ", len(bbnChecks))

# assumes data in the form of
# [
#   [type, x, y, c=None, alpha, label]
# ]
def makePlot(
    data, 
    fileName, 
    xlabel,
    ylabel,
    printColorbar=False,
    colorbarLabel='',
    axisFontSize = 14,
    minX = 1.e-10,
    maxX = 1.e-5,
    minY = 0.001,
    maxY = 10.,
    xScaleLog = True,
    yScaleLog = True,
    legend=None
):
    fig = plt.figure(figsize=(8,6))
    ax = fig.add_axes([0.1, 0.1, 0.6, 0.75])
    
    ax.set_xlabel(xlabel, fontsize=axisFontSize)
    ax.set_ylabel(ylabel, fontsize=axisFontSize)
    ax.tick_params(axis="x", labelsize=axisFontSize-2)
    ax.tick_params(axis="y", labelsize=axisFontSize-2)    

    for d in data:
        if len(d) < 5:
            raise ValueError("Data must have at least 5 elements: [type, x, y, c=None, alpha, label]")
        if d[0] == 'scatter':
            if d[3] is not None:
                sc = ax.scatter(d[1], d[2], c=d[3], alpha=d[4], label=d[5], s=4)
                if printColorbar:
                    clb = plt.colorbar(sc)
                    clb.ax.set_title(colorbarLabel,fontsize=14)
                if legend is not None:
                    lgd = ax.legend(loc='upper left', bbox_to_anchor=(1.05, 1.), ncol=1, fancybox=True, shadow=True, fontsize='small', prop={'size':axisFontSize-3})
            else:
                ax.scatter(d[1], d[2], alpha=d[4], label=d[5], s=4)
        elif d[0] == 'line':
            ax.plot(d[1], d[2], c=d[3], alpha=d[4], label=d[5])
        elif d[0] == 'text':
            ax.text(d[1], d[2], d[5], color=d[3], alpha=d[4], fontweight='heavy')
        else:
            raise ValueError(f"Unknown data type: {d[0]}")

    if xScaleLog == True:
        ax.set_xscale('log')
    if yScaleLog == True:
        ax.set_yscale('log')

    if legend is True:
        lgd = ax.legend(loc='upper left', bbox_to_anchor=(1.05, 1.), ncol=1, fancybox=True, shadow=True, fontsize='small', prop={'size':axisFontSize-3})

#    sc = ax.scatter( couplingsNew, data[5], c=data[-1], alpha=0.9)
#    ax.text(1.e-8, 0.13, r"$\displaystyle \Omega_{\text{tot}} h^2 = 0.12$", color='red',alpha=1,fontweight='heavy')
#    clb = plt.colorbar(sc)
#        clb.ax.set_title(r'$\displaystyle \Delta N_{\text{eff}}$',fontsize=14)
#    clb.ax.set_title(r'$\displaystyle log_{10} f_a (GeV)$',fontsize=14)

    plt.xlim( xmin=minX, xmax=maxX )
    plt.ylim( ymin=minY, ymax=maxY )
    plt.savefig(fileName)
    #    bbox_inches='tight')
    #    bbox_extra_artists=(lgd,), bbox_inches='tight')








def makePrettyRelicPlots(fullPaths):
    inputIds=[]
    couplings=[]
    fas=[]
    deltaNeffs=[]
    relicDensitiesAxionTP=[]
    relicDensitiesAxionCO=[]
    relicDensitiesSaxionTP=[]
    relicDensitiesSaxionCO=[]
    relicDensitiesNeutralino=[]
    relicDensitiesGravitino=[]
    relicDensitiesAxino=[]
    bbnCheckAxionTP=[]
    bbnCheckAxionCO=[]
    bbnCheckSaxionTP=[]
    bbnCheckSaxionCO=[]
    bbnCheckNeutralino=[]
    bbnCheckGravitino=[]
    bbnCheckAxino=[]
    saxMasses=[]

    for fullPath in paths:
        getRelicData(fullPath,'axion',1, inputIds, couplings, fas, [], relicDensitiesAxionCO, [], bbnCheckAxionCO)
        getRelicData(fullPath,'axion',0, [], [], [], [], relicDensitiesAxionTP, deltaNeffs, bbnCheckAxionTP)
        getRelicData(fullPath,'saxion',1, [], [], [], saxMasses, relicDensitiesSaxionCO, [], bbnCheckSaxionCO)
        getRelicData(fullPath,'saxion',0, [], [], [], [], relicDensitiesSaxionTP, [], bbnCheckSaxionTP)
        getRelicData(fullPath,'gravitino',0, [], [], [], [], relicDensitiesGravitino, [], bbnCheckGravitino)
        getRelicData(fullPath,'neutralino1',0, [], [], [], [], relicDensitiesNeutralino, [], bbnCheckNeutralino)
        getRelicData(fullPath,'axino',0, [], [], [], [], relicDensitiesAxino, [], bbnCheckAxino)

    if len(bbnCheckAxionCO) != len(bbnCheckAxionTP) or len(bbnCheckAxionCO) != len(bbnCheckSaxionTP) or len(bbnCheckAxionCO) != len(bbnCheckSaxionCO) or len(bbnCheckAxionCO) != len(bbnCheckNeutralino) or len(bbnCheckAxionCO) != len(bbnCheckGravitino) or len(bbnCheckAxionCO) != len(bbnCheckAxionCO):
        print("Error: Data lengths do not match for axion or saxion data.")
        exit()

    bbnColors = []
    for i in range(0, len(bbnCheckAxionCO)):
        bbnOK = (bbnCheckAxionCO[i] == 1) and (bbnCheckAxionTP[i] == 1) and (bbnCheckSaxionCO[i]) == 1 and bbnCheckSaxionTP[i] == 1 and bbnCheckNeutralino[i] == 1 and bbnCheckGravitino[i] == 1 and bbnCheckGravitino[i] == 1 and bbnCheckAxino[i] == 1
        if bbnOK and relicDensitiesAxionCO[i] < 0.12 and relicDensitiesAxionCO[i] > 0.06:
            bbnColors.append('green')
        elif bbnOK and relicDensitiesAxionCO[i] <= 0.06:
            bbnColors.append('blue')
        elif bbnOK and relicDensitiesAxionCO[i] >= 0.12:
            bbnColors.append('orange')
        else:
            bbnColors.append('red')

    logOH2axionCO = [ np.log10(oh2) for oh2 in relicDensitiesAxionCO ]

    makePlot(
        [
            ['scatter', saxMasses, fas, logOH2axionCO, 0.9, 'Axion CO']
        ],
        'newPlotTest_saxMass.png',
        xlabel=r"$\displaystyle m_s \, (GeV) $",
        ylabel=r"$\displaystyle f_a \, (GeV)$",
        colorbarLabel=r'$\displaystyle log_{10} \Omega h^2$',
        minY=min(fas) * 0.9,
        maxY=max(fas) * 1.1, 
        minX=min(saxMasses) * 0.9,
        maxX=max(saxMasses) * 1.1,
        printColorbar=True
    )

    makePlot(
        [
            ['scatter', saxMasses, fas, bbnColors, 0.9, 'Axion CO']
        ],
        'newPlotTestBBN_saxMass.png',
        xlabel=r"$\displaystyle m_s \, (GeV) $",
        ylabel=r"$\displaystyle f_a \, (GeV)$",
        colorbarLabel='BBN Satisfied',
        minY=min(fas) * 0.9,
        maxY=max(fas) * 1.1,
        minX=min(saxMasses) * 0.9,
        maxX=max(saxMasses) * 1.1,
        printColorbar=False
    )

baseDir = "output/run_1/"
dbName = "results.db"


path = baseDir+dbName
inputId='065cf5da-9cb5-459c-a4ad-902f4512d31b'
enModCo = []
enAxCo = []
enAxTh = []
enSaxCo = []
enSaxTh = []
enAxino = []
enGravitino = []
enNeutralino = []
enRadiation = []

scaleFactors = []
getEnergyDensityData(path, 'modulus', 1, inputId, enModCo, [], [], scaleFactors)
getEnergyDensityData(path, 'axion', 1, inputId, enAxCo, [], [], [])
getEnergyDensityData(path, 'axion', 0, inputId, enAxTh, [], [], [])
getEnergyDensityData(path, 'saxion', 1, inputId, enSaxCo, [], [], [])
getEnergyDensityData(path, 'saxion', 0, inputId, enSaxTh, [], [], [])
getEnergyDensityData(path, 'axino', 0, inputId, enAxino, [], [], [])
getEnergyDensityData(path, 'gravitino', 0, inputId, enGravitino, [], [], [])
getEnergyDensityData(path, 'neutralino1', 0, inputId, enNeutralino, [], [], [])
getEnergyDensityData(path, 'photon', 2, inputId, enRadiation, [], [], [])

makePlot(
    [
        ['line', scaleFactors, enRadiation, 'black', 0.9, 'Radiation'],
        ['line', scaleFactors, enModCo, 'gray', 0.9, 'Modulus'],
        ['line', scaleFactors, enNeutralino, 'pink', 0.9, 'Neutralino'],
        ['line', scaleFactors, enAxCo, 'blue', 0.9, 'Axion CO'],
        ['line', scaleFactors, enAxTh, 'orange', 0.9, 'Axion TP/DP'],
        ['line', scaleFactors, enSaxCo, 'green', 0.9, 'Saxion CO'],
        ['line', scaleFactors, enSaxTh, 'red', 0.9, 'Saxion TP/DP'],
        ['line', scaleFactors, enAxino, 'purple', 0.9, 'Axino'],
        ['line', scaleFactors, enGravitino, 'brown', 0.9, 'Gravitino']
    ],
    'newNPlot_thetaIConstadfasdf.png',
    xlabel="R/R_0",
    ylabel="rho (GeV4)",
    minY=1.e-40,#min(en) * 0.9,
    maxY=max(max(enRadiation), max(enModCo)) * 1.1, 
    minX=1.,
    maxX=max(scaleFactors),
    legend=True
#    printColorbar=True
)

exit()

baseDir = "output/run_1/"
dbName = "results.db"
path0 = baseDir + "db0/" + dbName
path1 = baseDir + "db1/" + dbName
path2 = baseDir + "db2/" + dbName
path3 = baseDir + "db3/" + dbName
path4 = baseDir + "db4/" + dbName
path5 = baseDir + "db5/" + dbName
path6 = baseDir + "db6/" + dbName
path7 = baseDir + "db7/" + dbName
path8 = baseDir + "db8/" + dbName
path9 = baseDir + "db9/" + dbName

paths = [path0, path1, path2]#, path3, path4, path5, path6, path7, path8, path9]
paths = [ baseDir + dbName ]

makePrettyRelicPlots(paths)
exit()

