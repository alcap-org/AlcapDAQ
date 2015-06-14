#!/usr/bin/perl

require "ctime.pl";
require "flush.pl";
use DBI;
use CGI qw(:standard);

$| = 1;

#
# Check which histogram files exist 
#
my %files = ();
#my $histdir = "/home/l_data/online-hists";
#my $histdir = "/mnt/cheetah-hists";
my $histdir = "/data/alcap/R15a/root";
if( -e $histdir ) {
    #system("ls -l $histdir > /tmp/files.txt");
    open(TMP,"ls -1 $histdir|");
    while( my $line = <TMP> ) {
	chop $line;
	if( $line =~ m/his(\d+)\.root/ ) {
	    $files{$1} = $line;
	}
    }
    close TMP;
}

my $selfurl = url(-relative=>1)."?".join("+",@ARGV);

#
# Parse arguments
#
my @selarg = split(":",$ARGV[0]);
my @modearg = split(":",$ARGV[1]);

my $limit = " ";
my $sel = " ";
my @selpar = ( );
if( $#selarg>-1 ) {
    if( $selarg[0] eq 'M' ) {
 	$sel = " and date_format(starttime,'%b-%y') like ? ";
 	push @selpar, $selarg[1];
    } elsif( $selarg[0] eq 'D' ) {
 	$sel = " and date_format(starttime,'%b-%d-%Y') like ? ";
 	push @selpar, $selarg[1];
    } elsif( $selarg[0] eq 'L' ) {
	$limit = " DESC LIMIT $selarg[1]";
#       $limit = " DESC LIMIT ?";
#  	push @selpar, $selarg[1];
    } elsif( $selarg[0] eq 'W' ) {
 	$sel = " and (week(StartTime,1)-4)=? ";
 	push @selpar, $selarg[1];
    } elsif( $selarg[0] eq 'R' ) {
 	$sel = " and r.run between ? and ? ";
 	push @selpar, $selarg[1];
 	push @selpar, $selarg[2];
    }
}

my $editable = 0;
if( $modearg[0] eq "E" ) { $editable=1; }
my $linkedit = undef;
if( $editable!=1 ) {
    $linkedit = "<A href=\"$selfurl+E\">Switch to editable mode</A></td>";
}

my $dsn = "DBI:mysql:alcap";
my $user = "webguest";
my $pass = undef;

#if( $editable==1 ) {
#    if( defined(param("User")) && defined(param("Password")) ) {
#	$user = param("User");
#	$pass = param("Password");
#    } else {
#	askForPassword();
#    }
#}

my $dbh;
if( defined($pass) ) {
    $dbh = DBI->connect($dsn, $user, $pass);
} else {
    $dbh = DBI->connect($dsn, $user);
}
if( ! $dbh ) { printError("Cannot connect to database."); }

my $sth; 
my $sth1; 

#if( defined(param("Switch")) ) { print h1(param("Switch")); }

printHeader();

print "<FORM>\n";
print '<TABLE RULES=all border=0 width="100%">',"\n";

$~ = HEADLINE;
write;

my %qcolors = ( "Y" => "99FF66", "N" => "FF9999", "C" => "66CCFF", "T" => "CCFF33", "?" => "FFFFFF" );

my $sth = $dbh->prepare("SELECT r.run,date_format(starttime,'%b-%d'),".
			"date_format(starttime,'%T'),".
			"date_format(stoptime,'%T'),".
			"nblock,".
			"comment,".
			"muon_polarity, size*1.0/(1024*1024), ".
			"quality, shift, beam_momentum_scale,".
			"target,".
			"tape,tapesize ".
 			"FROM runinfo as r ".
			"WHERE r.run>=0 and r.run<90000".$sel.
			"GROUP BY r.run ORDER BY r.run ".$limit);
