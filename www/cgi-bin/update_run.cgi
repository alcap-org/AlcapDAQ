#!/usr/bin/perl

require "ctime.pl";
require "flush.pl";
use DBI;
use CGI qw(:standard);

$| = 1;
$dsn = "DBI:mysql:alcap";
$user = "webguest";
$pass = "";

my $run;
if( $#ARGV<0 ) { exit(0); } else { $run=$ARGV[0]; }

my $dbh = DBI->connect($dsn, $user);
if( !$dbh ) {
    print header;
    print start_html('Update status');
    print h3("Cannot connect to database");
    print end_html;
    exit 0;
}

my ( $run, $field, $value ) = @ARGV;

my $sth = $dbh->prepare("update runinfo set `$field`=? where `run`=?");
$sth->execute($value,$run);
$dbh->disconnect;

print header;
print start_html('Update status');
print h3("The following information was updated:");
print "Run $run : $field = $value";
print end_html;

exit(0);

