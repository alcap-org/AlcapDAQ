#!/usr/bin/perl

require "ctime.pl";
require "flush.pl";
use DBI;

my $run;
if( $#ARGV<0 ) { exit(0); } else { $run=$ARGV[0]; }

print <<EOF;
Content-type: text/html

<html>
<head>
<title>Run $run information</title>
</head>
<frameset cols="220,*">
<frame src="run_links.cgi?$run">
<frame src="edit_run.cgi?$run" name="plot_view_window" scrolling=yes>
</frameset>
</html>
EOF

exit(0);

