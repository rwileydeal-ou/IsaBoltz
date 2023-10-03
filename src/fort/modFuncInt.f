******************************************************************
      FUNCTION MODFUNC(X)


      IMPLICIT NONE
      
      CHARACTER*6 PINF,PN1,PN2
      REAL*8 A
      COMMON/VARS/A(1800)
 
      COMMON /CTRL/
     &AS_MAX,XFINI,XF,COS_MIN,COS_MAX,CS_MIN,SUPPEXP, SUPPEXPMAX,
     &NDIMUSER,NDIMUSER_EFF,ISUM,NST_MAX,
     &NPROC_MIN,NPROC_MAX,NPROC_STEP

      REAL*8 AS_MAX,XFINI,XF,COS_MIN,COS_MAX,CS_MIN,SUPPEXP, SUPPEXPMAX
      INTEGER NDIMUSER,NDIMUSER_EFF,ISUM,NST_MAX,
     &NPROC_MIN,NPROC_MAX,NPROC_STEP
      
      REAL*8 X(25)
      REAL*8 P(0:3,4)

      REAL*8 RR,SN,PI,GEVTOPBN,MODFUNC,ds_da_dc,AS_MIN,PM,E,XMASS,Z,
     &COS1, PM1,PM2,PM3,PM4,SIN1,SFACT,CFACT,TFACT,B1,B2,FA,AS,DJACOB,
     &ECM,XFA,MODFA12,E1,E2,R1,R2,E3,R3,E4,R4,F_PS,FA12_integr,FUNC_TMP,
     &SQME
     
      INTEGER I
      
      RR(E,XMASS)  = SQRT(MAX(0d0,(E-XMASS)*(E+XMASS)))
      SN(Z)        = SQRT(MAX(0d0,(1-z)*(1+z)))
      
      
      PI=DACOS(-1d0)
      GEVTOPBN= 0.3893796623*1.E+09      
      
      MODFUNC=0.
      ds_da_dc=0.
      

 
      AS_MIN=2d0
      PM=abs(A(93))
C------------------------------------------ 
      COS1=1d0-2d0*X(1)


      IF(COS1.gt.COS_MAX) goto 999
      IF(COS1.lt.cos_min) goto 999
      SIN1=SN(COS1)


C------------------------------------------      
	
      DO I =NPROC_MIN,NPROC_MAX,NPROC_STEP
        
        CALL PMAS(I,1,PM1)
        CALL PMAS(I,2,PM2)
	IF(ABS(PM1/PM).gt.SUPPEXPMAX.or.ABS(PM2/PM).gt.SUPPEXPMAX) 
     >  goto 900
        CALL PMAS(I,3,PM3)
        CALL PMAS(I,4,PM4)
	
	PN1=PINF(I,1)
        PN2=PINF(I,2)
	SFACT=1.
	CFACT=1.

	IF(PN1.ne.PN2) SFACT=2.

	IF(PN1.ne.'~o1'.and.PN1.ne.'~o2'.and.PN1.ne.'~g')  CFACT=2.
	IF(PN2.ne.'~o1'.and.PN2.ne.'~o2'.and.PN2.ne.'~g')  CFACT=2.
	
	IF(PN1.eq.'~1+'.and.PN2.eq.'~1-')  CFACT=1.
	
	IF(PN1.eq.'~e1'.and.PN2.eq.'~E1')  CFACT=1.
	IF(PN1.eq.'~e2'.and.PN2.eq.'~E2')  CFACT=1.
	IF(PN1.eq.'~e3'.and.PN2.eq.'~E3')  CFACT=1.
	IF(PN1.eq.'~n1'.and.PN2.eq.'~N1')  CFACT=1.
	IF(PN1.eq.'~n2'.and.PN2.eq.'~N2')  CFACT=1.
	IF(PN1.eq.'~n3'.and.PN2.eq.'~N3')  CFACT=1.
	IF(PN1.eq.'~u1'.and.PN2.eq.'~U1')  CFACT=1.
	IF(PN1.eq.'~d1'.and.PN2.eq.'~D1')  CFACT=1.
	IF(PN1.eq.'~c1'.and.PN2.eq.'~C1')  CFACT=1.
	IF(PN1.eq.'~s1'.and.PN2.eq.'~S1')  CFACT=1.
	IF(PN1.eq.'~t1'.and.PN2.eq.'~T1')  CFACT=1.
	IF(PN1.eq.'~b1'.and.PN2.eq.'~B1')  CFACT=1.
	
	TFACT=SFACT*CFACT
	
     
      
	
	pm1=abs(pm1)
	pm2=abs(pm2)
	pm3=abs(pm3)
	pm4=abs(pm4)
	
      
	
	B1=pm1/pm
	B2=pm2/pm
        
	
     
        IF(NDIMUSER.eq.1) then
          FA=1.
          AS=AS_MAX
          DJACOB=2.
        ELSEIF(NDIMUSER.ge.2) then
	
          AS=AS_MIN+(AS_MAX-AS_MIN)*X(2)
          DJACOB=(AS_MAX-AS_MIN)*2.
	  IF(AS.le.b1+b2) goto 900
	  
	  IF(NDIMUSER.eq.2) THEN
	      FA=MODFA12(AS,I,XF)
	      
	    
	  ELSEIF(NDIMUSER.eq.3.and.NDIMUSER_EFF.eq.2) THEN
	
   	      FA=FA12_integr(AS,b1,b2,XF)*
     &        ((AS**2-b1**2-b2**2)**2-4.*b1**2*b2**2)
     &        /sqrt(AS)/(2.*sqrt(2.*pi)*b1**(3./2.)*b2**(3./2.))

         
          ELSEIF(NDIMUSER.eq.3.and.NDIMUSER_EFF.eq.3) THEN
	      XFA=X(3)
	      FA=MODFA12(AS,I,XFA)
	      
          ENDIF
      
        ENDIF
	
         
        ECM=AS*PM
        IF(ECM.GT.max(PM1+PM2,PM3+PM4)) then
	
	
          E1=(ECM+(PM1-PM2)*(PM1+PM2)/ECM)/2D0
          E2=(ECM+(PM2-PM1)*(PM1+PM2)/ECM)/2D0
          R1=RR(E1,PM1)
          R2=R1
 	
          P(0,1)=E1
          P(1,1)=0.
          P(2,1)=0.
          P(3,1)=-R1
          P(0,2)=E2
          P(1,2)=0.
          P(2,2)=0.
          P(3,2)=R1
	
          E3=(ECM+(PM3-PM4)*(PM3+PM4)/ECM)/2D0
          R3=RR(E3,PM3)
          E4=(ECM+(PM4-PM3)*(PM3+PM4)/ECM)/2D0
          R4=R3

          P(0,3)=E3
          P(1,3)=R3*SIN1
          P(2,3)=0.
          P(3,3)=R3*COS1
         
	  P(0,4)=E4
          P(1,4)=-P(1,3)
          P(2,4)=0.
          P(3,4)=-P(3,3)
           
       	 
	  F_PS=R3/R1/(32d0*Pi*ECM**2)
	  FUNC_TMP =TFACT*SQME(I,P)*F_PS*FA*DJACOB*GEVTOPBN
          ds_da_dc=FUNC_TMP+ds_da_dc
	  IF(I.eq.26.and.NDIMUSER.eq.3.and.NDIMUSER_EFF.eq.2) goto 999
	  
	  
        ENDIF
  900   CONTINUE     

      ENDDO
  999 MODFUNC=ds_da_dc
       

      IF(MODFUNC.le.1D-30) MODFUNC=1D-30
 
      return
      end

