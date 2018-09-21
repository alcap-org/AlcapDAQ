
L="";
for i in {9889..10080}; do
	f=$(printf data/$1%05d.root $i);
	if [ -f $f ]; then
		L="$L $f";
	fi;
done;
hadd -j -f data/$1al50_102.root $L

L=""
for i in {10081..10128}; do
	f=$(printf data/$1%05d.root $i);
	if [ -f $f ]; then
		L="$L $f";
	fi;
done;
hadd -j -f data/$1al50_103.root $L

hadd -j -f data/$1al50.root data/$1?????.root
