from cmath import sqrt
import numpy as np

from scripts import plotter
from scripts import dbconnection


fullPath = "output/run_283/cluster_0/results.db"


pwm = dbconnection.dbconnection.pullPartialWidthsVsMass(fullPath, combineGroups=True)
wm = dbconnection.dbconnection.pullTotalWidthVsMass(fullPath)

keys = pwm[0].keys()
partialWidthPlots = []
for key in keys:
    if key != "axion+axion" and key != "saxion+saxion" and key != "axino+axino":
        partialWidthPlots.append( [ pwm[1], pwm[0][key], key ] )

plotter.Plotter.make_temps_plot(
    temps=partialWidthPlots,
#    temps=[
#        [ [ x / 1000. for x in dnfs[1] ], dnfs[0], r"$\displaystyle \lambda_{\text{ALP}} = 1$" ],
#        [ [ x / 1000. for x in dnfs2[1] ], dnfs2[0], r"$\displaystyle \lambda_{\text{ALP}} = 0.816$ (LVS)" ],
#        [ [ min(dnfs[1])/1000., max(dnfs[1])/1000. ], [ 0.26, 0.26 ], r"Planck 2018", '--']
#    ], 
    title=r"",
    xlabel=r"$\displaystyle m_{\phi} \, ( \text{TeV} )$",
    ylabel=r"$\displaystyle \Gamma_\phi \, ( \text{GeV} ) $",
    filename='nosc/plots/DNEFFTEST_123.png'
)

exit()
#pwm = pullPartialWidthsVsMass(fullPath, combineGroups=True)
#wm = pullTotalWidthVsMass(fullPath)
#dnfs = pullDeltaNeffVsMass(fullPath)
#dnfs2 = pullDeltaNeffVsMass(path2)

#keys = pwm[0].keys()
#partialWidthPlots = []
#brs = []
#for key in keys:
#    partialWidthPlots.append( [ pwm[1], pwm[0][key], key ] )
#    if key == "alp+alp":
#        a = [ pwm[0][key][i] / wm[0][i] for i in range(0,len(wm[0])) ]
#        brs = a

tDs = pullTempDecayVsMass( fullPath )
tOscs = pullTempOscVsMass( fullPath )

make_temps_plot(
#    temps=partialWidthPlots,
    temps=[
        [ [ x / 1000. for x in tDs[-1] ], tDs[2], r"$\displaystyle g_{*S} (T_D) $" ],
        [ [ x / 1000. for x in tOscs[-1] ], tOscs[2], r"$\displaystyle g_{*S} (T_{\text{osc}}) $" ],
#        [ [ min(dnfs[1])/1000., max(dnfs[1])/1000. ], [ 0.26, 0.26 ], r"Planck 2018", '--']
    ], 
    title=r"DB test",
    xlabel=r"$\displaystyle m_{\phi} \, ( \text{TeV} )$",
    ylabel=r"$\displaystyle g_{*S} $",
    filename='nosc/plots/gStar_vs_mPhi.png'
)


exit()
make_temps_plot(
#    temps=partialWidthPlots,
    temps=[
        [ [ x / 1000. for x in dnfs[1] ], dnfs[0], r"$\displaystyle \lambda_{\text{ALP}} = 1$" ],
        [ [ x / 1000. for x in dnfs2[1] ], dnfs2[0], r"$\displaystyle \lambda_{\text{ALP}} = 0.816$ (LVS)" ],
        [ [ min(dnfs[1])/1000., max(dnfs[1])/1000. ], [ 0.26, 0.26 ], r"Planck 2018", '--']
    ], 
    title=r"DB test",
    xlabel=r"$\displaystyle m_{\phi} \, ( \text{TeV} )$",
    ylabel=r"$\displaystyle \Delta N_{\text{eff}} $",
    filename='nosc/plots/DNEFFTEST.png'
)

exit()


path = "output/dneff_noHiggsnoMatter/cluster_0/results.db"
#path2 = "output/run_271/cluster_0/results.db"
data = pullDeltaNeffVsMassVsCoupling(path)
#data2 = pullDeltaNeffVsMassVsCoupling(path2)
masses = [ x/ 1000. for x in data[1] ]
#masses2 = [ x/ 1000. for x in data2[1] ]
make_scatter_heat_plot( 
    arrays=[ 
    [
        masses,
        data[2],
        data[0]
    ],
#        [
#        masses2,
#        data2[2],
#        data2[0]
#    ]
    ],
    drawLine=[ [min(masses), max(masses)], [0.816, 0.816] ],
    title="TEST",
    xlabel=r"$\displaystyle m_{\phi} \, ( \text{TeV} )$",
    ylabel=r"$\displaystyle \lambda_{\text{ALP}} $",
    filename='nosc/plots/DNEFFTEST_scatter_noHiggs_noMatter.png'
)

#fullPath = "output/run_244/results.db"
#path2 = "output/run_245/results.db"



exit()
#pwm = pullPartialWidthsVsMass(fullPath, combineGroups=True)
#wm = pullTotalWidthVsMass(fullPath)
dnfs = pullDeltaNeffVsMass(fullPath)
dnfs2 = pullDeltaNeffVsMass(path2)

