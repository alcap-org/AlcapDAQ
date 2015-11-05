#!/usr/bin/perl

require "ctime.pl";
require "flush.pl";
use DBI;
use CGI qw(:standard);

$| = 1;
$dsn = "DBI:mysql:database=alcap;host=abe.psi.ch";
$user = "webguest";

my $run;
if( $#ARGV<0 ) { exit(0); } else { $run=$ARGV[0]; }

my $dbh = DBI->connect($dsn, $user);
my $sth = $dbh->prepare("select r.run,starttime,stoptime,nblock,comment ".
			"from runinfo as r ".
			"where r.run=?");
$sth->execute($run);
my @data;

if( ( @data ) = $sth->fetchrow_array() ) { 
} else {
    print header;
    print start_html('Run Info'),
    h1("No info for run $run"),
    end_html;
    goto out;
}
$sth->finish;

$dbh->disconnect;

print header;
print start_html('Run Information');
#print h2("Run $run");
print start_form;
print table({-cellspacing=>10},
	    Tr(
	       th(["Started ",textfield("start",$data[1],16,16)]),
	       th(["Finished ",textfield("end",$data[2],16,16)])
	       )
	    );
print table({-cellspacing=>10},
	    Tr(
	       th(["Blocks ".textfield("blocks",$data[3],5,5)]),
	       )
	    );
print table({-cellspacing=>10},
	    Tr({-valign=>"top"},
	       th("Comment: "),
	       td(textarea("comment",$data[4],5,50))
	       )
	    );
print end_form;
print end_html;


$dbh->disconnect;
exit(0);