$sth->execute(@selpar);
$~ = OUT;
my @data;
my $count = 0;
my $nruns = 0;
my $formid = 0;
my $qcolor;
while( (@data=$sth->fetchrow_array()) ) {
    if( $count==20 ) { 
	$~ = HEADLINE;
	write;
	$count = 0;
    }
    if( defined($data[5]) ) { $data[5] =~ s/\n/ /g; }
    if( $editable==1 ) {
	$~ = OUT;
    } else { 
	$~ = OUT1;
    }
    if( !defined($data[8]) ) { $data[8]="?"; }
    $qcolor = $qcolors{$data[8]}; 
    if( !defined($qcolor) ) { $qcolor="FFFFFF"; }
    write;
    $count++;
    $nruns++;
}
$sth->finish;


$~ = END;
write;

$dbh->disconnect;

print "</BODY>\n</HTML>\n";
exit(0);

format HEAD = 
.

format HEADLINE =
<tr bgcolor=#00ffff>
<th rowspan=2> H </th>
<th rowspan=2> Q </th>
<th rowspan=2> Mu </th>
<th rowspan=2> Run </th>
<th rowspan=2 width=30> Date </th>
<th rowspan=1> Start </th>
<th rowspan=2> Blocks </th>
<th rowspan=2> Size, MB </th>
<th rowspan=1> Tape </th>
<th rowspan=1> Shift </th>
<th rowspan=2> Target </th>
<th rowspan=2 align=left> Comment</th>
</tr>
<tr bgcolor=#00ffff>
<th rowspan=1> End </th>
<th rowspan=1> Size </th>
<th rowspan=1> Beam mom. </th>
</tr>
.

format END = 
</table>
</FORM>
.

format OUT =
# OUT controls the formatting in "Editable mode"
<tr valign=top>

# The +/- indicating if a histogram file exists
<td rowspan=2>@< </td>
defined($files{$data[0]})?"+":"-"

# Run Quality
# This is an example of updating the database in editable mode.
# Be careful that the @<<<<< fields are large enough.
<td bgcolor=@* rowspan=2>
$qcolor
	<INPUT TYPE=textfield SIZE=2 MAXLENGTH=2 VALUE=@* onChange="update(window,@*,@*,@*)">
$data[8],$data[0],"'quality'",($formid++)
</td>

# Beam polarity
<td rowspan=2> @<<<<<< </td>
$data[6]

# Run number
<td rowspan=2>
	<a href="show_run.cgi?@<<<<<" onClick="open_runs_view(href)" target="runs_view_window"> @<<<<<< </a>
$data[0],$data[0]
</td>

# Date
<td rowspan=2> @<<<<<< </td>
$data[1]

# Start time
<td rowspan=1> @<<<<<<< </td>
$data[2]

# Number of Blocks
<td align=right rowspan=2> @<<<<<< </td>
$data[4]

# File size, MB
<td align=right rowspan=2> @####.## </td>
$data[7]

# Disk
<td align=center rowspan=1> @* </td>
$data[12]

# Shift
<td rowspan=1>
	<INPUT TYPE=textfield SIZE=6 MAXLENGTH=20 VALUE=@* onChange="update(window,@<<<<<<<,@<<<<<<<<<,@<<<<<<)">
'"'.$data[9].'"',$data[0],"'shift'",($formid++)
</td>

# Target
<td rowspan=2>
	<TEXTAREA cols=40 rows=2 onChange="update(window,@<<<<<<<,@<<<<<<<<<,@<<<<<<)"> @* </TEXTAREA>
$data[0],"'Target'",($formid++),$data[11]
</td>

#Comment
<td rowspan=2>
	<TEXTAREA cols=40 rows=2 onChange="update(window,@<<<<<<<,@<<<<<<<<<,@<<<<<<)"> @* </TEXTAREA>
$data[0],"'Comment'",($formid++),$data[5]
</td>

</td>
</tr>
<tr>
# End time
<td rowspan=1> @<<<<<<< </td>
$data[3]

# Size on tape
<td align=right rowspan=1> @####.## </td>
$data[13]

