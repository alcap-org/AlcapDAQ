#!/bin/sh
#-*-tcl-*-
# the next line restarts using wish  \
exec /bin/nice bltwish "$0" -- ${1+"$@"}

#=========================================================================
# Gertjan Hofman, U. of Colorado, Boulder, March-00.
# 
# tcl program to replace the gstripchart thingy that comes with
# gnome
#
#
# requires:  tcl + tk + BLT = bltwish
#
# v 1.0  Mar - 00 for PAA/Triumf/Midas
#
# There are a few tricky things going on here.
# Since passing TCL lists to the graph seems to have a memory leak,
# I use BLT vectors, which are hot linked to the graph elements.
# Since I dont know a priori how many vectors to create, I the name
# of the item that I am plotting as the vector name, preceded by
# V_x_ or V_y_.
# To access an individual data point, which I need for the scrolling
# now have to do a double de-refencing. Say we create a vector
# vector create my_vect(10)
# then I can access the vector using   puts $my_vect(3)  
# but if I have
# set name my_vect
# vector create $x
# Now I need to translate x--> my_vect --> its value.
# you can do this in two ways:
# puts [set [set x](0)]  or  puts [ set ${x}(0)] or even
# puts [set $x\(0)].  In the first case, the inner 'set' returns
# my_vect. This is then appended to (0) and the second set returns
# the result. In the last case, the \ 'escapes' or seperates the (0)
# as not being part of the name. etc etc. Think about it.
#
# v. 1.0  -added help menu, added smarter scaling of of the expanded
#          graphs.
# v  1.1  -added mouse select of graph items
# v. 2.0  -added support for mhist (odb history)
# v. 2.1  -added postscript support + zooming of single graph windows
# v  3.0  -added scroll bars for variable selections
#         -corrected dd.mm.yy to yy.mm.dd for Stefan.
# things to do:
# - improve help/info menu's
# - trap errors ! 
# - check compatibility with gchart, esp. reading of fields
# - what about simple arithmetic ? (a la gchart).
# - think of some cool back grounds (chaos.../midas etc)
#
# 12.10.2000
# updates:
# 1. Search for most recent mhist executable and use that
# 2. Change history file menu's slightly
# 3. option -mhist is no longer necessary - default

# 17.10.2000
# fixed bug in online mode - string comparison was wrong
# re-ordered some code (no functional change)
#  Revision History:
#    $Log: stripchart.tcl,v $
#    Revision 1.1.1.1  2005/06/20 23:37:39  mucap
#    Importing release 1.9.5 of the MIDAS source code as distributed by PSI.
#    (Next, I'll commit our local customizations.)
#
#    Revision 1.15  2002/08/26 22:12:40  midas
#    Handle data that are arrays - queery MHIST for each element of the
#    array and pretend they are different data words. Requested by TWIST.
#    Fixed minor bugs wrt opening correct directories.
#
#    Revision 1.14  2002/06/15 00:32:10  midas
#    Fix problem with having EPICS like ':' in variable name
#
#    Revision 1.8  2001/12/08 01:05:43  pierre
#    correct path, new display
#
#    Revision 1.7  2000/10/20 19:07:50  pierre
#    - improve online/mhist toggle. (GH)
#    - added mhist path search. (GH)
#    - cleanup code. (GH)
#
#    Revision 1.6  2000/10/05 17:43:17  pierre
#    - add scroll bar instead of radio button.
#    - cosmetics
#
#    Revision 1.3  2000/04/20 17:06:50  pierre
#    # ODB or history stripchart in tcl. Requires bltwhish. (New application)
#
#
# 29.11.01
# Updates - discovered minor bugs in decoding time format for the year 2001
#         - minor bug in exec statements (was appending new items instead
#                                         of overwritting)
#         -* found BLT problesm - things that worked before dont with
#         RH72 distr - nameles  vector x(++end) $data  kind of statements
#         Had to use  vector append  instead.
# This is version 2.0
# 07.12.01
# Added file path input box.
# Added busy signal - changing colour of the screen

# Added cross hairs
# Added buttons to scan through the expanded/single graphs
# Allow resizing of expanded/single graphs

# May-2002. Cosmetic changes. 
#           Allow Manual entry of y-scale settings

#========================================================================


namespace import blt::*     ;# get BLT commands imported

#============================================================
# SHOW MESSAGE   - pop up message window and display error
#============================================================

proc show_message {message} {
    global message_done                      ;# indicate whether message has been read
    toplevel .messages                       ; # create a new window
    wm title .messages "stripchart warning"
    label    .messages.1   -text  "$message"
    pack     .messages.1   -side  top
    button   .messages.ok  -text  "OK" -command {
	destroy .messages 
	set message_done 1                  
	return}
    pack     .messages.ok  -side  bottom 
    update
}


#=============================================================
# Convert clock seconds to HH:MM:SS format
#=============================================================
# this function converts a 'x' axis value to a time format according to
# a format statement passed to it.
proc my_clock_format {format  graph ltime} {
   return "[clock format  [expr round( ($ltime/60.)*60)] -format $format]"
}


#=============================================================
# EXIT procedure
#=============================================================
proc nice_exit {} {
    exit
return
}
#=============================================================
# FIND LATEST MHIST VERSION -by checking modificationd dates
#=============================================================
proc find_newest_mhist {} {
    
    global mhist_path

    # search for the latest copy of mhist and store that path
    set mhistlist "/usr/local/bin /usr/bin . /midas/linux/bin/"
    set minmod 0
    foreach file  $mhistlist {
	# get the modification time
	if [file exist $file/mhist] { 
	    if {[file mtime $file/mhist] > $minmod } {
		set minmod [file mtime $file/mhist]
		set mhist_path $file
	    }
	}
    }
    
    if {$mhist_path==""} {
	tk_messageBox -message "Error: Can not find ANY mhist in \n
	$mhist_list \n"
	return 0
    } else {
	wm title  . "using mhist from path:  $mhist_path/mhist"
	return 1
    }


}
#==============================================================
# READ THE MCHART CONFIGURATION FILE
#==============================================================  
proc read_conf_file {conf_fname} {

    global item_fname item_fields item_pattern item_equation
    global item_color item_max item_min item_list
    global equip_name

    # open the file, get ptr. 
    set file_hndl [open $conf_fname r]
    
    while   {! [eof $file_hndl]} {
	
	gets $file_hndl string
	
	# skip over comments ...except catch Pierre's equipment name.
	if {[string first "#Equipment:" $string] != -1 } {
	    set equip_name [lindex $string 1]
	}
	
	if {[string index $string 0]=="#" || [llength $string]==0} {
	    continue
	}
	set string [string tolower $string]           ;# convert to lower case

	#disect string to obtain info. Look for begin line.
	# note: string first return location of first occurance -1 if not found
	if { [string first "begin:" $string] != -1} {
	    
	    # now fetch the second item - the value name
	    
	    set item [lindex $string 1]             ;# assume its the second item
	    
	    set item [filter_bad_chars $item]
	    
	    lappend item_list $item                 ;# create a list of items for looping

	    set item_color($item) [get_new_color]
	    #		    "color:"    { set item_color($item)    [lindex $string 1] }
	    # ok we got a begin. Check for the usefil items
	    while { [string first "end:" $string] == -1 } {
		gets $file_hndl string
		switch -glob -- [lindex $string 0] {      
		    "filename:" { set item_fname($item)    [lindex $string 1] }
		    "fields:"   { set item_fields($item)   [lindex $string 1] }
		    "pattern:"  { set item_pattern($item)  [lindex $string 1] }
		    "equation:" { set item_equation($item) [lindex $string 1] }
		    "maximum:"  { set item_max($item)      [lindex $string 1] }
		    "minimum:"  { set item_min($item)      [lindex $string 1] }
		    "begin:"    {
			show_message  "Error in .conf file -  begin but no previous end "
			after 5000
			exit  
		    }
		}
	    } 
	}
    }
    
    close $file_hndl


# do some checks on the max/min values, in case Pierre screwed them up
    foreach item $item_list {
	if { $item_max($item)==0 && $item_min($item)==0} {
	    puts "found zero max/min values for $item. Correcting"
	    set item_max($item) 1
	}
    }
    
    
    return
}


#======================================================================
# GRAPH INDIVIDUAL ITEM ON FULL SCALE
#======================================================================

proc select_graph {item} {
# here we show the REAL data, unscaled, on a seperate graph
# note: we want to be able to crack open multiple graphs
# so the names of the widget need to be a variable.
# can't remember how to make static variables, so make it
# global

#    global V_x_$item                ;# vectors associated with the plotte item
#    global V_y_$item
    global item_color               ;# colour associated with item
    global scale_ymin scale_ymax    ;# present best y-scale
    global winsize_x winsize_y      ;# size of toplevel and graph windows
    global doing_mhist

# check if window exist already - dont replot the same item twice
    if [winfo exists .fullscale$item ] {
	wm deiconify .fullscale$item          ;# de-iconize it it
	raise        .fullscale$item          ;# raise to foreground
	return
    }

    toplevel .fullscale$item   ;# create new window with unique name
    wm title .fullscale$item "$item"

# split screen in two parts -
    frame .fullscale$item.col1 ;# create to columes - rhs for graph
    frame .fullscale$item.col2 ;# left hand side for buttons.
    pack .fullscale$item.col1 -side left
    pack .fullscale$item.col2 -side right -fill both -expand 1

    set fgraph .fullscale$item.col2.graph

    graph $fgraph -title "" -relief ridge -bd 3 -background "#c8f8ce"  ;# make a new graph
    #cross hairs - why doesnt $fgraph crosshair on work ?
    Blt_Crosshairs  $fgraph
    $fgraph configure -width 5.0i -height 2.0i      ;# configure it a little 
    $fgraph legend configure -position @80,8  -anchor nw  -relief raised 

    pack $fgraph -side right -fill both -expand 1

    # now, autoscaling may not be the best. Lets try use the standard
    # deviation for the data points that we have stored right now.
    calc_best_scale $item   ;# returns calc values, or "" if not enough data

    $fgraph yaxis configure -max $scale_ymax -min $scale_ymin

    # add day of the week if using the history command
    if {$doing_mhist} {
	$fgraph xaxis configure -loose 1 -title "" -command {my_clock_format "%d/%m %H:%M"}
    } else {
	$fgraph xaxis configure -loose 1 -title "" -command {my_clock_format %H:%M}
    }
	
    $fgraph element create line 
    $fgraph element configure line  \
	    -label $item -color $item_color($item) -symbol "" \
	    -xdata V_x_$item -ydata V_y_$item

    # puts "data are now [set V_y_[set item](:)]"
    # create  exit button  
    button   .fullscale$item.col1.ok  -text  "close"  -relief flat -underline 0 -bd 0 \
	    -width 6 -command "destroy .fullscale$item  ;  return"
    pack     .fullscale$item.col1.ok  -side bottom

    # create re-scaling button
    
#    button   .fullscale$item.col1.scale1  -text  "AutoScale"   -relief flat -underline 0 -bd 0 \
#	    -width 6 -font 6x12 -command "scale_single_window Auto $item" 
#    pack     .fullscale$item.col1.scale1  -side top
#    button   .fullscale$item.col1.scale2  -text  "ReScale" -relief flat -underline 0 -bd 0 \
#	    -font 6x12 -width 6 -command "scale_single_window Rescale $item"
#    pack     .fullscale$item.col1.scale2  -side top   


    # trial code - scale menu:
    menubutton   .fullscale$item.col1.scale_gen -text "Scaling" -menu .fullscale$item.col1.scale_gen.mnu \
	    -width 6 
    menu  .fullscale$item.col1.scale_gen.mnu
    .fullscale$item.col1.scale_gen.mnu add command -command "scale_single_window Manual $item 1" \
	    -label "Manual Scale"
    .fullscale$item.col1.scale_gen.mnu add command -command "scale_single_window Rescale $item 1" \
	    -label "AutoScale 1"
    .fullscale$item.col1.scale_gen.mnu add command -command "scale_single_window Auto $item 1" \
	    -label "AutoScale 2"
    pack   .fullscale$item.col1.scale_gen -side top
    # end of trial code

    # create information button
    button   .fullscale$item.col1.info  -text  "Info/Help"   -relief flat -underline 0 -bd 0 \
	    -width 6  -command "show_item_info $item" 
    pack     .fullscale$item.col1.info  -side top

    # create hard copy pull down menu:
    menubutton .fullscale$item.col1.hard -text "HardCopy"  -relief flat -underline 0 -bd 0 \
	    -menu .fullscale$item.col1.hard.mnu -relief raised  

    set hardcopy_menu [ menu  .fullscale$item.col1.hard.mnu]

    $hardcopy_menu add command -command "hardcopy ps  $fgraph" -label "ps  file"
    $hardcopy_menu add command -command "hardcopy jpg $fgraph" -label "jpg file"
    $hardcopy_menu add command -command "hardcopy gif $fgraph" -label "gif file"
    $hardcopy_menu add command -command "hardcopy png $fgraph" -label "png file"

    pack  .fullscale$item.col1.hard
    # bindings for the zooming function.
    bind $fgraph <ButtonPress-1> "zoom_select %W %x %y start"
    bind $fgraph <ButtonRelease-1> "zoom_select %W %x %y stop"


    return
}