cccccccccccccccccccccccc===================================cccccccccccc

      Function MODFA12(as,I,x)
      IMPLICIT NONE
      REAL*8 MODFA12
      
      REAL*8 A
      COMMON/VARS/A(1800)

      COMMON /CTRL/
     &AS_MAX,XFINI,XF,COS_MIN,COS_MAX,CS_MIN,SUPPEXP, SUPPEXPMAX,
     &NDIMUSER,NDIMUSER_EFF,ISUM,NST_MAX,
     &NPROC_MIN,NPROC_MAX,NPROC_STEP

      REAL*8 AS_MAX,XFINI,XF,COS_MIN,COS_MAX,CS_MIN,SUPPEXP, SUPPEXPMAX
      INTEGER NDIMUSER,NDIMUSER_EFF,ISUM,NST_MAX,
     &NPROC_MIN,NPROC_MAX,NPROC_STEP
 
      REAL*8 PI,BesK1,BesK2,bj,bi,sqrlam,as,x,gj,gi,BF
      INTEGER ip1,ip2,JSUM,I

      REAL*8 BoostBesselK1,BoostBesselK2,BesK11,BesK12,BesK21,BesK22

      REAL*8 b,g,pm
      CHARACTER*6 names
      INTEGER J,idof,IPTOT
      COMMON /INPART/ b(29),g(29),pm(29),idof(29),IPTOT
      COMMON /NAMES/ names(29)
      
      CHARACTER*6 PINF1,PINF2,PINF
     
     
      Data Pi / 3.14159265358979323846D0 /

      DO J=1,IPTOT
        IF(PINF(I,1).eq.NAMES(J)) IP1=J
        IF(PINF(I,2).eq.NAMES(J)) IP2=J
        b(J)=PM(J)/PM(1)
 	g(J)=IDOF(J)
      ENDDO
     
  
        BF=0d0
      
       JSUM=IPTOT
       if(NPROC_MIN.ge.1.and.NPROC_MAX.le.26.and.ISUM.eq.0) JSUM=1   
       if(NDIMUSER_EFF.eq.2) JSUM=1
       DO J=1,JSUM
       BesK11 = BoostBesselK1(as/x) !*Exp(as/x)
       BesK21 = BoostBesselK2(b(J)/x) !*Exp(b(J)/x)
c       BesK11 = BesK1(as/x) * Exp(-as/x)
c       BesK21 = BesK2(b(J)/x) * Exp(-b(J)/x)