# Beam momentum
<td rowspan=1 nowrap align=center>  @* </td>
$data[10]
</tr>
.

format OUT1 =
# OUT1 controls the formating in normal mode
<tr valign=top>

# The +/- indicating if a histogram file exists
<td rowspan=2>@< </td>
defined($files{$data[0]})?"+":"-"

# Run Quality
<td bgcolor=@* rowspan=2> @* </td>
$qcolor,$data[8]

# Beam polarity
<td align=right rowspan=2> @<<<<<< </td>
$data[6]

# Run number
<td rowspan=2><a href="show_run.cgi?@<<<<<" onClick="open_runs_view(href)" target="runs_view_window"> @<<<<<< </a></td>
$data[0],$data[0]

# Date
<td rowspan=2> @<<<<<< </td>
$data[1]

# Start time
<td rowspan=1> @<<<<<<< </td>
$data[2]

# Number of Blocks
<td align=right rowspan=2> @####.## </td>
$data[4]

# File size, MB
<td align=center rowspan=2> @* </td>
$data[7]

# Disk
<td rowspan=1> @* </td>
$data[12]

# Shift
<td align=right rowspan=1> @* </td>
$data[9]

# Target
<td align=center rowspan=2> @* </td>
$data[11]

#Comment
<td rowspan=2> @* </td>
$data[5]

</td>
</tr>
<tr>

# End time
<td rowspan=1> @<<<<<<< </td>
$data[3]

# Size on tape
<td align=right rowspan=1> @############ </td>
$data[13]

# Beam momentum
<td rowspan=1 nowrap align=center> @* </td>
$data[10]
</tr>
.


sub printHeader() {
print <<EOF;
Content-type: text/html

<HTML>
<HEAD>
<TITLE>AlCap Run 2013 Log</TITLE>
</HEAD>
<BODY bgcolor="#ffffff" text="#880000" link="#0000ff" vlink="#0000ff">

<SCRIPT LANGUAGE="JavaScript">

var runs = new Object();

var runs_view_window = null;

function open_runs_view(href) {
    if( runs_view_window==null || runs_view_window.closed ) {
	runs_view_window = open (href,"runs_view_window",
				 "toolbar=0,location=0,directories=0,status=0,"+
				 "menubar=0,width=900,height=700,resizable=1,scrollbars=1");
    }
}

function update(w,run,field,id) { 
    value = w.document.forms[0].elements[id].value;
    w.status="Change "+field+" for "+run+" to "+value;
    var newurl = "http://"+w.location.host+"/cgi-bin/alcap/update_run.cgi?";
    newurl += run+"+"+field+"+"+value;
    update_w = open(newurl,"update_status_window",
		    "toolbar=0,location=0,directories=0,status=0,"+
		    "menubar=0,width=400,height=100,resizable=1,scrollbars=0");
}

</SCRIPT>

<TABLE ALIGN=center WIDTH="80%">
<tr>
<td align=left width=200></td>
<td align=center><H2>AlCap Run 2013 Log</H2></td>
<td align=right width=200>$linkedit</td>
</tr>
</TABLE>

<P>
Legend: 
<ul>
<li>H - "+" if file with online histograms exists
<li>Q - quality mark (Y - good, N - bad, T - test)
<li>Mu - polarity condition (mu+, mu-, off)
</ul>
<P>
EOF
}

#
# Print error message and exit
#
sub printError() {
    print header;
    print start_html("Error");
    print b("The following error occured:"),p;
    print @_,"\n";
    print end_html;
    exit 0;
}

# sub askForPassword() {
#     print header;
#     print start_html("Database password");
#     print start_form;
#     print table(
# 		Tr( 
# 		    th(['Database username: ',
# 			textfield('User',"",10,10),
# 			"Password: ",
# 			password_field("Password","",10,20),
# 			submit('Submit')]
# 		       )
# 		    )
# 		);
#     print end_form;
#     print end_html;
#     exit 0;
# }
