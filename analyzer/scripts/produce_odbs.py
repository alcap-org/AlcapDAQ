import shutil

ifile = open('WireMaps_R15b.csv', 'r')
odbs = zip(*[l.strip().split(',') for l in ifile])
label, odbs = odbs[0], odbs[1:]

print "Labels: "
print label
print
print "ODBs: "
print odbs

for db in odbs:
    with open('wm'+format(int(db[0]), '05d')+'.odb', 'w') as ofile:
        ofile.write('[/Analyzer/WireMap]\n');
        ofile.write('BankName = STRING[92] :\n');
        for ch in label[1:]:
            ofile.write('[80] ')
            ofile.write(ch)
            ofile.write('\n')
        ofile.write('DetectorName = STRING[92] :\n');
        for det in db[1:]:
            ofile.write('[80] ')
            ofile.write(det)
            ofile.write('\n')

# Now create copies for all the runs where nothing changed
n_cols=len(odbs)
final_run=10514
for i_col in range(0, n_cols):
    first_run=int(odbs[i_col][0])
    last_run=-1
#    print i_col
    if (i_col==n_cols-1):
        last_run=final_run
    else:
        last_run=int(odbs[i_col+1][0])
    print first_run
    print last_run
    for run in range(first_run+1, last_run):
        print first_run
        print run
        shutil.copy('wm'+format(first_run, '05d')+'.odb', 'wm'+format(run, '05d')+'.odb')