#keys = pwm[0].keys()
#partialWidthPlots = []
#for key in keys:
#    partialWidthPlots.append( [ pwm[1], pwm[0][key], key ] )

make_temps_plot(
#    temps=partialWidthPlots,
    temps=[
        [ [ x / 1000. for x in dnfs[1] ], dnfs[0], r"$\displaystyle \lambda_{\text{ALP}} = 1$" ],
        [ [ x / 1000. for x in dnfs2[1] ], dnfs2[0], r"$\displaystyle \lambda_{\text{ALP}} = 0.816$ (LVS)" ],
        [ [ min(dnfs[1])/1000., max(dnfs[1])/1000. ], [ 0.26, 0.26 ], r"Planck 2018", '--']
    ], 
    title=r"DB test",
    xlabel=r"$\displaystyle m_{\phi} \, ( \text{TeV} )$",
    ylabel=r"$\displaystyle \Delta N_{\text{eff}} $",
    filename='nosc/plots/DNEFFTEST.png'
)

exit()

def getLambdaFromNeff(gstrTD, gstrTosc, mphi, phi0, Tosc, Te):
    gsT = 3.9
    NeffExp = 0.26
    consts = 21. * (np.pi**2.) / 5. * (4./11.)**(4./3.) * NeffExp
    gstrs = gstrTosc * ( gstrTD )**(1./3.) / (gsT**(4./3.))
    rho = (mphi**2.) * (phi0**2.) / Te / (Tosc**3.)
    resultsqr = consts*gstrs / rho
    result = sqrt(resultsqr).real
    return result


mphi1 = 1000000.
mphi1_Te = 3.85e11
mphi1_Tosc = 4.04681e11
mphi1_gSTD = 68.294
mphi1_gSTosc = 224.

mphi2 = 10000.
mphi2_Te = 3.85e10
mphi2_Tosc = 4.04681e10
mphi2_gSTD = 6.533
mphi2_gSTosc = 224.

mphi3 = 100000.
mphi3_Te = 1.21845e11
mphi3_Tosc = 1.2797e11
mphi3_gSTD = 10.888385
mphi3_gSTosc = 224.

mphi4 = 10000000.
mphi4_Te = 9.52131e11
mphi4_Tosc = 1.13124e12
mphi4_gSTD = 14.465
mphi4_gSTosc = 224.

mphi5 = 100000000.
mphi5_Te = 9.52131e11
mphi5_Tosc = 2.01167e12
mphi5_gSTD = 121.556
mphi5_gSTosc = 224.

a = getLambdaFromNeff(mphi1_gSTD, mphi1_gSTosc, mphi1, 1.94e18, mphi1_Tosc, mphi1_Te)
mphi1_phi0 = [ 1e-7, 1. ]
mphi1_lambdaA = [ a, a ]

b = getLambdaFromNeff(mphi2_gSTD, mphi2_gSTosc, mphi2, 1.94e18, mphi2_Tosc, mphi2_Te)
mphi2_phi0 = [ 1e-7, 1. ]
mphi2_lambdaA = [ b, b ]

c = getLambdaFromNeff(mphi3_gSTD, mphi3_gSTosc, mphi3, 1.94e18, mphi3_Tosc, mphi3_Te)
mphi3_phi0 = [ 1e-7, 1. ]
mphi3_lambdaA = [ c, c ]

d = getLambdaFromNeff(mphi4_gSTD, mphi4_gSTosc, mphi4, 1.94e18, mphi4_Tosc, mphi4_Te)
mphi4_phi0 = [ 1e-7, 1. ]
mphi4_lambdaA = [ d, d ]

e = getLambdaFromNeff(mphi5_gSTD, mphi5_gSTosc, mphi5, 1.94e18, mphi5_Tosc, mphi5_Te)
mphi5_phi0 = [ 1e-7, 1. ]
mphi5_lambdaA = [ e, e ]

make_temps_plot(
#    temps=partialWidthPlots,
    temps=[
        [ mphi2_phi0, mphi2_lambdaA, r"$\displaystyle m_{\phi} = 10 \text{ TeV}$" ],
        [ mphi3_phi0, mphi3_lambdaA, r"$\displaystyle m_{\phi} = 100 \text{ TeV}$" ],
        [ mphi1_phi0, mphi1_lambdaA, r"$\displaystyle m_{\phi} = 1000 \text{ TeV}$" ],
        [ mphi4_phi0, mphi4_lambdaA, r"$\displaystyle m_{\phi} = 10000 \text{ TeV}$" ],
        [ mphi5_phi0, mphi5_lambdaA, r"$\displaystyle m_{\phi} = 100000 \text{ TeV}$" ],
        [ mphi1_phi0, [0.816, 0.816], r"$\displaystyle \text{LVS value}$", '--' ],
    ], 
    yMin=0.3,
    yMax=2.,
    title=r"Contours keeping $\Delta N_{\text{eff}} = 0.26$",
    xlabel=r"$\displaystyle \phi_0 / m_P $",
    ylabel=r"$\displaystyle \lambda_a $",
    filename='nosc/plots/NeffTest.png'
)

