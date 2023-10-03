from cmath import sqrt
import sqlite3
import numpy as np

def openDatabase(fullPath):
    con = sqlite3.connect(fullPath)
    cur = con.cursor()
    return cur

def pullMassFromParticle(particleId, db):
    db.execute('SELECT Mass FROM Particle WHERE ID="' + particleId + '"')
    p = db.fetchone()[0]
    return p

def pullCouplingFromInputId(inputId, key, db):
    db.execute('SELECT Coupling FROM Coupling WHERE InputID="' + inputId + '" AND CouplingKey="' + key + '"')
    p = db.fetchone()[0]
    return p

def pullTotalWidthVsMass(fullPath):
    print("Fetching total widths")
    db = openDatabase(fullPath)
    db.execute('SELECT * FROM TotalWidth')
    totalWidthsRaw = db.fetchall()
    
    totalWidths = []
    parentMasses = []

    for width in totalWidthsRaw:
        totalWidth = width[-3]
        totalWidths.append( totalWidth )
        parentId = width[3]
        p = pullMassFromParticle( parentId, db )
        parentMasses.append(p)

    db.close()
    print("Finished fetching total widths")
    return [totalWidths, parentMasses]


def pullTempDecayVsMass(fullPath):
    print("Fetching decay temps")
    db = openDatabase(fullPath)
    db.execute('SELECT * FROM TempDecay')
    tempsRaw = db.fetchall()
    
    temps = []
    gStrs = []
    gStrEnts = []
    particleMasses = []

    for temp in tempsRaw:
        temps.append( temp[3] )
        gStrs.append( temp[4] )
        gStrEnts.append( temp[5] )

        particleId = temp[2]
        p = pullMassFromParticle( particleId, db )
        particleMasses.append(p)

    db.close()
    print("Finished fetching decay temps")
    return [temps, gStrs, gStrEnts, particleMasses]

def pullTempOscVsMass(fullPath):
    print("Fetching osc. temps")
    db = openDatabase(fullPath)
    db.execute('SELECT * FROM TempOscillate')
    tempsRaw = db.fetchall()
    
    temps = []
    gStrs = []
    gStrEnts = []
    particleMasses = []

    for temp in tempsRaw:
        temps.append( temp[3] )
        gStrs.append( temp[4] )
        gStrEnts.append( temp[5] )

        particleId = temp[2]
        p = pullMassFromParticle( particleId, db )
        particleMasses.append(p)

    db.close()
    print("Finished fetching osc. temps")
    return [temps, gStrs, gStrEnts, particleMasses]


def pullDeltaNeffVsMass(fullPath):
    db = openDatabase(fullPath)
    db.execute('SELECT * FROM DeltaNeff')
    deltaNeffsRaw = db.fetchall()
    
    deltaNeffs = []
    parentMasses = []

    for deltaNeff in deltaNeffsRaw:
        deltaNeffs.append( min( deltaNeff[4], 0.26) )
        parentId = deltaNeff[2]
        p = pullMassFromParticle( parentId, db )
        parentMasses.append(p)

    db.close()
    return [deltaNeffs, parentMasses]

def pullDeltaNeffVsMassVsCoupling(fullPath):
    db = openDatabase(fullPath)
    db.execute('SELECT * FROM DeltaNeff')
    deltaNeffsRaw = db.fetchall()
    
    deltaNeffs = []
    parentMasses = []
    couplings = []

    for deltaNeff in deltaNeffsRaw:
        deltaNeffs.append( min( deltaNeff[4], 0.26 ) )
        parentId = deltaNeff[2]
        p = pullMassFromParticle( parentId, db )
        parentMasses.append(p)
        c = pullCouplingFromInputId( deltaNeff[1], "lambda_alp", db )
        couplings.append(c)

    db.close()
    return [deltaNeffs, parentMasses, couplings]

def convertNameToFamily(name):
    if "neutralino" in name:
        return "neutralino"
    elif "chargino" in name:
        return "chargino"
    elif "higgs" in name:
        return "higgs"
    elif "sup" in name or "sdown" in name or "sstrange" in name or "scharm" in name or "sbottom" in name or "stop" in name:
        return "squark"
    elif "sneutrino" in name or "selectron" in name or "smuon" in name or "stau" in name:
        return "slepton"
    elif name == "upq" or name == "downq" or name == "charmq" or name == "topq" or name == "bottomq" or name == "strangeq":
        return "quark"
    elif name == "electron" or name == "muon" or name == "tau":
        return "lepton"
    elif name == "zboson" or name == "wboson":
        return "massive gauge boson"
    elif name == "photon" or name == "gluon":
        return "massless gauge boson"
    return name

def convertFamilyToGroup(family):
    if family == "squark" or family == "slepton":
        return "sfermion"
    elif family == "quark" or family == "lepton":
        return "fermion"
    elif "gauge boson" in family:
        return "gauge boson"
    elif family == "neutralino" or family == "chargino":
        return "gaugino"
    return family

def createLabelFromChildren(childIds, db, combineFamilies=False, combineGroups=False):
    pLabel = ""
    for childId in childIds:
        db.execute('SELECT ParticleKey FROM Particle WHERE ID="' + childId + '"')
        if combineFamilies or combineGroups:
            name = db.fetchone()[0]
            p = convertNameToFamily(name)
            if combineGroups:
                p = convertFamilyToGroup(p)
        else:
            p = db.fetchone()[0]
        pLabel += p + "+"
    pLabel = pLabel[:-1]
    return pLabel

def pullPartialWidthsVsMass(fullPath, combineFamilies=False, combineGroups=False):
    print("Fetching partial widths")
    db = openDatabase(fullPath)
    db.execute('SELECT TotalWidthID, ParentID, ChildrenIDs, PartialWidth FROM PartialWidth')
    partialWidthsRaw = db.fetchall()

    parentMasses = []
    partialWidths = {}

    # the easiest way to do this is to split up the raw data into unique TotalWidthIDs - which will ALWAYS correspond to a single "point" and specific ParentId
    nPoints = len(set([ pw[0] for pw in partialWidthsRaw ]))
    pointWidths = np.array_split(np.array(partialWidthsRaw), nPoints)
    nPoint = 1

    # now iterate over each TotalWidthId
    for pointWidth in pointWidths:
        # parentId must be the same for all points with same TotalWidthId
        parentId = pointWidth[0][1]
        p = pullMassFromParticle( parentId, db )
        parentMasses.append(p)

        for width in pointWidth:
            widthValue = float(width[3])
            childIds = str(width[2]).replace(' ','').split(',')
            pLabel = createLabelFromChildren(childIds, db, combineFamilies, combineGroups)
            partialWidths.setdefault(pLabel, [])

            if combineFamilies or combineGroups:
                try:
                    if len(partialWidths[pLabel]) == nPoint:
                        partialWidths[pLabel][-1] += widthValue
                    else: 
                        partialWidths[pLabel].append( widthValue )
                except (KeyError, IndexError):
                    partialWidths[pLabel].append( widthValue )
            else:
                partialWidths[pLabel].append( widthValue )

        nPoint += 1
    print("Finished fetching partial widths")
    db.close()
    return [partialWidths, parentMasses]
