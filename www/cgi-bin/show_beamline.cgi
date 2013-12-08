#! /usr/bin/perl 

require "ctime.pl";
require "flush.pl";
use DBI;
use CGI qw(:standard);

$| = 1;

#
# Prepare list of elements
#
my $dsn = "DBI:mysql:musun;dolphin";
my $user = "webguest";
my $dbh = DBI->connect($dsn, $user);

if( ! $dbh ) { printError("Cannot connect to database"); }

my %elements = (undef, -1);
my $sth = $dbh->prepare("select name,id from BeamlineElements");
$sth->execute();
while( my @data = $sth->fetchrow_array() ) {
    $elements{$data[0]} = $data[1];
}
$sth->finish;

my $nelem = 4;
#my @defelem = ( 'QSK74', 'QSB75', 'SEP', 'QSB72' );
#my @defelem = ( 'QSB73', 'QSK72', 'SEP', 'QSB72' );
#my @defelem = ( 'QSB73', 'QSK72', 'SEP', 'QSB75' );
my @defelem = ( 'SEP71', 'QTD71', 'ASK71', 'ASK72' );
#@defelem = ( 'AEK01', 'ASK71', 'ASK72', 'HSA71',
#	     'HSA72', 'HSD71', 'QSB71', 'QSB72',
#	     'QSB73', 'QSB75', 'QSC01', 'QSD02',
#	     'QSK71', 'QSK72', 'QSK73', 'QSK74',
#	     'QTB71', 'QTD71', 'QTD72', 'SEP',
#	     'WEN' );


#
# Parse parameters
#

my %types = (
	     "all" => "All runs",
	     "last20" => "Last 20 runs",
	     "last100" => "Last 100 runs",
	     "24h" => "Last 24 hours",
	     "48h" => "Last 48 hours", 
	     "run" => "Selected runs",
	     "time" => "Selected time period"
	     );

#my $type = undef;
my ( $type, $elemarg ) = @ARGV;

foreach my $t ( %types ) {
    if( defined(param($t)) ) {
	$type = $t;
	last;
    }
}

my $rmin, $rmax, $tmin, $tmax;
my ( $rmin0, $rmax0, $tmin0, $tmax0 ) = ( 0, 1000000, '2005-01-01 00:00:00' , '2010-01-01 00:00:00' );
$sth = $dbh->prepare("select now()");
$sth->execute();
( $tmax0 ) = $sth->fetchrow_array();
$sth->finish;

if( $type eq "run" ) {
    $rmin = param("rmin");
    $rmax = param("rmax");
}

if( $type eq "time" ) {
    $tmin = param("tmin");
    $tmax = param("tmax");
}

my @elem;
my @elemname;
my $elem_exist = 0;
for( my $int=1; $i<=$nelem; $i++ ) {
    if( !defined(param("elem".$i)) ) { next; }
    $elem_exist = 1;
    my $id = $elements{param("elem".$i)};
    if( defined($id) && $id>=0 ) { 
	push @elem, $id;  
	push @elemname, param("elem".$i);  
    }
}
if( $elem_exist==0 ) {
    if( !defined($elemarg) ) { $elemarg=join(":",@defelem); }
    foreach my $el ( split(":",$elemarg) ) {
	my $id = $elements{$el};
	if( defined($id) && $id>=0 ) { 
	    push @elem, $id;  
	    push @elemname, param("elem".$i);  
	}
    }
}
#
# Get run numbers and times
#

