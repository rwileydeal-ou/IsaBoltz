SUBROUTINE DEFINEDATA()
!     Main subroutine to calculate MSSM input parameters for ISAJET
!     from renormalization group equations and supergravity.
!     All external names are of the form SUxxxx.
!     Must link with block data ALDATA.
!     Includes optional link to ISATOOLS, which requires libisared.a.
!     Make this from isared.tar; see the Makefile for instructions.
!
!...##### Reads input from file in unit IFL (plotsugra.in) and writes no output ####
!...!!! Input file must be already opened !!!
    
!...   10/12/2012: Updated to Isajet v7.83
!...   10/12/2012: When called also saves the running info in TRHPARS
!....04/21/2013: Switched TRHPARS to double precision
!....04/21/2013: Added auxiliary functions PSLAMB and COT
    
          
    IMPLICIT NONE

    COMMON/SSLUN/LOUT,LHEOUT
    INTEGER LOUT,LHEOUT
    SAVE /SSLUN/
!    SUSY parameters
!    AMGLSS         = gluino mass
!    AMULSS         = up-left squark mass
!    AMELSS         = left-selectron mass
!    AMERSS         = right-slepton mass
!    AMNiSS         = sneutrino mass for generation i
!    TWOM1          = Higgsino mass = - mu
!    RV2V1          = ratio v2/v1 of vev's
!    AMTLSS,AMTRSS        = left,right stop masses
!    AMT1SS,AMT2SS        = light,heavy stop masses
!    AMBLSS,AMBRSS        = left,right sbottom masses
!    AMB1SS,AMB2SS        = light,heavy sbottom masses
!    AMLLSS,AMLRSS        = left,right stau masses
!    AML1SS,AML2SS        = light,heavy stau masses
!    AMZiSS         = signed mass of Zi
!    ZMIXSS         = Zi mixing matrix
!    AMWiSS         = signed Wi mass
!    GAMMAL,GAMMAR        = Wi left, right mixing angles
!    AMHL,AMHH,AMHA       = neutral Higgs h0, H0, A0 masses
!    AMHC           = charged Higgs H+ mass
!    ALFAH          = Higgs mixing angle
!    AAT            = stop trilinear term
!    THETAT         = stop mixing angle
!    AAB            = sbottom trilinear term
!    THETAB         = sbottom mixing angle
!    AAL            = stau trilinear term
!    THETAL         = stau mixing angle
!    AMGVSS         = gravitino mass
!    MTQ            = top mass at MSUSY
!    MBQ            = bottom mass at MSUSY
!    MLQ            = tau mass at MSUSY
!    FBMA           = b-Yukawa at mA scale
!    VUQ            = Hu vev at MSUSY
!    VDQ            = Hd vev at MSUSY
!    SGNM3          = sign of gaugino mass M3
    COMMON/SSPAR/GORGE,AMGLSS,AMULSS,AMURSS,AMDLSS,AMDRSS,AMSLSS,AMSRSS,AMCLSS,AMCRSS,AMBLSS,AMBRSS,AMB1SS,AMB2SS,AMTLSS,AMTRSS, &
    AMT1SS,AMT2SS,AMELSS,AMERSS,AMMLSS,AMMRSS,AMLLSS,AMLRSS,AML1SS,AML2SS,AMN1SS,AMN2SS,AMN3SS,TWOM1,RV2V1,AMZ1SS,AMZ2SS,AMZ3SS, &
    AMZ4SS,ZMIXSS(4,4),AMW1SS,AMW2SS,GAMMAL,GAMMAR,AMHL,AMHH,AMHA,AMHC,ALFAH,AAT,THETAT,AAB,THETAB,AAL,THETAL,AMGVSS,MTQ,MBQ,MLQ, &
    FBMA,VUQ,VDQ,SGNM3
    REAL AMGLSS,AMULSS,AMURSS,AMDLSS,AMDRSS,AMSLSS,AMSRSS,AMCLSS,AMCRSS,AMBLSS,AMBRSS,AMB1SS,AMB2SS,AMTLSS,AMTRSS,AMT1SS,AMT2SS, &
    AMELSS,AMERSS,AMMLSS,AMMRSS,AMLLSS,AMLRSS,AML1SS,AML2SS,AMN1SS,AMN2SS,AMN3SS,TWOM1,RV2V1,AMZ1SS,AMZ2SS,AMZ3SS,AMZ4SS,ZMIXSS, &
    AMW1SS,AMW2SS,GAMMAL,GAMMAR,AMHL,AMHH,AMHA,AMHC,ALFAH,AAT,THETAT,AAB,THETAB,AAL,THETAL,AMGVSS,MTQ,MBQ,MLQ,FBMA,VUQ,VDQ,SGNM3
    LOGICAL GORGE
    REAL AMZISS(4)
    EQUIVALENCE (AMZISS(1),AMZ1SS)
    SAVE /SSPAR/
