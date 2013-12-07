#!/usr/bin/perl

use DBI;
use CGI qw(:standard);

my $dsn = "DBI:mysql:online;localhost";
my $user = "guest";
my $dbh = DBI->connect($dsn, $user);

#my $url = "http://mulantb05.psi.ch/cgi-bin/mulan/zipcode.cgi";
my $url = url(-full=>1);

my ( $sn, $zip, $crate, $fe, $slot, $chan, $run );

my $what = $ARGV[0];

my $str = join(":",@ARGV);

if( $what eq "s" ) {

    $sn = $ARGV[1];
    $run = (defined($ARGV[2]))?$ARGV[2]:30000;
    getZipBySn();

    $sth = $dbh->prepare("select crate,slot,ch,fe from wfdmap left join wfdmap_crate using (idcrate) ".
			 "left join wfdmap_conf using (idconf) where run0<? and run1>? and zipcode=?");
    $sth->execute($run,$run,$zip);
    ($crate,$slot,$chan,$fe) = $sth->fetchrow_array();
    $sth->finish;

} elsif( $what eq "z" ) {

    $zip = $ARGV[1];
    $run = (defined($ARGV[2]))?$ARGV[2]:30000;
    getSnByZip();

    $sth = $dbh->prepare("select crate,slot,ch,fe from wfdmap left join wfdmap_crate using (idcrate) ".
			 "left join wfdmap_conf using (idconf) where run0<? and run1>? and zipcode=?");
    $sth->execute($run,$run,$zip);
    ($crate,$slot,$chan,$fe) = $sth->fetchrow_array();
    $sth->finish;

} elsif( $what eq "c" ) {

    ( $slot, $chan, $crate ) = ( $ARGV[1], $ARGV[2], $ARGV[3] ) ;
    $run = (defined($ARGV[4]))?$ARGV[4]:30000;

    $sth = $dbh->prepare("select zipcode,fe from wfdmap left join wfdmap_crate using (idcrate) ".
			 "left join wfdmap_conf using (idconf) where ? between run0 and run1 and slot=? and ch=? and crate=?");
    $sth->execute($run,$slot,$chan,$crate);
    ($zip,$fe) = $sth->fetchrow_array();
    $sth->finish;
    getSnByZip();

} elsif( $what eq "f" ) {

    ( $slot, $chan, $fe ) = ( $ARGV[1], $ARGV[2], $ARGV[3] ) ;
    $run = (defined($ARGV[4]))?$ARGV[4]:30000;

    $sth = $dbh->prepare("select zipcode,crate from wfdmap left join wfdmap_crate using (idcrate) ".
			 "left join wfdmap_conf using (idconf) where ? between run0 and run1 and slot=? and ch=? and fe=?");
    $sth->execute($run,$slot,$chan,$fe);
    ($zip,$crate) = $sth->fetchrow_array();
    $sth->finish;
    getSnByZip();

} else {
    
    $run = 30000;

}

printPage();

############################################
sub getZipBySn() {
    my $base=6;
    if( $sn<=100 ) { $base=5; }
    my $house=int(($sn-($base*100-499))/(2*$base))+1;
    my $element=($sn-($base*100-499))%(2*$base)+1;
    $zip=$base*10000+$house*100+$element;
    if($zip<50101) { $zip= -1; }
    if($zip>62020) { $zip= -2; }
    if( $sn<1 ) { $zip=-1; }
}

sub getSnByZip() {
    if($zip==10000) {
	$sn=341;
    } elsif ($zip==10001) {
	$sn=342;
    } elsif ($zip==20000) {
	$sn=343;
    } else {
	$houseType = int($zip/10000);
	$houseNum = int(($zip-$houseType*10000)/100);
	$tileNum = $zip - 10000*$houseType - 100*$houseNum;
	$sn = ($houseNum-1)*2*$houseType+$tileNum+100*($houseType-5);
	if($tileNum > 2*$houseType || $tileNum<1) {
	    $sn = -3;
	} elsif($sn<1) {
	    $sn = -1;
	} elsif($sn>340) {
	    $sn = -2;
	}
    }
}

########################################################################

sub printPage() {
print <<EOF
Content-type: text/html

<HTML>
<HEAD>
<TITLE>Zipcode calculator</TITLE>
</HEAD>
<BODY bgcolor="#ffffff" text="#880000" link="#0000ff" vlink="#0000ff">

<SCRIPT LANGUAGE="JavaScript">
    function what(w,action) {
	sn    = w.document.forms[0].elements[0].value; 
	zip   = w.document.forms[0].elements[1].value; 
	crate = w.document.forms[0].elements[2].value; 
	slot  = w.document.forms[0].elements[3].value; 
	chan  = w.document.forms[0].elements[4].value; 
	fe    = w.document.forms[0].elements[5].value; 
	run   = w.document.forms[0].elements[6].value; 
	switch (action) {
	    case 1:
	    if( sn!=null && sn!="" ) {
		url = "$url"+"?s+"+w.document.forms[0].elements[0].value;
		if( run!=null ) url=url+"+"+run;
		w.location = url;
	    }
	    break;
	    case 2:
	    if( zip!=null && zip!="" ) {
		url = "$url"+"?z+"+zip;
		if( run!=null ) url=url+"+"+run;
		w.location = url;
	    }
	    break;
	    case 3:
	    if( slot==null || slot=="" ) break;
	    if( chan==null || chan=="" ) break;
	    if( crate==null || crate=="" ) {
		if( fe==null || fe=="" ) break;
		url = "$url"+"?f+"+slot+"+"+chan+"+"+fe;
		if( run!=null ) url=url+"+"+run;
		w.location = url;
	    } else {
		url = "$url"+"?c+"+slot+"+"+chan+"+"+crate;
		if( run!=null ) url=url+"+"+run;
		w.location = url;
	    }
	    break;
	    case 4:
	    if( slot==null || slot=="" ) break;
	    if( chan==null || chan=="" ) break;
	    if( crate==null || crate=="" ) break;
	    url = "$url"+"?c+"+slot+"+"+chan+"+"+crate;
	    if( run!=null ) url=url+"+"+run;
	    w.location = url;
	    break;
	    case 5:
	    if( slot==null || slot=="" ) break;
	    if( chan==null || chan=="" ) break;
	    if( fe==null || fe=="" ) break;
	    url = "$url"+"?f+"+slot+"+"+chan+"+"+fe;
	    if( run!=null ) url=url+"+"+run;
	    w.location = url;
	    break;
	  default:
	}
    }
</SCRIPT>

<form>
<table border=0>
<tr>
<th>Serial number</th>
<td><input type="text" name="sn" value="$sn" onChange="what(window,1)" size=5></td>
</tr>
<tr>
<th>Zip code</th>
<td><input type="text" name="zip" value="$zip" onChange="what(window,2)" size=5></td>
</tr>
<tr>
<th>Crate</th>
<td><input type="text" name="crate" value="$crate" onChange="what(window,4)" size=5></td>
</tr>
<tr>
<th>Slot</th>
<td><input type="text" name="slot" value="$slot" onChange="what(window,3)" size=5></td>
</tr>
<tr>
<th>Channel</th>
<td><input type="text" name="chan" value="$chan" onChange="what(window,3)" size=5></td>
</tr>
<tr>
<th>FE</th>
<td><input type="text" name="fe" value="$fe" onChange="what(window,5)" size=5></td>
</tr>
<tr>
<th>Run</th>
<td><input type="text" name="run" value="$run" onChange="what(window,7)" size=5></td>
</tr>
</table>
</form>

</body>
</html>
EOF
;
}