#======================================================================
# GRAPH ALL ON FULL SCALE (AS ABOVE), BUT PACKED TOGETHER
#======================================================================

proc show_all_full_scale { } {

    global item_list                ;# list of all items to plot
    global item_color               ;# colour associated with item
    global scale_ymin scale_ymax    ;# present best y-scale
    global winsize_x winsize_y      ;# size of toplevel and graph windows
    global doing_mhist

    global display_item_cnt         ;# pointer to start of 5 items to display
    global debug_code
    global item_counter_text        ;# copy of display_item but for label 
    global fix_y_scale              ;# whether or not mouse zoom changes y-scale
    
    
    # check if window exist already - if so, destroy the graphs on it.
    if { [winfo exists .fullscale_main ] } {
	foreach item  $item_list {
	    set full_name  .fullscale_main.fullscale$item
	    if {[winfo exists $full_name]} { destroy $full_name}
	}
    } else {
	toplevel .fullscale_main
	wm title .fullscale_main "zoom using left-mouse-drag"
	wm geometry .fullscale_main +[winfo rootx .]+[winfo rooty .]
    }

    #if we have more than 4 items provide  skip buttons
    if { [llength $item_list] > 4} {
	if {![winfo exist .fullscale_main.row1]} {
	    if {$debug_code} { puts "Creating top row << >> "}
	    frame .fullscale_main.row1 -relief ridge -bd 3 -height 0.3i
	    
	    button .fullscale_main.row1.prev1 -text "< " -command {
		incr display_item_cnt -4
		show_all_full_scale 
	    } 
	    button .fullscale_main.row1.next1 -text "> " -command {
		incr display_item_cnt 4
		show_all_full_scale 
	    }
	    button .fullscale_main.row1.prev2 -text "<<" -command {
		incr display_item_cnt -8
		show_all_full_scale 
	    } 
	    button .fullscale_main.row1.next2 -text ">>" -command {
		incr display_item_cnt 8
		show_all_full_scale 
	    }
	    button .fullscale_main.row1.quit -text "exit" -command {destroy .fullscale_main}
	    label  .fullscale_main.row1.number -textvariable item_counter_text
	    
	    button   .fullscale_main.row1.fixy    -text  "Freeze Y scale" \
		    -command {
		set fix_y_scale [expr !$fix_y_scale]  ; # invert toggle
		if {$fix_y_scale} {
		    .fullscale_main.row1.fixy  configure -bg red -activebackground red2 
		} else {
		    .fullscale_main.row1.fixy  configure -bg grey -activebackground "#d6d8d6"
		}
	    }
	    
	    pack    .fullscale_main.row1.fixy  -side right  



	    pack .fullscale_main.row1.prev2 -side left
	    pack .fullscale_main.row1.prev1 -side left
	    pack .fullscale_main.row1.quit -side left
	    pack .fullscale_main.row1.number -side right -fill x -padx 20
	    pack .fullscale_main.row1.next2 -side right
	    pack .fullscale_main.row1.next1 -side right

	    pack .fullscale_main.row1 -side top -expand 1
	    
	}
    }
  
    # make sure we dont over run the list
    if { $display_item_cnt < 0 } { set display_item_cnt 0}
    if { $display_item_cnt > [expr [llength $item_list]-4] } { 
	set display_item_cnt [expr [llength $item_list] -4 ]  
    }


    
    if { $debug_code } {
	puts "Debug: item to display list counter is $display_item_cnt"
	puts "Debug: item list length is [llength $item_list] "
    }
    
    set last_item  [expr $display_item_cnt + 3]
    set n_items 0

    # for display only:
    set item_counter_text "[expr $display_item_cnt+1] to [expr $last_item+1] out of [llength $item_list]"

    foreach item  [lrange $item_list $display_item_cnt $last_item] {
    
	# only plot first 4.
	incr n_items
	if {$n_items>4}  { break }

	set full_name  .fullscale_main.fullscale$item
	
	frame $full_name 
	
	# split screen in two parts -
	frame $full_name.col1       ;# create two columes - rhs for graph
	frame $full_name.col2       ;# left hand side for buttons.
	pack  $full_name.col1 -side left 
	pack  $full_name.col2 -side right -fill both -expand 1
	
	set fgraph $full_name.col2.graph
	
	graph $fgraph -title "" -relief ridge -bd 3 -background "#c8f8ce" 
	#-plotbackground black 
	# switch cross hairs on. For some reason $fgraph crosshairs on doesnt work?

	Blt_Crosshairs  $fgraph

	
	$fgraph configure -width 5.0i -height 2.0i      ;# configure it a little 
	$fgraph legend configure -position @80,8  -anchor nw  -relief raised 
	
	pack $fgraph -side right -fill both -expand 1
	
	# now, autoscaling may not be the best. Lets try use the standard
	# deviation for the data points that we have stored right now.

	calc_best_scale $item   ;# returns calc values, or "" if not enough data
	
	$fgraph yaxis configure -max $scale_ymax -min $scale_ymin 
	
	# add day of the week if using the history command
	if {$doing_mhist} {
	    $fgraph xaxis configure -loose 1 -title "" -command {my_clock_format "%d/%m %H:%M"}
	} else {
	    $fgraph xaxis configure -loose 1 -title "" -command {my_clock_format %H:%M}
	}
	
	$fgraph element create line 
	$fgraph element configure line  \
		-label $item -color $item_color($item) -symbol "" \
		-xdata V_x_$item -ydata V_y_$item
	

	# create  exit button  
	button   $full_name.col1.ok  -text  "close"  -width 6   -relief flat -underline 0 -bd 0 \
		-command "destroy $full_name  ;  return"
	pack     $full_name.col1.ok  -side bottom
	
	# create re-scaling button
#	button   $full_name.col1.scale1  -text  "AutoScale"  -width 6 -font 6x12 \
#		-command "scale_single_window2 Auto $item" 
#	pack     $full_name.col1.scale1  -side top
#	button   $full_name.col1.scale2  -text  "ReScale" -font 6x12 -width 6 \
#		-command "scale_single_window2 Rescale $item"
#	pack     $full_name.col1.scale2  -side top   
#	pack     $full_name.col1.scale2  -side top   
	

	# trial code - scale menu:
	menubutton   $full_name.col1.scale_gen -text "Scaling" -menu $full_name.col1.scale_gen.mnu \
		-width 6 
	menu  $full_name.col1.scale_gen.mnu
	$full_name.col1.scale_gen.mnu add command -command "scale_single_window Manual $item 2" \
		-label "Manual Scale"
	$full_name.col1.scale_gen.mnu add command -command "scale_single_window Rescale $item 2" \
		-label "AutoScale 1"
	$full_name.col1.scale_gen.mnu add command -command "scale_single_window Auto $item 2" \
		-label "AutoScale 2"
	pack   $full_name.col1.scale_gen -side top
	# end of trial code



	# create information button
	button   $full_name.col1.info  -text  "Info/Help"  -width 6  -relief flat -underline 0 -bd 0 \
		-command "show_item_info $item" 
	pack     $full_name.col1.info  -side top
	
	# create hard copy pull down menu:
	menubutton $full_name.col1.hard -text "HardCopy"   -relief flat -underline 0 -bd 0 \
		-menu $full_name.col1.hard.mnu 
	
	set hardcopy_menu [ menu  $full_name.col1.hard.mnu]
	
	$hardcopy_menu add command -command "hardcopy ps  $fgraph" -label "ps  file"
	$hardcopy_menu add command -command "hardcopy jpg $fgraph" -label "jpg file"
	$hardcopy_menu add command -command "hardcopy gif $fgraph" -label "gif file"
	$hardcopy_menu add command -command "hardcopy png $fgraph" -label "png file"
	
	pack  $full_name.col1.hard

	# bindings for the zooming function.
	bind $fgraph <ButtonPress-1> "zoom_select %W %x %y start"
	bind $fgraph <ButtonRelease-1> "zoom_select %W %x %y stop"
	pack $full_name -side top -expand 1 -fill both

    }


    return
}


#proc popup_next_prev_window { } {
#    
#    global  display_item_cnt
#    
#    toplevel .disp_next -background green
#
#    wm geometry .disp_next  \
#	    +[winfo rootx .fullscale_main]+[winfo rooty .fullscale_main]
#    label .disp_next.title -text "Prev 5  or Next 5 "  -background green
#    button .disp_next.prev -text "<<" -command {
#	incr display_item_cnt -5
#	show_all_full_scale 
#    }
#    button .disp_next.next -text ">>" -command {
#	incr display_item_cnt 5
#	show_all_full_scale 
#    }
#    button .disp_next.quit -text "exit" -command {destroy .disp_next}
#    pack .disp_next.title -side top
#    pack .disp_next.prev -side left
#    pack .disp_next.quit -side left
#    pack .disp_next.next -side right
#    update

#return
#}



 
#======================================================================
# ZOOM ON SELECTED GRAPHS USING MOUSE
#======================================================================
proc zoom_select {window x  y point} {
    global zoom_coor
    global fix_y_scale


    if {$point=="start"} {
	set zoom_coor(corner1)  [$window invtransform $x $y]
	return
    } elseif {$point=="stop"} {
	set zoom_coor(corner2)  [$window invtransform $x $y]
    } else {
	tk_messageBox -message "Error: wrong parameter to zoom routine"
	return
    }

    # ok have corner 1 and 2
    # reset graph axis:
    # robustness check:
    if { [lindex $zoom_coor(corner1) 0] <  [lindex $zoom_coor(corner2) 0] } {
	$window xaxis configure -min [lindex $zoom_coor(corner1) 0] -max  [lindex $zoom_coor(corner2) 0]
    }

    if {!$fix_y_scale} {
	if { [lindex $zoom_coor(corner1) 1] <  [lindex $zoom_coor(corner2) 1] } {
	    $window yaxis configure -min [lindex $zoom_coor(corner1) 1] -max  [lindex $zoom_coor(corner2) 1]
	} else {
	    $window yaxis configure -max [lindex $zoom_coor(corner1) 1] -min  [lindex $zoom_coor(corner2) 1]
	} 
    }
    
    return
}