!    SM ident code definitions. These are standard ISAJET but
!    can be changed.
    INTEGER IDUP,IDDN,IDST,IDCH,IDBT,IDTP
    INTEGER IDNE,IDE,IDNM,IDMU,IDNT,IDTAU
    INTEGER IDGL,IDGM,IDW,IDZ,IDH
    PARAMETER (IDUP=1,IDDN=2,IDST=3,IDCH=4,IDBT=5,IDTP=6)
    PARAMETER (IDNE=11,IDE=12,IDNM=13,IDMU=14,IDNT=15,IDTAU=16)
    PARAMETER (IDGL=9,IDGM=10,IDW=80,IDZ=90,IDH=81)
!    SUSY ident code definitions. They are chosen to be similar
!    to those in versions < 6.50 but may be changed.
    INTEGER ISUPL,ISDNL,ISSTL,ISCHL,ISBT1,ISTP1
    INTEGER ISNEL,ISEL,ISNML,ISMUL,ISNTL,ISTAU1
    INTEGER ISUPR,ISDNR,ISSTR,ISCHR,ISBT2,ISTP2
    INTEGER ISNER,ISER,ISNMR,ISMUR,ISNTR,ISTAU2
    INTEGER ISZ1,ISZ2,ISZ3,ISZ4,ISW1,ISW2,ISGL
    INTEGER ISHL,ISHH,ISHA,ISHC
    INTEGER ISGRAV
    INTEGER IDTAUL,IDTAUR
    PARAMETER (ISUPL=21,ISDNL=22,ISSTL=23,ISCHL=24,ISBT1=25,ISTP1=26)
    PARAMETER (ISNEL=31,ISEL=32,ISNML=33,ISMUL=34,ISNTL=35,ISTAU1=36)
    PARAMETER (ISUPR=41,ISDNR=42,ISSTR=43,ISCHR=44,ISBT2=45,ISTP2=46)
    PARAMETER (ISNER=51,ISER=52,ISNMR=53,ISMUR=54,ISNTR=55,ISTAU2=56)
    PARAMETER (ISGL=29)
    PARAMETER (ISZ1=30,ISZ2=40,ISZ3=50,ISZ4=60,ISW1=39,ISW2=49)
    PARAMETER (ISHL=82,ISHH=83,ISHA=84,ISHC=86)
    PARAMETER (ISGRAV=91)
    PARAMETER (IDTAUL=10016,IDTAUR=20016)
