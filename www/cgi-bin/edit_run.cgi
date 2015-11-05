#!/usr/bin/perl

require "ctime.pl";
require "flush.pl";
use DBI;
use CGI qw(:standard *table);

$| = 1;
$dsn = "DBI:mysql;database=alcap;host=abe.psi.ch";
$user = "webguest";

my $run;
if( $#ARGV<0 ) { exit(0); } else { $run=$ARGV[0]; }

my %items = (
	     'starttime'=>{ 'name'=> "Started" },
	     'stoptime'=>{ 'name'=> "Finished" },
	     'shift'=>{ 'name'=> "Shift" },
	     'nblock'=>{ 'name'=> "Blocks" },
	     'comment'=>{ 'name'=> "<b>Comment</b>" },
	     'size'=>{ 'name'=> "Size" },
	     'quality'=>{ 'name'=> "Quality (Y/N/T)" },
	     'muon_polarity'=>{ 'name'=> "Muon Polarity" },
	     'beam_momentum_scale'=>{ 'name'=> "Beam momentum scale" },
	     'target'=>{ 'name'=> "Target" },
	     'tape'=>{ 'name'=> "Tape" },
	     'tapesize' => { 'name' => "Size on the tape" }
	     );

my $inipass = cookie('dbpass');
my $mycook = undef;

if( defined(param('password')) && param('Cancel') ne 'Cancel' ) {
    my $pass1 = param('password');
    $mycook = cookie(-name=>"dbpass",-value=>$pass1,-expires=>"+300m",
		     -path=>"/cgi-bin/alcap");
}

if( defined($mycook) ) {
    print header(-cookie=>$mycook);
} else {
    print header();
}
print start_html('Run Info');

if( defined(param('password')) && param('Cancel') ne 'Cancel' ) {
    my $pass1 = param('password');
    if( ( $dbh1 = DBI->connect($dsn, "mucap", $pass1) ) ) {
  	my @values = ();
  	my $query = "update runinfo set";
	my $first = 1;
  	for $par ( keys %items ) {
  	    if( defined(param($par)) ) { 
		if( $first == 1 ) {
		    $query .= " $par = ? ";
		    $first = 0;
		} else {
		    $query .= " , $par = ? ";
		}
  		push @values,param($par);
  	    }
  	}
  	$query .= "where run=$run";
  	if( ( $ret = $dbh1->do($query,undef,@values) ) ) {
	    print h2("Information has been updated"),hr;
	} else { 
	    print h2("Error while updating database");
	    print "Query: ",$query,p;
	    print "Values: ",join(" ",@values),p,hr;
	}
	$dbh1->disconnect;
    } else {
	print h2("Cannot connect database, probably the wrong password");
	print start_form;
	print submit('Cancel','Return');
	print end_form;
	print end_html;
	goto out;
    }
}

my $dbh = DBI->connect($dsn, $user);
my $req = "select run";
my $id = 0;
foreach my $p ( keys %items ) {
    $id++;
    $items{$p}->{'id'} = $id;
    $req .= ",".$p;
}
$req .= " from runinfo where run=?";
my $sth = $dbh->prepare($req);
$sth->execute($run);
my @data = $sth->fetchrow_array();
$sth->finish;
$dbh->disconnect;

if( ! defined(@data) ) {
    print h1("No info for run $run"),
    end_html;
    goto out;
}

if( defined(param('Update')) ) {
    my @changed;
    for $par ( keys %items ) {
	if( defined(param($par)) ) { 
	    if( param($par) ne $data[$items{$par}->{'id'}] ) {
		push @changed, $par;
	    }
	}
    }
    if( $#changed<0 ) {
	print h2("No information has been changed");
	print start_form;
	print submit('Cancel','Return');
	print end_form;
    } else {
	print h2("The following information will be updated:");
	print start_table({-border=>0});
	for $par ( @changed ) {
	    print Tr({-valign=>'top'},
		     th([b($items{$par}->{"name"}." : "),
			 $data[$items{$par}->{'id'}],
			 " -> ",
			 param($par)]));
	}
	print end_table;
	print h3("If ok, please enter the password below and click Ok, otherwise click Cancel");
	print start_form;
	for $par ( @changed ) {
	    print hidden($par,param($par));
	}
	print table({-cellspacing=>10},
		    Tr({-valign=>top},
		       th(password_field('password',$inipass)),
		       th(submit('Ok','Ok')),
		       th(submit('Cancel','Cancel'))
		       )
		    );
	print end_form;
    }
} else {
    print start_form;
    print table({-cellspacing=>10},
		Tr({-valign=>top},
		   th({-width=>'70%',-align=>'left'},h2("Run $run")),
		   th(submit('Update','Update')),
		   th(defaults('Reset'))
		   )
		);
    print table({-cellspacing=>10},
		Tr(
		   th(my_textfield("starttime",20,20)),
		   th(my_textfield("stoptime",20,20)),
		   th(my_textfield("shift",20,20))
		   )
		);
    print table({-cellspacing=>10},
		Tr(
		   th(my_textfield("quality",1,1)),
		   th(my_textfield("condition",10,10)),
		   th(my_textfield("nevent",5,5)),
		   th(my_textfield("size",10,10)),
		   )
		);
    print table({-cellspacing=>10},
		Tr(
		   th(my_textfield("tape",10,20)),
		   th(my_textfield("tapesize",10,10))
		   )
		);
    print table({-cellspacing=>10},
		Tr(
		   th(my_textfield("tpc_hv",20,20)),
		   th(my_textfield("mupc1_hv",20,20)),
                  )
                );
    print table({-cellspacing=>10},
		Tr(
		   th(my_textfield("epc1_hv",20,20)),
		   th(my_textfield("epc2_hv",6,6)),
		   )
		);
    print table({-cellspacing=>10},
		Tr(
		   th(my_textfield("target",10,10)),
		   th(my_textfield("magnet",20,20))
		   )
		);
    print table({-cellspacing=>10},
		Tr(
		   th(my_textfield("kicker",5,5)),
		   th(my_textfield("kicker_hv_top",6,6)),
		   th(my_textfield("kicker_hv_bottom",6,6))
		   )
		);
    print table({-cellspacing=>10},
		Tr(
		   th(my_textfield("beam_pscale",6,6)),
		   th(my_textfield("slit3",20,20)), 
		   th(my_textfield("ft_mode",30,30)), 
		   )
		);
    print table({-cellspacing=>10},
		Tr({-valign=>"top"},
		   th(["Comment",my_textarea("comment",5,50,"")])
		   )
		);
    print end_form;
}

print end_html;

out:
exit(0);

sub my_textfield() {
    my ( $name, $p1, $p2, $title ) = @_;
    if( !defined($title) ) { $title=$items{$name}->{'name'}; }
    return $title." ".
	textfield($name,$data[$items{$name}->{'id'}],$p1,$p2);
}

sub my_textarea() {
    my ( $name, $p1, $p2, $title ) = @_;
    if( !defined($title) ) { $title=$items{$name}->{'name'}; }
    return $title." ".
	textarea($name,$data[$items{$name}->{'id'}],$p1,$p2);
}