#================================================================
# HARDCOPY TO POSTSCRIPT/JPG/PNG/GIF FILE
#================================================================

proc hardcopy {type window} {
    $window  postscript configure  -paperwidth 6.5i -paperheight 8i \
	    -landscape false
    $window  postscript output stripchart.ps 

    if {$type =="ps"} {
	tk_messageBox -message "Postscript output to file stripchart.ps"
	return
    }

    if {$type=="jpg"} {
	catch "exec convert stripchart.ps stripchart.jpg" err
	if {$err==""} {
	    tk_messageBox -message "JPEG  output to file stripchart.jpg"
	    return
	} else {
	    tk_messageBox -message "Error: $err.\n  Output defaults to ps file stripchart.ps"
	    return
	}
    } 
   if {$type=="png"} {
	catch "exec convert stripchart.ps stripchart.png" err
	if {$err==""} {
	    tk_messageBox -message "PNG output to file stripchart.png"
	    return
	} else {
	    tk_messageBox -message "Error: $err.\n  Output defaults to ps file stripchart.ps"
	    return
	}
    } 

   if {$type=="gif"} {
	catch "exec convert stripchart.ps stripchart.gif" err
	if {$err==""} {
	    tk_messageBox -message "GIF output to file stripchart.gif"
	    return
	} else {
	    tk_messageBox -message "Error: $err.\n  Output defaults to ps file stripchart.ps"
	    return
	}
    } 

    return
}


#================================================================
# RESET SCALE ON EXANDED WINDOW
#================================================================

proc scale_single_window {scale_mode item window} {

    global scale_ymax scale_ymin   ;# returned and calculated values

# window =1 for the individual guys =2 for the stacked guys

    if {$window==1} { set fgraph .fullscale$item.col2.graph }
    if {$window==2} { set fgraph .fullscale_main.fullscale$item.col2.graph }

    switch -glob -- $scale_mode {
	"Auto"    { set scale_ymax "" ; set scale_ymin "" } 
	"Rescale" { calc_best_scale $item}    ;# this returns new best values
	"Manual"  {
	    toplevel .inputbox                 ;# make a seperate window for this
	    # put new window at (x,y) of root window:
	    wm geometry .inputbox  +[winfo rootx .]+[winfo rooty .]
	    
	    frame .inputbox.row1
	    frame .inputbox.row2
	    frame .inputbox.row3
	    
	    grid .inputbox.row1 -row 1
	    grid .inputbox.row2 -row 2
	    grid .inputbox.row3 -row 3
	    
	    label .inputbox.row1.startlab -text "Ymin = " -height 2
	    entry .inputbox.row1.start  -textvariable scale_ymin -width 40 
	    pack  .inputbox.row1.start -side right    
	    pack  .inputbox.row1.startlab -side left
 
	    label .inputbox.row2.startlab -text "Ymax = " -height 2
	    entry .inputbox.row2.start  -textvariable scale_ymax -width 40 
	    pack  .inputbox.row2.start -side right
	    pack  .inputbox.row2.startlab -side left


	    # create a button which, when clicked, will read the box, then destroy the box
	    button .inputbox.row3.ok -text "ok" -command {
		.inputbox.row1.start  get    ;# gets read into var.
		destroy .inputbox      
	    }
	    button .inputbox.row3.cancel -text "cancel" -command {
		destroy .inputbox
	    }
	    
	    pack .inputbox.row3.ok -side left
	    pack .inputbox.row3.cancel -side left
	    tkwait window .inputbox              ;# wait until its been destroyed
	    update
	}

	    
    }

    $fgraph yaxis configure -min $scale_ymin -max $scale_ymax
    $fgraph xaxis configure -min "" -max ""
    update
    return
}	    

# copy of the routine, but called by the stacked detail graphs
#proc scale_single_window2 {scale_mode item} {
#    global scale_ymax scale_ymin   ;# returned and calculated values
#    set fgraph .fullscale_main.fullscale$item.col2.graph
#    switch -glob -- $scale_mode {
#	"Auto"    { set scale_ymax "" ; set scale_ymin "" } 
#	"Rescale" { calc_best_scale $item}    ;# this returns new best values
#    }
#
#    $fgraph yaxis configure -min $scale_ymin -max $scale_ymax
#    $fgraph xaxis configure -min "" -max ""
#    update
#    return
#}	    


#==================================================================
# CALCULATE Y-SCALE USING DATA VALUE STANDARD DEVIATIONS
#==================================================================

proc calc_best_scale { item } {
# now, autoscaling may not be the best. Lets try use the standard
# deviation for the data points that we have stored right now.
#    global V_x_$item
#    global V_y_$item
    global scale_ymin scale_ymax

    if { [vector expr length(V_y_$item)] > 10 } {
	set stand_dev [ vector expr sdev(V_y_$item) ]  ;# get standard deviation
	set mean      [ vector expr mean(V_y_$item) ]  ;# get the mean
	
	set scale_ymin  [expr $mean - 8.* $stand_dev]  
	set scale_ymax  [expr $mean + 8.* $stand_dev]  

	if { [expr abs($scale_ymin- $scale_ymax) ] < 10.E-10 } {
	    set scale_ymin [expr $mean - 1.]
	    set scale_ymax [expr $mean + 1.]
	}


	if { [expr abs($scale_ymin)]  <10.E-20} {
	    set scale_ymin [expr $mean  - 1.]
	}
	if { [expr abs($scale_ymax)]  <10.E-20} {
	    set scale_ymax [expr $mean  + 1.]
	}
	


	
	#robustness check:
	if { $scale_ymin >= $scale_ymax } {
#	    tk_messageBox -message "warning: data is constant - no y-scaling \
#		    possible for $item. \n\n  ymax/min = $scale_ymin $scale_ymax \n\
#		    Standard deviation = $stand_dev"
#	    puts "data are now [set V_y_[set item](:)]"
	    set scale_ymin ""
	    set scale_ymax ""
	}
	
    } else {
	set scale_ymin ""
	set scale_ymax ""
    }

    return
}

#====================================================================
# SLEEP ROUTINE (ms)
#====================================================================

proc wait_ms {mill_sec} {
    set mill_sec [expr $mill_sec/100.0]
    for {set count 0 } { $count < $mill_sec} { incr count} {
	after 100
	update
    }
    return
}

#======================================================================
# SET THE SCROLL TIME INTERVAL USING RADIO BUTTONS
#======================================================================

proc new_scroll_time { } {
    global time_limit

# did the button already get pressed ?
    if [winfo exists .main.radio ] {
	wm deiconify . 
	raise        . 
	return
    }

    frame .main.radio
    raise .
    update
    set radio .main.radio

    radiobutton $radio.radio1 -text  "100 sec " -relief flat \
	    -variable time_limit -value 100.        
    radiobutton $radio.radio2 -text  "5  min  " -relief flat \
	    -variable time_limit -value [expr 5*60.]    
    radiobutton $radio.radio3 -text  "1  hrs  " -relief flat \
	    -variable time_limit -value [expr 60*60.]  
    radiobutton $radio.radio4 -text  "5  hrs  " -relief flat \
	    -variable time_limit -value [expr 5*60*60.]
    radiobutton $radio.radio5 -text  "12  hrs  " -relief flat \
	    -variable time_limit -value [expr 12*60*60.]
    button $radio.ok  -text "ok" -command "destroy $radio"

    pack $radio
    for {set i 1} {$i<6} {incr i} {
	pack $radio.radio$i
    }

    pack $radio.ok

    update
    return
}

#=====================================================================
# SHOW INFO ON INDIVUDUAL ITEM
#=====================================================================

proc show_item_info {item} {
    global equip_name

    toplevel .item_info
    #  put a an <ok> button on it:
    button .item_info.ok -text "OK" -command {destroy .item_info ; return}
    pack   .item_info.ok -side bottom
    
    #  put the text in the window:
    message .item_info.mess -width 6i -text \
	    "note: <spaces> are substituted by _ as are \\+- and % signs \n\n \
	    item name :  $equip_name/ $item \n\n \
	    Available functions: \n\n \
	    Click-and-drag  Left button to zoom time axis.\
	    Click Rescale to return to normal mode\n \
	    Autoscale = optimun scale for graph size \n \
	    ReScale   = scale calculate using data standard deviation\n \
	    HardCopy  = output to JPEG/PS/PNG etc\n \
	    Close     = close this window\n"
	    


    pack .item_info.mess

    return
}


#=======================================================================
# LOCATE GRAPH ITEM POINTED TO BY MOUSE
#=======================================================================

proc mouse_find_item {window x y } {
    global item_list

    # transform X-win coordinates to graph coordinates.
    set graph_coor [$window invtransform $x $y]
    set x_coor [lindex $graph_coor 0]
    set y_coor [lindex $graph_coor 1]
    
    # loop over entire list of data find the closest point and display
    # the item belonging to that point.
    
    set closest [expr 10000000000000000.0]

    foreach item $item_list {
	global V_x_$item
	global V_y_plot_$item
	
	vector create dist                 ;# dummy vector
	dist expr "(V_x_$item- $x_coor)^2 + (V_y_plot_$item - $y_coor)^2 "

	# rember smallest element
	if { [vector expr min(dist)] <  $closest} {
	    set closest [vector expr min(dist)]
	    set closest_item $item
	}
    }
    # clean up
    vector destroy dist

    # call the graph spawn routine.
    select_graph $closest_item
    return
}