!    Frozen couplings from RG equations:
!     GSS( 1) = g_1        GSS( 2) = g_2        GSS( 3) = g_3
!     GSS( 4) = y_tau      GSS( 5) = y_b        GSS( 6) = y_t
!     GSS( 7) = M_1        GSS( 8) = M_2        GSS( 9) = M_3
!     GSS(10) = A_tau      GSS(11) = A_b        GSS(12) = A_t
!     GSS(13) = M_hd^2     GSS(14) = M_hu^2     GSS(15) = M_er^2
!     GSS(16) = M_el^2     GSS(17) = M_dnr^2    GSS(18) = M_upr^2
!     GSS(19) = M_upl^2    GSS(20) = M_taur^2   GSS(21) = M_taul^2
!     GSS(22) = M_btr^2    GSS(23) = M_tpr^2    GSS(24) = M_tpl^2
!     GSS(25) = mu         GSS(26) = B          GSS(27) = Y_N
!     GSS(28) = M_nr       GSS(29) = A_n        GSS(30) = vdq
!     GSS(31) = vuq
!    Masses:
!     MSS( 1) = glss     MSS( 2) = upl      MSS( 3) = upr
!     MSS( 4) = dnl      MSS( 5) = dnr      MSS( 6) = stl
!     MSS( 7) = str      MSS( 8) = chl      MSS( 9) = chr
!     MSS(10) = b1       MSS(11) = b2       MSS(12) = t1
!     MSS(13) = t2       MSS(14) = nuel     MSS(15) = numl
!     MSS(16) = nutl     MSS(17) = el-      MSS(18) = er-
!     MSS(19) = mul-     MSS(20) = mur-     MSS(21) = tau1
!     MSS(22) = tau2     MSS(23) = z1ss     MSS(24) = z2ss
!     MSS(25) = z3ss     MSS(26) = z4ss     MSS(27) = w1ss
!     MSS(28) = w2ss     MSS(29) = hl0      MSS(30) = hh0
!     MSS(31) = ha0      MSS(32) = h+
!    Unification:
!     MGUTSS  = M_GUT    GGUTSS  = g_GUT    AGUTSS  = alpha_GUT
    COMMON /SUGMG/ MSS(32),GSS(31),MGUTSS,GGUTSS,AGUTSS,FTGUT,FBGUT,FTAGUT,FNGUT
    REAL MSS,GSS,MGUTSS,GGUTSS,AGUTSS,FTGUT,FBGUT,FTAGUT,FNGUT
    SAVE /SUGMG/
!     XSUGIN contains the inputs to SUGRA:
!     XSUGIN(1) = M_0        XSUGIN(2) = M_(1/2)  XSUGIN(3) = A_0
!     XSUGIN(4) = tan(beta)  XSUGIN(5) = sgn(mu)  XSUGIN(6) = M_t
!     XSUGIN(7) = SUG BC scale
!     XGMIN(1) = LAM         XGMIN(2)  = M_MES    XGMIN(3)  = XN5
!     XGMIN(4) = tan(beta)   XGMIN(5)  = sgn(mu)  XGMIN(6) = M_t
!     XGMIN(7) = CGRAV       XGMIN(8)  =RSL       XGMIN(9)  = DEL_HD
!     XGMIN(10)  = DEL_HU    XGMIN(11) = DY       XGMIN(12) = N5_1
!     XGMIN(13)  = N5_2      XGMIN(14) = N5_3
!     XNRIN(1) = M_N3        XNRIN(2) = M_MAJ     XNRIN(3) = ANSS 
!     XNRIN(4) = M_N3SS
!     XISAIN contains the MSSMi inputs in natural order.
    COMMON /SUGXIN/ XISAIN(24),XSUGIN(7),XGMIN(14),XNRIN(4),XAMIN(11)
    REAL XISAIN,XSUGIN,XGMIN,XNRIN,XAMIN
    SAVE /SUGXIN/
    COMMON /SUGPAS/ XTANB,MSUSY,AMT,MGUT,MU,G2,GP,V,VP,XW,A1MZ,A2MZ,ASMZ,FTAMZ,FBMZ,B,SIN2B,FTMT,G3MT,VEV,HIGFRZ,FNMZ,AMNRMJ, &
    NOGOOD,IAL3UN,ITACHY,MHPNEG,MHLNEG,MHCNEG,MSQNEG,IGUTST,ASM3,VUMT,VDMT,ASMTP,ASMSS,M3Q,MHDSQ,MHUSQ,MHDSMG,MHUSMG,MUMG,BMG, &
    FT2Z1,FB2Z1,FL2Z1,SIGDMX,SIGUMX,C5MAX,CHSMAX
    REAL XTANB,MSUSY,AMT,MGUT,MU,G2,GP,V,VP,XW,A1MZ,A2MZ,ASMZ,FTAMZ,FBMZ,B,SIN2B,FTMT,G3MT,VEV,HIGFRZ,FNMZ,AMNRMJ,ASM3,VUMT, &
    VDMT,ASMTP,ASMSS,M3Q,MHDSQ,MHUSQ,MHDSMG,MHUSMG,MUMG,BMG,FT2Z1,FB2Z1,FL2Z1,SIGDMX,SIGUMX,C5MAX,CHSMAX
    INTEGER NOGOOD,IAL3UN,ITACHY,MHPNEG,MHLNEG,MHCNEG,MSQNEG,IGUTST
    SAVE /SUGPAS/

    COMMON /SUGNU/ XNUSUG(20),MDS,M10S,INUHM,IDTERM
    REAL XNUSUG,MDS,M10S
    INTEGER INUHM,IDTERM
    SAVE /SUGNU/

    !    ISAPW1 is used to check whether ALDATA is loaded
    COMMON/ISAPW/ISAPW1
    CHARACTER*30 ISAPW1
    SAVE /ISAPW/
    
    LOGICAL GOLHA,GOWIG
    INTEGER ILHA,IWIG,IMHL,IMHC,IMSQ
    REAL M0,MHF,A0,TANB,SGNMU,MT
    INTEGER NSTEP,IMODEL,IMODIN,ISATLS,IRGEFL
    INTEGER NOUT,IALLOW,IITEST,J
    CHARACTER*40 VERSN,VISAJE
    PARAMETER (NOUT=33)
    INTEGER IDOUT(NOUT)
    CHARACTER*30 ISAPW2
    SAVE ISAPW2
    
    !    Isatools common blocks and variables
    COMMON/SUGRED/OMGH2,SIGMA,XFREEZ,NSTEPS,FFF_V
    REAL OMGH2,SIGMA,XFREEZ,FFF_V
    INTEGER NSTEPS

    COMMON/SUGRES/SIGMA0PROT,SIGMA0NEUT,SIGMASPROT,SIGMASNEUT
    REAL*8 SIGMA0PROT,SIGMA0NEUT,SIGMASPROT,SIGMASNEUT
    SAVE/SUGRES/

    !-FP  INTEGER INUHM
    COMMON/RGEFNM/FNRGE
    CHARACTER*128 FNRGE
    
    DATA IDOUT/IDTP,ISGL,ISUPL,ISDNL,ISSTL,ISCHL,ISBT1,ISTP1,ISUPR,ISDNR,ISSTR,ISCHR,ISBT2,ISTP2,ISEL,ISMUL,ISTAU1,ISNEL,ISNML, &
    ISNTL,ISER,ISMUR,ISTAU2,ISZ1,ISZ2,ISZ3,ISZ4,ISW1,ISW2,ISHL,ISHH,ISHA,ISHC/
