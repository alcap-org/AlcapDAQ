#!/usr/bin/perl

require "ctime.pl";
require "flush.pl";
use DBI;
use CGI qw(:standard *table);

$| = 1;
$dsn = "DBI:mysql:mucap;dolphin";
$user = "webguest";

# if( $#ARGV!=2 ) { exit(0); }

my $inipass = cookie('dbpass');
my $mycook = undef;

if( defined(param('password')) ) {
    my $pass1 = param('password');
    $mycook = cookie(-name=>"dbpass",-value=>$pass1,-expires=>"+10m",
		     -path=>"/cgi-bin/mucap");
    $inipass = $pass1;
}

if( defined($mycook) ) {
    print header(-cookie=>$mycook);
} else {
    print header();
}

print start_html('Update run selection');

my @addrun = split(":",$ARGV[0]); shift @addrun;
my @delrun = split(":",$ARGV[1]); shift @delrun;
my @comrun = split(":",$ARGV[2]); shift @comrun;

my $dbh;

if( defined(param('password')) ) {
    if( ! ($dbh = DBI->connect($dsn,$user,$inipass)) ) {
	print h3({-text=>"#008800"},"Cannot connect to database. Try other password."),hr;
	ListOfRuns();
	MakeForm();
    } else {
	ChangeDB();
	UpdateAndClose();
    }
} else {
    ListOfRuns();
    MakeForm();
}

print end_html;
exit(0);

sub ListOfRuns() {
    if( $#addrun>-1 ) {
	print h3("Runs added to the list:"),join(" ",sort @addrun),p;
    }
    if( $#delrun>-1 ) {
	print h3("Runs deleted from the list:"),join(" ",sort @delrun),p;
    }
    if( $#comrun>-1 ) {
	print h3("Runs with updated comment:"),join(" ",sort @comrun),p;
    }
}

sub MakeForm() {
    print h3("If everything is correct, please enter the password and click Ok, or click Cancel to exit");
    print start_form;
    print table({-cellspacing=>10},
		Tr({-valign=>top},
		   th(password_field('password',$inipass)),
		   th(submit('Ok','Ok')),
		   th(button('Cancel','Cancel',"self.close()"))
		   )
		);
    print end_form;
}

sub ChangeDB() {
    my $run;
    if( $#addrun>-1 ) {
	my %exist = ();
	my $sth = $dbh->prepare("select run from runflags9 where run in (".join(",",@addrun).")");
	$sth->execute();
	while ( ( $run ) = $sth->fetchrow_array() ) { $exist{$run}=1; }
	my @create;
	for $run ( @addrun ) { if( ! defined($exist{$run}) ) { push @create,$run; } }
	if( $#create>-1 ) {
	    $dbh->do("replace runflags9 ( run ) values (".join("),(",@create).")");
	}
    }
    if( $#addrun>-1 ) {
	$dbh->do("update runflags9 set production=1 where run in (".join(",",@addrun).")");
    }
    if( $#delrun>-1 ) {
	$dbh->do("update runflags9 set production=0 where run in (".join(",",@delrun).")");
    }
}

sub UpdateAndClose() {
print <<EOF;
<script LANGUAGE="JavaScript">
opener.location.reload();
close();
</script>
EOF
}
