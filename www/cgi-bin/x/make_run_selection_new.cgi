#!/usr/bin/perl

require "ctime.pl";
require "flush.pl";

use CGI qw(:standard);

use DBI;
$| = 1;

#
# Check which histogram files exist 
#
my %files = ();
my $histdir = "/home/data/online-hists";
if( -e $histdir ) {
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
	$limit = " DESC LIMIT ?";
 	push @selpar, $selarg[1];
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

my $dsn = "DBI:mysql:mucap;dolphin";
my $user = "webguest";
my $pass = undef;

if( $editable==1 ) {
    if( defined(param("User")) && defined(param("Password")) ) {
	$user = param("User");
	$pass = param("Password");
    } else {
	askForPassword();
    }
}

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

$~ = HEADLINE;
write;

my %qcolors = ( "Y" => "99FF66", "N" => "FF9999", "C" => "66CCFF", "T" => "CCFF33" );

my $sth = $dbh->prepare("SELECT r.run,date_format(starttime,'%b-%d'),".
			"date_format(starttime,'%T'),".
			"date_format(stoptime,'%T'),".
			"nevent,".
			"comment,".
			"f.production, size*1.0/(1024*1024), ".
			"quality, shift, emc_hv, emc_magnet, p_beam,".
			"target, target_magnet, kicker, kicker_hv_top, ".
			"kicker_hv_bottom,slit3 ".
			"FROM Runlog as r ".
			"LEFT JOIN Runflags as f on r.run=f.run ".
			"WHERE r.run>9000 ".$sel.
			"GROUP BY r.run ORDER BY r.run".$limit);
$sth->execute(@selpar);
$~ = OUT;
my @data;
my $count = 0;
my $nruns = 0;
my $formid = 0;
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
<TABLE RULES=all border=0 width="100%">
<tr bgcolor=#00ffff>
<th rowspan=2> H </th>
<th rowspan=2> P </th>
<th rowspan=2> Q </th>
<th rowspan=2> Run </th>
<th rowspan=2 width=30> Date </th>
<th rowspan=1> Start </th>
<th rowspan=2> Events </th>
<th rowspan=2> Size, MB </th>
<th rowspan=2> Shift </th>
<th rowspan=1> EMC HV </th>
<th rowspan=1> Target </th>
<th rowspan=1 colspan=2> Kicker </th>
<th rowspan=1> Slit3 (o/u/l/r) </th>
<th rowspan=2 align=left> Comment</th>
</tr>
<tr bgcolor=#00ffff>
<th rowspan=1> End </th>
<th rowspan=1> Magnet </th>
<th rowspan=1> Magnet </th>
<th rowspan=1> top </th>
<th rowspan=1> bot </th>
<th rowspan=1> Beam mom. </th>
</tr>
.

format END = 
</table>
</FORM>
.

format OUT =
<tr valign=top>
<td rowspan=2>@< </td>
defined($files{$data[0]})?"+":"-"
<td rowspan=2><INPUT TYPE=checkbox DISABLED @<<<<<<<<< VALUE=@<<<<<></td>
$data[6]==1?"checked":" ",$data[0],($formid++)
<td bgcolor=@* rowspan=2><INPUT TYPE=textfield SIZE=2 MAXLENGTH=2 VALUE=@* onChange="update(window,@*,@*,@*)"></td>
$qcolors{$data[8]},$data[8],$data[0],"'quality'",($formid++)
<td rowspan=2><a href="show_run.cgi?@<<<<<" onClick="open_runs_view(href)" target="runs_view_window"> @<<<<<< </a></td>
$data[0],$data[0]
<td rowspan=2> @<<<<<< </td><td rowspan=1> @<<<<<<< </td>
$data[1],$data[2]
<td align=right rowspan=2> @<<<<<< </td>
$data[4]
<td align=right rowspan=2> @####.## </td>
$data[7]
<td rowspan=2><INPUT TYPE=textfield SIZE=6 MAXLENGTH=20 VALUE=@* onChange="update(window,@<<<<<<<,@<<<<<<<<<,@<<<<<<)"></td>
'"'.$data[9].'"',$data[0],"'shift'",($formid++)
<td align=center rowspan=1> @* </td>
$data[10]
<td align=center rowspan=1> @* </td>
$data[13]
<td colspan=2 rowspan=1 align=center> @* </td>
$data[15]
<td rowspan=1 align=center> @* </td>
$data[18]
<td rowspan=2><TEXTAREA cols=40 rows=2 onChange="update(window,@<<<<<<<,@<<<<<<<<<,@<<<<<<)"> @* </TEXTAREA></td>
$data[0],"'Comment'",($formid++),$data[5]
</td>
</tr>
<tr>
<td rowspan=1> @<<<<<<< </td>
$data[3]
<td rowspan=1 nowrap align=center>  @* </td>
$data[11]
<td rowspan=1 nowrap align=center> @* </td>
$data[14]
<td align=right rowspan=1> @###.# </td><td align=right rowspan=1> @###.# </td>
$data[16],$data[17]
<td rowspan=1 align=center> @* </td>
$data[12]
</tr>
.

format OUT1 =
<tr valign=top>
<td rowspan=2>@< </td>
defined($files{$data[0]})?"+":"-"
<td rowspan=2><INPUT TYPE=checkbox DISABLED @<<<<<<<<< VALUE=@<<<<<></td>
$data[6]==1?"checked":" ",$data[0]
<td bgcolor=@* rowspan=2> @* </td>
$qcolors{$data[8]},$data[8]
<td rowspan=2><a href="show_run.cgi?@<<<<<" onClick="open_runs_view(href)" target="runs_view_window"> @<<<<<< </a></td>
$data[0],$data[0]
<td rowspan=2> @<<<<<< </td><td rowspan=1> @<<<<<<< </td>
$data[1],$data[2]
<td align=right rowspan=2> @<<<<<< </td>
$data[4]
<td align=right rowspan=2> @####.## </td>
$data[7]
<td rowspan=2> @* </td>
$data[9]
<td align=center rowspan=1> @* </td>
$data[10]
<td align=center rowspan=1> @* </td>
$data[13]
<td colspan=2 rowspan=1 align=center> @* </td>
$data[15]
<td rowspan=1 align=center> @* </td>
$data[18]
<td rowspan=2> @* </td>
$data[5]
</td>
</tr>
<tr>
<td rowspan=1> @<<<<<<< </td>
$data[3]
<td rowspan=1 nowrap align=center> @* </td>
$data[11]
<td rowspan=1 nowrap align=center> @* </td>
$data[14]
<td align=right rowspan=1> @###.# </td><td align=right rowspan=1> @###.# </td>
$data[16],$data[17]
<td rowspan=1 align=center> @* </td>
$data[12]
</tr>
.


sub printHeader() {
print <<EOF;
Content-type: text/html

<HTML>
<HEAD>
<TITLE>2001 Run Log</TITLE>
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
    var newurl = "http://"+w.location.host+"/cgi-bin/mulan/update_run.cgi?";
    newurl += run+"+"+field+"+"+value;
    update_w = open(newurl,"update_status_window",
		    "toolbar=0,location=0,directories=0,status=0,"+
		    "menubar=0,width=400,height=100,resizable=1,scrollbars=0");
}

</SCRIPT>

<TABLE ALIGN=center WIDTH="80%">
<tr>
<td align=left width=200></td>
<td align=center><H2>Fall2005 Run Log</H2></td>
<td align=right width=200>$linkedit</td>
</tr>
</TABLE>

<P>
Legend: 
<ul>
<li>P - "+" if this is "production run" (as opposed to "test run")
<li>H - "+" if file with online histograms exists
<li>Q - quality mark (Y - good, N - bad, C - cosmic, T - test)
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

sub askForPassword() {
    print header;
    print start_html("Database password");
    print start_form;
    print table(
		Tr( 
		    th(['Database username: ',
			textfield('User',"",10,10),
			"Password: ",
			password_field("Password","",10,20),
			submit('Submit')]
		       )
		    )
		);
    print end_form;
    print end_html;
    exit 0;
}
