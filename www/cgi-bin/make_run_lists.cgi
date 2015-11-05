#!/usr/bin/perl

require "ctime.pl";
require "flush.pl";
use DBI;
use CGI qw(:standard);

$| = 1;
$dsn = "DBI:mysql:database=alcap;host=abe.psi.ch";
$user = "webguest";

my $dbh = DBI->connect($dsn, $user);
my $sth; 

my %dates = ( );
my @dlist = ( );
my $rmin = 10000000;
my $rmax = 0;

my $sth1 = $dbh->prepare("SELECT date_format(now(),'%b-%d-%Y')");
$sth1->execute();
my @data = $sth1->fetchrow_array();
my ( $today ) = @data;

my $sth = $dbh->prepare("SELECT min(run),max(run),count(run),".
			"date_format(starttime,'%b-%d-%Y') as date, ".
			"min(unix_timestamp(starttime)) as ts ".
			"FROM runinfo ".
			"WHERE run>=0 and run<90000 and starttime is not null ".
			"GROUP BY date ORDER BY ts");
$sth->execute();

while( (@data=$sth->fetchrow_array()) ) {
    push @dlist, $data[3];
    $dates{$data[3]} = [ $data[0], $data[1], $data[2] ];
    if( $rmin>$data[0] ) { $rmin=$data[0]; }
    if( $rmax<$data[0] ) { $rmax=$data[1]; }
}
$sth->finish;
$sth1->finish;
$dbh->disconnect;

print header;
print start_html("AlCap Run 2015b");
print h3("Select runs:");

print "<table>";
print "<tr><th width=150></th><th width=300>By Date</th><th width=150>By Run</th></tr>\n";
print "<tr valign=top>\n";

# General links
print "<td>\n";
print ul(li([
	     a({-href=>"make_run_selection.cgi"},"All runs"),
	     a({-href=>"make_run_selection.cgi?L:20"},"Last 20"),
	     a({-href=>"make_run_selection.cgi?L:100"},"Last 100"),
	     a({-href=>"make_run_selection.cgi?D:".$today},"Today")
	     ]));
print "</td>\n";

# Links by date
print "<td>\n";
print "<ul>";
foreach my $date ( @dlist ) {
    print "<li>";
    print a({-href=>"make_run_selection.cgi?D:".$date},
	    $date." (".$dates{$date}->[0]."-".$dates{$date}->[1].", ".
	    $dates{$date}->[2]." runs)");
}
print "</ul>";
print "</td>\n";

# Links by run
print "<td>\n";
print "<ul>";
for( my $r0 = int($rmin/500)*500; $r0<=$rmax; $r0+=500 ) {
    print "<li>";
    print a({-href=>"make_run_selection.cgi?R:".$r0.":".($r0+499)},
	    $r0."-".($r0+499));
}
print "</ul>";
print "</td>\n";

print "</tr>\n";
print "</table>";
print end_html;

exit(0);
