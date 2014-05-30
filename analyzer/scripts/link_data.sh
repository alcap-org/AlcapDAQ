#!/bin/bash

HOMES="/gpfs/home"
USERS="quirk_j hungerford_e hoai_n krikler_b litchfield_p edmonds_a grange_j miller_j"
DATADIR="data"
TREEDIR="$DATADIR/tree"
HISTDIR="$DATADIR/hist"
CORRDIR="$DATADIR/corr"
UTREEDIR="$HOME/$TREEDIR"
UHISTDIR="$HOME/$HISTDIR"
UCORRDIR="$HOME/$CORRDIR"

mkdir -p $UTREEDIR
mkdir -p $UHISTDIR
mkdir -p $UCORRDIR

# CLean up old links
TREES="$(ls $UTREEDIR)"
for ITREE in $TREES; do
    ITREEFILE="$UTREEDIR/$ITREE"
    if [ -L $ITREEFILE -a ! -f $ITREEFILE ]; then
	echo "Removing broken link ($ITREEFILE)"
	rm $ITREEFILE
    fi
done
HISTS="$(ls $UHISTDIR)"
for IHIST in $HISTS; do
    IHISTFILE="$UHISTDIR/$IHIST"
    if [ -L $IHISTFILE -a ! -f $IHISTFILE ]; then
	echo "Removing broken link ($IHISTFILE)"
	rm $IHISTFILE
    fi
done
CORRS="$(ls $CORRDIR)"
for ICORR in $CORRS; do
    ICORRFILE="$UCORRDIR/$ICORR"
    if [ -L $ICORRFILE -a ! -f $ICORRFILE ]; then
	echo "Removing broken link ($ICORRFILE)"
	rm $ICORRFILE
    fi
done

# Create new links
for IUSER in $USERS; do
    IHOME="$HOMES/$IUSER"
    if [ $IUSER != $USER ]; then
	# Link to all trees
	ITREEDIR="$IHOME/$TREEDIR"
	if [ -d $ITREEDIR ]; then
	    TREES="$(ls $ITREEDIR)"
	else
	    TREES=""
	fi
	for ITREE in $TREES; do
	    ITREEFILE="$ITREEDIR/$ITREE"
	    UTREEFILE="$UTREEDIR/$ITREE"
	    if [ -f "$ITREEFILE" -a ! -L "$ITREEFILE" ]; then
		if [ -f "$UTREEFILE" ]; then
		    if [ ! -L "$UTREEFILE" ]; then
			echo "Warning: $ITREE exists in $IUSER's data directory and yours."
		    fi
		else
		    echo "Making link to $IUSER's $ITREE"
		    ln -s $ITREEFILE $UTREEFILE
		fi
	    fi
	done
	# Link to all histograms
	IHISTDIR="$IHOME/$HISTDIR"
	if [ -d $IHISTDIR ]; then
	    HISTS="$(ls $IHISTDIR)"
	else
	    HISTS=""
	fi
	for IHIST in $HISTS; do
	    IHISTFILE="$IHISTDIR/$IHIST"
	    UHISTFILE="$UHISTDIR/$IHIST"
	    if [ -f "$IHISTFILE" -a ! -L "$IHISTFILE" ]; then
		if [ -f "$UHISTFILE" ]; then
		    if [ ! -L "$UHISTFILE" ]; then
			echo "Warning: $IHIST exists in $IUSER's data directory and yours."
		    fi
		else
		    echo "Making link to $IUSER's $IHIST"
		    ln -s $IHISTFILE $UHISTFILE
		fi
	    fi
	done
	# Link to all ODB corrections
	ICORRDIR="$IHOME/$CORRDIR"
	if [ -d $ICORRDIR ]; then
	    CORRS="$(ls $ICORRDIR)"
	else
	    CORRS=""
	fi
	for ICORR in $CORRS; do
	    ICORRFILE="$ICORRDIR/$ICORR"
	    UCORRFILE="$UCORRDIR/$ICORR"
	    if [ -f "$ICORRFILE" -a ! -L "$ICORRFILE" ]; then
		if [ -f "$UCORRFILE" ]; then
		    if [ ! -L "$UCORRFILE" ]; then
			echo "Warning: $ICORR exists in $IUSER's data directory and yours."
		    fi
		else
		    echo "Making link to $IUSER's $ICORR"
		    ln -s $ICORRFILE $UCORRFILE
		fi
	    fi
	done
    fi
done