#========================================================================
# READ THE OUTPUT OF MHIST, ALLOW SELECTION OF DATA AND GRAPH ** MHIST **
#========================================================================
proc read_mhist_file {open_file } {
    global event_choice
    global pick_event_var
    global event_var
    global history_time history_interval history_unit history_amount


    global item_fname item_fields item_pattern item_equation
    global item_color item_max item_min item_list
    global equip_name
    
    global strip_chart

    global exit_now
    global select_men
    global event_ids_info ; #mhist event ID and name
    global event_var      ; #varialbe name

    global debug
    global debug_code     ; #general code debugging 
    global button_toggle  ; #use to toggle lots of variables on/off simul.

    global mhist_path     ; # place to find mhist
    global selected_items ; # list of history items from listbox selection
    
    global hist_file      ; # name of history file to open
    global file_pref      ; # /tmp file prefix  

    global file_path      ; # path to the actual history files


    set error_var ""
    set exit_now  0



        
    if {![find_newest_mhist]} return     ; # locate mhist executable


    if {$debug_code} {puts "Debug: in routine read mhist file"}
    
# this is the FILE read version - 
#i.e. implicitly gets user to select a file.
# Then select the EVENT ID
# Then select items to be displayed
    if { [llength $item_list] >0} {
	clean_list_and_vectors_and_widgets          ;# remove all things associate
                                                    ;# with previous data
    }
    

    # get a new file or use the old ?
    if {$open_file} {
	if {$debug_code} {puts "Debug: file path is $file_path"} 
	# here we go..a *new* widget
	set types {{"History files .hst" {.hst}}}
	set hist_file [tk_getOpenFile -filetypes $types -initialdir $file_path]
	# but have to remember the path for the next call:
	if {$hist_file !=""}  { set file_path [file dirname $hist_file] }

    } else {
	if {$hist_file==""} { 
	    tk_messageBox -message "You need to open a file first \n"
	    return
	}
    }

    if {$debug_code} {puts "Debug: file path is now $file_path"} 
    if ![string compare $hist_file ""] return
    # ok, so we have the file name. Now repeat the previous excercicse
    # of getting the event ID.
    catch "exec rm -f /tmp/${file_pref}mhist" error_var
    set exec_string "$mhist_path/mhist -f $hist_file -l"

    if {$debug_code} {puts "debug1: exec string is  $exec_string"}
    
    catch "exec $exec_string > /tmp/${file_pref}mhist" error_var
    if {$error_var !="" } {
	tk_messageBox -message "Could not start mhist (*path problem ?) \n \
		$error_var \n. Command line was \n $exec_string"
	return
    }

    # assume the mhist out put is there and get event selection

    if {$debug_code} {puts "debug: calling get_mhist_list"}
    if {![get_mhist_list]} return
    
    if {$debug_code} {puts "debug: calling select_event_id"}
    if {[select_event_id]=="do_exit"} return

    
    # if an element is actually it self an array, expand that element to show
    # all array elements as seperate event variables
    if {[deal_with_array_vars]=="do_exit"} return


    # now do all the item selection stuff.
    if {$debug_code} {puts "debug: calling select_event_items"}
    if {[select_event_items old_file]=="do_exit"} return

    # now set the pick_event_var variables, just like before but using the results
    # from the list box retrieved from routine select_event_items
    # selected_items is only the index to the stuff that was picked from the scroll
    # list.
    foreach i $selected_items {
	set var [ lindex $event_var($event_choice) $i]
	set pick_event_var($var) 1
	if {$debug_code} {puts "Have picked index $i and name is $var"}
    }
    
    # now calculate history time (needed only for plotting here
    switch -glob -- $history_unit {
	"days"   {set history_time [expr $history_amount *1  ] }
	"weeks"  {set history_time [expr $history_amount *7  ] }
	"months" {set history_time [expr $history_amount *30 ] } 
    }

    # when opening old file, assume start date (-s) = name of file
    #                               end   date (-p) = calculated from user input above

    
    # calc start date from file name: (hist_file)
    # unfortunately, Stephan uses  yymmdd.hst. Oh well.
    # split list at the '.'
    # ah, but stephan has fixed the file names now
    # Here analyze the file name to get starting date.
    # remove unix path:
    set hist_file [lindex [split $hist_file /] end]
    set date_part [lindex [split $hist_file .] 0]


    if { [string range $date_part 0 1]=="97"  ||  [string range $date_part 0 1]=="98" }  {
	set start_time $date_part
    } else {
	set day_part  [string range $date_part 4 5]
	set mon_part  [string range $date_part 2 3]
	set yre_part  [string range $date_part 0 1]
	#	    set start_time $day_part$mon_part$yre_part
	set start_time $yre_part$mon_part$day_part
    }
    
    # ok, do the end part.
    if {$history_unit =="until_now"} {
	set stop_time [clock format [clock seconds] -format "%d%m%y"]
    } else {
	# gets tough. Must calculate a new day from old date and interval
	if {$debug_code} {puts "debug: calling calc_history_stop_time $day_part"}
	set stop_time [calc_history_stop_time $day_part $mon_part $yre_part]
    } 
    

    # now, now filter out which events we dont want and repeatedly call mhist
    # to get what we want.
    #  -b = time in seconds. 



    foreach  var $event_var($event_choice) {


	wm title  . "Loading data......... $var"
	.main.middle.strip_chart    configure -background  red
	update

	set ignore_item 0
	
	# was this guy enabled ?
	if { !$pick_event_var($var)} { continue }


	# check if it was an array - if so, add -i option and remove brackets
	if {[string match {*\[*\]} $var]} { 
	    if {$debug_code} {puts "Debug: back in read mhist - array var case"}
	    # extract index number:
	    set num_elem [string range $var [string last "\[" $var] [string last "\]" $var]]
	    set num_elem [string range $num_elem 1 [expr [string length $num_elem] -2]]
	    # remove bracket:
	    set new_var  [string range $var 0 [expr [string last "\[" $var] -1]]
	    # add -i to exec_string
	    if {$debug_code} {puts "Debug: array index is $num_elem  and new name $new_var"}
	    set exec_string \
		"$mhist_path/mhist -b -s $start_time \
		-p $stop_time -t $history_interval -e $event_choice  -v \"$new_var\" -i $num_elem"
	} else {

	    set exec_string \
		"$mhist_path/mhist -b -s $start_time \
		-p $stop_time -t $history_interval -e $event_choice  -v \"$var\" "
	}


	# note the surround by quotes
	# now add the path name to the files, in case we are not in the PWD.
	if {$file_path !=""} {
	    set exec_string "$exec_string -z $file_path"
	}

	# if we are not debugging....go for it.
	if { [string compare $debug "mhist"]  } {
	    if {$debug_code} {puts "debug2: Exec string is: $exec_string"}
	    catch {exec rm -f /tmp/${file_pref}mhist_data} error_var
	    catch "exec $exec_string > /tmp/${file_pref}mhist_data"  error_var
	}

	if {$error_var !=""} {
	    tk_messageBox -message "problem: error from mhist \n  $exec_string \n $error_var\n"
	    return
	}

	
	# check if MHIST was busy recovering the index files. This should only happen
	# once. Mhist does then give the data, but it's easier to run it again I think
	set file_hndl [open "/tmp/${file_pref}mhist_data" r ]
	if {![eof $file_hndl]} {
	    gets $file_hndl string
	    if {[string first "Recovering" $string]!=-1} {
		if {$debug_code} {puts "Ooops - mhist recovered index files ! -repeat extraction:"}
		if {$debug_code} {puts "Exec string is $exec_string"}
		catch {exec rm -f /tmp/${file_pref}mhist_data} error_var
		catch "exec $exec_string > /tmp/${file_pref}mhist_data"  error_var
	    }
	} else {
	    tk_messageBox -message "problem: mhist output /tmp/${file_pref}mhist_data emptry \n  \
		    $exec_string \n $error_var"
	    return
	}
	close $file_hndl
	# end of index generation checking

	

	# get ready to create the vector names
	set item $var
	set item [filter_bad_chars $item]           ;# remove spaces, slashes, etc etc

	# create the new vectors
	
	vector create V_x_$item
	vector create V_y_$item
	vector create V_y_plot_$item

	# get the data and read into BLT vectors as usual.
	set file_hndl [open "/tmp/${file_pref}mhist_data" r ]
	
	while {![eof $file_hndl]} {
	    gets $file_hndl string

#	    if { $debug_code } { puts "From file got string $string " }
	    if { [llength $string] == 2} {
		set x_val [string toupper [lindex $string 0]]
		set y_val [string toupper [lindex $string 1]]
		if { [string first "NAN" $x_val]==-1 && [string first "NAN" $y_val]==-1} {
#		    if {$debug_code} {puts "Adding to item $item value (x,y) $x_val $y_val " }
		    V_x_${item} append $x_val
		    V_y_${item} append $y_val
		} else {
		    puts "Stripchart:  - Found NAN number in MHIST data for $item -set to 0 "
		    V_x_${item} append 0
		    V_y_${item} append 0
		}
#		set V_x_${item}(++end) [lindex $string 0]
#		set V_y_${item}(++end) [lindex $string 1]

		# check for error from mhist:
	    } elseif {[llength $string] >=2 } { 
		if {[tk_messageBox -type abortretryignore \
			-message "problem: looking for data.\n  $exec_string\
			but got from mhist:\n $string\n\
			Ignore to skip this item \n\
			Abort  to cancel all items \n"]=="ignore"} {
		    set ignore_item 1
		    break
		} else {
		    return
		}

	    }
	}

	
	close $file_hndl


	if {!$ignore_item} { 
	    # fill in the same info we might have gotten from a .conf file:
	    # add to the list of items plotted
	    lappend item_list $item
	    # set the color
	    set item_color($item)  [get_new_color]
	    
	    # now for these data calculate appropiate max/min values, as we were
	    # given them from the .conf file:
	    if { [vector expr length(V_y_$item)] <= 1} {
		tk_messageBox  -default ok -message "Not enough data read for plotting" -type ok 
	    return
	    }
	    
	    if { [vector expr length(V_y_$item)] > 10 } {
		set stand_dev [ vector expr sdev(V_y_$item) ]  ;# get standard deviation
		set mean      [ vector expr mean(V_y_$item) ]  ;# get the mean
		
		set item_min($item)  [expr $mean - 8.* $stand_dev]  
		set item_max($item)  [expr $mean + 8.* $stand_dev]  
#		puts "before checl: min and max are $item_max($item) $item_min($item)  $mean"
		#robustness check:
		if { [expr abs($item_min($item)-$item_max($item))] < 10.E-10 } {
		    set item_min($item) [expr $mean - 1.]
		    set item_max($item) [expr $mean + 1.]
		}
		if { [expr abs($item_min($item))]  <10.E-20} {
		    set item_min($item) [expr $mean - 1.]
		}
		if { [expr abs($item_max($item))]  <10.E-20} {
		    set item_max($item) [expr $mean + 1.]
		}
#		puts "min and max are $item_max($item) $item_min($item)"
	    } else {
		set mean      [ vector expr mean(V_y_$item) ] 
		set item_min($item) [expr $mean - 100.]
		set item_max($item) [expr $mean + 100.]
#		puts "min and max are $item_max($item) $item_min($item) --------"
	    }
	    

	    # from this max/min value, calculate the plotted normalized histo values:
	    # do a vector calculation on this new guy.
	    
	    V_y_plot_$item \
		    expr "(V_y_$item -  $item_min($item)) / ($item_max($item)-$item_min($item))"  
	    
	    # finally associate/create a graph item for this guy
	    if {! [$strip_chart element exists line_$item]} {
		$strip_chart element create line_$item
	    }
	    $strip_chart element configure line_$item -label "" -color $item_color($item) -symbol ""
	    # now hot-link it to the new graph line 
	    $strip_chart element configure line_$item  -xdata V_x_$item  -ydata V_y_plot_$item
	    # change the labelling format on the x-axis if plotting more then one day
	    if {$history_time >= 1} {
		$strip_chart xaxis configure -command {my_clock_format "%d/%m %H:%M"}
	    } else {
		$strip_chart xaxis configure -command {my_clock_format "%H:%M"}
	    }
	    
	    # create the pull down item menu. Also align the text string
	    set blank_string "                            "
	    set item_length [string length $item]
	    if {$item_length > 13} {set item_length 13}
	    set menu_string "$item [string range $blank_string 1 [expr 13 - $item_length] ]"
	    append menu_string $item_color($item)
	    $select_men add command -command "select_graph $item" -label  $menu_string
	    
	    
	    # next item:
	    update
	}
    }

    return
}

