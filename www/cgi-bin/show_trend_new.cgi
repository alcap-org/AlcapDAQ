#! /usr/bin/perl 

require "ctime.pl";
require "flush.pl";
use DBI;
use CGI qw(:standard);

$| = 1;

if( defined(param("Go")) ) {
    $ARGV[0] = param("plot");
    $ARGV[1] = "run:".param("rmin").":".param("rmax");
    if( !defined(param("rmin")) || !defined(param("rmax")) ) {
	printError("Please, specify the run number");
    }
    if( param("rmin")<1 || param("rmin")>1000000 || param("rmax")<1 || param("rmax")>1000000 ) {
	printError("Run number is incorrect");
    }
}

my %plots = (
	     "rates" => "Rates",
             "lifetime" => "Lifetime fit",
             "capture" => "Impurity captures",
             "extinct" => "Kicker extinction",
	     "lcferrs" => "LCF errors",
             "stats" => "Accumulated statistics",
             "wfds" => "Wfd errors",
             "fadc" => "FADC rates",
	     "impurity" => "Brendan's Test Area for Impurities",
             "daq" => "DAQ livetime",
	     "crates" => "Crate data rates",
	     );
 
my %types = (
	     "all" => "All runs",
	     "last20" => "Last 20 runs",
	     "last100" => "Last 100 runs",
	     "24h" => "Last 24 hours",
	     "48h" => "Last 48 hours", 
	     "run" => "Selected runs"
	     );

my $topdir = "/home/l_data/www/musun";
my $outdir = "$topdir/html/trend";
my $rootdir = "$topdir/root/trend";
my $tmpepsfile = "tmp.eps";
my $tmprootfile = "tmp.root";
my $cache_time = 60;

my ( $plot, $type1 ) = @ARGV;

my ( $type, $arg1, $arg2 ) = split(":",$type1);

my $typec = $type1;
$typec =~ s/:/_/;

if( !defined($type) || !defined($types{$type}) ) { $type="all"; }

my $mname = "$rootdir/$plot.cxx";

# Check if plot with this name exists
printError("Trend plot $plot does not exist.") if ! defined($plots{$plot});

# Create the plot itself
my $outname = sprintf("%s_%s",$plot,$typec);
my $outepsfile = "$outdir/$outname.eps";
my $outjpgfile = "$outdir/$outname.jpg";
my $outpngfile = "$outdir/$outname.png";
my $outrootfile = "$outdir/$outname.root";

makePlot();

print header;
print start_html($plots{$plot}." trend plot");
#while (($Name, $Value) = each(%ENV)) 
#{
#  print "<b>" , $Name , ": </b>" , $Value , "<br>\n";
#}
#print h3({-align=>"center"},$plots{$plot}),p;
my $url = url(-full=>1);
print "<table>";
print "<tr valign=top>\n";
foreach my $t ( "all", "last20", "last100", "24h", "48h" ) {
    if( $type eq $t ) {
	print "<th width=100 bgcolor=yellow>";
    } else {
	print "<th width=100>";
    }
    print  a({-href=>"$url?$plot+$t"},$types{$t});
    print "</th>\n";
}
if( $type eq "run" ) {
    print "<th nowrap width=100 bgcolor=yellow>";
} else {
    print "<th nowrap width=100>";
}
print start_form;
print hidden("plot",$plot);
print "<table border=0 cellpading=0 cellspacing=1><tr>";
print "<th>Runs </th>";
print "<th>",textfield("rmin",$arg1,7,7),"</th>";
print "<th>",textfield("rmax",$arg2,7,7),"</th>";
print "<th>",submit("Go","Go"),"</th>";
print "</tr></table>";
print end_form;

print "</tr></table>\n";
print p;
print h3({-align=>"center"},$plots{$plot}."   (".$types{$type}.")"),p;
print img({-src=>"/musun/trend/$outname.png"});
print table({-border=>0},
	    Tr(td([
		   a({-href=>"/musun/trend/$outname.eps"},"Postscript version"),
		   a({-href=>"/musun/trend/$outname.root"},"ROOT version")
		   ])));
print end_html;
exit 0;

#
# Make plot
#
sub makePlot() {
    
    # if old files exist  - use them, if they are less than 1 minute old
    my $old_exist = 1;
#    printError("Trend plot $plot does not exist.") if ($old_exist==1);

   
    foreach my $name ( $outepsfile, $outpngfile, $outrootfile  ) { 
	$old_exist = 0;
	last;
    }

    if( $old_exist==1 ) {
		my @statinfo = stat $outepsfile;
	my $tnow = time();
	if( ($tnow-$statinfo[9])<$cache_time ) { return; }
    }

    # Clear data files if any

    foreach my $file ( "dates.dat", "good.dat", "all.dat" ) {
	unlink "$rootdir/$file" if -e "$rootdir/$file";
    }

    # Create new data files

#    if ($plot eq "impurity"){$plot = "stats";}
    unlink "$rootdir/data.log" if -e "$rootdir/data.log";
    system("cd $rootdir; perl $rootdir/make_dat_files.pl $plot $type1 >& $rootdir/data.log");

#    open(LOG,"$rootdir/bk.log");
#    print LOG "generated the data file\n";
    foreach my $file ( "dates.dat", "good.dat", "all.dat" ) {
	if( ! -e "$rootdir/$file" ) {
	    printErrorWithLog("Cannot prepare data file $rootdir/$file now","$rootdir/data.log");
	}
    }

    # Delete cached and tmp files
    
    foreach my $name ( $outepsfile, $outpngfile, $outrootfile  ) { 
	unlink $name if -e $name; 
    }
    
    if( -e "$rootdir/$tmpepsfile" ) { unlink "$rootdir/$tmpepsfile"; }
    if( -e "$rootdir/$tmprootfile" ) { unlink "$rootdir/$tmprootfile"; }
    
    # Start root and make the plot

    unlink "$rootdir/plot.log" if -e "$rootdir/plot.log";
    if( system("$rootdir/makeplot $mname >& $rootdir/plot.log")!=0 ) {
	printErrorWithLog("Error while executing macro","$rootdir/plot.log");
    }
    
    # Convert files and copy them to the destination directory
    
    if( system("cp $rootdir/$tmpepsfile $outepsfile") ) {
	printError("Cannot copy EPS image $rootdir/$tmpepsfile to the final destination $outepsfile");
    }
    if( system("cp $rootdir/$tmprootfile $outrootfile") ) {
	printError("Cannot copy ROOT file $rootdir/$tmprootfile to the final destination $outrootfile");
    }
    #if( system("convert -size 1000x700 $rootdir/$tmpepsfile -resize 1000x700 $outjpgfile") ) {
	if( system("convert -density 200x200 $rootdir/$tmpepsfile $outpngfile") ) {
	    printError("Cannot convert image to PNG");
	}
	close LOG;
    #}
}

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
