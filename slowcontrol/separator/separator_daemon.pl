#!/usr/bin/perl

    $separator="mpc1361.psi.ch";
#    $separator="sep2.psi.ch";
#    $separator="sepii.psi.ch";
    $outport=10000;
    $inport=10001;
    $outfile="/tmp/separator.txt";
#    $debug=0;
    $debug=1;
    
while(1){
#open listener before sending
#    open(NC_IN ,"nc -l -p $inport |");
    
#send request
    open(NC_IN,"echo *RALL | nc -i 5 $separator $outport |");
    if ($debug) {print "Sending request...\n";}
    #print NC_OUT "*RALL\0";
    if ($debug) {print "Sent request\n";}
    #close(NC_OUT);
    
#recieve response
    if ($debug) {print "reading response...\n";}
    @nc_in=<NC_IN>;
    if ($debug) {print "response:\n@nc_in\n"};
    close(NC_IN);
    
#parse the input
    ($dummy,$dummy,$HV,$dummy,$current)=split(/ /,$nc_in[1]);
    chomp($current);
    ($dummy,$VAC)=split(/ /,$nc_in[3]);
    chomp($VAC); # VAC ends in \r\n, unlike the other lines
    $VAC =~ s/\r//g; # chomp strips the \n, this line strips the \r
    $STATUS=$nc_in[5];
    
    if ($STATUS =~ "OK"){
	$STATUS_INT=0.0;
    }else{
	$STATUS_INT=-1.0;
    }
    
    open(IN,"date +%Y%m%d%H%M%S |");
    $date=<IN>;
    close(IN);
    chop $date;
    if ($debug) {print $date."\n";}
    
    if ($debug) {print "date=".$date."\n"."HV=".$HV."\n"."current=".$current."\n"."VAC=".$VAC."\n"."STATUS_INT=".$STATUS_INT."\n";}
    open(OUTFILE,">$outfile");
    #print $date." ".$HV." ".$current." ".$VAC." ".$STATUS_INT."\n";
    print OUTFILE $date."\n";
    print OUTFILE $HV."\n";
    print OUTFILE $current."\n";
    print OUTFILE $VAC."\n";
    print OUTFILE $STATUS_INT."\n";
    close(OUTFILE);
    
    #sleep 10;
}