#=========================================================================
# READ PRESENT MHIST DATA - this routine is very simular to the above
# but subtle difference with respect to picking the start/stop time
#=========================================================================
proc read_present_mhist { } {
    global event_choice
    global pick_event_var
    global event_var
    global history_time history_interval history_unit history_amount


    global item_fname item_fields item_pattern item_equation
    global item_color item_max item_min item_list
    global equip_name
    
    global strip_chart

    global exit_now
    global select_men
    
    global event_ids_info ;#mhist event ID and name
    global event_var      ;#varialbe name

    global debug_code     ; #general code debugging 
    global debug
    global button_toggle  ; #use to toggle lots of variables on/off simul.

    global mhist_path     ; # place to find mhist
    global selected_items ; # list of history items from listbox selection

    global file_pref      ; #/tmp tmp file prefix
    global file_path      ; # path to history files.

    if {![find_newest_mhist]} return     ; # locate mhist executable

    if {$debug_code} {puts "Debug: in routine read present_mhist file"}
  
    
    set error_var ""
    set exit_now  0

    
    if { [llength $item_list] >0} {
	if {$debug_code} {puts "Debug: calling clean_list_and_vectors"}
	clean_list_and_vectors_and_widgets          ;# remove all things associate
                                                    ;# with previous data
    }

    
# note: string compare is like 'c' - 0 == string match !!!
# so if NOT debugging, execute mhist:
   
    if {$file_path !=""} { 
	set exec_string "$mhist_path/mhist -l -z $file_path > /tmp/${file_pref}mhist"
    } else {
	set exec_string "$mhist_path/mhist -l > /tmp/${file_pref}mhist"
    }

    if { [string compare $debug "mhist"] } {
	if {$debug_code} {
	    puts "Executing mhist command: $exec_string"
	}
	catch "exec rm -f /tmp/${file_pref}mhist" error_var
	catch "exec $exec_string" error_var
    }

    if {$error_var !="" } {
	tk_messageBox -message "Could not start mhist (path problem ?) \n $error_var"
	return
    }

    # assume the mhist out put is there. Retrieve the ID list
    if {![get_mhist_list] } return
 
    # do selection of the event_id:
    if {$debug_code} {puts "Calling select_event_id"}
    if {[select_event_id]   =="do_exit"} return
    
    # if an element is actually it self an array, expand that element to show
    # all array elements as seperate event variables i.e. DATA[5] become 5 seperate items in the list.
    # see the -i option in mhist
    if {$debug_code} {puts "Calling deal with arrays"}
    if {[deal_with_array_vars]=="do_exit"} return

    if {$debug_code} {puts "Calling select_event_items"}
    if {[select_event_items today]=="do_exit"} return

    # now set the pick_event_var variables, just like before but using the results
    # from the list box.


    foreach i $selected_items {
	set var [ lindex $event_var($event_choice) $i]
	set pick_event_var($var) 1
    }
    
    
    # now calculate history time (assume todays history
    switch -glob -- $history_unit {
	"days"   {set history_time [expr $history_amount *1  ] }
	"weeks"  {set history_time [expr $history_amount *7  ] }
	"months" {set history_time [expr $history_amount *30 ] } 
    }

    # deal with the open file possibility
    # when opening old file, assume start date (-s) = name of file
    #                               end   date (-p) = calculated from user input above

    # now, now filter out which events we dont want and repeatedly call mhist
    # to get what we want.
    #  -b = time in seconds. 

    
    foreach  var $event_var($event_choice) {

	wm title  . "Loading data......... $var"
	.main.middle.strip_chart    configure -background  red
	update

	# was this guy enabled ?
	if { !$pick_event_var($var)} { continue }
	
	# was it an array data word (must use -i to get the actual value)
	if {[string match {*\[*\]} $var]} { 
	    if {$debug_code} {puts "Debug: back in read mhist - array var case"}
	    # extract index number:
	    set num_elem [string range $var [string last "\[" $var] [string last "\]" $var]]
	    set num_elem [string range $num_elem 1 [expr [string length $num_elem] -2]]
	    # remove bracket:
	    set new_var  [string range $var 0 [expr [string last "\[" $var] -1]]
	    # add -i to exec_string
	    if {$debug_code} {puts "Debug: array index is $num_elem  and new name $new_var"}
	    
	    set exec_string \
      "$mhist_path/mhist -b -d $history_time -t $history_interval -e $event_choice  -v \"$new_var\" -i $num_elem"
	} else {

	    set exec_string \
      "$mhist_path/mhist -b -d $history_time -t $history_interval -e $event_choice  -v \"$var\" "
	}



	if {$file_path !=""} { 
	    set exec_string "$exec_string -z $file_path"
	} 


	# if we are not debugging....go for it.
	if { [string compare $debug "mhist"]  } {
	    if {$debug_code} {puts "Debug5: exec string is:  $exec_string > /tmp/${file_pref}mhist_data"}
	    catch {exec rm -f /tmp/${file_pref}mhist_data} error_var
	    catch "exec $exec_string > /tmp/${file_pref}mhist_data"  error_var
	}

	if {$error_var !=""} {
	    tk_messageBox -message "problem: error from mhist \n  $exec_string \n $error_var"
	    return
	}


	# get ready to create the vector names
	set item $var
	set item [filter_bad_chars $item]           ;# remove spaces, slashes, etc etc


	# create the new vectors
	
	vector create V_x_$item
	vector create V_y_$item
	vector create V_y_plot_$item


	
	# get the data and read into BLT vectors as usual.
	set file_hndl [open "/tmp/${file_pref}mhist_data" r ]
	
	while {![eof $file_hndl]} {
	    gets $file_hndl string
	    if { [llength $string] == 2} {
		set x_val [string toupper [lindex $string 0]]
		set y_val [string toupper [lindex $string 1]]
		if { [string first "NAN" $x_val]==-1 && [string first "NAN" $y_val]==-1} {
		    V_x_${item} append $x_val
		    V_y_${item} append $y_val
		} else {
		    puts "Stripchart:  - Found NAN number in MHIST data for $item"
		    V_x_${item} append 0
		    V_y_${item} append 0
		}
		# Original code has: - which should work, but broke under RH7.2 distr
		#set V_x_${item}(++end) [lindex $string 0]
		#set V_y_${item}(++end) [lindex $string 1]

	    } elseif {[llength $string] >=2 } { 
		tk_messageBox -message "problem: looking for data.\n Send $exec_string \n \
			, but found \n $string \n\
			Probably means, there really is no file for today's date."
		return
	    }
	}

	close $file_hndl

	# fill in the same info we might have gotten from a .conf file:
	# add to the list of items plotted
	lappend item_list $item
	# set the color
	set item_color($item)  [get_new_color]

	if {$debug_code} {
	    puts  "Debug:"
	    puts "Item name $item"
	    puts  "Vector length y [vector expr length(V_y_$item)]"
	    puts  "Vector length x [vector expr length(V_x_$item)]"
	}
	
# now for these data calculate appropiate max/min values, as we were
# given them from the .conf file:
	if { [vector expr length(V_y_$item)] <= 1} {
	    tk_messageBox  -default ok -message "Not enough data read for plotting" -type ok 
	    return
	}

	if { [vector expr length(V_y_$item)] > 10 } {
	    set stand_dev [ vector expr sdev(V_y_$item) ]  ;# get standard deviation
	    set mean      [ vector expr mean(V_y_$item) ]  ;# get the mean
	    
	    set item_min($item)  [expr $mean - 8.* $stand_dev]  
	    set item_max($item)  [expr $mean + 8.* $stand_dev]  
	    
	    #robustness check:	
	    if { [expr abs($item_min($item)-$item_max($item))] < 10.E-10 } {
		set item_min($item) [expr $mean - 1.]
		set item_max($item) [expr $mean + 1.]
	    }
	    if { [expr abs($item_min($item))]  <10.E-20} {
		set item_min($item) [expr $mean  - 1.]
	    }
	    if { [expr abs($item_max($item))]  <10.E-20} {
		set item_max($item) [expr $mean + 1.]
	    }


	} else {
	    set mean      [ vector expr mean(V_y_$item) ] 
	    set item_min($item) [expr $mean - 100.]
	    set item_max($item) [expr $mean + 100.]
	}

	# from this max/min value, calculate the plotted normalized histo values:
	# do a vector calculation on this new guy.

	V_y_plot_$item \
		expr "(V_y_$item -  $item_min($item)) / ($item_max($item)-$item_min($item))"  

	# finally associate/create a graph item for this guy
	if {! [$strip_chart element exists line_$item]} {
	    $strip_chart element create line_$item
	}
	$strip_chart element configure line_$item -label "" -color $item_color($item) -symbol ""
	# now hot-link it to the new graph line 
	$strip_chart element configure line_$item  -xdata V_x_$item  -ydata V_y_plot_$item
	# change the labelling format on the x-axis if plotting more then one day
	if {$history_time >= 1} {
	    $strip_chart xaxis configure -command {my_clock_format "%d/%m %H:%M"}
	} else {
	    $strip_chart xaxis configure -command {my_clock_format "%H:%M"}
	}

	# create the pull down item menu. Also align the text string
	set blank_string "                            "
	set item_length [string length $item]
	if {$item_length > 13} {set item_length 13}
	set menu_string "$item [string range $blank_string 1 [expr 13 - $item_length] ]"
	append menu_string $item_color($item)
	$select_men add command -command "select_graph $item" -label  $menu_string
	

	# next item:
	update

    }


    return
}


#===================================================================
#   DEAL WITH VARIABLES THAT ARE ACTUALLY ARRAYS
# expand the actual variable list to include all elements as
# seperate variables i.e. MY_AR[4] adds 4 new elements
#===================================================================
proc deal_with_array_vars { } {

    global event_choice  # number of the chosen event
    global event_var     # this is the list of list of variables
    global debug_code

    if {$debug_code} {puts "Debug: In deal with array vars "}

#    set i 0
#    puts "new list "
#    foreach var $event_var($event_choice) {
#	puts "$i $var"
#    }
	


    set i 0
    foreach var $event_var($event_choice) {
	# look for [] in variable name
	if {[string match {*\[*\]} $var]} {

	    # extract the number of elements for this variable:
	    set num_elem [string range $var [string last "\[" $var] [string last "\]" $var]]
	    set num_elem [string range $num_elem 1 [expr [string length $num_elem] -2]]
	    if {$debug_code} {puts "Debug: Found an array variable $var with $num_elem elements"}
	    # expand the array and add each as a new element 
	    for {set k 0} { $k < $num_elem} {incr k} {
		set new_var_name [string range $var 0 [expr [string last "\[" $var] -1]]
		set new_var_name ${new_var_name}\[$k\]
		if {$k==0} {
		    # replace the first -
		    set event_var($event_choice) [lreplace $event_var($event_choice) $i $i $new_var_name]
		} else {
		    set event_var($event_choice) [linsert  $event_var($event_choice) $i $new_var_name]
		}
		incr i
	    }
	    incr i -1
	}
	incr i
    }

#    puts "new list "
#    foreach var $event_var($event_choice) {
#	puts $var
#    }
	
    return ""
}