!    ISAPW2 is used to check whether ALDATA is loaded
    DATA ISAPW2/'ALDATA REQUIRED BY FORTRAN G,H'/
    DATA ILHA/11/,IWIG/12/

    !     XNUSUG contains non-universal GUT scale soft terms for SUGRA:
!     XNUSUG(1)=M1 
!     XNUSUG(2)=M2 
!     XNUSUG(3)=M3
!     XNUSUG(4)=A_tau 
!     XNUSUG(5)=A_b 
!     XNUSUG(6)=A_t
!     XNUSUG(7)=m_Hd 
!     XNUSUG(8)=m_Hu 
!    XNUSUG(9)=m_eR 
!    XNUSUG(10)=m_eL
!    XNUSUG(11)=m_dR 
!    XNUSUG(12)=m_uR 
!    XNUSUG(13)=m_uL 
!    XNUSUG(14)=m_lR
!    XNUSUG(15)=m_lL 
!    XNUSUG(16)=m_bR 
!    XNUSUG(17)=m_tR 
!    XNUSUG(18)=m_tL
!    XNUSUG(19)=mu_Q
!    XNUSUG(20)=mA_Q

    IF(ISAPW1.NE.ISAPW2) THEN
        PRINT*, ' ERROR: BLOCK DATA ALDATA HAS NOT BEEN LOADED.'
        PRINT*, ' ISAJET CANNOT RUN WITHOUT IT.'
        PRINT*, ' PLEASE READ THE FINE MANUAL FOR ISAJET.'
!        STOP99
    ENDIF

    LOUT=1
    NSTEP=1000
    XNRIN(2)=1.E20
    XSUGIN(7)=0.
    INUHM=0
    IDTERM=0

    OMGH2=0.
    SIGMA0PROT=0.
    SIGMA0NEUT=0.
    SIGMASPROT=0.
    SIGMASNEUT=0.

end subroutine
    