if( $type eq "run" ) {

    $sth = $dbh->prepare("select min(starttime),max(stoptime) from run6 ".
			 "where starttime is not null and stoptime is not null and run between ? and ?");
    if( ! $sth->execute($rmin,$rmax) ) { printError("Database error : ".$DBI::errstr); }
    ( $tmin, $tmax ) = $sth->fetchrow_array();
    $sth->finish;

} elsif( $type eq "time" ) {
    
    $sth = $dbh->prepare("select min(run),max(run) from run6 ".
			 "where stoptime>? and starttime<? and run>9000");
    if( ! $sth->execute($tmin,$tmax) ) { printError("Database error : ".$DBI::errstr); }
    ( $rmin, $rmax ) = $sth->fetchrow_array();
    $sth->finish;

} elsif( $type eq "all" ) {
    
    $sth = $dbh->prepare("select min(run),max(run) from run6 ".
			 "where stoptime is not null and run>9000");
    if( ! $sth->execute() ) { printError("Database error : ".$DBI::errstr); }
    ( $rmin, $rmax ) = $sth->fetchrow_array();
    $sth->finish;

    ( $tmin, $tmax ) = ( $tmin0, $tmax0 );

} elsif( $type eq "24h" ) {
    
    $tmax = $tmax0;
    $sth = $dbh->prepare("select from_unixtime(unix_timestamp(now())-?)");
    $sth->execute(24*3600);
    ( $tmin ) = $sth->fetchrow_array();
    $sth->finish;
    $sth = $dbh->prepare("select min(run) from run6 where stoptime>? and starttime<? and run>9000");
    if( ! $sth->execute($tmin,$tmax) ) { printError("Database error : ".$DBI::errstr); }
    ( $rmin ) = $sth->fetchrow_array();
    $rmax = $rmax0;
    $sth->finish;

} elsif( $type eq "48h" ) {
    
    $tmax = $tmax0;
    $sth = $dbh->prepare("select from_unixtime(unix_timestamp(now())-?)");
    $sth->execute(2*24*3600);
    ( $tmin ) = $sth->fetchrow_array();
    $sth->finish;
    $sth = $dbh->prepare("select min(run) from run6 where stoptime>? and starttime<?");
    if( ! $sth->execute($tmin,$tmax) ) { printError("Database error : ".$DBI::errstr); }
    ( $rmin ) = $sth->fetchrow_array();
    $rmax = $rmax0;
    $sth->finish;

} elsif( $type eq "last20" ) {

    $rmax = $rmax0;
    $sth = $dbh->prepare("select run from run6 where run>9000 order by run desc limit 20");
    $sth->execute();
    $rmin = 10000000;
    while( my @data = $sth->fetchrow_array() ) {
        if( $data[0]<$rmin ) { $rmin=$data[0]; }
    }
    $sth->finish;

    $sth = $dbh->prepare("select min(starttime) from run6 where starttime is not null and run>? ");
    if( ! $sth->execute($rmin) ) { printError("Database error : ".$DBI::errstr); }
    ( $tmin ) = $sth->fetchrow_array();
    $tmax = $tmax0;
    $sth->finish;

} elsif( $type eq "last100" ) {

    $rmax = $rmax0;
    $sth = $dbh->prepare("select run from run6 where run>9000 order by run desc limit 100");
    $sth->execute();
    $rmin = 10000000;
    while( my @data = $sth->fetchrow_array() ) {
        if( $data[0]<$rmin ) { $rmin=$data[0]; }
    }
    $sth->finish;

    $sth = $dbh->prepare("select min(starttime) from run6 where starttime is not null and run>? ");
    if( ! $sth->execute($rmin) ) { printError("Database error : ".$DBI::errstr); }
    ( $tmin ) = $sth->fetchrow_array();
    $tmax = $tmax0;
    $sth->finish;

}

$sth = $dbh->prepare("select unix_timestamp(?),unix_timestamp(?)");
if( ! $sth->execute($tmin,$tmax) ) { printError("Database error : ".$DBI::errstr); }
my ( $tminst, $tmaxst ) = $sth->fetchrow_array();
$sth->finish;


#
# Get the data
#

my $topdir = "/home/l_data/www/musun";
my $outdir = "$topdir/html/trend";
my $rootdir = "$topdir/root/trend";
my $tmpepsfile = "tmp.eps";
my $tmprootfile = "tmp.root";

if( defined($type) ) {
    #printError("cd $rootdir; $rootdir/make_dat_beamline.pl '$tmin' '$tmax' ".join(" ",@elem)." >& beam.log");
    if( system("cd $rootdir; $rootdir/make_dat_beamline.pl '$tmin' '$tmax' ".join(" ",@elem)." >& beam.log") ) {
 	printErrorWithLog("Error while preparing data files","$rootdir/beam.log");
    }
}

#
# Create the plot
#

my $outname = sprintf("beamline_%s_%s_%d_%d",$plot,join("_",@elem),$tminst,$tmaxst);
my $outepsfile = "$outdir/$outname.eps";
my $outjpgfile = "$outdir/$outname.jpg";
my $outpngfile = "$outdir/$outname.png";
my $outrootfile = "$outdir/$outname.root";

foreach my $name ( $outepsfile, $outpngfile, $outrootfile  ) { unlink $name if -e $name; }