#===================================================================
#   PICK THE ITEMS TO BE PLOTTED 
#===================================================================
proc select_event_items { today_or_file} {
    
    global event_choice
    global pick_event_var
    global event_var
    global history_time history_interval history_unit history_amount

    global exit_now
    global debug_code
    # ok, now display and pick list of events_names. Use checkbutton
    toplevel .select_vars
    wm title .select_vars "mhist variable select"
    # glue it to the existing window:
#    wm geometry .select_vars +[winfo rootx .]+[winfo rooty .]
    
    frame .select_vars.col1row1  -relief ridge  -bd 3 
    frame .select_vars.col2row1  -relief ridge  -bd 3 
    frame .select_vars.col1row2  -relief ridge  -bd 3 
    frame .select_vars.col2row2  -relief ridge  -bd 3 
    frame .select_vars.col1row3  -relief ridge  -bd 3 
    frame .select_vars.col2row3  -relief ridge  -bd 3 
    frame .select_vars.col1row4  -relief ridge  -bd 3
    frame .select_vars.col2row4  -relief ridge  -bd 3
    frame .select_vars.col1row5  -relief ridge  -bd 3
    grid .select_vars.col1row1 -row 1 -column 1
    grid .select_vars.col2row1 -row 1 -column 2
    grid .select_vars.col1row2 -row 2 -column 1
    grid .select_vars.col2row2 -row 2 -column 2
    grid .select_vars.col1row3 -row 3 -column 1  -columnspan 2
    grid .select_vars.col2row3 -row 3 -column 2
    grid .select_vars.col1row4 -row 4 -column 1
    grid .select_vars.col2row4 -row 4 -column 2
    grid .select_vars.col1row5 -row 5 -column 1  -columnspan 2

    # trial code for the list box with scroll bar, to replaces the radiobuttons:
    # create scroll bar:
    scrollbar .select_vars.col1row3.scroll -command ".select_vars.col1row3.listb yview"
    # create the list box:
    listbox .select_vars.col1row3.listb \
	    -yscrollcommand ".select_vars.col1row3.scroll set" \
	    -selectmode extended -height 15
    pack  .select_vars.col1row3.listb -side left
    pack  .select_vars.col1row3.scroll -fill y -side right

    # fill the list box
    set i 0
    foreach var $event_var($event_choice) {
	if {$debug_code} {puts "List contains $var"}
	set pick_event_var($var) 0
	.select_vars.col1row3.listb insert end $var
        .select_vars.col1row3.listb selection set $i
        incr i
    }
    
    label .select_vars.col1row5.lab -text "Use <ctrl/shift> for multiple select"
    pack  .select_vars.col1row5.lab -side bottom -fill x
    
    
    set button_toggle 1
    checkbutton .select_vars.col1row4.all -text "select all/none" -relief ridge \
	    -variable button_toggle -command { 
	
	for {set i 0} {$i< [llength $event_var($event_choice) ]} {incr i} {
            if {$button_toggle} {
		.select_vars.col1row3.listb selection set  $i 
            } else {
               .select_vars.col1row3.listb selection clear $i  
            }
        }
    }

    pack  .select_vars.col1row4.all -fill x -expand 1

    button .select_vars.col2row4.ok  -text "ok" -command {
	# for the list box (no more radiobuttons), get the chosen events
	set selected_items [.select_vars.col1row3.listb curselection]
	destroy .select_vars
    }
    
    button .select_vars.col2row4.cancel \
	    -text "cancel" -command "set exit_now 1; destroy .select_vars"
    
    pack .select_vars.col2row4.ok     -side left
    pack .select_vars.col2row4.cancel -side right
    
    
    # add radio buttons for the time duration and interval
    
    # if using old file, add some text and an 'until now' choice
    if {$today_or_file=="old_file"} {
	frame .select_vars.col1row0
	grid  .select_vars.col1row0 -row 0 -column 1 -columnspan 2
	label .select_vars.col1row0.text -text "Old history file selected. Displayed \
		period is \n\ calculated starting from date of the history file " \
		-relief ridge  -bd 3 
	pack  .select_vars.col1row0.text
    }

    frame .select_vars.col1row1.left           ;# subdivide even further: day/weeks etc
    frame .select_vars.col1row1.right          ;# multiplier
    grid  .select_vars.col1row1.right -column 2 -row 1
    grid  .select_vars.col1row1.left  -column 1 -row 1

    set radio .select_vars.col1row1.left
    set history_time 1

    label .select_vars.col1row1.lab1 -text "History Time:" -anchor center -justify center
    grid  .select_vars.col1row1.lab1 -row 0 -columnspan 2

    label .select_vars.col1row1.left.lab -text "Units"
    pack  .select_vars.col1row1.left.lab
    
    set radio .select_vars.col1row1.left

    # set defualt variable values
    set history_time 1
    set history_unit days

    
    radiobutton $radio.day  -text "days"   -relief flat -anchor w  \
            -variable history_unit   -value days
    pack $radio.day -fill x -expand 1
    radiobutton $radio.week  -text "weeks"   -relief flat -anchor w  \
            -variable history_unit   -value weeks
    pack $radio.week -fill x -expand 1
    radiobutton $radio.month  -text "months"   -relief flat -anchor w  \
            -variable history_unit   -value months
    pack $radio.month -fill x -expand 1

    if {$today_or_file=="old_file"} {
	radiobutton $radio.until_now  -text "until now"   -relief flat -anchor w  \
		-variable history_unit   -value until_now
	pack $radio.until_now -fill x -expand 1
    }
    
    # now the day/week/month multiplier
    set history_amount 1
    label .select_vars.col1row1.right.lab -text "Multiplier"
    pack  .select_vars.col1row1.right.lab

    set radio .select_vars.col1row1.right
    

    radiobutton $radio.1  -text "1"   -relief flat -anchor w  \
            -variable history_amount   -value 1
    pack $radio.1 -fill x -expand 1
    radiobutton $radio.2  -text "2"   -relief flat -anchor w  \
	    -variable history_amount   -value 2
    pack $radio.2 -fill x -expand 1
    radiobutton $radio.3  -text "3"   -relief flat -anchor w  \
            -variable history_amount   -value 3
    pack $radio.3 -fill x -expand 1
    radiobutton $radio.4  -text "4"   -relief flat -anchor w  \
            -variable history_amount   -value 4
    pack $radio.4 -fill x -expand 1
    radiobutton $radio.5  -text "5"   -relief flat -anchor w  \
	    -variable history_amount   -value 5
    pack $radio.5 -fill x -expand 1
    

    # for the interval:
    set history_interval 300                  ;# default values

    label .select_vars.col2row1.lab -text "History Interval"
    pack  .select_vars.col2row1.lab

    set radio .select_vars.col2row1
    
    radiobutton $radio.1day  -text "100 s"   -relief flat -anchor w  \
            -variable history_interval   -value 100
    pack $radio.1day -fill x -expand 1
    radiobutton $radio.2day  -text "5 mins"   -relief flat -anchor w  \
            -variable history_interval   -value 300
    pack $radio.2day -fill x -expand 1
    radiobutton $radio.3day  -text "30 mins"   -relief flat -anchor w  \
            -variable history_interval   -value [expr 30*60]
    pack $radio.3day -fill x -expand 1
    radiobutton $radio.4day  -text "1 hours"   -relief flat -anchor w  \
            -variable history_interval   -value [expr 60*60]
    pack $radio.4day -fill x -expand 1
    radiobutton $radio.5day  -text "5 hours"   -relief flat -anchor w  \
            -variable history_interval   -value [expr 5*60*60]
    pack $radio.5day -fill x -expand 1
   
    update
    
    # now wait until the window with the variables/times is gone before going on:
    
    tkwait window .select_vars

    if {$exit_now} {return "do_exit"}
    
    
}



#===================================================================
#   CALCULATE HISTORTY STOP TIME FROM START DATE AND INTERVAL
#===================================================================

proc calc_history_stop_time {day_part mon_part yre_part} {
    global history_unit history_amount

# problem - if the amount starts with 0, like 08 say, tcl
# thinks its octal !
# So check whether its two digits:if so trim of leading zero.

    if {[string length $day_part] == 2} {
	set day_part [string trimleft $day_part 0]
    }
    if {[string length $mon_part] == 2} {
	set mon_part [string trimleft $mon_part 0]
    }

    switch -glob -- $history_unit {
	"days"   {
	    set lday_part  [expr $day_part + $history_amount ]
	    set lmon_part  $mon_part
	    set lyre_part  $yre_part
	    if {$lday_part > 28} {
		set lmon_part [incr lmon_part]
		set lday_part [expr $lday_part - 28]
	    }
	   
	}
	"weeks"  {
	    set lday_part  [expr $day_part + $history_amount*7 ]
	    set lmon_part  $mon_part
	    set lyre_part  $yre_part
	    if {$lday_part > 28} {
		set lmon_part [incr lmon_part]
		set lday_part [expr $lday_part - 28]
	    }
	}
	"months" {
	    set lday_part  $day_part
	    set lmon_part   [expr $mon_part + $history_amount  ]
	    set lyre_part  $yre_part
	    if {$lmon_part > 12} {
		set lyre_part [incr lyre_part]
		set lmon_part [expr $lmon_part - 12]
	    }    
	}
    }

    # make sure they are all 2 digits
    if {[string length $lday_part] == 1} {
	set lday_part "0$lday_part"
    }
    if {[string length $lmon_part] == 1} {
	set lmon_part "0$lmon_part"
    }
    if {[string length $lyre_part] == 1} {
	set lyre_part "0$lyre_part"
    }

    # ok now put the string back together
    #return $lday_part$lmon_part$lyre_part
    return $lyre_part$lmon_part$lday_part
           
}

#=========================================================================
#  SELECT THE EVENT ID TO DISPLAY IN MHIST
#=========================================================================


proc select_event_id { } {
    global event_choice
    global pick_event_var

    global event_var      ;#variable name
    global history_time history_interval history_unit history_amount


    global item_fname item_fields item_pattern item_equation
    global item_color item_max item_min item_list
    global equip_name
    
    global strip_chart

    global exit_now
    global select_men
    
    global event_ids_info ; #mhist event ID and name


    global debug
    global button_toggle  ; #use to toggle lots of variables on/off simul.

    set exit_now  0


    # ok, first throw up a selection window for the event id. I guess
    # this should be a radio button ?
    
    if [winfo exists .select_event ] {
	wm deiconify .select_event 
	raise        .select_event 
    } else {
	toplevel  .select_event
	wm title  .select_event "mhist event select"
	# the next puts the new window at the same position as the root window
	wm geometry .select_event +[winfo rootx .]+[winfo rooty .]
	frame     .select_event.radio
	frame     .select_event.row2
    }

    set radio .select_event.radio 
    # set default choice
    set event_choice [lindex [lindex $event_ids_info 0] 0]
    
    # note: the variables in a -variable statement MUST be global
    foreach event_id $event_ids_info {
	set win_name [filter_bad_chars [lindex $event_id 0]]
	radiobutton $radio.radio$win_name -text $event_id  -relief flat -anchor w  \
		-variable event_choice   -value [lindex $event_id 0]
	pack $radio.radio$win_name -fill x -expand 1
    }
    
    button .select_event.row2.ok      -text "ok"     -command "destroy .select_event "
    button .select_event.row2.cancel  -text "cancel" -command "set exit_now 1 ; destroy .select_event"
#    if {$which_time=="first_time"} {
#	button .select_event.row2.openfile \
#		-text "open file" -command "set open_file 1;destroy .select_event"
#	pack .select_event.row2.openfile -side right	
#    }
        
    pack $radio
    pack .select_event.row2 -side bottom
    pack .select_event.row2.ok -side left
    pack .select_event.row2.cancel -side right
    update
    
    # !! The next command forces the script to halt until the .select_event
    # dialog box has been destroyed. This is the only way to get a 'top down'
    # order of events.
    
    tkwait window .select_event
    
    
    if {$exit_now}  {
	return "do_exit"
    }

    return "continue"
    
}