c 	BF=BF+b(J)**2*g(J)*BesK21*Exp(b(1)/x)!*Exp((-b(J)+b(1))/x)
 	BF=BF+b(J)**2*g(J)*BesK21!*Exp((-b(J)+b(1))/x)
      ENDDO
      
C Changed from /Exp((-2d0*b(1)+as)/x) to avoid floating overflow. FEP
        MODFA12= 
     _ sqrlam(as,b(ip1),b(ip2))**2*
     _ g(ip1)*g(ip2)*(BesK11/BF) /
     - 4d0/(x*(BF))
!     - 4d0*Exp(-(-2d0*b(1)+as)/x)/(x*(BF)**2)

      if (ISNAN(MODFA12) .and. BesK11.eq.0) MODFA12=0
      if (ISNAN(MODFA12) .and. ISNAN(BesK21)) MODFA12=0
       End   

C-------------------------------------------------------
C#######################################################
C-------------------------------------------------------

      FUNCTION MODFUNC_INT(XFINPUT)
      IMPLICIT NONE
C------- BASES COMMON BLOCKS ---------------------
      
      EXTERNAL MODFUNC
      REAL*8  MODFUNC_INT,MODFUNC,XFINPUT
      
      INTEGER NDIM,IG,MXDIM,NWILD,NCALL
      REAL*8 XL,XU
      PARAMETER (MXDIM = 50 )                                           
      COMMON /BPARM1/ XL(MXDIM),XU(MXDIM),NDIM,NWILD,IG(MXDIM),NCALL    
c      DATA NWILD/2/,NCALL/1000/      
      INTEGER INTV, IPNT, NLOOP, MLOOP,ITMX1,ITMX2
      REAL*8 ACC1,ACC2
      COMMON /BPARM2/ ACC1,ACC2,ITMX1,ITMX2                             
c      DATA  ITMX1/5/,ITMX2/5/,ACC1/0.1d0/,ACC2/0.1d0/
      COMMON /BSCNTL/ INTV, IPNT, NLOOP, MLOOP      
c---------------------------------------------------
      REAL*8 ESTIM, SIGMA, CTIME

     
      COMMON /CTRL/
     &AS_MAX,XFINI,XF,COS_MIN,COS_MAX,CS_MIN,SUPPEXP, SUPPEXPMAX,
     &NDIMUSER,NDIMUSER_EFF,ISUM,NST_MAX,
     &NPROC_MIN,NPROC_MAX,NPROC_STEP

      REAL*8 AS_MAX,XFINI,XF,COS_MIN,COS_MAX,CS_MIN,SUPPEXP, SUPPEXPMAX
      INTEGER NDIMUSER,NDIMUSER_EFF,ISUM,NST_MAX,
     &NPROC_MIN,NPROC_MAX,NPROC_STEP
     
      INTEGER I,IPRINT,IT1,IT2
      
      IPRINT=0
      XF=XFINPUT
      NDIMUSER=2
      NDIMUSER_EFF=2

c      WRITE(*,*) "AS_MAX: ",AS_MAX
c      WRITE(*,*) "XFINI: ",XFINI
c      WRITE(*,*) "XF: ",XF
c      WRITE(*,*) "COS_MIN: ",COS_MIN
c      WRITE(*,*) "COS_MAX: ",COS_MAX
c      WRITE(*,*) "CS_MIN: ",CS_MIN
c      WRITE(*,*) "SUPPEXP: ",SUPPEXP
c      WRITE(*,*) "SUPPEXPMAX: ",SUPPEXPMAX
c      WRITE(*,*) "NDIMUSER: ",NDIMUSER
c      WRITE(*,*) "NDIMUSER_EFF: ",NDIMUSER_EFF
c      WRITE(*,*) "ISUM: ",ISUM
c      WRITE(*,*) "NST_MAX: ",NST_MAX
c      WRITE(*,*) "NPROC_MIN: ",NPROC_MIN
c      WRITE(*,*) "NPROC_MAX: ",NPROC_MAX
c      WRITE(*,*) "NPROC_STEP: ",NPROC_STEP


      CALL BSINIT 
      
      ITMX1=5
      ITMX2=5
      NDIM =NDIMUSER_EFF
      NWILD=NDIM
      ACC1=1.
      ACC2=1.
      
      DO 1 I=1,NDIMUSER_EFF
        XL(I)=0d0
        XU(I)=1d0
    1 CONTINUE
      XL(3)=0.000001
      XU(3)=XF
   
c-------------------------------------------------

      INTV=IPRINT
      
c       print *,'NCALLS=', NCALL,IPRINT,INTV,NPROC_MIN,NPROC_MAX
      
c      CALL BSINIT                                                  
      CALL BASES( MODFUNC, ESTIM, SIGMA, CTIME, IT1, IT2 )             
      
      MODFUNC_INT=ESTIM
      
c      WRITE(*,*) "FUNC_INT", FUNC_INT
      
      RETURN
      END
