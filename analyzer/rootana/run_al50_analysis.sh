#!/usr/bin/env bash

SCRDIR=scripts/Al50
NMUSCR=$SCRDIR/R15b_Al50_nmu.C
EVDESCR=$SCRDIR/R15b_Al50_EvdE.C
MCEVDESCR=$SCRDIR/Al50/R15b_Al50_MC_EvdE.C
PSELSCR=$SCRDIR/R15b_Al50_psel.C
TMSCR=$SCRDIR/R15b_Al50_TransferMatrix.C
RECOSCR=$SCRDIR/R15b_Al50_Unfold.C

OUTDIR=$HOME/data/R15b
mkdir -p $OUTDIR
ROOTCMD="root -l -b -q"
PARALLELCMD="parallel --eta"
RUN1=09889
RUN2=10128
NMCFILES=12
NPART=4

# Compile all before multithreading
$ROOTCMD $NMUSCR+g
$ROOTCMD $EVDESCR+g
$ROOTCMD $MCEVDESCR+g
$ROOTCMD $PSELSCR+g
$ROOTCMD $TMSCR+g
$ROOTCMD $RECOSCR+g

$PARALLELCMD "$ROOTCMD $NMUSCR+g\(\\\"~/R15bTME/Al50/tme{}.root\\\",\\\"data/Al50/nmu{}.root\\\",{},10e3)" ::: {$RUN1..$RUN2}
hadd -f $OUTDIR/Al50/nmual50.root $OUTDIR/Al50/nmu?????.root
$ROOTCMD scripts/Al50/r15b_al50_nmu_draw.C

$PARALLELCMD "$ROOTCMD $EVDESCR+g\({}\)" ::: {$RUN1..$RUN2}
hadd -f $OUTDIR/evdeal50.root $OUTDIR/evde?????.root
$ROOTCMD scripts/Al50/r15b_al50_evde_draw.C

# $PARALLELCMD "$ROOTCMD $MCEVDESCR+g\({}\)" ::: {1,$NMCFILES},{1..$NPART}
# hadd -f $OUTDIR/Proton_EvdE_MC.root   $OUTDIR/Proton_EvdE_MC_*.root
# hadd -f $OUTDIR/Deuteron_EvdE_MC.root $OUTDIR/Deuteron_EvdE_MC_*.root
# hadd -f $OUTDIR/Triton_EvdE_MC.root   $OUTDIR/Triton_EvdE_MC_*.root
# hadd -f $OUTDIR/Alpha_EvdE_MC.root    $OUTDIR/Alpha_EvdE_MC_*.root

$PARALLELCMD "$ROOTCMD $PSELSCR+g\({}\)" ::: {$RUN1..$RUN2}
hadd -f $OUTDIR/pselal50.root $OUTDIR/psel?????.root
$ROOTCMD scripts/Al50/r15b_al50_psel_draw.C+g

$PARALLELCMD "$ROOTCMD $TMSCR+g\({}\)" ::: {1..$NPART}
$ROOTCMD scripts/Al50/r15b_al50_tm_draw.C

$PARALLELCMD "$ROOTCMD $RECOSCR+g\({}\)" ::: {1..$NPART}
$ROOTCMD scripts/Al50/r15b_al50_unfold_draw.C