#===================================================================
# EMPTY THE ITEM LIST DESTROY EXISTING VECTORS
#===================================================================

proc clean_list_and_vectors_and_widgets { } {
    global item_fname item_fields item_pattern item_equation
    global item_color item_max item_min item_list
    global select_men

    $select_men delete 1 [llength $item_list]   ;# remove pull down menu's

# GJH Nov-01. This doesnt seem to work. I dont get the scope of vectors   
    foreach item $item_list {
	global V_x_$item
	global V_y_$item
	global V_y_plot_$item
	if {[array exist V_x_$item]} {vector destroy V_x_$item}
	if {[array exist V_y_$item]} {vector destroy V_y_$item}
	if {[array exist V_y_plot_$item]} {vector destroy V_y_plot_$item}
    }

    set item_list ""
    
    if {[array exist item_fname]}    {unset item_fname}
    if {[array exist item_fields]}   {unset item_fields}
    if {[array exist item_pattern]}  {unset item_pattern}
    if {[array exist item_equation]} {unset item_equation}
    if {[array exist item_color]}    {unset item_color}
    if {[array exist item_max]}      {unset item_max}
    if {[array exist item_min]}      {unset item_min}



    return

}
    
    
#===================================================================
# READ THE MHIST -l OUTPUT TO FIND EVID's, VARIABLE NAMES
#===================================================================

proc get_mhist_list { } {
    global event_ids_info ;#mhist event ID and name
    global event_var      ;#varialbe name
    global debug_code     ;#general code debug
    global file_pref      ; #/tmp tmp file prefix
    global file_path      ;# path to dir containing history files.

# erase the previous list

    set event_ids_info ""
    if {[array exist event_var]} {unset event_var}

    set file_hndl [open "/tmp/${file_pref}mhist" r]
    
    while   {! [eof $file_hndl]} {
	gets $file_hndl string

# check for a couple of commom errors:
	if  {[string first "cannot find recent history file" $string ] !=-1} {
	    tk_messageBox -message "mhist output: \n $string \n\n\
		    Possible cause: you are not in the current working directory.\n\
		    Set directory using Mhist-->Set History Path"
	    return 0;
	}
	
# look for a couple of things: ID: give event iD number
# x: gives some of the possible variables.
	if {[string first "Event ID" $string ] !=-1 } {
	    # ok, found an event ID: read the id, name
	    # look for ID and the ':'
	    set pos1  [string first "ID" $string]
	    set pos2 [string first  ":"  $string]
	    set evid [string range $string [expr $pos1 +2 ] [expr $pos2 -1] ]
	    set evid [string trim $evid]
	    set evnam [string range $string [expr $pos2 +1] [string length $string]]
	    set evnam [string trim $evnam]
	    lappend event_ids_info "$evid $evnam"

	    # skip next spaces:
	    set string ""
	    while   {![eof $file_hndl] && [llength $string ] == 0  } { 
		gets $file_hndl string
	    }
	    # read the variables
	    while   {![eof $file_hndl] && [llength $string] != 0 } {
		# skip midas error messages:
		while {[string first "midas.c" $string ] !=-1 } {
		    gets $file_hndl string
		}
		# take everything beyond the first :
		set pos  [string first ":" $string]
		set nam  [string range $string [expr $pos +1 ] end ]
		set nam [string trim $nam]
		if {[string length $nam] !=0} {
		    lappend event_var($evid) $nam 
		}
		gets $file_hndl string
	    }
	}
    }

    close $file_hndl

    return 1
}

#=========================================================================
# REMOVE SPACES, UNDERSCORES, OTHER NON ALPHANUMERIC CHARACTERS
#=========================================================================

proc filter_bad_chars {word} {
    set word [string tolower $word]
    regsub -all "\\." $word "_"  word    ;# eliminate '.' characters
    regsub -all "\\+" $word "p"  word    ;# eliminate '+' characters
    regsub -all "\\-" $word "_"  word    ;# eliminate '-' characters
    regsub -all {\[}  $word "_"  word    ;# eliminate '[' characters
    regsub -all {\]}  $word "_"  word    ;# eliminate ']' characters
    regsub -all " "   $word "_"  word    ;# eliminate ' ' characters
    regsub -all "#"   $word "_nr_"  word    ;# eliminate '#' characters
    regsub -all "%"   $word "_"     word    ;# eliminate '%' characters
    return $word
}


#==========================================================================
# GENERATE NEW COLORS FOR GRAPHS
#==========================================================================
proc get_new_color { } {

    global color_pointer
    global max_color_number

    set color {SpringGreen1 navy purple orange  red cyan DarkGreen blue3  brown green goldenrod \
	    orange maroon DarkSlateGrey purple blue4 LimeGreen sienna}

    set max_color_number 17

    incr color_pointer

    if {$color_pointer > $max_color_number} {set color_pointer 0}

    return [lindex $color $color_pointer]

}

proc set_main_y_scale {new_ratio} {
    # this routine changes the effective y-scale from a the
    # standard value of 0-1 to a bigger range using the
    # value extracted rom the RHS slider
    global slider_scale
    global strip_chart

    if { $new_ratio<= 2 } {
	set mmax 1
	set mmin 0
    } else {
	set mmax $new_ratio
	set mmin [expr -1.* $new_ratio]
    }
    
    $strip_chart yaxis configure  -min $mmin -max $mmax
    update
    return
}


#===========================================================================
#
#===========================================================================
proc set_file_path { } {
    
    global file_path             ;# path to actual history files
    global debug_code
    global tit_font

    # get current working dir as first guess
    if {$file_path==""} {catch "exec pwd" file_path}


    toplevel .inputbox                 ;# make a seperate window for this
# put new window at (x,y) of root window:
    wm geometry .inputbox  +[winfo rootx .]+[winfo rooty .]
    
    frame .inputbox.row1 
    frame .inputbox.row3
    
    grid .inputbox.row1 -row 1
    grid .inputbox.row3 -row 3
    
    label .inputbox.row1.startlab -text "Path to history files: " -height 2
    entry .inputbox.row1.start  -textvariable file_path -width 40 -font $tit_font
    pack  .inputbox.row1.start    -side right 
    pack  .inputbox.row1.startlab -side left
    
    # create a button which, when clicked, will read the box, then destroy the box
    button .inputbox.row3.ok -text "ok" -command {
        .inputbox.row1.start  get    ;# gets read into var.
        destroy .inputbox      
    }
    button .inputbox.row3.cancel -text "cancel" -command {
        destroy .inputbox
    }
    
    pack .inputbox.row3.ok -side left
    pack .inputbox.row3.cancel -side left
    tkwait window .inputbox              ;# wait until its been destroyed
    
    update
    if {$debug_code}  { puts "Setting file path to $file_path"}
    return

    
}


#===========================================================================
# DISPLAY HELP TEXT
#===========================================================================
proc help_menu { } {
    
    toplevel .info          ; # create a new window
    #  put a an <ok> button on it:
    button .info.ok -text "OK" -command {destroy .info ; return}
    pack   .info.ok -side bottom
    
    # now create the text widget
    # text .info.text -height 25 -width 65 
    # pack .info.text -side top
    
    #  put the text in the window:
    #  .info.text insert end   " \ 
    
    # use a message window instead of a text window - simpler
    message .info.mess -width 6.5i -text \
	    "Introduction: Stripchart can:\n\n\
	    1. plot any data in the\
	    midas database. It uses mchart, (a midas utility) to actually\
	    extract the data from odb\n\n\
	    2. view the data stored by mhist, the MIDAS history program.\n\n\n \
            This program should live in the ~/bin directory. To start it  \
	    type\n stripchart <name_of_conf_file> or just\n stripchart \n to look\
	    at data in the history files\n\n \
	    The configuration files \
	    are generated by mchart (eg. target.conf, chvi.conf) and \
	    are in the same format as that understood by gnome \
            gstripchart.  \n\n \
	     Example: to see the CHAOS target info type: \n \
            mchart  -q /equipment/target/variables -f target.conf\n\
	    and then start\n stripchart target.conf\n or just type \n \
	    mchart  -q /equipment/target/variables -g -f target.conf \n \
	    \n and stripchart is invoked automatically \n\n\n\
	    Note that all overlayed graphs are scaled using the max/min values \
            defined in the .conf file to fit between 0 and 1. Hence the \
            unlabelled y-scale on the main window.\n\
	    To see a particular data set in its normal, unscaled \
            units, use 'view full graph' and select the line or just click \
	    with the mouse on a point close to the line of interest. \n\n \
	    Note that the single graphs can be zoomed by dragging the mouse, \
	    auto-scaled, or 'best' scaled. Hardcopy is available in ps, jpg, gif \
	    or png format and goes to a file.\n\n\
	    To use the history function, click on the mhist button. You must be \
	    in the directory containing the .hst files. You will\
	    then be asked to select the event number and data words, as well as the\
	    history duration and interval\
	    \n\n \
	    The tcl interpreter bltwish  must be in your system.\n
	    G.J. Hofman    gertjan@triumf.ca \n \
	    Nov-01 \n \
	    TRIUMF"

    pack .info.mess -side bottom
    
    update
    return
}




#======================================================================================
#                                    MAIN PROGRAM 
#======================================================================================

# set a few parameters

set color_pointer 0

set tit_font "-*-helvetica-bold-r-normal-*-13-180-*-*-*-*-*-*"

# define the default scrolling time period
set time_limit [expr 60*60]               ;# in seconds 5 min default.

# default y-scaling  values for the pop-up windows
set scale_ymin ""
set scale_ymax "" 

set last_rescale_time  [clock seconds]  ;# last time  we checked the scaling
                                         # of the pop up windows
set conf_fname ""
set equip_name ""
set item_list  ""

set doing_mhist 0                       ;# data from mhist OR mchart conf file
set debug ""
set debug_code 0

set hist_file ""

#new:search for latest mhist
set mhist_path ""
set file_path ""

# get the file name of the .conf files. Command line parsing
set arguse 0               ;# no arguments used up by options

set display_item_cnt 0     ;# when showing 5 detailed at same time, the pointer.

set fix_y_scale 0             ;# whether or not mouse zoom changes y-scale

# no option = doing mhistory
# last option parameter = .conf file name
# assume doing mhist
set doing_mhist 1

if {$argc!=0} {
    for {set i 0} {$i < $argc } {incr i} {
	switch -glob -- [lindex  $argv  $i] {
	    "-h"           {puts "Usage: stripchart <-options> <config-file>\n\
		    -mhist (look at history file -default)\n\
		    -dmhist  debug mhist \n\
		    -debug   debug stripchart\n\
		    config_file:  see mchart"
	    exit}
	    "-mhist"       { set doing_mhist 1 ; incr arguse}
	    "-dmhist"      { set debug mhist   ; incr arguse}
	    "-debug"       { set debug_code 1  ; incr arguse}
	}
    }
    if {$arguse < $argc} { 
	set conf_fname [lindex $argv [expr $argc -1] ]
	set doing_mhist 0
    }
}

if {$debug_code} {
    puts "Debugging on"
    if {$doing_mhist} { 
	puts "Debug: doing mhist "
    } else {
	puts "Debug: doing mchart "
    }
}


if {!$doing_mhist} {
    if {![file exists $conf_fname]} {
	tk_messageBox  -message "Error: conf  file $conf_fname not found_!" -type ok
	exit
    } else {
	# read in the .conf file and parse:
	read_conf_file $conf_fname
    }
}


