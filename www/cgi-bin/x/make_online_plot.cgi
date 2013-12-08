#! /usr/bin/perl 

require "ctime.pl";
require "flush.pl";
use DBI;
use CGI qw(:standard);

$| = 1;

my %plots = (
	     "raw" => { -nick=>"raw", -macro=>"raw_parameter_display.C"},
	     "fill" => { -nick=>"fill", -macro=>"fill_display.C"},
	     "EMC" => { -nick=>"EMC", -macro=>"emc_display.C"},
	     "detector" => { -nick=>"detector", -macro=>"detector_display.C"},
	     "calibration" => { -nick=>"calibration", -macro=>"calibration.C"},
	     "wfdbanks" => { -nick=>"wfdbanks", -macro=>"wfdbanks.C"},
	     "wfdsize" => { -nick=>"wfdsize", -macro=>"wfdsize.C"}
	     );

my $topdir = "/home/mulan/www";
my $tmpepsfile = "tmp.eps";
my $tmprootfile = "tmp.root";
my $outdir = "$topdir/html/cache";
my $rootdir = "$topdir/root";
#my $datadir = "/tmp/hist";
my $datadir = "/export/raid";

my ( $run, $plot ) = @ARGV;

# Check if plot with this name exists
printError("Plot $plot does not exist.") if ! defined($plots{$plot});

my $mname = $plots{$plot}->{"-macro"};
printError("Macro for $plot is not defined.") if ! defined($mname);

printError("Macro $mname does not exist.") if ! -e "$rootdir/$mname";


# Check if file exist
my $hfile = sprintf("%s/his%d.root",$datadir,$run);
printError("File $hfile does not exist.") if ! -e $hfile; 

# Check if file already exist
my $outname = sprintf("%s_%d",$plots{$plot}->{"-nick"},$run);
my $outepsfile = "$outdir/$outname.eps";
my $outjpgfile = "$outdir/$outname.jpg";
my $outpngfile = "$outdir/$outname.png";
my $outrootfile = "$outdir/$outname.root";

if( (! -e $outepsfile) || (! -e $outpngfile) || (! -e $outrootfile) ) {

    foreach my $name ( $outepsfile, $outpngfile, $outrootfile  ) { 
	unlink $name if -e $name; 
    }

    if( -e "$rootdir/$tmpepsfile" ) { unlink "$rootdir/$tmpepsfile"; }
    if( -e "$rootdir/$tmprootfile" ) { unlink "$rootdir/$tmprootfile"; }

    if( system("$topdir/root/makeplot $hfile $mname >& $topdir/log/plot.log")!=0 ) {
	printErrorWithLog("Error while executing macro","$topdir/log/plot.log");
    }

    if( system("cp $rootdir/$tmpepsfile $outepsfile") ) {
	printError("Cannot copy EPS image to the final destination");
    }
    if( system("cp $rootdir/$tmprootfile $outrootfile") ) {
	printError("Cannot copy ROOT file to the final destination");
    }
    if( system("convert -quality 10 -size 800x600 $rootdir/$tmpepsfile -resize 800x600 $outpngfile") ) {
	printError("Cannot convert image to JPG");
    }

}

print header;
print start_html("Online plots for run $run: $pname");
print table(Tr(td([
		   a({-href=>"/mulan/cache/$outname.eps"},"Postscript version"),
		   "&nbsp;",
		   a({-href=>"/mulan/cache/$outname.root"},"ROOT version"),
		   "&nbsp;&nbsp;",
		   a({-href=>"/mulan/plot.log"},"View ROOT log file")
		   ])));
print p;
print img({-src=>"/mulan/cache/$outname.png"});
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