if( defined($type) ) {

    if( -e "$rootdir/$tmpepsfile" ) { unlink "$rootdir/$tmpepsfile"; }
    if( -e "$rootdir/$tmprootfile" ) { unlink "$rootdir/$tmprootfile"; }
    
    if( system("$rootdir/makebeamline ".join(" ",@elemname)." >& $rootdir/beam.log")!=0 ) {
	printErrorWithLog("Error while executing macro","$rootdir/beam.log");
    }
    
    if( system("cp $rootdir/$tmpepsfile $outepsfile") ) {
	printError("Cannot copy EPS image to the final destination");
    }
    if( system("cp $rootdir/$tmprootfile $outrootfile") ) {
	printError("Cannot copy ROOT file to the final destination");
    }
    if( system("convert -density 120x120 $rootdir/$tmpepsfile $outpngfile") ) {
	printError("Cannot convert image to PNG");
    }
}

#
# Create the page
#
print header;
print start_html("Beamline trend plot");
print start_form;
my $url = url(-full=>1);
print "<table>";
print "<tr valign=top>\n";
foreach my $t ( "all", "last20", "last100", "24h", "48h" ) {
    if( $type eq $t ) {
	print "<th width=100 bgcolor=yellow>";
    } else {
	print "<th width=100>";
    }
    print submit($t,$types{$t});
    print "</th>\n";
}
if( $type eq "run" ) {
    print "<th nowrap width=100 bgcolor=yellow>";
} else {
    print "<th nowrap width=100>";
}
print "<table border=0 cellpading=0 cellspacing=1><tr>";
print "<th>Runs </th>";
print "<th><input type=text name=\"rmin\" value=\"$rmin\"></th>";
print "<th><input type=text name=\"rmax\" value=\"$rmax\"></th>";
#print "<th>",textfield("rmin",$rmin,7,7),"</th>";
#print "<th>",textfield("rmax",$rmax,7,7),"</th>";
print "<th>",submit("run","Go"),"</th>";
print "</tr></table></th>";
print "<tr>";
print "<th>Elements:</th>";
for( my $i=1; $i<=4; $i++ ) {
    print "<th>";
    print popup_menu(-name=>"elem$i",
		     -values=>[sort keys %elements],
		     -default=>$defelem[$i-1]
		     );
    print "</th>\n";
}
if( $type eq "time" ) {
    print "<th nowrap bgcolor=yellow>";
} else {
    print "<th nowrap >";
}
print "<table border=0 cellpading=0 cellspacing=1><tr>";
print "<th>Time </th>";
print "<th><input type=text name=\"tmin\" value=\"$tmin\"></th>";
print "<th><input type=text name=\"tmax\" value=\"$tmax\"></th>";
#print "<th>",textfield("tmin",$tmin,20,20),"</th>";
#print "<th>",textfield("tmax",$tmax,20,20),"</th>";
print "<th>",submit("time","Go"),"</th>";
print "</tr></table>";
for( my $i=5; $i<=nelem; $i++ ) {
    if(($i % 4)==1){
	print "<tr>";
	print "<th>         </th>";
    }
    print "<th>";
    print popup_menu(-name=>"elem$i",
		     -values=>[sort keys %elements],
		     -default=>$defelem[$i-1]
		     );
    print "</th>\n";
}

print "</th>";
print "</tr></table>";
print end_form;

#print "$type:$rmin:$rmax:$tmin:$tmax";

if( defined($type) ) {
    print p,h3({-align=>"center"},"Beamline history (".$types{$type}.")"),p;
    print img({-src=>"/musun/trend/$outname.png"});
    print table({-border=>0},
		Tr(td([
		       a({-href=>"/musun//trend/$outname.eps"},"Postscript version"),
		       "&nbsp;",
		       a({-href=>"/musun/trend/$outname.root"},"ROOT version")
		       ])));
}

print end_html;

exit 0;

#
# Print error message and exit
#
sub printError() {
    print header;
    print start_html("Error");
    print b("The following error occured while making the plot:"),p;
    print @_,"\n";
    print end_html;
    exit 0;
}

#
# Print error message, log file and exit
#
sub printErrorWithLog() {
    print header;
    print start_html("Error");
    print b("The following error occured while making the plot:"),p;
    print @_[0];
    print p;
    print "<pre>\n";
    if( open(TMP,"<@_[1]") ) {
	while( <TMP> ) { print; }
	close TMP;
    }
    print end_html;
    exit 0;
}