frame .main  -background  beige               ;# define name of main window.
frame .main.toprow   -relief ridge -bd 3      ;# for the buttons
frame .main.middle   -relief ridge -bd 3      ;# for the graph
frame .main.botrow                            ;# unused for now

wm title  . "midas stripchart: $equip_name  (GJH v.2.2)"              ;# configure the FVWM window
wm iconname . "StripC"

set strip_chart .main.middle.strip_chart

#graph $strip_chart -background goldenrod  -title ""                 ;# no title on the graph.
graph $strip_chart -background "#02a2fe" -title ""                 ;# no title on the graph.

# standard size is 5.5ix 2.0i
$strip_chart configure -width 6.5i -height 2.5i -font $tit_font

#$strip_chart marker create bitmap -under yes -coords { .2 .7}  -bitmap @midas.xbm


# configure the x-axis to plot Clock time instead of plain numbers
$strip_chart xaxis configure -loose 0 -title ""  \
	-command "my_clock_format %H:%M" -tickfont $tit_font -titlefont $tit_font

# fix the yaxis scale
$strip_chart yaxis configure -min 0 -max 1 \
	-tickfont $tit_font -titlefont $tit_font -showticks 0

# switch on cross hairs
#Blt_Crosshairs $strip_chart      ;# what is this comand ???
# bind the mouse click to the item search routine and pass 
# window name, x and y coordinate
bind $strip_chart <B1-ButtonRelease> "mouse_find_item %W %x %y"

# create an exit button. The -bd 0 mean a tight fit of text - no border
button  .main.toprow.exit -text "exit" -command {nice_exit} -relief flat -underline 0 
pack .main.toprow.exit -side left


# create an exit button
button  .main.toprow.help -text "help" -command {help_menu} -relief flat -underline 0 -bd 0
pack .main.toprow.help -side left


# menu for selecting of individual items = make a 'menubutton'
set  select_but [menubutton  .main.toprow.select -text "detail-single" \
        -menu .main.toprow.select.mnu  -relief flat -underline 0 -bd 0 ]
set  select_men [menu $select_but.mnu]

pack $select_but -side left -fill x


# Nov 201- new way of showing detail graphs:
button  .main.toprow.show_all -text "detail-all" -command {show_all_full_scale} \
	-relief flat -underline 0 -bd 0
pack .main.toprow.show_all -side left



# create an *new* scroll time selection. 

menubutton .main.toprow.scrollt -text "scroll time" -menu .main.toprow.scrollt.mnu \
	-relief flat -underline 0 -bd 0
menu .main.toprow.scrollt.mnu 
.main.toprow.scrollt.mnu  add radiobutton -label "100 s"    -variable  time_limit \
	-value 100
.main.toprow.scrollt.mnu  add radiobutton -label "5 mins"   -variable  time_limit \
	-value 300 
.main.toprow.scrollt.mnu  add radiobutton -label "30 mins"  -variable  time_limit \
	-value [expr 30*60] 
.main.toprow.scrollt.mnu  add radiobutton -label "1 hour"   -variable  time_limit \
	-value [expr 60*60]
.main.toprow.scrollt.mnu  add radiobutton -label "5 hour"   -variable  time_limit \
	-value [expr 5*60*60]
.main.toprow.scrollt.mnu  add radiobutton -label "10 hours" -variable  time_limit \
	-value [expr 10*60*60] 
.main.toprow.scrollt.mnu  add radiobutton -label "24 hours" -variable  time_limit \
	-value [expr 24*60*60]
pack  .main.toprow.scrollt  -side left -fill x


# old mhist button, now replaced by a pull down meni
#button .main.toprow.hist -text "mhist" -command {read_mhist}  \
#	-relief flat -underline 0 -bd 0
#pack .main.toprow.hist -side left

set mhist_but [menubutton .main.toprow.mhist -text "mhist" \
	-menu .main.toprow.mhist.mnu -relief flat -underline 0 -bd 0]
set mhist_men [menu $mhist_but.mnu]
pack $mhist_but -side left -fill x
$mhist_men add command -command "read_present_mhist" -label "Today's MHIST"
$mhist_men add command -command "read_mhist_file 1" -label "Open old history file"
$mhist_men add command -command "read_mhist_file 0" -label "Same file, New event"
$mhist_men add command -command "set_file_path"     -label "Set history-file path"


# create a warning label to show whether we have new  data coming in
label .main.toprow.update -relief sunken   \
	-textvariable prev_update_time_formatted -bd 0
pack .main.toprow.update -side right
label  .main.toprow.warning -relief flat -pady 4  \
	-text "Update: " -background green -bd 0
pack .main.toprow.warning -side right -fill x

# pack all the remaining windows: - first pack graph in middle row:
# note: its the 'fill both' and 'expand 1' that does the resizing
# of the graph when the window size changes.

pack $strip_chart -side left  -fill both -expand 1  ;# put it in its parent.

# then pack middlerow in the .main:
pack .main.middle -side top -fill both -expand 1 
pack .main.toprow -side top -fill x    -expand 1

#pack .main.botrow -side top -fill both -expand 1 ; # bot row unused
 
pack .main -fill both -expand 1                   ;# put it all on the screen
update

# try the slider to change the scale of the main graph
# note: the -command automatically appends the value of the slide
#set scale_slider 1
#scale .main.middle.slide -orient vertical  -label ""  -variable scale_slider \
#	-borderwidth 0  -showvalue 0 -width 10 -from 10 -to .1 \
#	-command "set_main_y_scale"
#pack .main.middle.slide -side right -fill y -expand 1



# check if we can write to /tmp/ - get a file namw prefix.
# catch returns zero if it's ok.

set file_pref "0"
set fcnt 0
# look until we have no error from deletion:
while {  [catch "exec rm -f /tmp/${file_pref}mhist" error_var] ||   \
	 [catch "exec rm -f /tmp/${file_pref}mhist_data" error_var]      } {
    incr file_pref 
}



#============================================================================
# CODE HERE IS INFINITE LOOP FOR MHIST USAGE
#============================================================================

# ok, done all the things we can do for both mhist AND normal stripcharting
# if we didn't open a .conf file, just sit here and wait:
if {$doing_mhist} {
    .main.toprow.warning configure -text "mhist " -background  cyan 

    while {1} {
	$strip_chart configure -background "#02a2fe"
	wm title  . "midas stripchart: $equip_name  (GJH v.2.0)"
	wait_ms 200
    }
}

    

# ===============================================================================
# CODE BELOW FOR ONLINE STRIPCHARTING ONLY
# ===============================================================================

# remove the mist button - dont need it for mchar applications
destroy $mhist_but


# create the pull down item menu. Also align the text string
set blank_string "                            "
foreach item $item_list {
    set item_length [string length $item]
    if {$item_length > 13} {set item_length 13}
    set menu_string "$item [string range $blank_string 1 [expr 13 - $item_length
    ] ]"
    append menu_string $item_color($item)
    $select_men add command -command "select_graph $item" -label  $menu_string
}


# ok, finally, create vectors associated with the data.
# create also the graph elements:

foreach item $item_list {
    if {! [$strip_chart element exists line_$item]} {
        $strip_chart element create line_$item
    } else {
        tk_messageBox -message "Error(online): line $item already exist - skipped "
	continue
    }
    $strip_chart element configure line_$item -label ""  -color $item_color($item)  -symbol ""
    vector create V_y_$item                ;# for storing raw data
    vector create V_y_plot_$item           ;# 'scaled' vector for plotting
    vector create V_x_$item                ;# time vector.
    # now hot-link it to the new graph line 
    $strip_chart element configure line_$item  -xdata V_x_$item  -ydata V_y_plot_$item
}



# for now I am assuming all data values are read from the SAME file.

set data_fname  $item_fname([lindex $item_list 0])   ;# just grab the first filename


# main plotting loop:

set prev_update_time  0            ;# last time the data file was written

set time_limit 600

while {1} {

# check if the file exist and when it was last written

    if {![file exists $data_fname]} {
        show_message "Error: data  file $data_fname not found !"
	.main.toprow.warning configure -text "No Data File" -background  red 
	vwait message_done         ;# wait until message ok button was hit.
	# go into loop, waiting for file:
	while {![file exists $data_fname]} {
	    wait_ms 1000
	}
    } else {
	
	# check the last modification time of the file:
	while { [file mtime $data_fname]== $prev_update_time} {
	    after 50
	    update
	    set time_passed [expr ( [clock seconds] - [file mtime $data_fname])]
	    if { $time_passed > 200 } { 
		.main.toprow.warning configure -text "No Data" -background  red 
	    }   
	}
	wait_ms 1000
    }

# ok, we have a new file:
    .main.toprow.warning configure -text "Update: "  -background green

    set prev_update_time [file mtime $data_fname]
# for output to screen, format the time for humans:
    set prev_update_time_formatted  [clock format $prev_update_time -format "%H:%M:%S"]
    
    set file_hndl [open $data_fname r]

    set cnter 0    
    # loop over all lines in the file
    while   {! [eof $file_hndl]} {

	gets $file_hndl string

	# for some reason, the last line is an empty string. Not sure why
	# skip empty strings
	if {[string length $string]==0} {
	    break
	}

	set item_string [lindex $string 0]
	
	set item_string [string tolower $item_string]     
	# remove underscores etc.
	set item_string [filter_bad_chars $item_string]

	# loop over all known keyword and compare to file.


	foreach item $item_list {
	    if { $item==$item_string} {
		incr cnter
		set new_data  [expr 1.0 *[lindex $string 1]]
		set vec_name  V_x_$item
		set ${vec_name}(++end)  $prev_update_time
		set vec_name V_y_$item
		set ${vec_name}(++end)  $new_data

		# now scale the vector to fit on the graph.
		set plot_vec_name  V_y_plot_$item
		set norm_data \
		[expr " ($new_data -  $item_min($item)) / ($item_max($item)-$item_min($item)) " \
		+ $cnter/100.]
		# note I add a small offset... just so that lines dont overlap each other
		set ${plot_vec_name}(++end) $norm_data
	    }
	} 
	update
    }

    close $file_hndl


    # take care of the scrolling:
    # I do some tricky double de-referencing here. I need to access the
    # vector but I only have the name of the vector. So the inner 'set'
    # returns the name of the vector. I then concatenate (0) and 
    # finally get the actual value stored in the vector


    foreach item $item_list {
	set oldest_time [ set V_x_[set  item](0)]
	set newest_time [ set V_x_[set  item](end)]

#	puts "data are now [set V_y_[set item](:)]"

	if {  [expr ( $newest_time - $oldest_time)] > $time_limit } {
	    V_x_$item delete 0
	    V_y_$item delete 0
	    V_y_plot_$item delete 0
	}
    }


    # also check if we you re-evaluate the scaling on the individual plots
    # say at every 300 s.
    set time_now [clock seconds]
    if { [expr $time_now - $last_rescale_time ] > 200} {
	foreach item $item_list {
	    if  [winfo exists .fullscale$item ] {
		scale_single_window Rescale $item 1
	    }
	    if [winfo exist .fullscale_main.fullscale$item] {
		scale_single_window Rescale $item 2
	    }
	}
	set last_rescale_time $time_now
    }

    update

    wait_ms 10000


}



#=======================================================================================
#====================== END OF CODE ====================================================
#=======================================================================================
