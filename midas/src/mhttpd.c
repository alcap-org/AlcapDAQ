/********************************************************************\

  Name:         mhttpd.c
  Created by:   Stefan Ritt

  Contents:     Web server program for midas RPC calls

  $Log: mhttpd.c,v $
  Revision 1.1.1.1  2005/06/20 23:37:33  mucap
  Importing release 1.9.5 of the MIDAS source code as distributed by PSI.
  (Next, I'll commit our local customizations.)

  Revision 1.277  2004/10/06 23:54:39  midas
  Removed event builder section

  Revision 1.276  2004/09/30 17:01:50  midas
  Don't disconnect from experiment if favicon.ico is requested

  Revision 1.275  2004/09/29 17:00:05  midas
  mhttpd does not exit on rpc_shutdown, only disconnect

  Revision 1.274  2004/09/17 23:43:19  midas
  Correct email header to pass SpamAssassin

  Revision 1.273  2004/09/17 22:03:58  midas
  Added refresh to SC page

  Revision 1.272  2004/08/26 20:21:00  pierre
  remove ; in History all

  Revision 1.271  2004/08/11 00:06:08  olchansk
  fix mhttpd crashes from "broken pipe" signals (SIGPIPE) when the user's web browser prematurely closes the connection (i.e. when the user hits the "stop" button while mhttpd is generating the data).

  Revision 1.270  2004/07/29 14:27:35  midas
  Added red error display in status page

  Revision 1.269  2004/07/29 14:10:49  midas
  Added red error display in messages page

  Revision 1.268  2004/07/12 14:16:03  midas
  Increase dynamically history buffer size

  Revision 1.267  2004/05/07 19:40:11  midas
  Replaced min/max by MIN/MAX macros

  Revision 1.266  2004/04/30 19:11:45  midas
  Newly created strings in ODB now get default length of 32

  Revision 1.265  2004/03/31 04:57:38  olchansk
  display PNG image files, same as GIF and JPG. (I remember adding this code before, but now it is gone. What gives?)
  one more strcpy() -> strlcpy()

  Revision 1.264  2004/02/06 00:39:16  pierre
  Update Midas doc links

  Revision 1.263  2004/01/19 16:56:49  olchansk
  use ss_timezone()

  Revision 1.262  2004/01/08 08:40:10  midas
  Implemented standard indentation

  Revision 1.261  2003/12/01 11:41:34  midas
  Added history panel groups (code from PAA)

  Revision 1.260  2003/11/20 11:29:44  midas
  Implemented db_check_record and use it in most places instead of db_create_record

  Revision 1.259  2003/11/18 14:39:44  midas
  Remove red color on 'events analyzed' if /Analyzer does not exist

  Revision 1.258  2003/11/01 01:37:33  olchansk
  stop frobbing /runinfo on every show_status_page(), this
    used to tickle the race condition in db_create_record(). Not it only
    tickles generally deficient error handling in the db_xxx() code.
  abort if cannot read /runinfo/run number
  setbuf(stderr,0)
  setbuf(stdout,0)

  Revision 1.257  2003/10/31 10:07:27  midas
  Added 'run parameter/comment' in status display

  Revision 1.256  2003/10/29 13:25:38  midas
  Removed snprintf (not available under Windows)

  Revision 1.255  2003/10/29 13:09:50  midas
  Fixed crash when current_filename is not set for ftp

  Revision 1.254  2003/10/13 00:07:40  olchansk
  refuse run number zero and abort on corrupted run numbers

  Revision 1.253  2003/10/12 22:56:33  olchansk
  when submitting new Elog message, add the message text to the outgoing email.
  add traps for some array overruns (see http://midas.triumf.ca/forum/Development%20Area/12)

  Revision 1.252  2003/10/10 22:47:40  olchansk
  fix crash in mhttpd when runinfo.state is corrupted

  Revision 1.251  2003/05/19 11:52:22  midas
  Fixed bug with content_length

  Revision 1.250  2003/05/17 15:59:05  midas
  Frontend displayed correctly when running FAL

  Revision 1.249  2003/05/14 09:20:27  midas
  Fixed bug with /CNAF

  Revision 1.248  2003/05/12 14:29:19  midas
  Added expiration header

  Revision 1.247  2003/05/09 14:03:22  midas
  Added 'labels=0/1' flag to history display

  Revision 1.246  2003/05/07 10:57:36  midas
  Removed tabs

  Revision 1.245  2003/05/02 09:03:01  midas
  Fixed buffer overflows by strlcpy()

  Revision 1.244  2003/04/25 14:37:43  midas
  Fixed compiler warnings

  Revision 1.243  2003/04/16 20:14:43  midas
  Fixed compiler warnings coming up with -Wuninitialized

  Revision 1.242  2003/04/16 19:05:49  olchansk
  Fix a few compiler warnings:
  - a format mismatch,
  - a few uninitialized variables,
  - removed a few unused variables

  Revision 1.241  2003/03/26 21:05:15  midas
  Switched to POST method for changing ODB values (for large multi-line entries)

  Revision 1.240  2003/03/26 16:19:32  midas
  Replaced 50000 by TEXT_SIZE

  Revision 1.239  2003/03/14 06:10:36  pierre
  - Fix find_odb_tag() for ctl character (LF)
  - Modify for 50K Custom weeb page

  Revision 1.238  2003/02/20 13:11:13  midas
  Fixed compiler warning

  Revision 1.237  2003/01/13 17:07:00  midas
  Fixed problem with missing history in recent files

  Revision 1.236  2002/10/19 06:36:56  midas
  Fixed problem with Mozilla 0.99

  Revision 1.235  2002/10/18 07:48:45  midas
  Fixed time consuming display of runlog.txt

  Revision 1.234  2002/10/07 04:06:59  olchansk
  teach mhttpd about PNG images.

  Revision 1.233  2002/07/01 07:57:05  midas
  Fixed bug with email notification and several experiments

  Revision 1.232  2002/06/03 06:07:15  midas
  Added extra parameter to ss_daemon_init to keep stdout

  Revision 1.231  2002/05/31 09:32:07  midas
  Drain mail server messages

  Revision 1.230  2002/05/31 09:26:41  midas
  Added '/Elog/Host name' for mail notification

  Revision 1.229  2002/05/31 06:53:01  midas
  Added debug info in sendmail()

  Revision 1.228  2002/05/31 05:55:26  midas
  Fixed bug in command line parsing, thanks to Gertjan Hofman

  Revision 1.227  2002/05/29 07:33:41  midas
  Increase WEB_BUFFER_SIZE and added warning if too small

  Revision 1.226  2002/05/29 07:25:12  midas
  Fixed bug with shutting down programs

  Revision 1.225  2002/05/28 11:31:49  midas
  Added debug output via '-d'

  Revision 1.224  2002/05/28 09:00:03  midas
  Fixed bug with opera browser

  Revision 1.223  2002/05/28 07:39:29  midas
  Added HELO statement in sendmail

  Revision 1.222  2002/05/22 16:49:45  midas
  Avoid restarting of mhttpd if only one experiment defined

  Revision 1.221  2002/05/22 05:46:44  midas
  Increased disconnect time to 1 day

  Revision 1.220  2002/05/22 05:27:37  midas
  Make history display configuration page work with unnamed arrays

  Revision 1.219  2002/05/21 23:02:36  midas
  Put <empty> in event selection list (problems with netsacpe) and fixed problem with followed links in history configuration

  Revision 1.218  2002/05/21 22:51:45  midas
  Adjust maxlength in start page according to string length

  Revision 1.217  2002/05/16 23:21:07  midas
  Fixed reconnection due to experiment names with different case

  Revision 1.216  2002/05/16 23:16:24  midas
  Fixed bug in history configuration page

  Revision 1.215  2002/05/16 18:01:13  midas
  Added subdir creation in logger and improved program restart scheme

  Revision 1.214  2002/05/15 04:20:19  midas
  Fixed wrong <br> in history panel list

  Revision 1.213  2002/05/15 00:24:41  midas
  Inactive logging channels displayed in yellow instead of red on status page

  Revision 1.212  2002/05/14 19:54:20  pierre
  small bug

  Revision 1.211  2002/05/14 19:17:35  midas
  Removed addition of .txt to files

  Revision 1.210  2002/05/14 06:33:36  midas
  Display link destination on ODB pages

  Revision 1.209  2002/05/14 04:24:04  midas
  Fixed small bug on history config page

  Revision 1.208  2002/05/13 21:53:18  midas
  Removed bars in run marker display

  Revision 1.207  2002/05/13 21:47:00  midas
  Avoid overwriting of run marker number

  Revision 1.206  2002/05/11 01:23:49  midas
  Fix wrong variable size in ODB

  Revision 1.205  2002/05/10 20:29:12  midas
  Fixed another small bug

  Revision 1.204  2002/05/10 20:18:30  stefan
  Fixed small bugs

  Revision 1.203  2002/05/10 19:22:03  midas
  Added history panel editor

  Revision 1.202  2002/05/10 01:41:19  midas
  Added optional debug output to cm_transition

  Revision 1.201  2002/05/08 22:27:24  midas
  Added 'more lines' button in runlog display

  Revision 1.200  2002/05/08 20:56:32  midas
  Added spaces between panel names in history display

  Revision 1.199  2002/05/08 19:54:40  midas
  Added extra parameter to function db_get_value()

  Revision 1.198  2002/05/08 18:34:43  midas
  Added 'last x' buttons in Elog display

  Revision 1.197  2002/05/08 17:56:12  midas
  Omit overflow in history display

  Revision 1.196  2002/05/08 16:39:25  midas
  Fixed problem with '+' in key names

  Revision 1.195  2002/05/07 22:41:23  midas
  Disabled lingering

  Revision 1.194  2002/03/19 08:04:07  midas
  Fixed few small bugs, thanks to Thomas Prokscha

  Revision 1.193  2002/03/13 08:36:51  midas
  Added periodic alarms, more buttons in Elog display

  Revision 1.192  2002/02/26 09:29:07  midas
  Fixed bug where query in elog stopped at 12:00

  Revision 1.191  2002/02/21 16:03:05  midas
  Fixed &nbsp

  Revision 1.190  2002/02/21 15:58:19  midas
  Fixed a few bugs, thanks to Thomas Prokscha

  Revision 1.189  2002/02/12 15:59:36  midas
  Improved time axis, added time query for history

  Revision 1.188  2002/02/05 04:55:16  midas
  Improved log axis limit calculation

  Revision 1.187  2002/02/04 03:00:10  midas
  Fixed bug with offset

  Revision 1.186  2002/02/04 02:41:21  midas
  Added offset for history display

  Revision 1.185  2002/02/04 00:34:12  midas
  Added zoom buttons in history display

  Revision 1.184  2002/02/02 11:34:29  midas
  Added run markes in history

  Revision 1.183  2002/02/01 13:15:23  midas
  Improved log axis labeling

  Revision 1.182  2002/01/31 13:25:43  midas
  Added log axis in history display and relative path names

  Revision 1.181  2002/01/31 09:43:22  midas
  Fixed small bug with 'Edit run number'

  Revision 1.180  2001/12/13 18:08:41  pierre
  fix Edit run number var

  Revision 1.179  2001/12/13 08:39:02  midas
  Added "/Experiment/Edit on start/Edit run number"

  Revision 1.178  2001/12/05 11:31:44  midas
  Changed creation of "/Logger/History dir"

  Revision 1.177  2001/12/04 08:36:37  midas
  Increased ref[80] to ref[256]

  Revision 1.176  2001/11/20 14:42:15  midas
  Added "/logger/history dir" and "/logger/elog dir"

  Revision 1.175  2001/11/19 12:03:22  midas
  Added event builder section

  Revision 1.174  2001/11/19 11:56:23  midas
  Fixed bug in rsputs

  Revision 1.173  2001/10/17 10:08:03  midas
  Speeded up return of long pages like HV slow control

  Revision 1.172  2001/10/15 09:24:26  midas
  Fixed small bug with new comment display on start page

  Revision 1.171  2001/10/12 12:08:42  midas
  If custom page /Custom/Status is present, it gets displayed instead of
  the standard status page

  Revision 1.170  2001/10/12 11:12:13  midas
  Added /Experiment/Parameter Comment feature

  Revision 1.169  2001/09/14 18:48:39  pierre
  corrected help link

  Revision 1.168  2001/08/30 09:05:58  midas
  Increased text box size to 80 in start run page

  Revision 1.167  2001/08/02 13:07:10  midas
  Fixed a bug in the mail system

  Revision 1.166  2001/08/02 12:52:54  midas
  Added email facility

  Revision 1.165  2001/07/31 12:20:39  midas
  Increased display limit to 1000

  Revision 1.164  2001/07/31 12:16:35  midas
  Don't display long (>500 values) arrays in ODB/SC any more

  Revision 1.163  2001/07/31 06:31:29  midas
  Fixed kb/s display for lazylogger

  Revision 1.162  2001/07/30 08:26:05  midas
  Fixed bug with automatich attachments in forms

  Revision 1.161  2001/07/25 09:08:03  midas
  Added attachments in Elog forms

  Revision 1.160  2001/07/25 08:15:08  midas
  Custom pages now work with multiple experiments. Do do that, one has to
  define the following field in the custom page:
  <input type=hidden name=exp value=[experiment]>
  where [experiment] is the experiment name

  Revision 1.159  2001/07/24 13:28:43  midas
  Use form names as subject

  Revision 1.158  2001/07/24 10:43:44  midas
  Added custom pages

  Revision 1.157  2001/07/23 07:02:48  midas
  Modified SC page to display more columns

  Revision 1.156  2001/07/23 06:54:25  midas
  Made last mod. (155) also work for single experiment

  Revision 1.155  2001/07/10 21:15:39  pierre
  - Added exp_name in Elog for last day request.

  Revision 1.154  2001/06/27 12:29:35  midas
  Added auto refresh to history page

  Revision 1.153  2001/06/15 09:13:09  midas
  Display "<" and ">" correctly

  Revision 1.152  2001/06/01 11:41:40  midas
  Fixed bug with expired password

  Revision 1.151  2001/05/23 08:16:40  midas
  Fixed bug when POST request comes in two blocks

  Revision 1.150  2001/05/22 09:26:48  midas
  Fixed bug with rsprintf/rsputs

  Revision 1.149  2001/05/15 03:03:51  pierre
  - Add "orange" color for disabled equipment

  Revision 1.148  2000/12/18 08:08:03  midas
  Added subtree with symbolic links in /Scripts system

  Revision 1.147  2000/12/15 08:52:58  midas
  Added "/Script" facility and "/Alias new window" tree

  Revision 1.146  2000/11/14 12:19:23  midas
  Fixed bug in cm_msg_retrieve, added improved "more" feature in message display

  Revision 1.145  2000/11/06 14:18:54  midas
  Don't draw history polygons if no data points present

  Revision 1.144  2000/10/12 01:08:02  pierre
  Fix ? for All panel

  Revision 1.143  2000/10/06 08:34:58  midas
  Fixed bug which did not display "all" history panel if experiment is defined

  Revision 1.142  2000/09/29 20:04:05  pierre
  - Fix & for TID_STRING

  Revision 1.141  2000/09/28 13:02:05  midas
  Added manual triggered events

  Revision 1.140  2000/09/26 06:54:17  midas
  Updated http link to Midas manual

  Revision 1.139  2000/08/28 11:04:17  midas
  Optimized header decoding

  Revision 1.138  2000/08/24 14:39:35  midas
  Added password check for slow control value change

  Revision 1.137  2000/08/23 10:57:47  midas
  Break lines if more than 10 history panels

  Revision 1.136  2000/08/22 14:25:27  midas
  Changed BG color of program list on status page (avoid yellow)

  Revision 1.135  2000/08/21 12:43:09  midas
  Added "last x days" feature in Elog

  Revision 1.134  2000/08/14 10:42:47  midas
  Display error message if variable not found in history file

  Revision 1.133  2000/08/11 07:30:59  midas
  Reworked chaning of Slow Control values:
  - /Equipment/<name>/Settings/Editable can contain a list of values which
    can be chanted
  - When clicking a variable, a in-place edit field opens instead of jumping
    into the ODB edit mode and never coming back

  Revision 1.132  2000/06/20 07:24:59  midas
  Increased history/display/<panel>/name length from 32 to 64

  Revision 1.131  2000/06/15 06:54:43  midas
  Fixed bug with ymin == ymax

  Revision 1.130  2000/06/06 10:52:27  midas
  Increase max. number of colums in slow control page to 9

  Revision 1.129  2000/05/24 08:00:32  midas
  Use gethostbyaddr only when creating new elog entry -> fix hangups

  Revision 1.128  2000/05/23 13:45:48  midas
  Added possibility to use local files as attachments (via /EL/?cmd=New&file=xxx)

  Revision 1.127  2000/05/16 12:13:42  midas
  Use "Logbook" in elog mode everywhere

  Revision 1.126  2000/05/16 11:47:37  midas
  Change "experiment" to "logbook" in elog mode

  Revision 1.125  2000/05/11 14:13:04  midas
  Fixed two bugs

  Revision 1.124  2000/05/08 15:19:36  midas
  Display alarm condition for internal alarms

  Revision 1.123  2000/05/08 14:29:38  midas
  Added delete option in ELog

  Revision 1.122  2000/05/08 08:38:28  midas
  Run number display can be switched off via /Elog/Display run number

  Revision 1.121  2000/05/05 12:19:51  midas
  Added offset in history display

  Revision 1.120  2000/05/05 08:32:05  midas
  Fixe bug with wrong state when running mhttpd on a new ODB

  Revision 1.119  2000/05/04 12:53:31  midas
  Fixed bug with locking author (strip @host)

  Revision 1.118  2000/05/04 12:03:59  midas
  Added -E for ELog mode (only display elog pages, no experiment status etc.)

  Revision 1.117  2000/05/04 08:10:30  midas
  Fixed bug with wrong factor in history display

  Revision 1.116  2000/05/02 14:56:58  midas
  Fixed bug with '/' and '\' in history attachments

  Revision 1.115  2000/05/02 14:15:39  midas
  Fixed bug with number of curves being displayed

  Revision 1.114  2000/05/02 14:03:18  midas
  Added alarm level display and display of individual curves

  Revision 1.113  2000/04/28 15:27:14  midas
  Adjusted colors

  Revision 1.112  2000/04/28 14:48:58  midas
  Added ELog from History capability

  Revision 1.111  2000/04/28 09:48:34  midas
  Added history config button

  Revision 1.110  2000/04/28 09:10:46  midas
  Added history picture display

  Revision 1.109  2000/04/20 13:41:39  midas
  Display alarm class instead "Alarm!"

  Revision 1.108  2000/04/03 12:52:24  midas
  Chaned table colors

  Revision 1.107  2000/04/03 12:26:48  midas
  Added ODB attachments

  Revision 1.106  2000/03/29 09:14:02  midas
  Increased parameter name length to 256

  Revision 1.105  2000/03/21 13:48:06  midas
  Web password expires after one day instead of one hour

  Revision 1.104  2000/03/18 08:02:32  midas
  Increased idle time logout to 1h

  Revision 1.103  2000/03/17 13:10:49  midas
  Make port bind check before becoming a daemon

  Revision 1.102  2000/03/15 08:54:39  midas
  Removed web passwd check from "submit" button (necessary if someone edits
  an elog entry for more than an hour)

  Revision 1.101  2000/03/13 09:41:53  midas
  Added refresh of Web password

  Revision 1.100  2000/03/08 23:43:49  midas
  Fixed bug that wrong size was given to el_retrieve

  Revision 1.99  2000/03/08 22:42:01  midas
  Fixed problem with elog froms and several experiments

  Revision 1.98  2000/03/03 01:45:13  midas
  Added web password for mhttpd, added webpasswd command in odbedit

  Revision 1.97  2000/03/01 23:04:49  midas
  Avoid analyzer ratio > 1

  Revision 1.96  2000/03/01 00:53:14  midas
  Use double events_sent values

  Revision 1.95  2000/02/25 23:41:02  midas
  Fixed secondary problem with conversion flags, adjusted mhttpd display of
  event number (M and G)

  Revision 1.94  2000/02/25 22:19:09  midas
  Improved Ctrl-C handling

  Revision 1.93  2000/02/24 22:29:24  midas
  Added deferred transitions

  Revision 1.92  1999/12/08 12:12:22  midas
  Modified "&str" to "str"

  Revision 1.91  1999/11/26 10:49:58  midas
  Display alarym system on/off flag on main status page

  Revision 1.90  1999/11/23 10:17:55  midas
  SC equipment is now normally displayed if no equipment settings are defined

  Revision 1.89  1999/11/11 12:46:36  midas
  Changed alias bar color

  Revision 1.88  1999/11/11 11:53:10  midas
  Link to auto restart and alarms on/off directly goes to "set" page

  Revision 1.87  1999/11/10 10:39:35  midas
  Changed text field size for ODB set page

  Revision 1.86  1999/11/08 14:31:09  midas
  Added hotlink to auto restart in status page

  Revision 1.85  1999/11/08 13:56:09  midas
  Added different alarm types

  Revision 1.84  1999/10/28 13:26:16  midas
  Added "alarms on/off" button

  Revision 1.83  1999/10/28 09:01:25  midas
  Added show_error to display proper HTTP header in error messages

  Revision 1.82  1999/10/22 10:48:25  midas
  Added auto restart in status page

  Revision 1.81  1999/10/20 09:22:39  midas
  Added -c flag

  Revision 1.80  1999/10/19 11:04:13  midas
  Changed expiration date of midas_ref cookie to one year

  Revision 1.79  1999/10/18 11:47:33  midas
  Corrected link to lazy settings

  Revision 1.78  1999/10/15 12:15:52  midas
  Added daemon function

  Revision 1.77  1999/10/15 10:54:03  midas
  Changed column width in elog page

  Revision 1.76  1999/10/15 10:39:51  midas
  Defined function rsputs, because rsprintf screwed if '%' in arguments

  Revision 1.74  1999/10/11 14:15:29  midas
  Use ss_system to launch programs

  Revision 1.73  1999/10/11 12:13:43  midas
  Increased socket timeout

  Revision 1.72  1999/10/11 11:57:39  midas
  Fixed bug with search in full text

  Revision 1.71  1999/10/11 10:52:43  midas
  Added full text search feature

  Revision 1.70  1999/10/11 10:40:44  midas
  Fixed bug with form submit

  Revision 1.69  1999/10/08 22:00:29  midas
  Finished editing of elog messages

  Revision 1.68  1999/10/08 15:07:04  midas
  Program check creates new internal alarm when triggered

  Revision 1.67  1999/10/08 08:38:09  midas
  Added "last<n> messages" feature

  Revision 1.66  1999/10/07 14:04:49  midas
  Added select() before recv() to avoid hangups

  Revision 1.65  1999/10/07 13:31:18  midas
  Fixed truncated date in el_submit, cut off @host in author search

  Revision 1.64  1999/10/07 13:17:34  midas
  Put a few EXPRT im msystem.h to make NT happy, updated NT makefile

  Revision 1.63  1999/10/06 18:01:12  midas
  Fixed bug with subject in new messages

  Revision 1.62  1999/10/06 07:57:55  midas
  Added "last 10 messages" feature

  Revision 1.61  1999/10/06 07:04:59  midas
  Edit mode half finished

  Revision 1.60  1999/10/05 13:36:15  midas
  Use strencode

  Revision 1.59  1999/10/05 13:25:43  midas
  Evaluate global alarm flag

  Revision 1.58  1999/10/05 12:48:17  midas
  Added expiration date 2040 for refresh cookie

  Revision 1.57  1999/10/04 14:16:10  midas
  Fixed bug in form submit

  Revision 1.56  1999/10/04 14:11:43  midas
  Added full display for query

  Revision 1.55  1999/09/29 21:08:54  pierre
  - Modified the Lazy status line for multiple channels

  Revision 1.54  1999/09/28 10:24:15  midas
  Display text attachments

  Revision 1.52  1999/09/27 12:53:50  midas
  Finished alarm system

  Revision 1.51  1999/09/27 09:18:05  midas
  Fixed bug

  Revision 1.50  1999/09/27 09:14:54  midas
  Use _blank for new windows

  Revision 1.49  1999/09/27 09:11:13  midas
  Check domain name in hostname

  Revision 1.48  1999/09/27 08:57:29  midas
  Use <pre> for plain text

  Revision 1.46  1999/09/22 08:57:08  midas
  Implemented auto start and auto stop in /programs

  Revision 1.45  1999/09/21 14:57:39  midas
  Added "execute on start/stop" under /programs

  Revision 1.44  1999/09/21 14:41:35  midas
  Close server socket before system() call

  Revision 1.43  1999/09/21 14:15:03  midas
  Replaces cm_execute by system()

  Revision 1.42  1999/09/21 13:47:39  midas
  Added "programs" page

  Revision 1.41  1999/09/17 15:59:03  midas
  Added internal alarms

  Revision 1.40  1999/09/17 15:11:18  midas
  Fixed bug (triggered is INT, not BOOL)

  Revision 1.39  1999/09/17 15:06:46  midas
  Moved al_check into cm_yield() and rpc_server_thread

  Revision 1.38  1999/09/17 11:47:52  midas
  Added setgid()

  Revision 1.36  1999/09/16 07:36:10  midas
  Added automatic host name in author field

  Revision 1.35  1999/09/15 13:33:32  midas
  Added remote el_submit functionality

  Revision 1.34  1999/09/15 09:33:05  midas
  Re-establish ctrlc-handler

  Revision 1.33  1999/09/15 09:30:49  midas
  Added Ctrl-C handler

  Revision 1.32  1999/09/15 08:05:09  midas
  - Added "last" button
  - Fixed bug that only partial file attachments were returned
  - Corrected query title, since dates are sorted beginning with oldest

  Revision 1.31  1999/09/14 15:15:44  midas
  Moved el_xxx funtions into midas.c

  Revision 1.30  1999/09/14 13:06:32  midas
  Added raw file display (for runlog.txt for now...)

  Revision 1.29  1999/09/14 11:45:25  midas
  Finished run number query

  Revision 1.28  1999/09/14 09:47:36  midas
  Fixed '/' and '\' handling with attachments

  Revision 1.27  1999/09/14 09:20:18  midas
  Finished reply

  Revision 1.26  1999/09/13 15:45:50  midas
  Added forms

  Revision 1.25  1999/09/13 09:50:16  midas
  Finished filtered browsing

  Revision 1.24  1999/09/10 06:10:46  midas
  Reply half finished

  Revision 1.23  1999/09/02 15:02:13  midas
  Display attachments

  Revision 1.22  1999/09/02 10:17:16  midas
  Implemented POST method

  Revision 1.21  1999/08/31 15:12:02  midas
  Finished query

  Revision 1.20  1999/08/27 08:14:26  midas
  Added query

  Revision 1.19  1999/08/26 15:19:05  midas
  Added Next/Previous button

  Revision 1.18  1999/08/24 13:45:44  midas
  - Made Ctrl-C working
  - Re-enabled reuse of port address
  - Added http:// destinations to /alias
  - Made refresh working under kfm (05 instead 5!)

  Revision 1.17  1999/08/13 09:41:07  midas
  Replaced getdomainname by gethostbyaddr

  Revision 1.14  1999/08/12 15:44:50  midas
  Fixed bug when subkey was in variables list, added domainname for unit

  Revision 1.13  1999/07/01 11:12:32  midas
  Changed lazy display if in FTP mode

  Revision 1.12  1999/06/23 09:39:31  midas
  Added lazy logger display

  Revision 1.11  1999/06/05 13:59:48  midas
  Converted ftp entry into ftp://host/dir/file

  Revision 1.10  1999/04/19 07:48:21  midas
  Message display uses cm_msg_retrieve to display old messages

  Revision 1.9  1999/02/18 11:23:50  midas
  Added level parameter to search_callback

  Revision 1.8  1999/01/20 13:51:56  midas
  Fixed some bugs in displaying/setting slow control values

  Revision 1.7  1998/12/11 11:15:50  midas
  Rearranged URL to make it work under the KDE browser, but auto refresh still
  does not work there.

  Revision 1.6  1998/10/28 13:48:17  midas
  Added message display

  Revision 1.5  1998/10/28 12:39:45  midas
  Added hex display in ODB page

  Revision 1.4  1998/10/28 12:25:13  midas
  Fixed bug causing comments in start page being truncated

  Revision 1.3  1998/10/23 14:21:49  midas
  - Modified version scheme from 1.06 to 1.6.0
  - cm_get_version() now returns versino as string

  Revision 1.2  1998/10/12 12:19:02  midas
  Added Log tag in header


\********************************************************************/

#include <math.h>
#include <assert.h>
#include "midas.h"
#include "msystem.h"
#include "mgd.h"

/* refresh times in seconds */
#define DEFAULT_REFRESH 60

/* time until mhttpd disconnects from MIDAS */
#define CONNECT_TIME  3600*24

/* size of buffer for incoming data, must fit sum of all attachments */
#define WEB_BUFFER_SIZE 10000000

char return_buffer[WEB_BUFFER_SIZE];
int strlen_retbuf;
int return_length;
char host_name[256];
char exp_name[32];
BOOL connected, no_disconnect;
int tcp_port = 80;

#define MAX_GROUPS    32
#define MAX_PARAM    100
#define VALUE_SIZE   256
#define PARAM_LENGTH 256
#define TEXT_SIZE  50000

char _param[MAX_PARAM][PARAM_LENGTH];
char _value[MAX_PARAM][VALUE_SIZE];
char _text[TEXT_SIZE];
char *_attachment_buffer[3];
INT _attachment_size[3];
struct in_addr remote_addr;
INT _sock;
BOOL elog_mode = FALSE;
BOOL verbose = FALSE;

char *mname[] = {
   "January",
   "February",
   "March",
   "April",
   "May",
   "June",
   "July",
   "August",
   "September",
   "October",
   "November",
   "December"
};

char type_list[20][NAME_LENGTH] = {
   "Routine",
   "Shift summary",
   "Minor error",
   "Severe error",
   "Fix",
   "Question",
   "Info",
   "Modification",
   "Reply",
   "Alarm",
   "Test",
   "Other"
};

char system_list[20][NAME_LENGTH] = {
   "General",
   "DAQ",
   "Detector",
   "Electronics",
   "Target",
   "Beamline"
};


struct {
   char ext[32];
   char type[32];
} filetype[] = {

   {
   ".JPG", "image/jpeg",}, {
   ".GIF", "image/gif",}, {
   ".PNG", "image/png",}, {
   ".PS", "application/postscript",}, {
   ".EPS", "application/postscript",}, {
   ".HTML", "text/html",}, {
   ".HTM", "text/html",}, {
   ".XLS", "application/x-msexcel",}, {
   ".DOC", "application/msword",}, {
   ".PDF", "application/pdf",}, {
   ".TXT", "text/plain",}, {
   ".ASC", "text/plain",}, {
   ".ZIP", "application/x-zip-compressed",}, {
""},};

void show_hist_page(char *path, char *buffer, int *buffer_size, int refresh);

/*------------------------------------------------------------------*/

void rsputs(const char *str)
{
   if (strlen_retbuf + strlen(str) > sizeof(return_buffer)) {
      strcpy(return_buffer, "<H1>Error: return buffer too small</H1>");
      strlen_retbuf = strlen(return_buffer);
   } else {
      strcpy(return_buffer + strlen_retbuf, str);
      strlen_retbuf += strlen(str);
   }
}

/*------------------------------------------------------------------*/

void rsputs2(const char *str)
{
   int i, j, k;
   char *p, link[256];

   if (strlen_retbuf + strlen(str) > sizeof(return_buffer)) {
      strcpy(return_buffer, "<H1>Error: return buffer too small</H1>");
      strlen_retbuf = strlen(return_buffer);
   } else {
      j = strlen_retbuf;
      for (i = 0; i < (int) strlen(str); i++) {
         if (strncmp(str + i, "http://", 7) == 0) {
            p = (char *) (str + i + 7);
            i += 7;
            for (k = 0; *p && *p != ' ' && *p != '\n'; k++, i++)
               link[k] = *p++;
            link[k] = 0;

            sprintf(return_buffer + j, "<a href=\"http://%s\">http://%s</a>", link, link);
            j += strlen(return_buffer + j);
         } else
            switch (str[i]) {
            case '<':
               strcat(return_buffer, "&lt;");
               j += 4;
               break;
            case '>':
               strcat(return_buffer, "&gt;");
               j += 4;
               break;
            default:
               return_buffer[j++] = str[i];
            }
      }

      return_buffer[j] = 0;
      strlen_retbuf = j;
   }
}

/*------------------------------------------------------------------*/

void rsprintf(const char *format, ...)
{
   va_list argptr;
   char str[10000];

   va_start(argptr, format);
   vsprintf(str, (char *) format, argptr);
   va_end(argptr);

   if (strlen_retbuf + strlen(str) > sizeof(return_buffer))
      strcpy(return_buffer, "<H1>Error: return buffer too small</H1>");
   else
      strcpy(return_buffer + strlen_retbuf, str);

   strlen_retbuf += strlen(str);
}

/*------------------------------------------------------------------*/

/* Parameter handling functions similar to setenv/getenv */

void initparam()
{
   memset(_param, 0, sizeof(_param));
   memset(_value, 0, sizeof(_value));
   _text[0] = 0;
}

void setparam(char *param, char *value)
{
   int i;

   if (equal_ustring(param, "text")) {
      if (strlen(value) >= TEXT_SIZE)
         printf("Error: parameter value too big\n");

      strlcpy(_text, value, TEXT_SIZE);
      _text[TEXT_SIZE - 1] = 0;
      return;
   }

   for (i = 0; i < MAX_PARAM; i++)
      if (_param[i][0] == 0)
         break;

   if (i < MAX_PARAM) {
      strcpy(_param[i], param);

      if (strlen(value) >= VALUE_SIZE)
         printf("Error: parameter value too big\n");

      strlcpy(_value[i], value, VALUE_SIZE);
      _value[i][VALUE_SIZE - 1] = 0;
   } else {
      printf("Error: parameter array too small\n");
   }
}

char *getparam(char *param)
{
   int i;

   if (equal_ustring(param, "text"))
      return _text;

   for (i = 0; i < MAX_PARAM && _param[i][0]; i++)
      if (equal_ustring(param, _param[i]))
         break;

   if (i < MAX_PARAM)
      return _value[i];

   return NULL;
}

BOOL isparam(char *param)
{
   int i;

   for (i = 0; i < MAX_PARAM && _param[i][0]; i++)
      if (equal_ustring(param, _param[i]))
         break;

   if (i < MAX_PARAM && _param[i][0])
      return TRUE;

   return FALSE;
}

void unsetparam(char *param)
{
   int i;

   for (i = 0; i < MAX_PARAM; i++)
      if (equal_ustring(param, _param[i]))
         break;

   if (i < MAX_PARAM) {
      _param[i][0] = 0;
      _value[i][0] = 0;
   }
}

/*------------------------------------------------------------------*/

void urlDecode(char *p)
/********************************************************************\
   Decode the given string in-place by expanding %XX escapes
\********************************************************************/
{
   char *pD, str[3];
   int i;

   pD = p;
   while (*p) {
      if (*p == '%') {
         /* Escape: next 2 chars are hex representation of the actual character */
         p++;
         if (isxdigit(p[0]) && isxdigit(p[1])) {
            str[0] = p[0];
            str[1] = p[1];
            str[2] = 0;
            sscanf(p, "%02X", &i);

            *pD++ = (char) i;
            p += 2;
         } else
            *pD++ = '%';
      } else if (*p == '+') {
         /* convert '+' to ' ' */
         *pD++ = ' ';
         p++;
      } else {
         *pD++ = *p++;
      }
   }
   *pD = '\0';
}

void urlEncode(char *ps)
/********************************************************************\
   Encode the given string in-place by adding %XX escapes
\********************************************************************/
{
   char *pd, *p, str[256];

   pd = str;
   p = ps;
   while (*p) {
      if (strchr(" %&=+#\"'", *p)) {
         sprintf(pd, "%%%02X", *p);
         pd += 3;
         p++;
      } else {
         *pd++ = *p++;
      }
   }
   *pd = '\0';
   strcpy(ps, str);
}

/*------------------------------------------------------------------*/

char message_buffer[256] = "";

INT print_message(const char *message)
{
   strcpy(message_buffer, message);
   return SUCCESS;
}

void receive_message(HNDLE hBuf, HNDLE id, EVENT_HEADER * pheader, void *message)
{
   time_t tm;
   char str[80], line[256];

   /* prepare time */
   time(&tm);
   strcpy(str, ctime(&tm));
   str[19] = 0;

   /* print message text which comes after event header */
   strcpy(line, str + 11);
   strlcat(line, (char *) message, sizeof(line));
   print_message(line);
}

/*-------------------------------------------------------------------*/

INT sendmail(char *smtp_host, char *from, char *to, char *subject, char *text)
{
   struct sockaddr_in bind_addr;
   struct hostent *phe;
   int i, s, strsize, offset;
   char *str, *p, buf[256];
   time_t now;
   struct tm *ts;

   if (verbose)
      printf("\n\nEmail from %s to %s, SMTP host %s:\n", from, to, smtp_host);

   /* create a new socket for connecting to remote server */
   s = socket(AF_INET, SOCK_STREAM, 0);
   if (s == -1)
      return -1;

   /* connect to remote node port 25 */
   memset(&bind_addr, 0, sizeof(bind_addr));
   bind_addr.sin_family = AF_INET;
   bind_addr.sin_port = htons((short) 25);

   phe = gethostbyname(smtp_host);
   if (phe == NULL)
      return -1;
   memcpy((char *) &(bind_addr.sin_addr), phe->h_addr, phe->h_length);

   if (connect(s, (void *) &bind_addr, sizeof(bind_addr)) < 0) {
      closesocket(s);
      return -1;
   }

   strsize = TEXT_SIZE + 1000;
   str = malloc(strsize);

   recv_string(s, str, strsize, 3000);
   if (verbose)
      puts(str);

   /* drain server messages */
   do {
      str[0] = 0;
      recv_string(s, str, strsize, 300);
      if (verbose)
         puts(str);
   } while (str[0]);

   sprintf(str, "HELO %s\r\n", host_name);
   send(s, str, strlen(str), 0);
   if (verbose)
      puts(str);
   recv_string(s, str, strsize, 3000);
   if (verbose)
      puts(str);

   if (strchr(from, '<')) {
      strlcpy(buf, strchr(from, '<')+1, sizeof(buf));
      if (strchr(buf, '>'))
         *strchr(buf, '>') = 0;
   } else
      strlcpy(buf, from, sizeof(buf));

   sprintf(str, "MAIL FROM: %s\n", buf);
   send(s, str, strlen(str), 0);
   if (verbose)
      puts(str);
   recv_string(s, str, strsize, 3000);
   if (verbose)
      puts(str);

   sprintf(str, "RCPT TO: <%s>\r\n", to);
   send(s, str, strlen(str), 0);
   if (verbose)
      puts(str);
   recv_string(s, str, strsize, 3000);
   if (verbose)
      puts(str);

   sprintf(str, "DATA\r\n");
   send(s, str, strlen(str), 0);
   if (verbose)
      puts(str);
   recv_string(s, str, strsize, 3000);
   if (verbose)
      puts(str);

   sprintf(str, "To: %s\r\nFrom: %s\r\nSubject: %s\r\n", to, from, subject);
   send(s, str, strlen(str), 0);
   if (verbose)
      puts(str);

   sprintf(str, "X-Mailer: mhttpd, midas version %s\r\n", MIDAS_VERSION);
   send(s, str, strlen(str), 0);
   if (verbose)
      puts(str);

   time(&now);
   ts = localtime(&now);
   strftime(buf, sizeof(buf), "%a, %d %b %Y %H:%M:%S", ts);
   offset = (-(int) timezone);
   if (ts->tm_isdst)
      offset += 3600;
   sprintf(str, "Date: %s %+03d%02d\r\n", buf, (int) (offset / 3600),
            (int) ((abs((int) offset) / 60) % 60));
   send(s, str, strlen(str), 0);
   if (verbose)
      puts(str);

   sprintf(str, "Content-Type: TEXT/PLAIN; charset=US-ASCII\r\n\r\n");
   send(s, str, strlen(str), 0);
   if (verbose)
      puts(str);

   /* analyze text for "." at beginning of line */
   p = text;
   str[0] = 0;
   while (strstr(p, "\r\n.\r\n")) {
      i = (int) strstr(p, "\r\n.\r\n") - (int) p + 1;
      strlcat(str, p, i);
      p += i + 4;
      strlcat(str, "\r\n..\r\n", strsize);
   }
   strlcat(str, p, strsize);
   strlcat(str, "\r\n", strsize);
   send(s, str, strlen(str), 0);
   if (verbose)
      puts(str);

   /* send ".<CR>" to signal end of message */
   sprintf(str, ".\r\n");
   send(s, str, strlen(str), 0);
   if (verbose)
      puts(str);
   recv_string(s, str, strsize, 3000);
   if (verbose)
      puts(str);

   sprintf(str, "QUIT\n");
   send(s, str, strlen(str), 0);
   if (verbose)
      puts(str);
   recv_string(s, str, strsize, 3000);
   if (verbose)
      puts(str);

   closesocket(s);
   free(str);

   return 1;
}

/*------------------------------------------------------------------*/

void redirect(char *path)
{
   /* redirect */
   rsprintf("HTTP/1.0 302 Found\r\n");
   rsprintf("Server: MIDAS HTTP %s\r\n", cm_get_version());
   rsprintf("Content-Type: text/html; charset=iso-8859-1\r\n");

   if (exp_name[0]) {
      if (strchr(path, '?'))
         rsprintf("Location: /%s&exp=%s\n\n<html>redir</html>\r\n", path, exp_name);
      else
         rsprintf("Location: /%s?exp=%s\n\n<html>redir</html>\r\n", path, exp_name);
   } else
      rsprintf("Location: /%s\r\n\r\n<html>redir</html>\r\n", path);
}

void redirect2(char *path)
{
   redirect(path);
   send_tcp(_sock, return_buffer, strlen(return_buffer) + 1, 0);
   closesocket(_sock);
   return_length = -1;
}

/*------------------------------------------------------------------*/

INT search_callback(HNDLE hDB, HNDLE hKey, KEY * key, INT level, void *info)
{
   INT i, size, status;
   char *search_name, *p;
   static char data_str[MAX_ODB_PATH];
   static char str1[MAX_ODB_PATH], str2[MAX_ODB_PATH], ref[MAX_ODB_PATH];
   char path[MAX_ODB_PATH], data[10000];

   search_name = (char *) info;

   /* convert strings to uppercase */
   for (i = 0; key->name[i]; i++)
      str1[i] = toupper(key->name[i]);
   str1[i] = 0;
   for (i = 0; key->name[i]; i++)
      str2[i] = toupper(search_name[i]);
   str2[i] = 0;

   if (strstr(str1, str2) != NULL) {
      db_get_path(hDB, hKey, str1, MAX_ODB_PATH);
      strcpy(path, str1 + 1);   /* strip leading '/' */
      strcpy(str1, path);
      urlEncode(str1);

      if (key->type == TID_KEY || key->type == TID_LINK) {
         /* for keys, don't display data value */
         if (exp_name[0])
            rsprintf("<tr><td bgcolor=#FFD000><a href=\"/?exp=%s&path=%s\">%s</a></tr>\n",
                     exp_name, str1, path);
         else
            rsprintf("<tr><td bgcolor=#FFD000><a href=\"/%s\">%s</a></tr>\n", str1, path);
      } else {
         /* strip variable name from path */
         p = path + strlen(path) - 1;
         while (*p && *p != '/')
            *p-- = 0;
         if (*p == '/')
            *p = 0;

         /* display single value */
         if (key->num_values == 1) {
            size = sizeof(data);
            status = db_get_data(hDB, hKey, data, &size, key->type);
            if (status == DB_NO_ACCESS)
               strcpy(data_str, "<no read access>");
            else
               db_sprintf(data_str, data, key->item_size, 0, key->type);

            if (exp_name[0])
               sprintf(ref, "/%s?cmd=Set&exp=%s", str1, exp_name);
            else
               sprintf(ref, "/%s?cmd=Set", str1);

            rsprintf("<tr><td bgcolor=#FFFF00>");

            if (exp_name[0])
               rsprintf("<a href=\"/%s?exp=%s\">%s</a>/%s", path, exp_name, path,
                        key->name);
            else
               rsprintf("<a href=\"/%s\">%s</a>/%s", path, path, key->name);

            rsprintf("<td><a href=\"%s\">%s</a></tr>\n", ref, data_str);
         } else {
            /* display first value */
            rsprintf("<tr><td rowspan=%d bgcolor=#FFFF00>%s\n", key->num_values, path);

            for (i = 0; i < key->num_values; i++) {
               size = sizeof(data);
               db_get_data(hDB, hKey, data, &size, key->type);
               db_sprintf(data_str, data, key->item_size, i, key->type);

               if (exp_name[0])
                  sprintf(ref, "/%s?cmd=Set&index=%d&exp=%s", str1, i, exp_name);
               else
                  sprintf(ref, "/%s?cmd=Set&index=%d", str1, i);

               if (i > 0)
                  rsprintf("<tr>");

               rsprintf("<td><a href=\"%s\">[%d] %s</a></tr>\n", ref, i, data_str);
            }
         }
      }
   }

   return SUCCESS;
}

/*------------------------------------------------------------------*/

void show_help_page()
{
   rsprintf("<html><head>\n");
   rsprintf("<title>MIDAS WWW Gateway Help</title>\n");
   rsprintf("</head>\n\n");

   rsprintf("<body>\n");
   rsprintf("<h1>Using the MIDAS WWW Gateway</h1>\n");
   rsprintf("With the MIDAS WWW Gateway basic experiment control can be achieved.\n");
   rsprintf("The status page displays the current run status including front-end\n");
   rsprintf("and logger statistics. The Start and Stop buttons can start and stop\n");
   rsprintf("a run. The ODB button switches into the Online Database mode, where\n");
   rsprintf
       ("the contents of the experiment database can be displayed and modified.<P>\n\n");

   rsprintf("For more information, refer to the\n");
   rsprintf("<A HREF=\"http://midas.psi.ch/htmldoc/index.html\">PSI MIDAS manual</A>,\n");
   rsprintf
       ("<A HREF=\"http://midas.triumf.ca/doc/html/index.html\">Triumf MIDAS manual</A>.<P>\n\n");

   rsprintf("<hr>\n");
   rsprintf("<address>\n");
   rsprintf("<a href=\"http://pibeta.psi.ch/~stefan\">S. Ritt</a>, 26 Sep 2000");
   rsprintf("</address>");

   rsprintf("</body></html>\r\n");
}

/*------------------------------------------------------------------*/

void show_header(HNDLE hDB, char *title, char *method, char *path, int colspan,
                 int refresh)
{
   time_t now;
   char str[256];
   int size;

   /* header */
   rsprintf("HTTP/1.0 200 Document follows\r\n");
   rsprintf("Server: MIDAS HTTP %s\r\n", cm_get_version());
   rsprintf("Pragma: no-cache\r\n");
   rsprintf("Expires: Fri, 01 Jan 1983 00:00:00 GMT\r\n");
   rsprintf("Content-Type: text/html; charset=iso-8859-1\r\n\r\n");

   rsprintf("<!DOCTYPE HTML PUBLIC \"-//IETF//DTD HTML 2.0//EN\">\n");
   rsprintf("<html><head>\n");

   /* auto refresh */
   if (refresh > 0)
      rsprintf("<meta http-equiv=\"Refresh\" content=\"%02d\">\n", refresh);

   rsprintf("<title>%s</title></head>\n", title);

   if (equal_ustring(method, "POST"))
      rsprintf
          ("<body><form name=\"form1\" method=\"POST\" action=\"/%s\" enctype=\"multipart/form-data\">\n\n",
           path);
   else
      rsprintf("<body><form name=\"form1\" method=\"%s\" action=\"/%s\">\n\n", method,
               path);

   /* title row */

   size = sizeof(str);
   str[0] = 0;
   db_get_value(hDB, 0, "/Experiment/Name", str, &size, TID_STRING, TRUE);
   time(&now);

   /* define hidden field for experiment */
   if (exp_name[0])
      rsprintf("<input type=hidden name=exp value=\"%s\">\n", exp_name);

   rsprintf("<table border=3 cellpadding=2>\n");
   rsprintf("<tr><th colspan=%d bgcolor=\"#A0A0FF\">MIDAS experiment \"%s\"", colspan,
            str);

   if (refresh > 0)
      rsprintf("<th colspan=%d bgcolor=\"#A0A0FF\">%s &nbsp;&nbsp;Refr:%d</tr>\n",
               colspan, ctime(&now), refresh);
   else
      rsprintf("<th colspan=%d bgcolor=\"#A0A0FF\">%s</tr>\n", colspan, ctime(&now));
}

/*------------------------------------------------------------------*/

void show_error(char *error)
{
   /* header */
   rsprintf("HTTP/1.0 200 Document follows\r\n");
   rsprintf("Server: MIDAS HTTP %s\r\n", cm_get_version());
   rsprintf("Content-Type: text/html; charset=iso-8859-1\r\n\r\n");

   rsprintf("<html><head><title>MIDAS error</title></head>\n");
   rsprintf("<body><H1>%s</H1></body></html>\n", error);
}

/*------------------------------------------------------------------*/

void exec_script(HNDLE hkey)
/********************************************************************\

  Routine: exec_script

  Purpose: Execute script from /Script tree

  exec_script is enabled by the tree /Script
  The /Script struct is composed of list of keys
  from which the name of the key is the button name
  and the sub-structure is a record as follow:

  /Script/<button_name> = <script command> (TID_STRING)

  The "Script command", containing possible arguements,
  is directly executed.

  /Script/<button_name>/<script command>
                        <soft link1>|<arg1>
                        <soft link2>|<arg2>
                           ...

  The arguments for the script are derived from the
  subtree below <button_name>, where <button_name> must be
  TID_KEY. The subtree may then contain arguments or links
  to other values in the ODB, like run number etc.

\********************************************************************/
{
   INT i, size;
   KEY key;
   HNDLE hDB, hsubkey;
   char command[256];
   char data[1000], str[256];

   cm_get_experiment_database(&hDB, NULL);
   db_get_key(hDB, hkey, &key);
   command[0] = 0;

   if (key.type == TID_STRING) {
      size = sizeof(command);
      db_get_data(hDB, hkey, command, &size, TID_STRING);
   } else
      for (i = 0;; i++) {
         db_enum_key(hDB, hkey, i, &hsubkey);
         if (!hsubkey)
            break;
         db_get_key(hDB, hsubkey, &key);

         if (key.type != TID_KEY) {
            size = sizeof(data);
            db_get_data(hDB, hsubkey, data, &size, key.type);
            db_sprintf(str, data, key.item_size, 0, key.type);

            if (i > 0)
               strcat(command, " ");

            strcat(command, str);
         }
      }

   /* printf("exec_script: %s\n", command); */

   if (command[0])
      ss_system(command);

   return;
}

/*------------------------------------------------------------------*/

void show_status_page(int refresh, char *cookie_wpwd)
{
   int i, j, k, status, size, type;
   BOOL flag, first;
   char str[1000], name[32], ref[256], bgcol[32], fgcol[32], alarm_class[32];
   char *trans_name[] = { "Start", "Stop", "Pause", "Resume" };
   time_t now, difftime;
   double analyzed, analyze_ratio, d;
   float value;
   HNDLE hDB, hkey, hLKey, hsubkey, hkeytmp;
   KEY key;
   BOOL ftp_mode, previous_mode;
   char client_name[NAME_LENGTH];
   struct tm *gmt;
   BOOL new_window;

   RUNINFO_STR(runinfo_str);
   RUNINFO runinfo;
   EQUIPMENT_INFO equipment;
   EQUIPMENT_STATS equipment_stats;
   CHN_SETTINGS chn_settings;
   CHN_STATISTICS chn_stats;

   cm_get_experiment_database(&hDB, NULL);

   status = db_check_record(hDB, 0, "/Runinfo", strcomb(runinfo_str), FALSE);
   if (status == DB_STRUCT_MISMATCH) {
      cm_msg(MERROR, "show_status_page", "Aborting on mismatching /Runinfo structure");
      cm_disconnect_experiment();
      abort();
   }

   if (status != DB_SUCCESS) {
      cm_msg(MERROR, "show_status_page",
             "Aborting on invalid access to ODB /Runinfo, status=%d", status);
      cm_disconnect_experiment();
      abort();
   }

   db_find_key(hDB, 0, "/Runinfo", &hkey);
   assert(hkey);

   size = sizeof(runinfo);
   status = db_get_record(hDB, hkey, &runinfo, &size, 0);
   assert(status == DB_SUCCESS);

   /* header */
   rsprintf("HTTP/1.1 200 OK\r\n");
   rsprintf("Server: MIDAS HTTP %s\r\n", cm_get_version());
   rsprintf("Content-Type: text/html; charset=iso-8859-1\r\n");
   rsprintf("Pragma: no-cache\r\n");
   rsprintf("Expires: Fri, 01-Jan-1983 00:00:00 GMT\r\n");
   if (cookie_wpwd[0]) {
      time(&now);
      now += 3600 * 24;
      gmt = gmtime(&now);
      strftime(str, sizeof(str), "%A, %d-%b-%Y %H:%M:%S GMT", gmt);

      rsprintf("Set-Cookie: midas_wpwd=%s; path=/; expires=%s\r\n", cookie_wpwd, str);
   }

   rsprintf("\r\n<html>\n");

   /* auto refresh */
   if (refresh > 0)
      rsprintf("<head><meta http-equiv=\"Refresh\" content=\"%02d\">\n", refresh);

   rsprintf("<title>MIDAS status</title></head>\n");
   rsprintf("<body><form method=\"GET\" action=\"/\">\n");

   /* define hidden field for experiment */
   if (exp_name[0])
      rsprintf("<input type=hidden name=exp value=\"%s\">\n", exp_name);

   rsprintf("<table border=3 cellpadding=2>\n");

  /*---- title row ----*/

   size = sizeof(str);
   str[0] = 0;
   db_get_value(hDB, 0, "/Experiment/Name", str, &size, TID_STRING, TRUE);
   time(&now);

   rsprintf("<tr><th colspan=3 bgcolor=#A0A0FF>MIDAS experiment \"%s\"", str);
   rsprintf("<th colspan=3 bgcolor=#A0A0FF>%s &nbsp;&nbsp;Refr:%d</tr>\n", ctime(&now),
            refresh);

  /*---- menu buttons ----*/

   rsprintf("<tr><td colspan=6 bgcolor=#C0C0C0>\n");

   if (runinfo.state == STATE_STOPPED)
      rsprintf("<input type=submit name=cmd value=Start>\n");
   else if (runinfo.state == STATE_PAUSED)
      rsprintf("<input type=submit name=cmd value=Resume>\n");
   else
      rsprintf
          ("<input type=submit name=cmd value=Stop>\n<input type=submit name=cmd value=Pause>\n");

   rsprintf("<input type=submit name=cmd value=ODB>\n");
   rsprintf("<input type=submit name=cmd value=CNAF>\n");
   rsprintf("<input type=submit name=cmd value=Messages>\n");
   rsprintf("<input type=submit name=cmd value=ELog>\n");
   rsprintf("<input type=submit name=cmd value=Alarms>\n");
   rsprintf("<input type=submit name=cmd value=Programs>\n");
   rsprintf("<input type=submit name=cmd value=History>\n");
   rsprintf("<input type=submit name=cmd value=Config>\n");
   rsprintf("<input type=submit name=cmd value=Help>\n");

  /*---- script buttons ----*/

   status = db_find_key(hDB, 0, "Script", &hkey);
   if (status == DB_SUCCESS) {
      rsprintf("<tr><td colspan=6 bgcolor=#D0D0D0>\n");

      for (i = 0;; i++) {
         db_enum_link(hDB, hkey, i, &hsubkey);
         if (!hsubkey)
            break;
         db_get_key(hDB, hsubkey, &key);
         rsprintf("<input type=submit name=script value=\"%s\">\n", key.name);
      }
   }

   rsprintf("</tr>\n\n");

  /*---- alarms ----*/

   /* go through all triggered alarms */
   db_find_key(hDB, 0, "/Alarms/Alarms", &hkey);
   if (hkey) {
      /* check global alarm flag */
      flag = TRUE;
      size = sizeof(flag);
      db_get_value(hDB, 0, "/Alarms/Alarm System active", &flag, &size, TID_BOOL, TRUE);
      if (flag) {
         for (i = 0;; i++) {
            db_enum_link(hDB, hkey, i, &hsubkey);

            if (!hsubkey)
               break;

            size = sizeof(flag);
            db_get_value(hDB, hsubkey, "Triggered", &flag, &size, TID_INT, TRUE);
            if (flag) {
               /*
                  if (exp_name[0])
                  sprintf(ref, "/?exp=%s&cmd=alarms", exp_name);
                  else
                  sprintf(ref, "/?cmd=alarms");
                */

               size = sizeof(alarm_class);
               db_get_value(hDB, hsubkey, "Alarm Class", alarm_class, &size, TID_STRING,
                            TRUE);

               strcpy(bgcol, "red");
               sprintf(str, "/Alarms/Classes/%s/Display BGColor", alarm_class);
               size = sizeof(bgcol);
               db_get_value(hDB, 0, str, bgcol, &size, TID_STRING, TRUE);

               strcpy(fgcol, "black");
               sprintf(str, "/Alarms/Classes/%s/Display FGColor", alarm_class);
               size = sizeof(fgcol);
               db_get_value(hDB, 0, str, fgcol, &size, TID_STRING, TRUE);

               size = sizeof(str);
               db_get_value(hDB, hsubkey, "Alarm Message", str, &size, TID_STRING, TRUE);

               rsprintf("<tr><td colspan=6 bgcolor=\"%s\" align=center>", bgcol);

               rsprintf("<font color=\"%s\" size=+3>%s: %s</font></tr>\n",
                        fgcol, alarm_class, str);
            }
         }
      }
   }

  /*---- manual triggered equipment ----*/

   if (db_find_key(hDB, 0, "/equipment", &hkey) == DB_SUCCESS) {
      first = TRUE;
      for (i = 0;; i++) {
         db_enum_key(hDB, hkey, i, &hsubkey);
         if (!hsubkey)
            break;

         db_get_key(hDB, hsubkey, &key);

         db_find_key(hDB, hsubkey, "Common", &hkeytmp);

         if (hkeytmp) {
            size = sizeof(type);
            db_get_value(hDB, hkeytmp, "Type", &type, &size, TID_INT, TRUE);
            if (type & EQ_MANUAL_TRIG) {
               if (first)
                  rsprintf("<tr><td colspan=6 bgcolor=#C0C0C0>\n");

               first = FALSE;

               sprintf(str, "Trigger %s event", key.name);
               rsprintf("<input type=submit name=cmd value=\"%s\">\n", str);
            }
         }
      }
      if (!first)
         rsprintf("</tr>\n\n");
   }

  /*---- aliases ----*/

   first = TRUE;

   db_find_key(hDB, 0, "/Alias", &hkey);
   if (hkey) {
      if (first) {
         rsprintf("<tr><td colspan=6 bgcolor=#C0C0C0>\n");
         first = FALSE;
      }
      for (i = 0;; i++) {
         db_enum_link(hDB, hkey, i, &hsubkey);
         if (!hsubkey)
            break;

         db_get_key(hDB, hsubkey, &key);

         strcpy(name, key.name);
         new_window = (name[strlen(name) - 1] != '&');
         if (!new_window)
            name[strlen(name) - 1] = 0;

         if (key.type == TID_STRING) {
            /* html link */
            size = sizeof(ref);
            db_get_data(hDB, hsubkey, ref, &size, TID_STRING);
            if (new_window)
               rsprintf("<a href=\"%s\" target=\"_blank\">%s</a> ", ref, name);
            else
               rsprintf("<a href=\"%s\">%s</a> ", ref, name);
         } else if (key.type == TID_LINK) {
            /* odb link */
            if (exp_name[0])
               sprintf(ref, "/Alias/%s?exp=%s", key.name, exp_name);
            else
               sprintf(ref, "/Alias/%s", key.name);

            if (new_window)
               rsprintf("<a href=\"%s\" target=\"_blank\">%s</a> ", ref, name);
            else
               rsprintf("<a href=\"%s\">%s</a> ", ref, name);
         }
      }
   }

   /*---- custom pages ----*/

   db_find_key(hDB, 0, "/Custom", &hkey);
   if (hkey) {
      if (first) {
         rsprintf("<tr><td colspan=6 bgcolor=#C0C0C0>\n");
         first = FALSE;
      }
      for (i = 0;; i++) {
         db_enum_link(hDB, hkey, i, &hsubkey);
         if (!hsubkey)
            break;

         db_get_key(hDB, hsubkey, &key);

         strcpy(name, key.name);
         new_window = (name[strlen(name) - 1] != '&');
         if (!new_window)
            name[strlen(name) - 1] = 0;

         if (key.type == TID_STRING) {
            if (exp_name[0])
               sprintf(ref, "/CS/%s?exp=%s", key.name, exp_name);
            else
               sprintf(ref, "/CS/%s", key.name);

            if (new_window)
               rsprintf("<a href=\"%s\" target=\"_blank\">%s</a> ", ref, name);
            else
               rsprintf("<a href=\"%s\">%s</a> ", ref, name);
         }
      }
   }

   /*---- run info ----*/

   rsprintf("<tr align=center><td>Run #%d", runinfo.run_number);

   if (runinfo.state == STATE_STOPPED)
      rsprintf("<td colspan=1 bgcolor=#FF0000>Stopped");
   else if (runinfo.state == STATE_PAUSED)
      rsprintf("<td colspan=1 bgcolor=#FFFF00>Paused");
   else if (runinfo.state == STATE_RUNNING)
      rsprintf("<td colspan=1 bgcolor=#00FF00>Running");
   else
      rsprintf("<td colspan=1 bgcolor=#FFFFFF>Unknown");

   if (runinfo.requested_transition)
      for (i = 0; i < 4; i++)
         if (runinfo.requested_transition & (1 << i))
            rsprintf("<br><b>%s requested</b>", trans_name[i]);

   if (exp_name[0])
      sprintf(ref, "/Alarms/Alarm system active?cmd=set&exp=%s", exp_name);
   else
      sprintf(ref, "/Alarms/Alarm system active?cmd=set");

   size = sizeof(flag);
   db_get_value(hDB, 0, "/Alarms/Alarm system active", &flag, &size, TID_BOOL, TRUE);
   strcpy(str, flag ? "00FF00" : "FFC0C0");
   rsprintf("<td bgcolor=#%s><a href=\"%s\">Alarms: %s</a>", str, ref,
            flag ? "On" : "Off");

   if (exp_name[0])
      sprintf(ref, "/Logger/Auto restart?cmd=set&exp=%s", exp_name);
   else
      sprintf(ref, "/Logger/Auto restart?cmd=set");

   size = sizeof(flag);
   db_get_value(hDB, 0, "/Logger/Auto restart", &flag, &size, TID_BOOL, TRUE);
   strcpy(str, flag ? "00FF00" : "FFFF00");
   rsprintf("<td bgcolor=#%s><a href=\"%s\">Restart: %s</a>", str, ref,
            flag ? "Yes" : "No");

   if (cm_exist("Logger", FALSE) != CM_SUCCESS && cm_exist("FAL", FALSE) != CM_SUCCESS)
      rsprintf("<td colspan=2 bgcolor=#FF0000>Logger not running</tr>\n");
   else {
      /* write data flag */
      size = sizeof(flag);
      db_get_value(hDB, 0, "/Logger/Write data", &flag, &size, TID_BOOL, TRUE);

      if (!flag)
         rsprintf("<td colspan=2 bgcolor=#FFFF00>Logging disabled</tr>\n");
      else {
         size = sizeof(str);
         db_get_value(hDB, 0, "/Logger/Data dir", str, &size, TID_STRING, TRUE);

         rsprintf("<td colspan=3>Data dir: %s</tr>\n", str);
      }
   }

   /*---- time ----*/

   rsprintf("<tr align=center><td colspan=3>Start: %s", runinfo.start_time);

   difftime = now - runinfo.start_time_binary;
   if (runinfo.state == STATE_STOPPED)
      rsprintf("<td colspan=3>Stop: %s</tr>\n", runinfo.stop_time);
   else
      rsprintf("<td colspan=3>Running time: %dh%02dm%02ds</tr>\n",
               difftime / 3600, difftime % 3600 / 60, difftime % 60);

   /*---- run comment ----*/

   size = sizeof(str);
   if (db_get_value(hDB, 0, "/Experiment/Run parameters/Comment", str,
                    &size, TID_STRING, FALSE) == DB_SUCCESS)
      rsprintf("<tr align=center><td colspan=6 bgcolor=#E0E0FF><b>%s</b></td></tr>\n",
               str);

   /*---- Equipment list ----*/

   rsprintf("<tr><th>Equipment<th>FE Node<th>Events");
   rsprintf("<th>Event rate[/s]<th>Data rate[kB/s]<th>Analyzed</tr>\n");

   if (db_find_key(hDB, 0, "/equipment", &hkey) == DB_SUCCESS) {
      for (i = 0;; i++) {
         db_enum_key(hDB, hkey, i, &hsubkey);
         if (!hsubkey)
            break;

         db_get_key(hDB, hsubkey, &key);

         memset(&equipment, 0, sizeof(equipment));
         memset(&equipment_stats, 0, sizeof(equipment_stats));

         db_find_key(hDB, hsubkey, "Common", &hkeytmp);

         if (hkeytmp) {
            db_get_record_size(hDB, hkeytmp, 0, &size);
            /* discard wrong equipments (caused by analyzer) */
            if (size == sizeof(equipment))
               db_get_record(hDB, hkeytmp, &equipment, &size, 0);
         }

         db_find_key(hDB, hsubkey, "Statistics", &hkeytmp);

         if (hkeytmp) {
            db_get_record_size(hDB, hkeytmp, 0, &size);
            if (size == sizeof(equipment_stats))
               db_get_record(hDB, hkeytmp, &equipment_stats, &size, 0);
         }

         if (exp_name[0])
            sprintf(ref, "/SC/%s?exp=%s", key.name, exp_name);
         else
            sprintf(ref, "/SC/%s", key.name);

         /* check if client running this equipment is present */
         if (cm_exist(equipment.frontend_name, TRUE) != CM_SUCCESS &&
             cm_exist("FAL", TRUE) != CM_SUCCESS)
            rsprintf
                ("<tr><td><a href=\"%s\">%s</a><td align=center bgcolor=#FF0000>(inactive)",
                 ref, key.name);
         else {
            if (equipment.enabled)
               rsprintf
                   ("<tr><td><a href=\"%s\">%s</a><td align=center bgcolor=#00FF00>%s@%s",
                    ref, key.name, equipment.frontend_name, equipment.frontend_host);
            else
               rsprintf
                   ("<tr><td><a href=\"%s\">%s</a><td align=center bgcolor=#FFFF00>%s@%s",
                    ref, key.name, equipment.frontend_name, equipment.frontend_host);
         }

         /* event statistics */
         d = equipment_stats.events_sent;
         if (d > 1E9)
            sprintf(str, "%1.3lfG", d / 1E9);
         else if (d > 1E6)
            sprintf(str, "%1.3lfM", d / 1E6);
         else
            sprintf(str, "%1.0lf", d);

         rsprintf("<td align=center>%s<td align=center>%1.1lf<td align=center>%1.1lf\n",
                  str, equipment_stats.events_per_sec, equipment_stats.kbytes_per_sec);

         /* check if /Analyzer is defined */
         if (db_find_key(hDB, 0, "/Analyzer", &hkeytmp) == DB_SUCCESS) {
            /* get analyzed ratio */
            analyze_ratio = 0;
            sprintf(ref, "/Analyzer/%s", key.name);
            db_find_key(hDB, 0, ref, &hkeytmp);
            if (hkeytmp) {
               size = sizeof(double);
               if (db_get_value(hDB, hkeytmp, "Statistics/Events received",
                                &analyzed, &size, TID_DOUBLE, TRUE) == DB_SUCCESS &&
                   equipment_stats.events_sent > 0)
                  analyze_ratio = analyzed / equipment_stats.events_sent;
               if (analyze_ratio > 1)
                  analyze_ratio = 1;
            }

            /* check if analyzer is running */
            if (cm_exist("Analyzer", FALSE) == CM_SUCCESS ||
                cm_exist("FAL", FALSE) == CM_SUCCESS)
               rsprintf("<td align=center bgcolor=#00FF00>%3.1lf%%</tr>\n",
                        analyze_ratio * 100.0);
            else
               rsprintf("<td align=center bgcolor=#FF0000>%3.1lf%%</tr>\n",
                        analyze_ratio * 100.0);
         } else {
            rsprintf("<td align=center>N/A</td></tr>\n");
         }
      }
   }

   /*---- Logging channels ----*/

   rsprintf
       ("<tr><th colspan=2>Channel<th>Active<th>Events<th>MB written<th>GB total</tr>\n");

   if (db_find_key(hDB, 0, "/Logger/Channels", &hkey) == DB_SUCCESS) {
      for (i = 0;; i++) {
         db_enum_key(hDB, hkey, i, &hsubkey);
         if (!hsubkey)
            break;

         db_get_key(hDB, hsubkey, &key);

         db_find_key(hDB, hsubkey, "Settings", &hkeytmp);
         size = sizeof(chn_settings);
         if (db_get_record(hDB, hkeytmp, &chn_settings, &size, 0) != DB_SUCCESS)
            continue;

         db_find_key(hDB, hsubkey, "Statistics", &hkeytmp);
         size = sizeof(chn_stats);
         if (db_get_record(hDB, hkeytmp, &chn_stats, &size, 0) != DB_SUCCESS)
            continue;

         /* filename */

         strcpy(str, chn_settings.current_filename);

         if (equal_ustring(chn_settings.type, "FTP")) {
            char *token, orig[256];

            strcpy(orig, str);

            strcpy(str, "ftp://");
            token = strtok(orig, ", ");
            if (token) {
               strcat(str, token);
               token = strtok(NULL, ", ");
               token = strtok(NULL, ", ");
               token = strtok(NULL, ", ");
               token = strtok(NULL, ", ");
               if (token) {
                  strcat(str, "/");
                  strcat(str, token);
                  strcat(str, "/");
                  token = strtok(NULL, ", ");
                  strcat(str, token);
               }
            }
         }

         if (exp_name[0])
            sprintf(ref, "/Logger/Channels/%s/Settings?exp=%s", key.name, exp_name);
         else
            sprintf(ref, "/Logger/Channels/%s/Settings", key.name);

         rsprintf("<tr><td colspan=2><B><a href=\"%s\">%s</a></B> %s", ref, key.name,
                  str);

         /* active */

         if (cm_exist("Logger", FALSE) != CM_SUCCESS &&
             cm_exist("FAL", FALSE) != CM_SUCCESS)
            rsprintf("<td align=center bgcolor=\"FF0000\">No Logger");
         else if (!flag)
            rsprintf("<td align=center bgcolor=\"FFFF00\">Disabled");
         else if (chn_settings.active)
            rsprintf("<td align=center bgcolor=\"00FF00\">Yes");
         else
            rsprintf("<td align=center bgcolor=\"FFFF00\">No");

         /* statistics */

         rsprintf
             ("<td align=center>%1.0lf<td align=center>%1.3lf<td align=center>%1.3lf</tr>\n",
              chn_stats.events_written, chn_stats.bytes_written / 1024 / 1024,
              chn_stats.bytes_written_total / 1024 / 1024 / 1024);
      }
   }

   /*---- Lazy Logger ----*/

   if (db_find_key(hDB, 0, "/Lazy", &hkey) == DB_SUCCESS) {
      status = db_find_key(hDB, 0, "System/Clients", &hkey);
      if (status != DB_SUCCESS)
         return;

      k = 0;
      previous_mode = -1;
      /* loop over all clients */
      for (j = 0;; j++) {
         status = db_enum_key(hDB, hkey, j, &hsubkey);
         if (status == DB_NO_MORE_SUBKEYS)
            break;

         if (status == DB_SUCCESS) {
            /* get client name */
            size = sizeof(client_name);
            db_get_value(hDB, hsubkey, "Name", client_name, &size, TID_STRING, TRUE);
            client_name[4] = 0; /* search only for the 4 first char */
            if (equal_ustring(client_name, "Lazy")) {
               sprintf(str, "/Lazy/%s", &client_name[5]);
               status = db_find_key(hDB, 0, str, &hLKey);
               if (status == DB_SUCCESS) {
                  size = sizeof(str);
                  db_get_value(hDB, hLKey, "Settings/Backup Type", str, &size, TID_STRING,
                               TRUE);
                  ftp_mode = equal_ustring(str, "FTP");

                  if (previous_mode != ftp_mode)
                     k = 0;
                  if (k == 0) {
                     if (ftp_mode)
                        rsprintf
                            ("<tr><th colspan=2>Lazy Destination<th>Progress<th>File Name<th>Speed [kb/s]<th>Total</tr>\n");
                     else
                        rsprintf
                            ("<tr><th colspan=2>Lazy Label<th>Progress<th>File Name<th># Files<th>Total</tr>\n");
                  }
                  previous_mode = ftp_mode;
                  if (ftp_mode) {
                     size = sizeof(str);
                     db_get_value(hDB, hLKey, "Settings/Path", str, &size, TID_STRING,
                                  TRUE);
                     if (strchr(str, ','))
                        *strchr(str, ',') = 0;
                  } else {
                     size = sizeof(str);
                     db_get_value(hDB, hLKey, "Settings/List Label", str, &size,
                                  TID_STRING, TRUE);
                     if (str[0] == 0)
                        strcpy(str, "(empty)");
                  }

                  if (exp_name[0])
                     sprintf(ref, "/Lazy/%s/Settings?exp=%s", &client_name[5], exp_name);
                  else
                     sprintf(ref, "/Lazy/%s/Settings", &client_name[5]);

                  rsprintf("<tr><td colspan=2><B><a href=\"%s\">%s</a></B>", ref, str);

                  size = sizeof(value);
                  db_get_value(hDB, hLKey, "Statistics/Copy progress [%]", &value, &size,
                               TID_FLOAT, TRUE);
                  rsprintf("<td align=center>%1.0f %%", value);

                  size = sizeof(str);
                  db_get_value(hDB, hLKey, "Statistics/Backup File", str, &size,
                               TID_STRING, TRUE);
                  rsprintf("<td align=center>%s", str);

                  if (ftp_mode) {
                     size = sizeof(value);
                     db_get_value(hDB, hLKey, "Statistics/Copy Rate [bytes per s]",
                                  &value, &size, TID_FLOAT, TRUE);
                     rsprintf("<td align=center>%1.1f", value / 1024.0f);
                  } else {
                     size = sizeof(i);
                     db_get_value(hDB, hLKey, "/Statistics/Number of files", &i, &size,
                                  TID_INT, TRUE);
                     rsprintf("<td align=center>%d", i);
                  }

                  size = sizeof(value);
                  db_get_value(hDB, hLKey, "Statistics/Backup status [%]", &value, &size,
                               TID_FLOAT, TRUE);
                  rsprintf("<td align=center>%1.1f %%", value);
                  k++;
               }
            }
         }
      }

      rsprintf("</tr>\n");
   }

   /*---- Messages ----*/

   rsprintf("<tr><td colspan=6>");

   if (message_buffer[0]) {

      if (strchr(message_buffer+9, '[') && strchr(message_buffer+9, ':'))
         rsprintf("<span style=\"color:white;background-color:red\"><b>%s</b></span>", message_buffer);
      else
         rsprintf("<b>%s</b>", message_buffer);
   }

   rsprintf("</tr>");

   /*---- Clients ----*/

   if (db_find_key(hDB, 0, "/System/Clients", &hkey) == DB_SUCCESS) {
      for (i = 0;; i++) {
         db_enum_key(hDB, hkey, i, &hsubkey);
         if (!hsubkey)
            break;

         if (i % 3 == 0)
            rsprintf("<tr bgcolor=#E0E0FF>");

         size = sizeof(name);
         db_get_value(hDB, hsubkey, "Name", name, &size, TID_STRING, TRUE);
         size = sizeof(str);
         db_get_value(hDB, hsubkey, "Host", str, &size, TID_STRING, TRUE);

         rsprintf("<td colspan=2 align=center>%s [%s]", name, str);

         if (i % 3 == 2)
            rsprintf("</tr>\n");
      }

      if (i % 3 != 0)
         rsprintf("</tr>\n");
   }

   rsprintf("</table>\n");
   rsprintf("</body></html>\r\n");
}

/*------------------------------------------------------------------*/

void show_messages_page(int refresh, int n_message)
{
   int size, more;
   char str[256], buffer[100000], line[256], *pline;
   time_t now;
   HNDLE hDB;
   BOOL eob;

   cm_get_experiment_database(&hDB, NULL);

   /* header */
   rsprintf("HTTP/1.0 200 Document follows\r\n");
   rsprintf("Server: MIDAS HTTP %s\r\n", cm_get_version());
   rsprintf("Content-Type: text/html; charset=iso-8859-1\r\n\r\n");

   rsprintf("<html><head>\n");

   /* auto refresh */
   if (refresh > 0)
      rsprintf("<meta http-equiv=\"Refresh\" content=\"%d\">\n\n", refresh);

   rsprintf("<title>MIDAS messages</title></head>\n");
   rsprintf("<body><form method=\"GET\" action=\"/\">\n");

   /* define hidden field for experiment */
   if (exp_name[0])
      rsprintf("<input type=hidden name=exp value=\"%s\">\n", exp_name);

   rsprintf("<table columns=2 border=3 cellpadding=2>\n");

  /*---- title row ----*/

   size = sizeof(str);
   str[0] = 0;
   db_get_value(hDB, 0, "/Experiment/Name", str, &size, TID_STRING, TRUE);
   time(&now);

   rsprintf("<tr><th bgcolor=#A0A0FF>MIDAS experiment \"%s\"", str);
   rsprintf("<th bgcolor=#A0A0FF>%s</tr>\n", ctime(&now));

  /*---- menu buttons ----*/

   rsprintf("<tr><td colspan=2 bgcolor=#C0C0C0>\n");

   rsprintf("<input type=submit name=cmd value=ODB>\n");
   rsprintf("<input type=submit name=cmd value=Status>\n");
   rsprintf("<input type=submit name=cmd value=Config>\n");
   rsprintf("<input type=submit name=cmd value=Help>\n");
   rsprintf("</tr>\n\n");

   /*---- messages ----*/

   rsprintf("<tr><td colspan=2>\n");

   /* more button */
   if (n_message == 20)
      more = 100;
   else
      more = n_message + 100;

   rsprintf("<input type=submit name=cmd value=More%d><p>\n", more);

   size = sizeof(buffer);

   cm_msg_retrieve(n_message, buffer, &size);

   pline = buffer;
   eob = FALSE;

   do {
      strncpy(line, pline, sizeof(line));

      /* extract single line */
      if (strchr(line, '\n'))
         *strchr(line, '\n') = 0;
      if (strchr(line, '\r'))
         *strchr(line, '\r') = 0;

      pline += strlen(line);

      while (*pline == '\r' || *pline == '\n')
         pline++;

      /* check for error */
      if (strchr(line+26, '[') && strchr(line+25, ':'))
         rsprintf("<span style=\"color:white;background-color:red\">%s</span>", line);
      else
         rsprintf("%s", line);

      rsprintf("<br>\n");
   } while (!eob && *pline);

   rsprintf("</tr></table>\n");
   rsprintf("</body></html>\r\n");
}

/*------------------------------------------------------------------*/

void strencode(char *text)
{
   int i;

   for (i = 0; i < (int) strlen(text); i++) {
      switch (text[i]) {
      case '\n':
         rsprintf("<br>\n");
         break;
      case '<':
         rsprintf("&lt;");
         break;
      case '>':
         rsprintf("&gt;");
         break;
      case '&':
         rsprintf("&amp;");
         break;
      case '\"':
         rsprintf("&quot;");
         break;
      default:
         rsprintf("%c", text[i]);
      }
   }
}

/*------------------------------------------------------------------*/

void strencode2(char *b, char *text)
{
   int i;

   for (i = 0; i < (int) strlen(text); b++, i++) {
      switch (text[i]) {
      case '\n':
         sprintf(b, "<br>\n");
         break;
      case '<':
         sprintf(b, "&lt;");
         break;
      case '>':
         sprintf(b, "&gt;");
         break;
      case '&':
         sprintf(b, "&amp;");
         break;
      case '\"':
         sprintf(b, "&quot;");
         break;
      default:
         sprintf(b, "%c", text[i]);
      }
   }
   *b = 0;
}

/*------------------------------------------------------------------*/

void strencode3(char *text)
{
   int i;

   for (i = 0; i < (int) strlen(text); i++) {
      switch (text[i]) {
      case '<':
         rsprintf("&lt;");
         break;
      case '>':
         rsprintf("&gt;");
         break;
      case '&':
         rsprintf("&amp;");
         break;
      case '\"':
         rsprintf("&quot;");
         break;
      default:
         rsprintf("%c", text[i]);
      }
   }
}

/*------------------------------------------------------------------*/

void show_elog_new(char *path, BOOL bedit, char *odb_att)
{
   int i, j, size, run_number, wrap, status;
   char str[256], ref[256], *p;
   char date[80], author[80], type[80], system[80], subject[256], text[10000],
       orig_tag[80], reply_tag[80], att1[256], att2[256], att3[256], encoding[80];
   time_t now;
   HNDLE hDB, hkey, hsubkey;
   BOOL display_run_number;
   KEY key;

   cm_get_experiment_database(&hDB, NULL);
   display_run_number = TRUE;
   size = sizeof(BOOL);
   db_get_value(hDB, 0, "/Elog/Display run number", &display_run_number, &size, TID_BOOL,
                TRUE);

   /* get message for reply */
   type[0] = system[0] = 0;
   att1[0] = att2[0] = att3[0] = 0;
   subject[0] = 0;
   run_number = 0;

   if (path) {
      strcpy(str, path);
      size = sizeof(text);
      el_retrieve(str, date, &run_number, author, type, system, subject,
                  text, &size, orig_tag, reply_tag, att1, att2, att3, encoding);
   }

   if (run_number < 0) {
      cm_msg(MERROR, "show_elog_new", "aborting on attempt to use invalid run number %d",
             run_number);
      abort();
   }

   /* header */
   rsprintf("HTTP/1.0 200 Document follows\r\n");
   rsprintf("Server: MIDAS HTTP %s\r\n", cm_get_version());
   rsprintf("Content-Type: text/html; charset=iso-8859-1\r\n\r\n");

   rsprintf("<html><head><title>MIDAS ELog</title></head>\n");
   rsprintf
       ("<body><form method=\"POST\" action=\"/EL/\" enctype=\"multipart/form-data\">\n");

   /* define hidden field for experiment */
   if (exp_name[0])
      rsprintf("<input type=hidden name=exp value=\"%s\">\n", exp_name);

   rsprintf("<table border=3 cellpadding=5>\n");

  /*---- title row ----*/

   size = sizeof(str);
   str[0] = 0;
   db_get_value(hDB, 0, "/Experiment/Name", str, &size, TID_STRING, TRUE);

   rsprintf("<tr><th bgcolor=#A0A0FF>MIDAS Electronic Logbook");
   if (elog_mode)
      rsprintf("<th bgcolor=#A0A0FF>Logbook \"%s\"</tr>\n", str);
   else
      rsprintf("<th bgcolor=#A0A0FF>Experiment \"%s\"</tr>\n", str);

  /*---- menu buttons ----*/

   rsprintf("<tr><td colspan=2 bgcolor=#C0C0C0>\n");

   rsprintf("<input type=submit name=cmd value=Submit>\n");
   rsprintf("</tr>\n\n");

  /*---- entry form ----*/

   if (display_run_number) {
      if (bedit) {
         rsprintf("<tr><td bgcolor=#FFFF00>Entry date: %s<br>", date);
         time(&now);
         rsprintf("Revision date: %s", ctime(&now));
      } else {
         time(&now);
         rsprintf("<tr><td bgcolor=#FFFF00>Entry date: %s", ctime(&now));
      }

      if (!bedit) {
         run_number = 0;
         size = sizeof(run_number);
         status =
             db_get_value(hDB, 0, "/Runinfo/Run number", &run_number, &size, TID_INT,
                          TRUE);
         assert(status == SUCCESS);
      }

      if (run_number < 0) {
         cm_msg(MERROR, "show_elog_new",
                "aborting on attempt to use invalid run number %d", run_number);
         abort();
      }

      rsprintf("<td bgcolor=#FFFF00>Run number: ");
      rsprintf("<input type=\"text\" size=10 maxlength=10 name=\"run\" value=\"%d\"</tr>",
               run_number);
   } else {
      if (bedit) {
         rsprintf("<tr><td colspan=2 bgcolor=#FFFF00>Entry date: %s<br>", date);
         time(&now);
         rsprintf("Revision date: %s", ctime(&now));
      } else {
         time(&now);
         rsprintf("<tr><td colspan=2 bgcolor=#FFFF00>Entry date: %s", ctime(&now));
      }
   }

   if (bedit) {
      strcpy(str, author);
      if (strchr(str, '@'))
         *strchr(str, '@') = 0;
   } else
      str[0] = 0;

   rsprintf
       ("<tr><td bgcolor=#FFA0A0>Author: <input type=\"text\" size=\"15\" maxlength=\"80\" name=\"Author\" value=\"%s\">\n",
        str);

   /* get type list from ODB */
   size = 20 * NAME_LENGTH;
   if (db_find_key(hDB, 0, "/Elog/Types", &hkey) != DB_SUCCESS)
      db_set_value(hDB, 0, "/Elog/Types", type_list, NAME_LENGTH * 20, 20, TID_STRING);
   db_find_key(hDB, 0, "/Elog/Types", &hkey);
   if (hkey)
      db_get_data(hDB, hkey, type_list, &size, TID_STRING);

   /* add types from forms */
   for (j = 0; j < 20 && type_list[j][0]; j++);
   db_find_key(hDB, 0, "/Elog/Forms", &hkey);
   if (hkey)
      for (i = 0; j < 20; i++) {
         db_enum_link(hDB, hkey, i, &hsubkey);
         if (!hsubkey)
            break;

         db_get_key(hDB, hsubkey, &key);
         strncpy(type_list[j++], key.name, NAME_LENGTH);
      }

   /* get system list from ODB */
   size = 20 * NAME_LENGTH;
   if (db_find_key(hDB, 0, "/Elog/Systems", &hkey) != DB_SUCCESS)
      db_set_value(hDB, 0, "/Elog/Systems", system_list, NAME_LENGTH * 20, 20,
                   TID_STRING);
   db_find_key(hDB, 0, "/Elog/Systems", &hkey);
   if (hkey)
      db_get_data(hDB, hkey, system_list, &size, TID_STRING);

   if (exp_name[0])
      sprintf(ref, "/ELog/?exp=%s", exp_name);
   else
      sprintf(ref, "/ELog/");

   rsprintf
       ("<td bgcolor=#FFA0A0><a href=\"%s\" target=\"_blank\">Type:</a> <select name=\"type\">\n",
        ref);
   for (i = 0; i < 20 && type_list[i][0]; i++)
      if ((path && !bedit && equal_ustring(type_list[i], "reply")) ||
          (bedit && equal_ustring(type_list[i], type)))
         rsprintf("<option selected value=\"%s\">%s\n", type_list[i], type_list[i]);
      else
         rsprintf("<option value=\"%s\">%s\n", type_list[i], type_list[i]);
   rsprintf("</select></tr>\n");

   rsprintf
       ("<tr><td bgcolor=#A0FFA0><a href=\"%s\" target=\"_blank\">  System:</a> <select name=\"system\">\n",
        ref);
   for (i = 0; i < 20 && system_list[i][0]; i++)
      if (path && equal_ustring(system_list[i], system))
         rsprintf("<option selected value=\"%s\">%s\n", system_list[i], system_list[i]);
      else
         rsprintf("<option value=\"%s\">%s\n", system_list[i], system_list[i]);
   rsprintf("</select>\n");

   str[0] = 0;
   if (path && !bedit)
      sprintf(str, "Re: %s", subject);
   else
      sprintf(str, "%s", subject);
   rsprintf
       ("<td bgcolor=#A0FFA0>Subject: <input type=text size=20 maxlength=\"80\" name=Subject value=\"%s\"></tr>\n",
        str);

   if (path) {
      /* hidden text for original message */
      rsprintf("<input type=hidden name=orig value=\"%s\">\n", path);

      if (bedit)
         rsprintf("<input type=hidden name=edit value=1>\n");
   }

   /* increased wrapping for replys (leave space for '> ' */
   wrap = (path && !bedit) ? 78 : 76;

   rsprintf("<tr><td colspan=2>Text:<br>\n");
   rsprintf("<textarea rows=10 cols=%d wrap=hard name=Text>", wrap);

   if (path) {
      if (bedit) {
         rsputs(text);
      } else {
         p = text;
         do {
            if (strchr(p, '\r')) {
               *strchr(p, '\r') = 0;
               rsprintf("> %s\n", p);
               p += strlen(p) + 1;
               if (*p == '\n')
                  p++;
            } else {
               rsprintf("> %s\n\n", p);
               break;
            }

         } while (TRUE);
      }
   }

   rsprintf("</textarea><br>\n");

   /* HTML check box */
   if (bedit && encoding[0] == 'H')
      rsprintf
          ("<input type=checkbox checked name=html value=1>Submit as HTML text</tr>\n");
   else
      rsprintf("<input type=checkbox name=html value=1>Submit as HTML text</tr>\n");

   if (bedit && att1[0])
      rsprintf
          ("<tr><td colspan=2 align=center bgcolor=#8080FF>If no attachment are resubmitted, the original ones are kept</tr>\n");

   /* attachment */
   rsprintf
       ("<tr><td colspan=2 align=center>Enter attachment filename(s) or ODB tree(s), use \"\\\" as an ODB directory separator:</tr>");

   if (odb_att) {
      str[0] = 0;
      if (odb_att[0] != '\\' && odb_att[0] != '/')
         strcpy(str, "\\");
      strcat(str, odb_att);
      rsprintf
          ("<tr><td colspan=2>Attachment1: <input type=hidden name=attachment0 value=\"%s\"><b>%s</b></tr>\n",
           str, str);
   } else
      rsprintf
          ("<tr><td colspan=2>Attachment1: <input type=\"file\" size=\"60\" maxlength=\"256\" name=\"attfile1\" value=\"%s\" accept=\"filetype/*\"></tr>\n",
           att1);

   rsprintf
       ("<tr><td colspan=2>Attachment2: <input type=\"file\" size=\"60\" maxlength=\"256\" name=\"attfile2\" value=\"%s\" accept=\"filetype/*\"></tr>\n",
        att2);
   rsprintf
       ("<tr><td colspan=2>Attachment3: <input type=\"file\" size=\"60\" maxlength=\"256\" name=\"attfile3\" value=\"%s\" accept=\"filetype/*\"></tr>\n",
        att3);

   rsprintf("</table>\n");
   rsprintf("</body></html>\r\n");
}

/*------------------------------------------------------------------*/

void show_elog_query()
{
   int i, size;
   char str[256];
   time_t now;
   struct tm *tms;
   HNDLE hDB, hkey, hkeyroot;
   KEY key;
   BOOL display_run_number;

   /* get flag for displaying run number */
   cm_get_experiment_database(&hDB, NULL);
   display_run_number = TRUE;
   size = sizeof(BOOL);
   db_get_value(hDB, 0, "/Elog/Display run number", &display_run_number, &size, TID_BOOL,
                TRUE);

   /* header */
   rsprintf("HTTP/1.0 200 Document follows\r\n");
   rsprintf("Server: MIDAS HTTP %s\r\n", cm_get_version());
   rsprintf("Content-Type: text/html; charset=iso-8859-1\r\n\r\n");

   rsprintf("<html><head><title>MIDAS ELog</title></head>\n");
   rsprintf("<body><form method=\"GET\" action=\"/EL/\">\n");

   /* define hidden field for experiment */
   if (exp_name[0])
      rsprintf("<input type=hidden name=exp value=\"%s\">\n", exp_name);

   rsprintf("<table border=3 cellpadding=5>\n");

  /*---- title row ----*/

   size = sizeof(str);
   str[0] = 0;
   db_get_value(hDB, 0, "/Experiment/Name", str, &size, TID_STRING, TRUE);

   rsprintf("<tr><th colspan=2 bgcolor=#A0A0FF>MIDAS Electronic Logbook");
   if (elog_mode)
      rsprintf("<th colspan=2 bgcolor=#A0A0FF>Logbook \"%s\"</tr>\n", str);
   else
      rsprintf("<th colspan=2 bgcolor=#A0A0FF>Experiment \"%s\"</tr>\n", str);

  /*---- menu buttons ----*/

   rsprintf("<tr><td colspan=4 bgcolor=#C0C0C0>\n");

   rsprintf("<input type=submit name=cmd value=\"Submit Query\">\n");
   rsprintf("<input type=reset value=\"Reset Form\">\n");
   rsprintf("</tr>\n\n");

  /*---- entry form ----*/

   rsprintf("<tr><td colspan=2 bgcolor=#C0C000>");
   rsprintf("<input type=checkbox name=mode value=\"summary\">Summary only\n");
   rsprintf("<td colspan=2 bgcolor=#C0C000>");
   rsprintf("<input type=checkbox name=attach value=1>Show attachments</tr>\n");

   time(&now);
   now -= 3600 * 24;
   tms = localtime(&now);
   tms->tm_year += 1900;

   rsprintf("<tr><td bgcolor=#FFFF00>Start date: ");
   rsprintf("<td colspan=3 bgcolor=#FFFF00><select name=\"m1\">\n");

   for (i = 0; i < 12; i++)
      if (i == tms->tm_mon)
         rsprintf("<option selected value=\"%s\">%s\n", mname[i], mname[i]);
      else
         rsprintf("<option value=\"%s\">%s\n", mname[i], mname[i]);
   rsprintf("</select>\n");

   rsprintf("<select name=\"d1\">");
   for (i = 0; i < 31; i++)
      if (i + 1 == tms->tm_mday)
         rsprintf("<option selected value=%d>%d\n", i + 1, i + 1);
      else
         rsprintf("<option value=%d>%d\n", i + 1, i + 1);
   rsprintf("</select>\n");

   rsprintf(" <input type=\"text\" size=5 maxlength=5 name=\"y1\" value=\"%d\">",
            tms->tm_year);
   rsprintf("</tr>\n");

   rsprintf("<tr><td bgcolor=#FFFF00>End date: ");
   rsprintf("<td colspan=3 bgcolor=#FFFF00><select name=\"m2\" value=\"%s\">\n",
            mname[tms->tm_mon]);

   rsprintf("<option value=\"\">\n");
   for (i = 0; i < 12; i++)
      rsprintf("<option value=\"%s\">%s\n", mname[i], mname[i]);
   rsprintf("</select>\n");

   rsprintf("<select name=\"d2\">");
   rsprintf("<option selected value=\"\">\n");
   for (i = 0; i < 31; i++)
      rsprintf("<option value=%d>%d\n", i + 1, i + 1);
   rsprintf("</select>\n");

   rsprintf(" <input type=\"text\" size=5 maxlength=5 name=\"y2\">");
   rsprintf("</tr>\n");

   if (display_run_number) {
      rsprintf("<tr><td bgcolor=#A0FFFF>Start run: ");
      rsprintf
          ("<td bgcolor=#A0FFFF><input type=\"text\" size=\"10\" maxlength=\"10\" name=\"r1\">\n");
      rsprintf("<td bgcolor=#A0FFFF>End run: ");
      rsprintf
          ("<td bgcolor=#A0FFFF><input type=\"text\" size=\"10\" maxlength=\"10\" name=\"r2\">\n");
      rsprintf("</tr>\n");
   }

   /* get type list from ODB */
   size = 20 * NAME_LENGTH;
   if (db_find_key(hDB, 0, "/Elog/Types", &hkey) != DB_SUCCESS)
      db_set_value(hDB, 0, "/Elog/Types", type_list, NAME_LENGTH * 20, 20, TID_STRING);
   db_find_key(hDB, 0, "/Elog/Types", &hkey);
   if (hkey)
      db_get_data(hDB, hkey, type_list, &size, TID_STRING);

   /* get system list from ODB */
   size = 20 * NAME_LENGTH;
   if (db_find_key(hDB, 0, "/Elog/Systems", &hkey) != DB_SUCCESS)
      db_set_value(hDB, 0, "/Elog/Systems", system_list, NAME_LENGTH * 20, 20,
                   TID_STRING);
   db_find_key(hDB, 0, "/Elog/Systems", &hkey);
   if (hkey)
      db_get_data(hDB, hkey, system_list, &size, TID_STRING);

   rsprintf("<tr><td colspan=2 bgcolor=#FFA0A0>Author: ");
   rsprintf("<input type=\"test\" size=\"15\" maxlength=\"80\" name=\"author\">\n");

   rsprintf("<td colspan=2 bgcolor=#FFA0A0>Type: ");
   rsprintf("<select name=\"type\">\n");
   rsprintf("<option value=\"\">\n");
   for (i = 0; i < 20 && type_list[i][0]; i++)
      rsprintf("<option value=\"%s\">%s\n", type_list[i], type_list[i]);
   /* add forms as types */
   db_find_key(hDB, 0, "/Elog/Forms", &hkeyroot);
   if (hkeyroot)
      for (i = 0;; i++) {
         db_enum_link(hDB, hkeyroot, i, &hkey);
         if (!hkey)
            break;
         db_get_key(hDB, hkey, &key);
         rsprintf("<option value=\"%s\">%s\n", key.name, key.name);
      }
   rsprintf("</select></tr>\n");

   rsprintf("<tr><td colspan=2 bgcolor=#A0FFA0>System: ");
   rsprintf("<select name=\"system\">\n");
   rsprintf("<option value=\"\">\n");
   for (i = 0; i < 20 && system_list[i][0]; i++)
      rsprintf("<option value=\"%s\">%s\n", system_list[i], system_list[i]);
   rsprintf("</select>\n");

   rsprintf("<td colspan=2 bgcolor=#A0FFA0>Subject: ");
   rsprintf("<input type=\"text\" size=\"15\" maxlength=\"80\" name=\"subject\"></tr>\n");

   rsprintf("<tr><td colspan=4 bgcolor=#FFA0FF>Text: ");
   rsprintf("<input type=\"text\" size=\"15\" maxlength=\"80\" name=\"subtext\">\n");
   rsprintf("<i>(case insensitive substring)</i><tr>\n");

   rsprintf("</tr></table>\n");
   rsprintf("</body></html>\r\n");
}

/*------------------------------------------------------------------*/

void show_elog_delete(char *path)
{
   HNDLE hDB;
   int size, status;
   char str[256];
   BOOL allow_delete;

   /* get flag for allowing delete */
   cm_get_experiment_database(&hDB, NULL);
   allow_delete = FALSE;
   size = sizeof(BOOL);
   db_get_value(hDB, 0, "/Elog/Allow delete", &allow_delete, &size, TID_BOOL, TRUE);

   /* redirect if confirm = NO */
   if (getparam("confirm") && *getparam("confirm") &&
       strcmp(getparam("confirm"), "No") == 0) {
      sprintf(str, "EL/%s", path);
      redirect(str);
      return;
   }

   /* header */
   sprintf(str, "EL/%s", path);
   show_header(hDB, "Delete ELog entry", "GET", str, 1, 0);

   if (!allow_delete) {
      rsprintf
          ("<tr><td colspan=2 bgcolor=#FF8080 align=center><h1>Message deletion disabled in ODB</h1>\n");
   } else {
      if (getparam("confirm") && *getparam("confirm")) {
         if (strcmp(getparam("confirm"), "Yes") == 0) {
            /* delete message */
            status = el_delete_message(path);
            rsprintf("<tr><td colspan=2 bgcolor=#80FF80 align=center>");
            if (status == EL_SUCCESS)
               rsprintf("<b>Message successfully deleted</b></tr>\n");
            else
               rsprintf("<b>Error deleting message: status = %d</b></tr>\n", status);

            rsprintf("<input type=hidden name=cmd value=last>\n");
            rsprintf
                ("<tr><td colspan=2 align=center><input type=submit value=\"Goto last message\"></tr>\n");
         }
      } else {
         /* define hidden field for command */
         rsprintf("<input type=hidden name=cmd value=delete>\n");

         rsprintf("<tr><td colspan=2 bgcolor=#FF8080 align=center>");
         rsprintf("<b>Are you sure to delete this message?</b></tr>\n");

         rsprintf("<tr><td align=center><input type=submit name=confirm value=Yes>\n");
         rsprintf("<td align=center><input type=submit name=confirm value=No>\n");
         rsprintf("</tr>\n\n");
      }
   }

   rsprintf("</table>\n");
   rsprintf("</body></html>\r\n");
}

/*------------------------------------------------------------------*/

void show_elog_submit_query(INT last_n)
{
   int i, size, run, status, m1, d2, m2, y2, index, colspan;
   char date[80], author[80], type[80], system[80], subject[256], text[10000],
       orig_tag[80], reply_tag[80], attachment[3][256], encoding[80];
   char str[256], str2[10000], tag[256], ref[256], file_name[256];
   HNDLE hDB;
   BOOL full, show_attachments, display_run_number;
   DWORD ltime_start, ltime_end, ltime_current, now;
   struct tm tms, *ptms;
   FILE *f;

   /* get flag for displaying run number */
   cm_get_experiment_database(&hDB, NULL);
   display_run_number = TRUE;
   size = sizeof(BOOL);
   db_get_value(hDB, 0, "/Elog/Display run number", &display_run_number, &size, TID_BOOL,
                TRUE);

   /* header */
   rsprintf("HTTP/1.0 200 Document follows\r\n");
   rsprintf("Server: MIDAS HTTP %s\r\n", cm_get_version());
   rsprintf("Content-Type: text/html; charset=iso-8859-1\r\n\r\n");

   rsprintf("<html><head><title>MIDAS ELog</title></head>\n");
   rsprintf("<body><form method=\"GET\" action=\"/EL/\">\n");

   /* define hidden field for experiment */
   if (exp_name[0])
      rsprintf("<input type=hidden name=exp value=\"%s\">\n", exp_name);

   rsprintf("<table border=3 cellpadding=2 width=\"100%%\">\n");

   /* get mode */
   if (last_n) {
      full = TRUE;
      show_attachments = FALSE;
   } else {
      full = !(*getparam("mode"));
      show_attachments = (*getparam("attach") > 0);
   }

  /*---- title row ----*/

   size = sizeof(str);
   str[0] = 0;
   db_get_value(hDB, 0, "/Experiment/Name", str, &size, TID_STRING, TRUE);

   colspan = full ? 3 : 4;
   if (!display_run_number)
      colspan--;

   rsprintf("<tr><th colspan=3 bgcolor=#A0A0FF>MIDAS Electronic Logbook");
   if (elog_mode)
      rsprintf("<th colspan=%d bgcolor=#A0A0FF>Logbook \"%s\"</tr>\n", colspan, str);
   else
      rsprintf("<th colspan=%d bgcolor=#A0A0FF>Experiment \"%s\"</tr>\n", colspan, str);

  /*---- menu buttons ----*/

   if (!full) {
      colspan = display_run_number ? 7 : 6;
      rsprintf("<tr><td colspan=%d bgcolor=#C0C0C0>\n", colspan);

      rsprintf("<input type=submit name=cmd value=\"Query\">\n");
      rsprintf("<input type=submit name=cmd value=\"ELog\">\n");
      rsprintf("<input type=submit name=cmd value=\"Status\">\n");
      rsprintf("</tr>\n\n");
   }

  /*---- convert end date to ltime ----*/

   ltime_end = ltime_start = 0;
   m1 = m2 = d2 = y2 = 0;

   if (!last_n) {
      strcpy(str, getparam("m1"));
      for (m1 = 0; m1 < 12; m1++)
         if (equal_ustring(str, mname[m1]))
            break;
      if (m1 == 12)
         m1 = 0;

      if (*getparam("m2") || *getparam("y2") || *getparam("d2")) {
         if (*getparam("m2")) {
            strcpy(str, getparam("m2"));
            for (m2 = 0; m2 < 12; m2++)
               if (equal_ustring(str, mname[m2]))
                  break;
            if (m2 == 12)
               m2 = 0;
         } else
            m2 = m1;

         if (*getparam("y2"))
            y2 = atoi(getparam("y2"));
         else
            y2 = atoi(getparam("y1"));

         if (*getparam("d2"))
            d2 = atoi(getparam("d2"));
         else
            d2 = atoi(getparam("d1"));

         memset(&tms, 0, sizeof(struct tm));
         tms.tm_year = y2 % 100;
         tms.tm_mon = m2;
         tms.tm_mday = d2;
         tms.tm_hour = 24;

         if (tms.tm_year < 90)
            tms.tm_year += 100;
         ltime_end = mktime(&tms);
      }
   }

  /*---- title row ----*/

   colspan = full ? 6 : 7;
   if (!display_run_number)
      colspan--;

   /* menu buttons */
   rsprintf("<tr><td colspan=%d bgcolor=#C0C0C0>\n", colspan);
   rsprintf("<input type=submit name=cmd value=Query>\n");
   rsprintf("<input type=submit name=cmd value=Last>\n");
   if (!elog_mode)
      rsprintf("<input type=submit name=cmd value=Status>\n");
   rsprintf("</tr>\n");

   if (*getparam("r1")) {
      if (*getparam("r2"))
         rsprintf
             ("<tr><td colspan=%d bgcolor=#FFFF00><b>Query result between runs %s and %s</b></tr>\n",
              colspan, getparam("r1"), getparam("r2"));
      else
         rsprintf
             ("<tr><td colspan=%d bgcolor=#FFFF00><b>Query result between run %s and today</b></tr>\n",
              colspan, getparam("r1"));
   } else {
      if (last_n) {
         if (last_n < 24) {
            if (exp_name[0])
               rsprintf
                   ("<tr><td colspan=6><a href=\"last%d?exp=%s\">Last %d hours</a></tr>\n",
                    last_n * 2, exp_name, last_n * 2);
            else
               rsprintf("<tr><td colspan=6><a href=\"last%d\">Last %d hours</a></tr>\n",
                        last_n * 2, last_n * 2);

            rsprintf("<tr><td colspan=6 bgcolor=#FFFF00><b>Last %d hours</b></tr>\n",
                     last_n);
         } else {
            if (exp_name[0])
               rsprintf
                   ("<tr><td colspan=6><a href=\"last%d?exp=%s\">Last %d days</a></tr>\n",
                    last_n * 2, exp_name, last_n / 24 * 2);
            else
               rsprintf("<tr><td colspan=6><a href=\"last%d\">Last %d days</a></tr>\n",
                        last_n * 2, last_n / 24 * 2);

            rsprintf("<tr><td colspan=6 bgcolor=#FFFF00><b>Last %d days</b></tr>\n",
                     last_n / 24);
         }
      }

      else if (*getparam("m2") || *getparam("y2") || *getparam("d2"))
         rsprintf
             ("<tr><td colspan=%d bgcolor=#FFFF00><b>Query result between %s %s %s and %s %d %d</b></tr>\n",
              colspan, getparam("m1"), getparam("d1"), getparam("y1"), mname[m2], d2, y2);
      else {
         time(&now);
         ptms = localtime(&now);
         ptms->tm_year += 1900;

         rsprintf
             ("<tr><td colspan=%d bgcolor=#FFFF00><b>Query result between %s %s %s and %s %d %d</b></tr>\n",
              colspan, getparam("m1"), getparam("d1"), getparam("y1"),
              mname[ptms->tm_mon], ptms->tm_mday, ptms->tm_year);
      }
   }

   rsprintf("</tr>\n<tr>");

   rsprintf("<td colspan=%d bgcolor=#FFA0A0>\n", colspan);

   if (*getparam("author"))
      rsprintf("Author: <b>%s</b>   ", getparam("author"));

   if (*getparam("type"))
      rsprintf("Type: <b>%s</b>   ", getparam("type"));

   if (*getparam("system"))
      rsprintf("System: <b>%s</b>   ", getparam("system"));

   if (*getparam("subject"))
      rsprintf("Subject: <b>%s</b>   ", getparam("subject"));

   if (*getparam("subtext"))
      rsprintf("Text: <b>%s</b>   ", getparam("subtext"));

   rsprintf("</tr>\n");

  /*---- table titles ----*/

   if (display_run_number) {
      if (full)
         rsprintf("<tr><th>Date<th>Run<th>Author<th>Type<th>System<th>Subject</tr>\n");
      else
         rsprintf
             ("<tr><th>Date<th>Run<th>Author<th>Type<th>System<th>Subject<th>Text</tr>\n");
   } else {
      if (full)
         rsprintf("<tr><th>Date<th>Author<th>Type<th>System<th>Subject</tr>\n");
      else
         rsprintf("<tr><th>Date<th>Author<th>Type<th>System<th>Subject<th>Text</tr>\n");
   }

  /*---- do query ----*/

   if (last_n) {
      time(&now);
      ltime_start = now - 3600 * last_n;
      ptms = localtime(&ltime_start);

      sprintf(tag, "%02d%02d%02d.0", ptms->tm_year % 100, ptms->tm_mon + 1,
              ptms->tm_mday);
   } else if (*getparam("r1")) {
      /* do run query */
      el_search_run(atoi(getparam("r1")), tag);
   } else {
      /* do date-date query */
      sprintf(tag, "%02d%02d%02d.0", atoi(getparam("y1")) % 100, m1 + 1,
              atoi(getparam("d1")));
   }

   do {
      size = sizeof(text);
      status = el_retrieve(tag, date, &run, author, type, system, subject,
                           text, &size, orig_tag, reply_tag,
                           attachment[0], attachment[1], attachment[2], encoding);
      strcat(tag, "+1");

      /* check for end run */
      if (*getparam("r2") && atoi(getparam("r2")) < run)
         break;

      /* convert date to unix format */
      memset(&tms, 0, sizeof(struct tm));
      tms.tm_year = (tag[0] - '0') * 10 + (tag[1] - '0');
      tms.tm_mon = (tag[2] - '0') * 10 + (tag[3] - '0') - 1;
      tms.tm_mday = (tag[4] - '0') * 10 + (tag[5] - '0');
      tms.tm_hour = (date[11] - '0') * 10 + (date[12] - '0');
      tms.tm_min = (date[14] - '0') * 10 + (date[15] - '0');
      tms.tm_sec = (date[17] - '0') * 10 + (date[18] - '0');

      if (tms.tm_year < 90)
         tms.tm_year += 100;
      ltime_current = mktime(&tms);

      /* check for start date */
      if (ltime_start > 0)
         if (ltime_current < ltime_start)
            continue;

      /* check for end date */
      if (ltime_end > 0) {
         if (ltime_current > ltime_end)
            break;
      }

      if (status == EL_SUCCESS) {
         /* do filtering */
         if (*getparam("type") && !equal_ustring(getparam("type"), type))
            continue;
         if (*getparam("system") && !equal_ustring(getparam("system"), system))
            continue;

         if (*getparam("author")) {
            strcpy(str, getparam("author"));
            for (i = 0; i < (int) strlen(str); i++)
               str[i] = toupper(str[i]);
            str[i] = 0;
            for (i = 0; i < (int) strlen(author) && author[i] != '@'; i++)
               str2[i] = toupper(author[i]);
            str2[i] = 0;

            if (strstr(str2, str) == NULL)
               continue;
         }

         if (*getparam("subject")) {
            strcpy(str, getparam("subject"));
            for (i = 0; i < (int) strlen(str); i++)
               str[i] = toupper(str[i]);
            str[i] = 0;
            for (i = 0; i < (int) strlen(subject); i++)
               str2[i] = toupper(subject[i]);
            str2[i] = 0;

            if (strstr(str2, str) == NULL)
               continue;
         }

         if (*getparam("subtext")) {
            strcpy(str, getparam("subtext"));
            for (i = 0; i < (int) strlen(str); i++)
               str[i] = toupper(str[i]);
            str[i] = 0;
            for (i = 0; i < (int) strlen(text); i++)
               str2[i] = toupper(text[i]);
            str2[i] = 0;

            if (strstr(str2, str) == NULL)
               continue;
         }

         /* filter passed: display line */

         strcpy(str, tag);
         if (strchr(str, '+'))
            *strchr(str, '+') = 0;
         if (exp_name[0])
            sprintf(ref, "/EL/%s?exp=%s", str, exp_name);
         else
            sprintf(ref, "/EL/%s", str);

         strncpy(str, text, 80);
         str[80] = 0;

         if (full) {
            if (display_run_number) {
               rsprintf("<tr><td><a href=%s>%s</a><td>%d<td>%s<td>%s<td>%s<td>%s</tr>\n",
                        ref, date, run, author, type, system, subject);
               rsprintf("<tr><td colspan=6>");
            } else {
               rsprintf("<tr><td><a href=%s>%s</a><td>%s<td>%s<td>%s<td>%s</tr>\n", ref,
                        date, author, type, system, subject);
               rsprintf("<tr><td colspan=5>");
            }

            if (equal_ustring(encoding, "plain")) {
               rsputs("<pre>");
               rsputs2(text);
               rsputs("</pre>");
            } else
               rsputs(text);

            if (!show_attachments && attachment[0][0]) {
               if (attachment[1][0])
                  rsprintf("Attachments: ");
               else
                  rsprintf("Attachment: ");
            } else
               rsprintf("</tr>\n");

            for (index = 0; index < 3; index++) {
               if (attachment[index][0]) {
                  for (i = 0; i < (int) strlen(attachment[index]); i++)
                     str[i] = toupper(attachment[index][i]);
                  str[i] = 0;

                  if (exp_name[0])
                     sprintf(ref, "/EL/%s?exp=%s", attachment[index], exp_name);
                  else
                     sprintf(ref, "/EL/%s", attachment[index]);

                  if (!show_attachments) {
                     rsprintf("<a href=\"%s\"><b>%s</b></a> ",
                              ref, attachment[index] + 14);
                  } else {
                     colspan = display_run_number ? 6 : 5;
                     if (strstr(str, ".GIF") || strstr(str, ".PNG")
                         || strstr(str, ".JPG")) {
                        rsprintf
                            ("<tr><td colspan=%d>Attachment: <a href=\"%s\"><b>%s</b></a><br>\n",
                             colspan, ref, attachment[index] + 14);
                        if (show_attachments)
                           rsprintf("<img src=\"%s\"></tr>", ref);
                     } else {
                        rsprintf
                            ("<tr><td colspan=%d bgcolor=#C0C0FF>Attachment: <a href=\"%s\"><b>%s</b></a>\n",
                             colspan, ref, attachment[index] + 14);

                        if ((strstr(str, ".TXT") ||
                             strstr(str, ".ASC") ||
                             strchr(str, '.') == NULL) && show_attachments) {
                           /* display attachment */
                           rsprintf("<br><pre>");

                           file_name[0] = 0;
                           size = sizeof(file_name);
                           memset(file_name, 0, size);
                           db_get_value(hDB, 0, "/Logger/Data dir", file_name, &size,
                                        TID_STRING, TRUE);
                           if (file_name[0] != 0)
                              if (file_name[strlen(file_name) - 1] != DIR_SEPARATOR)
                                 strcat(file_name, DIR_SEPARATOR_STR);
                           strcat(file_name, attachment[index]);

                           f = fopen(file_name, "rt");
                           if (f != NULL) {
                              while (!feof(f)) {
                                 str[0] = 0;
                                 fgets(str, sizeof(str), f);
                                 rsputs2(str);
                              }
                              fclose(f);
                           }

                           rsprintf("</pre>\n");
                        }
                        rsprintf("</tr>\n");
                     }
                  }
               }
            }

            if (!show_attachments && attachment[0][0])
               rsprintf("</tr>\n");

         } else {
            if (display_run_number)
               rsprintf("<tr><td><a href=%s>%s</a><td>%d<td>%s<td>%s<td>%s<td>%s\n", ref,
                        date, run, author, type, system, subject);
            else
               rsprintf("<tr><td><a href=%s>%s</a><td>%s<td>%s<td>%s<td>%s\n", ref, date,
                        author, type, system, subject);

            if (equal_ustring(encoding, "HTML"))
               rsputs(text);
            else
               strencode(text);

            rsprintf("</tr>\n");
         }
      }

   } while (status == EL_SUCCESS);

   rsprintf("</table>\n");
   rsprintf("</body></html>\r\n");
}

/*------------------------------------------------------------------*/

void show_rawfile(char *path)
{
   int size, lines, i, buf_size, offset;
   char *p;
   FILE *f;
   char file_name[256], str[100], buffer[100000];
   HNDLE hDB;

   cm_get_experiment_database(&hDB, NULL);

   lines = 10;
   if (*getparam("lines"))
      lines = atoi(getparam("lines"));

   if (*getparam("cmd") && equal_ustring(getparam("cmd"), "More lines"))
      lines *= 2;

   /* header */
   rsprintf("HTTP/1.0 200 Document follows\r\n");
   rsprintf("Server: MIDAS HTTP %s\r\n", cm_get_version());
   rsprintf("Content-Type: text/html; charset=iso-8859-1\r\n\r\n");

   rsprintf("<html><head><title>MIDAS File Display %s</title></head>\n", path);
   rsprintf("<body><form method=\"GET\" action=\"/EL/%s\">\n", path);

   /* define hidden field for experiment */
   if (exp_name[0])
      rsprintf("<input type=hidden name=exp value=\"%s\">\n", exp_name);

   rsprintf("<input type=hidden name=lines value=%d>\n", lines);

   rsprintf("<table border=3 cellpadding=1 width=\"100%%\">\n");

  /*---- title row ----*/

   size = sizeof(str);
   str[0] = 0;
   db_get_value(hDB, 0, "/Experiment/Name", str, &size, TID_STRING, TRUE);

   rsprintf("<tr><th bgcolor=#A0A0FF>MIDAS File Display <code>\"%s\"</code>", path);
   if (elog_mode)
      rsprintf("<th bgcolor=#A0A0FF>Logbook \"%s\"</tr>\n", str);
   else
      rsprintf("<th bgcolor=#A0A0FF>Experiment \"%s\"</tr>\n", str);

  /*---- menu buttons ----*/

   rsprintf("<tr><td colspan=2 bgcolor=#C0C0C0>\n");

   rsprintf("<input type=submit name=cmd value=\"ELog\">\n");
   if (!elog_mode)
      rsprintf("<input type=submit name=cmd value=\"Status\">\n");

   rsprintf("<input type=submit name=cmd value=\"More lines\">\n");

   rsprintf("</tr>\n\n");

  /*---- open file ----*/

   cm_get_experiment_database(&hDB, NULL);
   file_name[0] = 0;
   if (hDB > 0) {
      size = sizeof(file_name);
      memset(file_name, 0, size);
      db_get_value(hDB, 0, "/Logger/Data dir", file_name, &size, TID_STRING, TRUE);
      if (file_name[0] != 0)
         if (file_name[strlen(file_name) - 1] != DIR_SEPARATOR)
            strcat(file_name, DIR_SEPARATOR_STR);
   }
   strcat(file_name, path);

   f = fopen(file_name, "r");
   if (f == NULL) {
      rsprintf("<h3>Cannot find file \"%s\"</h3>\n", file_name);
      rsprintf("</body></html>\n");
      return;
   }

  /*---- file contents ----*/

   rsprintf("<tr><td colspan=2>\n");

   rsprintf("<pre>\n");

   buf_size = sizeof(buffer);

   /* position buf_size bytes before the EOF */
   fseek(f, -(buf_size - 1), SEEK_END);
   offset = ftell(f);
   if (offset != 0) {
      /* go to end of line */
      fgets(buffer, buf_size - 1, f);
      offset = ftell(f) - offset;
      buf_size -= offset;
   }

   memset(buffer, 0, buf_size);
   fread(buffer, 1, buf_size - 1, f);
   buffer[buf_size - 1] = 0;
   fclose(f);

   p = buffer + (buf_size - 2);

   /* goto end of buffer */
   while (p != buffer && *p == 0)
      p--;

   /* strip line break */
   while (p != buffer && (*p == '\n' || *p == '\r'))
      *(p--) = 0;

   /* trim buffer so that last lines remain */
   for (i = 0; i < lines; i++) {
      while (p != buffer && *p != '\n')
         p--;

      while (p != buffer && (*p == '\n' || *p == '\r'))
         p--;
   }
   if (p != buffer) {
      p++;
      while (*p == '\n' || *p == '\r')
         p++;
   }

   buf_size = (buf_size - 1) - ((PTYPE) p - (PTYPE) buffer);

   memmove(buffer, p, buf_size);
   buffer[buf_size] = 0;

   rsputs(buffer);

   rsprintf("</pre>\n");

   rsprintf("</td></tr></table></body></html>\r\n");
}

/*------------------------------------------------------------------*/

void show_form_query()
{
   int i = 0, size, run_number, status;
   char str[256];
   time_t now;
   HNDLE hDB, hkey, hkeyroot;
   KEY key;

   cm_get_experiment_database(&hDB, NULL);

   /* header */
   rsprintf("HTTP/1.0 200 Document follows\r\n");
   rsprintf("Server: MIDAS HTTP %s\r\n", cm_get_version());
   rsprintf("Content-Type: text/html; charset=iso-8859-1\r\n\r\n");

   rsprintf("<html><head><title>MIDAS ELog</title></head>\n");
   rsprintf("<body><form method=\"GET\" action=\"/EL/\">\n");

   if (*getparam("form") == 0)
      return;

   /* define hidden field for experiment */
   if (exp_name[0])
      rsprintf("<input type=hidden name=exp value=\"%s\">\n", exp_name);

   /* hidden field for form */
   rsprintf("<input type=hidden name=form value=\"%s\">\n", getparam("form"));

   rsprintf("<table border=3 cellpadding=1>\n");

  /*---- title row ----*/

   rsprintf("<tr><th colspan=2 bgcolor=#A0A0FF>MIDAS Electronic Logbook");
   rsprintf("<th colspan=2 bgcolor=#A0A0FF>Form \"%s\"</tr>\n", getparam("form"));

  /*---- menu buttons ----*/

   rsprintf("<tr><td colspan=4 bgcolor=#C0C0C0>\n");

   rsprintf("<input type=submit name=cmd value=\"Submit\">\n");
   rsprintf("<input type=reset value=\"Reset Form\">\n");
   rsprintf("</tr>\n\n");

  /*---- entry form ----*/

   time(&now);
   rsprintf("<tr><td colspan=2 bgcolor=#FFFF00>Entry date: %s", ctime(&now));

   run_number = 0;
   size = sizeof(run_number);
   status =
       db_get_value(hDB, 0, "/Runinfo/Run number", &run_number, &size, TID_INT, TRUE);
   assert(status == SUCCESS);

   if (run_number < 0) {
      cm_msg(MERROR, "show_form_query",
             "aborting on attempt to use invalid run number %d", run_number);
      abort();
   }

   rsprintf("<td bgcolor=#FFFF00>Run number: ");
   rsprintf("<input type=\"text\" size=10 maxlength=10 name=\"run\" value=\"%d\"</tr>",
            run_number);

   rsprintf
       ("<tr><td colspan=2 bgcolor=#FFA0A0>Author: <input type=\"text\" size=\"15\" maxlength=\"80\" name=\"Author\">\n");

   rsprintf
       ("<tr><th bgcolor=#A0FFA0>Item<th bgcolor=#FFFF00>Checked<th bgcolor=#A0A0FF colspan=2>Comment</tr>\n");

   sprintf(str, "/Elog/Forms/%s", getparam("form"));
   db_find_key(hDB, 0, str, &hkeyroot);
   i = 0;
   if (hkeyroot)
      for (i = 0;; i++) {
         db_enum_link(hDB, hkeyroot, i, &hkey);
         if (!hkey)
            break;

         db_get_key(hDB, hkey, &key);

         strcpy(str, key.name);
         if (str[0])
            str[strlen(str) - 1] = 0;
         if (equal_ustring(str, "attachment")) {
            size = sizeof(str);
            db_get_data(hDB, hkey, str, &size, TID_STRING);
            rsprintf("<tr><td colspan=2 align=center bgcolor=#FFFFFF><b>%s:</b>",
                     key.name);
            rsprintf
                ("<td bgcolor=#A0A0FF colspan=2><input type=text size=30 maxlength=255 name=c%d value=\"%s\"></tr>\n",
                 i, str);
         } else {
            rsprintf("<tr><td bgcolor=#A0FFA0>%d <b>%s</b>", i + 1, key.name);
            rsprintf
                ("<td bgcolor=#FFFF00 align=center><input type=checkbox name=x%d value=1>",
                 i);
            rsprintf
                ("<td bgcolor=#A0A0FF colspan=2><input type=text size=30 maxlength=255 name=c%d></tr>\n",
                 i);
         }
      }


  /*---- menu buttons at bottom ----*/

   if (i > 10) {
      rsprintf("<tr><td colspan=4 bgcolor=#C0C0C0>\n");

      rsprintf("<input type=submit name=cmd value=\"Submit\">\n");
      rsprintf("</tr>\n\n");
   }

   rsprintf("</tr></table>\n");
   rsprintf("</body></html>\r\n");
}

/*------------------------------------------------------------------*/

void gen_odb_attachment(char *path, char *b)
{
   HNDLE hDB, hkeyroot, hkey;
   KEY key;
   INT i, j, size;
   char str[256], data_str[256], hex_str[256];
   char data[10000];
   time_t now;

   cm_get_experiment_database(&hDB, NULL);
   db_find_key(hDB, 0, path, &hkeyroot);

   /* title row */
   size = sizeof(str);
   str[0] = 0;
   db_get_value(hDB, 0, "/Experiment/Name", str, &size, TID_STRING, TRUE);
   time(&now);

   sprintf(b, "<table border=3 cellpadding=1>\n");
   sprintf(b + strlen(b), "<tr><th colspan=2 bgcolor=#A0A0FF>%s</tr>\n", ctime(&now));
   sprintf(b + strlen(b), "<tr><th colspan=2 bgcolor=#FFA0A0>%s</tr>\n", path);

   /* enumerate subkeys */
   for (i = 0;; i++) {
      db_enum_link(hDB, hkeyroot, i, &hkey);
      if (!hkey)
         break;
      db_get_key(hDB, hkey, &key);

      /* resolve links */
      if (key.type == TID_LINK) {
         db_enum_key(hDB, hkeyroot, i, &hkey);
         db_get_key(hDB, hkey, &key);
      }

      if (key.type == TID_KEY) {
         /* for keys, don't display data value */
         sprintf(b + strlen(b), "<tr><td colspan=2 bgcolor=#FFD000>%s</td></tr>\n",
                 key.name);
      } else {
         /* display single value */
         if (key.num_values == 1) {
            size = sizeof(data);
            db_get_data(hDB, hkey, data, &size, key.type);
            db_sprintf(data_str, data, key.item_size, 0, key.type);
            db_sprintfh(hex_str, data, key.item_size, 0, key.type);

            if (data_str[0] == 0 || equal_ustring(data_str, "<NULL>")) {
               strcpy(data_str, "(empty)");
               hex_str[0] = 0;
            }

            if (strcmp(data_str, hex_str) != 0 && hex_str[0])
               sprintf(b + strlen(b),
                       "<tr><td bgcolor=#FFFF00>%s</td><td bgcolor=#FFFFFF>%s (%s)</td></tr>\n",
                       key.name, data_str, hex_str);
            else {
               sprintf(b + strlen(b),
                       "<tr><td bgcolor=#FFFF00>%s</td><td bgcolor=#FFFFFF>", key.name);
               strencode2(b + strlen(b), data_str);
               sprintf(b + strlen(b), "</td></tr>\n");
            }
         } else {
            /* display first value */
            sprintf(b + strlen(b), "<tr><td  bgcolor=#FFFF00 rowspan=%d>%s</td>\n",
                    key.num_values, key.name);

            for (j = 0; j < key.num_values; j++) {
               size = sizeof(data);
               db_get_data_index(hDB, hkey, data, &size, j, key.type);
               db_sprintf(data_str, data, key.item_size, 0, key.type);
               db_sprintfh(hex_str, data, key.item_size, 0, key.type);

               if (data_str[0] == 0 || equal_ustring(data_str, "<NULL>")) {
                  strcpy(data_str, "(empty)");
                  hex_str[0] = 0;
               }

               if (j > 0)
                  sprintf(b + strlen(b), "<tr>");

               if (strcmp(data_str, hex_str) != 0 && hex_str[0])
                  sprintf(b + strlen(b),
                          "<td bgcolor=#FFFFFF>[%d] %s (%s)<br></td></tr>\n", j, data_str,
                          hex_str);
               else
                  sprintf(b + strlen(b), "<td bgcolor=#FFFFFF>[%d] %s<br></td></tr>\n", j,
                          data_str);
            }
         }
      }
   }

   sprintf(b + strlen(b), "</table>\n");
}

/*------------------------------------------------------------------*/

void submit_elog()
{
   char str[80], author[256], path[256], path1[256];
   char mail_to[256], mail_from[256], mail_text[10000], mail_list[256],
       smtp_host[256], tag[80], mail_param[1000];
   char *buffer[3], *p, *pitem;
   HNDLE hDB, hkey;
   char att_file[3][256];
   int i, fh, size, n_mail, index;
   struct hostent *phe;
   char mhttpd_full_url[256];

   cm_get_experiment_database(&hDB, NULL);
   strcpy(att_file[0], getparam("attachment0"));
   strcpy(att_file[1], getparam("attachment1"));
   strcpy(att_file[2], getparam("attachment2"));

   /* check for author */
   if (*getparam("author") == 0) {
      rsprintf("HTTP/1.0 200 Document follows\r\n");
      rsprintf("Server: MIDAS HTTP %s\r\n", cm_get_version());
      rsprintf("Content-Type: text/html; charset=iso-8859-1\r\n\r\n");

      rsprintf("<html><head><title>ELog Error</title></head>\n");
      rsprintf("<i>Error: No author supplied.</i><p>\n");
      rsprintf("Please go back and enter your name in the <i>author</i> field.\n");
      rsprintf("<body></body></html>\n");
      return;
   }

   /* check for valid attachment files */
   for (i = 0; i < 3; i++) {
      buffer[i] = NULL;
      sprintf(str, "attachment%d", i);
      if (getparam(str) && *getparam(str) && _attachment_size[i] == 0) {
         /* replace '\' by '/' */
         strcpy(path, getparam(str));
         strcpy(path1, path);
         while (strchr(path, '\\'))
            *strchr(path, '\\') = '/';

         /* check if valid ODB tree */
         if (db_find_key(hDB, 0, path, &hkey) == DB_SUCCESS) {
            buffer[i] = M_MALLOC(100000);
            gen_odb_attachment(path, buffer[i]);
            strcpy(att_file[i], path);
            strcat(att_file[i], ".html");
            _attachment_buffer[i] = buffer[i];
            _attachment_size[i] = strlen(buffer[i]) + 1;
         }
         /* check if local file */
         else if ((fh = open(path1, O_RDONLY | O_BINARY)) >= 0) {
            size = lseek(fh, 0, SEEK_END);
            buffer[i] = M_MALLOC(size);
            lseek(fh, 0, SEEK_SET);
            read(fh, buffer[i], size);
            close(fh);
            strcpy(att_file[i], path);
            _attachment_buffer[i] = buffer[i];
            _attachment_size[i] = size;
         } else if (strncmp(path, "/HS/", 4) == 0) {
            buffer[i] = M_MALLOC(100000);
            size = 100000;
            strcpy(str, path + 4);
            if (strchr(str, '?')) {
               p = strchr(str, '?') + 1;
               p = strtok(p, "&");
               while (p != NULL) {
                  pitem = p;
                  p = strchr(p, '=');
                  if (p != NULL) {
                     *p++ = 0;
                     urlDecode(pitem);
                     urlDecode(p);

                     setparam(pitem, p);

                     p = strtok(NULL, "&");
                  }
               }
               *strchr(str, '?') = 0;
            }
            show_hist_page(str, buffer[i], &size, 0);
            strcpy(att_file[i], str);
            _attachment_buffer[i] = buffer[i];
            _attachment_size[i] = size;
            unsetparam("scale");
            unsetparam("offset");
            unsetparam("width");
            unsetparam("index");
         } else {
            rsprintf("HTTP/1.0 200 Document follows\r\n");
            rsprintf("Server: MIDAS HTTP %s\r\n", cm_get_version());
            rsprintf("Content-Type: text/html; charset=iso-8859-1\r\n\r\n");

            rsprintf("<html><head><title>ELog Error</title></head>\n");
            rsprintf("<i>Error: Attachment file <i>%s</i> not valid.</i><p>\n",
                     getparam(str));
            rsprintf
                ("Please go back and enter a proper filename (use the <b>Browse</b> button).\n");
            rsprintf("<body></body></html>\n");
            return;
         }
      }
   }

   /* add remote host name to author */
   phe = gethostbyaddr((char *) &remote_addr, 4, PF_INET);
   if (phe == NULL) {
      /* use IP number instead */
      strcpy(str, (char *) inet_ntoa(remote_addr));
   } else
      strcpy(str, phe->h_name);

   strcpy(author, getparam("author"));
   strcat(author, "@");
   strcat(author, str);

   tag[0] = 0;
   if (*getparam("edit"))
      strcpy(tag, getparam("orig"));

   el_submit(atoi(getparam("run")), author, getparam("type"),
             getparam("system"), getparam("subject"), getparam("text"),
             getparam("orig"), *getparam("html") ? "HTML" : "plain",
             att_file[0], _attachment_buffer[0], _attachment_size[0],
             att_file[1], _attachment_buffer[1], _attachment_size[1],
             att_file[2], _attachment_buffer[2], _attachment_size[2], tag, sizeof(tag));

   /* supersede host name with "/Elog/Host name" */
   size = sizeof(host_name);
   db_get_value(hDB, 0, "/Elog/Host name", host_name, &size, TID_STRING, TRUE);

   if (tcp_port == 80)
      sprintf(mhttpd_full_url, "http://%s/", host_name);
   else
      sprintf(mhttpd_full_url, "http://%s:%d/", host_name, tcp_port);

   /* check for mail submissions */
   mail_param[0] = 0;
   n_mail = 0;

   for (index = 0; index <= 1; index++) {
      if (index == 0)
         sprintf(str, "/Elog/Email %s", getparam("type"));
      else
         sprintf(str, "/Elog/Email %s", getparam("system"));

      if (db_find_key(hDB, 0, str, &hkey) == DB_SUCCESS) {
         size = sizeof(mail_list);
         db_get_data(hDB, hkey, mail_list, &size, TID_STRING);

         if (db_find_key(hDB, 0, "/Elog/SMTP host", &hkey) != DB_SUCCESS) {
            show_error("No SMTP host defined under /Elog/SMTP host");
            return;
         }
         size = sizeof(smtp_host);
         db_get_data(hDB, hkey, smtp_host, &size, TID_STRING);

         p = strtok(mail_list, ",");
         for (i = 0; p; i++) {
            strcpy(mail_to, p);
            sprintf(mail_from, "MIDAS <MIDAS@%s>", host_name);

            size = sizeof(str);
            str[0] = 0;
            db_get_value(hDB, 0, "/Experiment/Name", str, &size, TID_STRING, TRUE);

            sprintf(mail_text, "A new entry has been submitted by %s:\n\n", author);
            sprintf(mail_text + strlen(mail_text), "Experiment : %s\n", str);
            sprintf(mail_text + strlen(mail_text), "Type       : %s\n", getparam("type"));
            sprintf(mail_text + strlen(mail_text), "System     : %s\n",
                    getparam("system"));
            sprintf(mail_text + strlen(mail_text), "Subject    : %s\n",
                    getparam("subject"));

            if (exp_name[0])
               sprintf(mail_text + strlen(mail_text), "Link       : %sEL/%s?exp=%s\n",
                       mhttpd_full_url, tag, exp_name);
            else
               sprintf(mail_text + strlen(mail_text), "Link       : %sEL/%s\n",
                       mhttpd_full_url, tag);

            assert(strlen(mail_text) + 100 < sizeof(mail_text));        // bomb out on array overrun.

            strlcat(mail_text + strlen(mail_text), "\n", sizeof(mail_text));
            strlcat(mail_text + strlen(mail_text), getparam("text"),
                    sizeof(mail_text) - strlen(mail_text) - 50);
            strlcat(mail_text + strlen(mail_text), "\n", sizeof(mail_text));

            assert(strlen(mail_text) < sizeof(mail_text));      // bomb out on array overrun.

            sendmail(smtp_host, mail_from, mail_to, getparam("type"), mail_text);

            if (mail_param[0] == 0 && exp_name[0] == 0)
               strcpy(mail_param, "?");
            else
               strcat(mail_param, "&");
            sprintf(mail_param + strlen(mail_param), "mail%d=%s", n_mail++, mail_to);

            p = strtok(NULL, ",");
            if (!p)
               break;
            while (*p == ' ')
               p++;
         }
      }
   }

   for (i = 0; i < 3; i++)
      if (buffer[i])
         M_FREE(buffer[i]);

   rsprintf("HTTP/1.0 302 Found\r\n");
   rsprintf("Server: MIDAS HTTP %s\r\n", cm_get_version());

   if (exp_name[0])
      rsprintf("Location: /EL/%s?exp=%s%s\n\n<html>redir</html>\r\n", tag, exp_name,
               mail_param);
   else {
      if (mail_param[0])
         rsprintf("Location: /EL/%s?%s\n\n<html>redir</html>\r\n", tag, mail_param + 1);
      else
         rsprintf("Location: /EL/%s\n\n<html>redir</html>\r\n", tag);
   }
}

/*------------------------------------------------------------------*/

void submit_form()
{
   char str[256], att_name[256];
   char text[10000];
   int i, n_att, size;
   HNDLE hDB, hkey, hkeyroot;
   KEY key;

   /* check for author */
   if (*getparam("author") == 0) {
      rsprintf("HTTP/1.0 200 Document follows\r\n");
      rsprintf("Server: MIDAS HTTP %s\r\n", cm_get_version());
      rsprintf("Content-Type: text/html; charset=iso-8859-1\r\n\r\n");

      rsprintf("<html><head><title>ELog Error</title></head>\n");
      rsprintf("<i>Error: No author supplied.</i><p>\n");
      rsprintf("Please go back and enter your name in the <i>author</i> field.\n");
      rsprintf("<body></body></html>\n");
      return;
   }

   /* assemble text from form */
   cm_get_experiment_database(&hDB, NULL);
   sprintf(str, "/Elog/Forms/%s", getparam("form"));
   db_find_key(hDB, 0, str, &hkeyroot);
   text[0] = 0;
   n_att = 0;
   if (hkeyroot)
      for (i = 0;; i++) {
         db_enum_link(hDB, hkeyroot, i, &hkey);
         if (!hkey)
            break;

         db_get_key(hDB, hkey, &key);

         strcpy(str, key.name);
         if (str[0])
            str[strlen(str) - 1] = 0;
         if (equal_ustring(str, "attachment")) {
            /* generate attachments */
            size = sizeof(str);
            db_get_data(hDB, hkey, str, &size, TID_STRING);
            _attachment_size[n_att] = 0;
            sprintf(att_name, "attachment%d", n_att++);

            sprintf(str, "c%d", i);
            setparam(att_name, getparam(str));
         } else {
            sprintf(str, "x%d", i);
            sprintf(text + strlen(text), "%d %s : [%c]  ", i + 1, key.name,
                    *getparam(str) == '1' ? 'X' : ' ');
            sprintf(str, "c%d", i);
            sprintf(text + strlen(text), "%s\n", getparam(str));
         }
      }

   /* set parameters for submit_elog() */
   setparam("type", getparam("form"));
   setparam("system", "General");
   setparam("subject", getparam("form"));
   setparam("text", text);
   setparam("orig", "");
   setparam("html", "");

   submit_elog();
}

/*------------------------------------------------------------------*/

void show_elog_page(char *path)
{
   int size, i, run, msg_status, status, fh, length, first_message, last_message, index;
   char str[256], orig_path[256], command[80], ref[256], file_name[256];
   char date[80], author[80], type[80], system[80], subject[256], text[10000],
       orig_tag[80], reply_tag[80], attachment[3][256], encoding[80], att[256];
   HNDLE hDB, hkey, hkeyroot, hkeybutton;
   KEY key;
   FILE *f;
   BOOL display_run_number, allow_delete;
   char def_button[][NAME_LENGTH] = { "8h", "24h", "7d" };

   /* get flag for displaying run number and allow delete */
   cm_get_experiment_database(&hDB, NULL);
   display_run_number = TRUE;
   allow_delete = FALSE;
   size = sizeof(BOOL);
   db_get_value(hDB, 0, "/Elog/Display run number", &display_run_number, &size, TID_BOOL,
                TRUE);
   db_get_value(hDB, 0, "/Elog/Allow delete", &allow_delete, &size, TID_BOOL, TRUE);

   /*---- interprete commands ---------------------------------------*/

   strcpy(command, getparam("cmd"));

   if (*getparam("form")) {
      if (*getparam("type")) {
         sprintf(str, "EL/?form=%s", getparam("form"));
         redirect(str);
         return;
      }
      if (equal_ustring(command, "submit"))
         submit_form();
      else
         show_form_query();
      return;
   }

   if (equal_ustring(command, "new")) {
      if (*getparam("file"))
         show_elog_new(NULL, FALSE, getparam("file"));
      else
         show_elog_new(NULL, FALSE, NULL);
      return;
   }

   if (equal_ustring(command, "Create ELog from this page")) {
      strcpy(str, path);
      while (strchr(path, '/'))
         *strchr(path, '/') = '\\';

      show_elog_new(NULL, FALSE, path);
      return;
   }

   if (equal_ustring(command, "edit")) {
      show_elog_new(path, TRUE, NULL);
      return;
   }

   if (equal_ustring(command, "reply")) {
      show_elog_new(path, FALSE, NULL);
      return;
   }

   if (equal_ustring(command, "submit")) {
      submit_elog();
      return;
   }

   if (equal_ustring(command, "query")) {
      show_elog_query();
      return;
   }

   if (equal_ustring(command, "submit query")) {
      show_elog_submit_query(0);
      return;
   }

   if (strncmp(command, "Last ", 5) == 0) {
      if (command[strlen(command) - 1] == 'h')
         sprintf(str, "EL/last%d", atoi(command + 5));
      else if (command[strlen(command) - 1] == 'd')
         sprintf(str, "EL/last%d", atoi(command + 5) * 24);

      redirect(str);
      return;
   }

   if (equal_ustring(command, "delete")) {
      show_elog_delete(path);
      return;
   }

   if (strncmp(path, "last", 4) == 0) {
      show_elog_submit_query(atoi(path + 4));
      return;
   }

   if (equal_ustring(command, "runlog")) {
      sprintf(str, "EL/runlog.txt");
      redirect(str);
      return;
   }

  /*---- check if file requested -----------------------------------*/

   if (strlen(path) > 13 && path[6] == '_' && path[13] == '_') {
      cm_get_experiment_database(&hDB, NULL);
      file_name[0] = 0;
      if (hDB > 0) {
         size = sizeof(file_name);
         memset(file_name, 0, size);

         status =
             db_get_value(hDB, 0, "/Logger/Elog dir", file_name, &size, TID_STRING,
                          FALSE);
         if (status != DB_SUCCESS)
            db_get_value(hDB, 0, "/Logger/Data dir", file_name, &size, TID_STRING, TRUE);

         if (file_name[0] != 0)
            if (file_name[strlen(file_name) - 1] != DIR_SEPARATOR)
               strcat(file_name, DIR_SEPARATOR_STR);
      }
      strcat(file_name, path);

      fh = open(file_name, O_RDONLY | O_BINARY);
      if (fh > 0) {
         lseek(fh, 0, SEEK_END);
         length = TELL(fh);
         lseek(fh, 0, SEEK_SET);

         rsprintf("HTTP/1.0 200 Document follows\r\n");
         rsprintf("Server: MIDAS HTTP %s\r\n", cm_get_version());
         rsprintf("Accept-Ranges: bytes\r\n");

         /* return proper header for file type */
         for (i = 0; i < (int) strlen(path); i++)
            str[i] = toupper(path[i]);
         str[i] = 0;

         for (i = 0; filetype[i].ext[0]; i++)
            if (strstr(str, filetype[i].ext))
               break;

         if (filetype[i].ext[0])
            rsprintf("Content-Type: %s\r\n", filetype[i].type);
         else if (strchr(str, '.') == NULL)
            rsprintf("Content-Type: text/plain\r\n");
         else
            rsprintf("Content-Type: application/octet-stream\r\n");

         rsprintf("Content-Length: %d\r\n\r\n", length);

         /* return if file too big */
         if (length > (int) (sizeof(return_buffer) - strlen(return_buffer))) {
            printf("return buffer too small\n");
            close(fh);
            return;
         }

         return_length = strlen(return_buffer) + length;
         read(fh, return_buffer + strlen(return_buffer), length);

         close(fh);
      }

      return;
   }

  /*---- check if runlog is requested ------------------------------*/

   if (path[0] > '9') {
      show_rawfile(path);
      return;
   }

  /*---- check next/previous message -------------------------------*/

   last_message = first_message = FALSE;
   if (equal_ustring(command, "next") || equal_ustring(command, "previous") ||
       equal_ustring(command, "last")) {
      strcpy(orig_path, path);

      if (equal_ustring(command, "last"))
         path[0] = 0;

      do {
         strcat(path, equal_ustring(command, "next") ? "+1" : "-1");
         status = el_search_message(path, &fh, TRUE);
         close(fh);
         if (status != EL_SUCCESS) {
            if (equal_ustring(command, "next"))
               last_message = TRUE;
            else
               first_message = TRUE;
            strcpy(path, orig_path);
            break;
         }

         size = sizeof(text);
         el_retrieve(path, date, &run, author, type, system, subject,
                     text, &size, orig_tag, reply_tag,
                     attachment[0], attachment[1], attachment[2], encoding);

         if (strchr(author, '@'))
            *strchr(author, '@') = 0;
         if (*getparam("lauthor") == '1' && !equal_ustring(getparam("author"), author))
            continue;
         if (*getparam("ltype") == '1' && !equal_ustring(getparam("type"), type))
            continue;
         if (*getparam("lsystem") == '1' && !equal_ustring(getparam("system"), system))
            continue;
         if (*getparam("lsubject") == '1') {
            strcpy(str, getparam("subject"));
            for (i = 0; i < (int) strlen(str); i++)
               str[i] = toupper(str[i]);
            for (i = 0; i < (int) strlen(subject); i++)
               subject[i] = toupper(subject[i]);

            if (strstr(subject, str) == NULL)
               continue;
         }

         sprintf(str, "EL/%s", path);

         if (*getparam("lauthor") == '1') {
            if (strchr(str, '?') == NULL)
               strcat(str, "?lauthor=1");
            else
               strcat(str, "&lauthor=1");
         }

         if (*getparam("ltype") == '1') {
            if (strchr(str, '?') == NULL)
               strcat(str, "?ltype=1");
            else
               strcat(str, "&ltype=1");
         }

         if (*getparam("lsystem") == '1') {
            if (strchr(str, '?') == NULL)
               strcat(str, "?lsystem=1");
            else
               strcat(str, "&lsystem=1");
         }

         if (*getparam("lsubject") == '1') {
            if (strchr(str, '?') == NULL)
               strcat(str, "?lsubject=1");
            else
               strcat(str, "&lsubject=1");
         }

         redirect(str);
         return;

      } while (TRUE);
   }

  /*---- get current message ---------------------------------------*/

   size = sizeof(text);
   strcpy(str, path);
   subject[0] = 0;
   msg_status = el_retrieve(str, date, &run, author, type, system, subject,
                            text, &size, orig_tag, reply_tag,
                            attachment[0], attachment[1], attachment[2], encoding);

  /*---- header ----*/

   /* header */
   rsprintf("HTTP/1.0 200 Document follows\r\n");
   rsprintf("Server: MIDAS HTTP %s\r\n", cm_get_version());
   rsprintf("Content-Type: text/html; charset=iso-8859-1\r\n\r\n");

   rsprintf("<html><head><title>MIDAS ELog - %s</title></head>\n", subject);
   rsprintf("<body><form method=\"GET\" action=\"/EL/%s\">\n", str);

   /* define hidden field for experiment */
   if (exp_name[0])
      rsprintf("<input type=hidden name=exp value=\"%s\">\n", exp_name);

   rsprintf("<table cols=2 border=2 cellpadding=2>\n");

  /*---- title row ----*/

   size = sizeof(str);
   str[0] = 0;
   db_get_value(hDB, 0, "/Experiment/Name", str, &size, TID_STRING, TRUE);

   rsprintf("<tr><th bgcolor=#A0A0FF>MIDAS Electronic Logbook");
   if (elog_mode)
      rsprintf("<th bgcolor=#A0A0FF>Logbook \"%s\"</tr>\n", str);
   else
      rsprintf("<th bgcolor=#A0A0FF>Experiment \"%s\"</tr>\n", str);

  /*---- menu buttons ----*/

   rsprintf("<tr><td colspan=2 bgcolor=#C0C0C0>\n");
   rsprintf("<input type=submit name=cmd value=New>\n");
   rsprintf("<input type=submit name=cmd value=Edit>\n");
   if (allow_delete)
      rsprintf("<input type=submit name=cmd value=Delete>\n");
   rsprintf("<input type=submit name=cmd value=Reply>\n");
   rsprintf("<input type=submit name=cmd value=Query>\n");

   /* check forms from ODB */
   db_find_key(hDB, 0, "/Elog/Forms", &hkeyroot);
   if (hkeyroot)
      for (i = 0;; i++) {
         db_enum_link(hDB, hkeyroot, i, &hkey);
         if (!hkey)
            break;

         db_get_key(hDB, hkey, &key);

         rsprintf("<input type=submit name=form value=\"%s\">\n", key.name);
      }

   rsprintf("<input type=submit name=cmd value=Runlog>\n");

   if (!elog_mode)
      rsprintf("<input type=submit name=cmd value=Status>\n");
   rsprintf("</tr>\n");

   /* "last x" button row */
   rsprintf("<tr><td colspan=2 bgcolor=#D0D0D0>\n");

   db_find_key(hDB, 0, "/Elog/Buttons", &hkeybutton);
   if (hkeybutton == 0) {
      /* create default buttons */
      db_create_key(hDB, 0, "/Elog/Buttons", TID_STRING);
      db_find_key(hDB, 0, "/Elog/Buttons", &hkeybutton);
      db_set_data(hDB, hkeybutton, def_button, sizeof(def_button), 3, TID_STRING);
   }

   db_get_key(hDB, hkeybutton, &key);

   for (i = 0; i < key.num_values; i++) {
      size = sizeof(str);
      db_get_data_index(hDB, hkeybutton, str, &size, i, TID_STRING);
      rsprintf("<input type=submit name=cmd value=\"Last %s\">\n", str);
   }

   rsprintf("</tr>\n");

   rsprintf("<tr><td colspan=2 bgcolor=#E0E0E0>");
   rsprintf("<input type=submit name=cmd value=Next>\n");
   rsprintf("<input type=submit name=cmd value=Previous>\n");
   rsprintf("<input type=submit name=cmd value=Last>\n");
   rsprintf("<i>Check a category to browse only entries from that category</i>\n");
   rsprintf("</tr>\n\n");

   if (msg_status != EL_FILE_ERROR && (reply_tag[0] || orig_tag[0])) {
      rsprintf("<tr><td colspan=2 bgcolor=#F0F0F0>");
      if (orig_tag[0]) {
         if (exp_name[0])
            sprintf(ref, "/EL/%s?exp=%s", orig_tag, exp_name);
         else
            sprintf(ref, "/EL/%s", orig_tag);
         rsprintf("  <a href=\"%s\">Original message</a>  ", ref);
      }
      if (reply_tag[0]) {
         if (exp_name[0])
            sprintf(ref, "/EL/%s?exp=%s", reply_tag, exp_name);
         else
            sprintf(ref, "/EL/%s", reply_tag);
         rsprintf("  <a href=\"%s\">Reply to this message</a>  ", ref);
      }
      rsprintf("</tr>\n");
   }

  /*---- message ----*/

   if (msg_status == EL_FILE_ERROR)
      rsprintf
          ("<tr><td bgcolor=#FF0000 colspan=2 align=center><h1>No message available</h1></tr>\n");
   else {
      if (last_message)
         rsprintf
             ("<tr><td bgcolor=#FF0000 colspan=2 align=center><b>This is the last message in the ELog</b></tr>\n");

      if (first_message)
         rsprintf
             ("<tr><td bgcolor=#FF0000 colspan=2 align=center><b>This is the first message in the ELog</b></tr>\n");

      /* check for mail submissions */
      for (i = 0;; i++) {
         sprintf(str, "mail%d", i);
         if (*getparam(str)) {
            if (i == 0)
               rsprintf("<tr><td colspan=2 bgcolor=#FFC020>");
            rsprintf("Mail sent to <b>%s</b><br>\n", getparam(str));
         } else
            break;
      }
      if (i > 0)
         rsprintf("</tr>\n");


      if (display_run_number) {
         rsprintf("<tr><td bgcolor=#FFFF00>Entry date: <b>%s</b>", date);

         rsprintf("<td bgcolor=#FFFF00>Run number: <b>%d</b></tr>\n\n", run);
      } else
         rsprintf("<tr><td colspan=2 bgcolor=#FFFF00>Entry date: <b>%s</b></tr>\n\n",
                  date);


      /* define hidded fields */
      strcpy(str, author);
      if (strchr(str, '@'))
         *strchr(str, '@') = 0;
      rsprintf("<input type=hidden name=author  value=\"%s\">\n", str);
      rsprintf("<input type=hidden name=type    value=\"%s\">\n", type);
      rsprintf("<input type=hidden name=system  value=\"%s\">\n", system);
      rsprintf("<input type=hidden name=subject value=\"%s\">\n\n", subject);

      if (*getparam("lauthor") == '1')
         rsprintf
             ("<tr><td bgcolor=#FFA0A0><input type=\"checkbox\" checked name=\"lauthor\" value=\"1\">");
      else
         rsprintf
             ("<tr><td bgcolor=#FFA0A0><input type=\"checkbox\" name=\"lauthor\" value=\"1\">");
      rsprintf("  Author: <b>%s</b>\n", author);

      if (*getparam("ltype") == '1')
         rsprintf
             ("<td bgcolor=#FFA0A0><input type=\"checkbox\" checked name=\"ltype\" value=\"1\">");
      else
         rsprintf
             ("<td bgcolor=#FFA0A0><input type=\"checkbox\" name=\"ltype\" value=\"1\">");
      rsprintf("  Type: <b>%s</b></tr>\n", type);

      if (*getparam("lsystem") == '1')
         rsprintf
             ("<tr><td bgcolor=#A0FFA0><input type=\"checkbox\" checked name=\"lsystem\" value=\"1\">");
      else
         rsprintf
             ("<tr><td bgcolor=#A0FFA0><input type=\"checkbox\" name=\"lsystem\" value=\"1\">");

      rsprintf("  System: <b>%s</b>\n", system);

      if (*getparam("lsubject") == '1')
         rsprintf
             ("<td bgcolor=#A0FFA0><input type=\"checkbox\" checked name=\"lsubject\" value=\"1\">");
      else
         rsprintf
             ("<td bgcolor=#A0FFA0><input type=\"checkbox\" name=\"lsubject\" value=\"1\">");
      rsprintf("  Subject: <b>%s</b></tr>\n", subject);


      /* message text */
      rsprintf("<tr><td colspan=2>\n");
      if (equal_ustring(encoding, "plain")) {
         rsputs("<pre>");
         rsputs2(text);
         rsputs("</pre>");
      } else
         rsputs(text);
      rsputs("</tr>\n");

      for (index = 0; index < 3; index++) {
         if (attachment[index][0]) {
            for (i = 0; i < (int) strlen(attachment[index]); i++)
               att[i] = toupper(attachment[index][i]);
            att[i] = 0;

            if (exp_name[0])
               sprintf(ref, "/EL/%s?exp=%s", attachment[index], exp_name);
            else
               sprintf(ref, "/EL/%s", attachment[index]);

            if (strstr(att, ".GIF") || strstr(att, ".PNG") || strstr(att, ".JPG")) {
               rsprintf
                   ("<tr><td colspan=2>Attachment: <a href=\"%s\"><b>%s</b></a><br>\n",
                    ref, attachment[index] + 14);
               rsprintf("<img src=\"%s\"></tr>", ref);
            } else {
               rsprintf
                   ("<tr><td colspan=2 bgcolor=#C0C0FF>Attachment: <a href=\"%s\"><b>%s</b></a>\n",
                    ref, attachment[index] + 14);
               if (strstr(att, ".TXT") || strstr(att, ".ASC") || strchr(att, '.') == NULL) {
                  /* display attachment */
                  rsprintf("<br>");
                  if (!strstr(att, ".HTML"))
                     rsprintf("<pre>");

                  file_name[0] = 0;
                  size = sizeof(file_name);
                  memset(file_name, 0, size);
                  db_get_value(hDB, 0, "/Logger/Data dir", file_name, &size, TID_STRING,
                               TRUE);
                  if (file_name[0] != 0)
                     if (file_name[strlen(file_name) - 1] != DIR_SEPARATOR)
                        strcat(file_name, DIR_SEPARATOR_STR);
                  strcat(file_name, attachment[index]);

                  f = fopen(file_name, "rt");
                  if (f != NULL) {
                     while (!feof(f)) {
                        str[0] = 0;
                        fgets(str, sizeof(str), f);
                        if (!strstr(att, ".HTML"))
                           rsputs2(str);
                        else
                           rsputs(str);
                     }
                     fclose(f);
                  }

                  if (!strstr(att, ".HTML"))
                     rsprintf("</pre>");
                  rsprintf("\n");
               }
               rsprintf("</tr>\n");
            }
         }
      }
   }

   rsprintf("</table>\n");
   rsprintf("</body></html>\r\n");
}

/*------------------------------------------------------------------*/

BOOL is_editable(char *eq_name, char *var_name)
{
   HNDLE hDB, hkey;
   KEY key;
   char str[256];
   int i, size;

   cm_get_experiment_database(&hDB, NULL);
   sprintf(str, "/Equipment/%s/Settings/Editable", eq_name);
   db_find_key(hDB, 0, str, &hkey);

   /* if no editable entry found, use default */
   if (!hkey) {
      return (equal_ustring(var_name, "Demand") ||
              equal_ustring(var_name, "Output") || strncmp(var_name, "D_", 2) == 0);
   }

   db_get_key(hDB, hkey, &key);
   for (i = 0; i < key.num_values; i++) {
      size = sizeof(str);
      db_get_data_index(hDB, hkey, str, &size, i, TID_STRING);
      if (equal_ustring(var_name, str))
         return TRUE;
   }
   return FALSE;
}

void show_sc_page(char *path, int refresh)
{
   int i, j, k, colspan, size, n_var, i_edit, i_set;
   char str[256], eq_name[32], group[32], name[32], ref[256];
   char group_name[MAX_GROUPS][32], data[256];
   HNDLE hDB, hkey, hkeyeq, hkeyset, hkeynames, hkeyvar, hkeyroot;
   KEY eqkey, key, varkey;
   char data_str[256], hex_str[256];

   cm_get_experiment_database(&hDB, NULL);

   /* check if variable to edit */
   i_edit = -1;
   if (equal_ustring(getparam("cmd"), "Edit"))
      i_edit = atoi(getparam("index"));

   /* check if variable to set */
   i_set = -1;
   if (equal_ustring(getparam("cmd"), "Set"))
      i_set = atoi(getparam("index"));

   /* split path into equipment and group */
   strcpy(eq_name, path);
   strcpy(group, "All");
   if (strchr(eq_name, '/')) {
      strcpy(group, strchr(eq_name, '/') + 1);
      *strchr(eq_name, '/') = 0;
   }

   /* check for "names" in settings */
   if (eq_name[0]) {
      sprintf(str, "/Equipment/%s/Settings", eq_name);
      db_find_key(hDB, 0, str, &hkeyset);
      hkeynames = 0;
      if (hkeyset) {
         for (i = 0;; i++) {
            db_enum_link(hDB, hkeyset, i, &hkeynames);

            if (!hkeynames)
               break;

            db_get_key(hDB, hkeynames, &key);

            if (strncmp(key.name, "Names", 5) == 0)
               break;
         }

      }

      /* redirect if no names found */
      if (!hkeyset || !hkeynames) {
         /* redirect */
         sprintf(str, "Equipment/%s/Variables", eq_name);
         redirect(str);
         return;
      }
   }

   sprintf(str, "SC/%s/%s", eq_name, group);
   show_header(hDB, "MIDAS slow control", "GET", str, 8, i_edit == -1 ? refresh : 0);

  /*---- menu buttons ----*/

   rsprintf("<tr><td colspan=15 bgcolor=#C0C0C0>\n");

   rsprintf("<input type=submit name=cmd value=ODB>\n");
   rsprintf("<input type=submit name=cmd value=Status>\n");
   rsprintf("<input type=submit name=cmd value=Help>\n");
   rsprintf("</tr>\n\n");

  /*---- enumerate SC equipment ----*/

   rsprintf("<tr><td colspan=15 bgcolor=#FFFF00><i>Equipment:</i> &nbsp;&nbsp;\n");

   db_find_key(hDB, 0, "/Equipment", &hkey);
   if (hkey)
      for (i = 0;; i++) {
         db_enum_link(hDB, hkey, i, &hkeyeq);

         if (!hkeyeq)
            break;

         db_get_key(hDB, hkeyeq, &eqkey);

         db_find_key(hDB, hkeyeq, "Settings", &hkeyset);
         if (hkeyset) {
            for (j = 0;; j++) {
               db_enum_link(hDB, hkeyset, j, &hkeynames);

               if (!hkeynames)
                  break;

               db_get_key(hDB, hkeynames, &key);

               if (strncmp(key.name, "Names", 5) == 0) {
                  if (equal_ustring(eq_name, eqkey.name))
                     rsprintf("<b>%s</b> &nbsp;&nbsp;", eqkey.name);
                  else {
                     if (exp_name[0])
                        rsprintf("<a href=\"/SC/%s?exp=%s\">%s</a> &nbsp;&nbsp;",
                                 eqkey.name, exp_name, eqkey.name);
                     else
                        rsprintf("<a href=\"/SC/%s?\">%s</a> &nbsp;&nbsp;", eqkey.name,
                                 eqkey.name);
                  }
                  break;
               }
            }
         }
      }
   rsprintf("</tr>\n");

   if (!eq_name[0]) {
      rsprintf("</table>");
      return;
   }

   /*---- display SC ----*/

   n_var = 0;
   sprintf(str, "/Equipment/%s/Settings/Names", eq_name);
   db_find_key(hDB, 0, str, &hkey);

   if (hkey) {
      
      /*---- single name array ----*/
      rsprintf("<tr><td colspan=15 bgcolor=#FFFFA0><i>Groups:</i> &nbsp;&nbsp;");

      /* "all" group */
      if (equal_ustring(group, "All"))
         rsprintf("<b>All</b> &nbsp;&nbsp;");
      else {
         if (exp_name[0])
            rsprintf("<a href=\"/SC/%s/All?exp=%s\">All</a> &nbsp;&nbsp;", eq_name,
                     exp_name);
         else
            rsprintf("<a href=\"/SC/%s/All?\">All</a> &nbsp;&nbsp;", eq_name);
      }

      /* collect groups */

      memset(group_name, 0, sizeof(group_name));
      db_get_key(hDB, hkey, &key);

      for (i = 0; i < key.num_values; i++) {
         size = sizeof(str);
         db_get_data_index(hDB, hkey, str, &size, i, TID_STRING);

         if (strchr(str, '%')) {
            *strchr(str, '%') = 0;
            for (j = 0; j < MAX_GROUPS; j++) {
               if (equal_ustring(group_name[j], str) || group_name[j][0] == 0)
                  break;
            }
            if (group_name[j][0] == 0)
               strcpy(group_name[j], str);
         }
      }

      for (i = 0; i < MAX_GROUPS && group_name[i][0]; i++) {
         if (equal_ustring(group_name[i], group))
            rsprintf("<b>%s</b> &nbsp;&nbsp;", group_name[i]);
         else {
            if (exp_name[0])
               rsprintf("<a href=\"/SC/%s/%s?exp=%s\">%s</a> &nbsp;&nbsp;",
                        eq_name, group_name[i], exp_name, group_name[i]);
            else
               rsprintf("<a href=\"/SC/%s/%s?\">%s</a> &nbsp;&nbsp;",
                        eq_name, group_name[i], group_name[i]);
         }
      }
      rsprintf("</tr>\n");

      /* count variables */
      sprintf(str, "/Equipment/%s/Variables", eq_name);
      db_find_key(hDB, 0, str, &hkeyvar);
      if (!hkeyvar) {
         rsprintf("</table>");
         return;
      }
      for (i = 0;; i++) {
         db_enum_link(hDB, hkeyvar, i, &hkey);
         if (!hkey)
            break;
      }

      if (i == 0 || i > 15) {
         rsprintf("</table>");
         return;
      }

      /* title row */
      colspan = 15 - i;
      rsprintf("<tr><th colspan=%d>Names", colspan);

      /* display entries for this group */
      for (i = 0;; i++) {
         db_enum_link(hDB, hkeyvar, i, &hkey);

         if (!hkey)
            break;

         db_get_key(hDB, hkey, &key);
         rsprintf("<th>%s", key.name);
      }

      rsprintf("</tr>\n");

      /* data for current group */
      sprintf(str, "/Equipment/%s/Settings/Names", eq_name);
      db_find_key(hDB, 0, str, &hkeyset);
      db_get_key(hDB, hkeyset, &key);
      for (i = 0; i < key.num_values; i++) {
         size = sizeof(str);
         db_get_data_index(hDB, hkeyset, str, &size, i, TID_STRING);

         name[0] = 0;
         if (strchr(str, '%')) {
            *strchr(str, '%') = 0;
            strcpy(name, str + strlen(str) + 1);
         } else
            strcpy(name, str);

         if (!equal_ustring(group, "All") && !equal_ustring(str, group))
            continue;

         rsprintf("<tr><td colspan=%d>%s", colspan, name);

         for (j = 0;; j++) {
            db_enum_link(hDB, hkeyvar, j, &hkey);
            if (!hkey)
               break;
            db_get_key(hDB, hkey, &varkey);

            size = sizeof(data);
            db_get_data_index(hDB, hkey, data, &size, i, varkey.type);
            db_sprintf(str, data, varkey.item_size, 0, varkey.type);

            if (is_editable(eq_name, varkey.name)) {
               if (n_var == i_set) {
                  /* set value */
                  strcpy(str, getparam("value"));
                  db_sscanf(str, data, &size, 0, varkey.type);
                  db_set_data_index(hDB, hkey, data, size, i, varkey.type);

                  /* redirect (so that 'reload' does not reset value) */
                  strlen_retbuf = 0;
                  sprintf(str, "SC/%s/%s", eq_name, group);
                  redirect(str);
                  return;
               }
               if (n_var == i_edit) {
                  rsprintf
                      ("<td align=center><input type=text size=10 maxlenth=80 name=value value=\"%s\">\n",
                       str);
                  rsprintf("<input type=submit size=20 name=cmd value=Set>\n");
                  rsprintf("<input type=hidden name=index value=%d>\n", i_edit);
                  rsprintf("<input type=hidden name=cmd value=Set>\n");
                  n_var++;
               } else {
                  if (exp_name[0])
                     sprintf(ref, "/SC/%s/%s?cmd=Edit&index=%d&exp=%s",
                             eq_name, group, n_var, exp_name);
                  else
                     sprintf(ref, "/SC/%s/%s?cmd=Edit&index=%d", eq_name, group, n_var);

                  rsprintf("<td align=center><a href=\"%s\">%s</a>", ref, str);
                  n_var++;
               }
            } else
               rsprintf("<td align=center>%s", str);
         }

         rsprintf("</tr>\n");
      }
   } else {
      /*---- multiple name arrays ----*/
      rsprintf("<tr><td colspan=15 bgcolor=#FFFFA0><i>Groups:</i> ");

      /* "all" group */
      if (equal_ustring(group, "All"))
         rsprintf("<b>All</b> &nbsp;&nbsp;");
      else {
         if (exp_name[0])
            rsprintf("<a href=\"/SC/%s?exp=%s\">All</a> &nbsp;&nbsp;", eq_name, exp_name);
         else
            rsprintf("<a href=\"/SC/%s?\">All</a> &nbsp;&nbsp;", eq_name);
      }

      /* groups from Variables tree */

      sprintf(str, "/Equipment/%s/Variables", eq_name);
      db_find_key(hDB, 0, str, &hkeyvar);

      for (i = 0;; i++) {
         db_enum_link(hDB, hkeyvar, i, &hkey);

         if (!hkey)
            break;

         db_get_key(hDB, hkey, &key);

         if (equal_ustring(key.name, group))
            rsprintf("<b>%s</b> &nbsp;&nbsp;", key.name);
         else {
            if (exp_name[0])
               rsprintf("<a href=\"/SC/%s/%s?exp=%s\">%s</a> &nbsp;&nbsp;",
                        eq_name, key.name, exp_name, key.name);
            else
               rsprintf("<a href=\"/SC/%s/%s?\">%s</a> &nbsp;&nbsp;",
                        eq_name, key.name, key.name);
         }
      }

      rsprintf("</tr>\n");

      /* enumerate variable arrays */

      for (i = 0;; i++) {
         db_enum_link(hDB, hkeyvar, i, &hkey);

         if (!hkey)
            break;

         db_get_key(hDB, hkey, &varkey);

         if (!equal_ustring(group, "All") && !equal_ustring(varkey.name, group))
            continue;

         /* title row */
         rsprintf("<tr><th colspan=9>Names<th>%s</tr>\n", varkey.name);


         if (varkey.type == TID_KEY) {
            hkeyroot = hkey;

            /* enumerate subkeys */
            for (j = 0;; j++) {
               db_enum_key(hDB, hkeyroot, j, &hkey);
               if (!hkey)
                  break;
               db_get_key(hDB, hkey, &key);

               if (key.type == TID_KEY) {
                  /* for keys, don't display data value */
                  rsprintf("<tr><td colspan=9>%s<br></tr>\n", key.name);
               } else {
                  /* display single value */
                  if (key.num_values == 1) {
                     size = sizeof(data);
                     db_get_data(hDB, hkey, data, &size, key.type);
                     db_sprintf(data_str, data, key.item_size, 0, key.type);
                     db_sprintfh(hex_str, data, key.item_size, 0, key.type);

                     if (data_str[0] == 0 || equal_ustring(data_str, "<NULL>")) {
                        strcpy(data_str, "(empty)");
                        hex_str[0] = 0;
                     }

                     if (strcmp(data_str, hex_str) != 0 && hex_str[0])
                        rsprintf
                            ("<tr><td colspan=9>%s<td align=center>%s (%s)<br></tr>\n",
                             key.name, data_str, hex_str);
                     else
                        rsprintf("<tr><td colspan=9>%s<td align=center>%s<br></tr>\n",
                                 key.name, data_str);
                  } else {
                     /* display first value */
                     rsprintf("<tr><td colspan=9 rowspan=%d>%s\n", key.num_values,
                              key.name);

                     for (k = 0; k < key.num_values; k++) {
                        size = sizeof(data);
                        db_get_data_index(hDB, hkey, data, &size, k, key.type);
                        db_sprintf(data_str, data, key.item_size, 0, key.type);
                        db_sprintfh(hex_str, data, key.item_size, 0, key.type);

                        if (data_str[0] == 0 || equal_ustring(data_str, "<NULL>")) {
                           strcpy(data_str, "(empty)");
                           hex_str[0] = 0;
                        }

                        if (k > 0)
                           rsprintf("<tr>");

                        if (strcmp(data_str, hex_str) != 0 && hex_str[0])
                           rsprintf("<td>[%d] %s (%s)<br></tr>\n", k, data_str, hex_str);
                        else
                           rsprintf("<td>[%d] %s<br></tr>\n", k, data_str);
                     }
                  }
               }
            }
         } else {
            /* data for current group */
            sprintf(str, "/Equipment/%s/Settings/Names %s", eq_name, varkey.name);
            db_find_key(hDB, 0, str, &hkeyset);

            if (varkey.num_values > 1000)
               rsprintf("<tr><td colspan=9>%s<td align=center><i>... %d values ...</i>",
                        varkey.name, varkey.num_values);
            else {
               for (j = 0; j < varkey.num_values; j++) {
                  if (hkeyset) {
                     size = sizeof(name);
                     db_get_data_index(hDB, hkeyset, name, &size, j, TID_STRING);
                  } else
                     sprintf(name, "%s[%d]", varkey.name, j);

                  rsprintf("<tr><td colspan=9>%s", name);

                  size = sizeof(data);
                  db_get_data_index(hDB, hkey, data, &size, j, varkey.type);
                  db_sprintf(str, data, varkey.item_size, 0, varkey.type);

                  if (is_editable(eq_name, varkey.name)) {
                     if (n_var == i_set) {
                        /* set value */
                        strcpy(str, getparam("value"));
                        db_sscanf(str, data, &size, 0, varkey.type);
                        db_set_data_index(hDB, hkey, data, size, j, varkey.type);

                        /* redirect (so that 'reload' does not reset value) */
                        strlen_retbuf = 0;
                        sprintf(str, "SC/%s/%s", eq_name, group);
                        redirect(str);
                        return;
                     }
                     if (n_var == i_edit) {
                        rsprintf
                            ("<td align=center><input type=text size=10 maxlenth=80 name=value value=\"%s\">\n",
                             str);
                        rsprintf("<input type=submit size=20 name=cmd value=Set></tr>\n");
                        rsprintf("<input type=hidden name=index value=%d>\n", i_edit);
                        rsprintf("<input type=hidden name=cmd value=Set>\n");
                        n_var++;
                     } else {
                        if (exp_name[0])
                           sprintf(ref, "/SC/%s/%s?cmd=Edit&index=%d&exp=%s",
                                   eq_name, group, n_var, exp_name);
                        else
                           sprintf(ref, "/SC/%s/%s?cmd=Edit&index=%d",
                                   eq_name, group, n_var);

                        rsprintf("<td align=center><a href=\"%s\">%s</a>", ref, str);
                        n_var++;
                     }

                  } else
                     rsprintf("<td align=center>%s", str);
               }
            }

            rsprintf("</tr>\n");
         }
      }
   }

   rsprintf("</table>\r\n");
}

/*------------------------------------------------------------------*/

char *find_odb_tag(char *p, char *path, BOOL * edit)
{
   char str[256], *ps;

   *edit = 0;
   do {
      while (*p && *p != '<')
         p++;

      /* return if end of string reached */
      if (!*p)
         return NULL;

      p++;
      while (*p && ((*p == ' ') || iscntrl(*p)))
         p++;

      strncpy(str, p, 4);
      str[4] = 0;

      if (equal_ustring(str, "odb ")) {
         ps = p - 1;
         p += 4;
         while (*p && ((*p == ' ') || iscntrl(*p)))
            p++;

         do {
            strncpy(str, p, 4);
            str[4] = 0;
            if (equal_ustring(str, "src=")) {
               p += 4;
               if (*p == '\"') {
                  p++;
                  while (*p && *p != '\"')
                     *path++ = *p++;
                  *path = 0;
               } else {
                  while (*p && *p != ' ' && *p != '>')
                     *path++ = *p++;
                  *path = 0;
               }
               p++;
            } else {
               strncpy(str, p, 5);
               str[5] = 0;
               if (equal_ustring(str, "edit=")) {
                  p += 5;
                  *edit = atoi(p);
                  while (*p && *p != ' ' && *p != '>')
                     p++;
               }
            }

            while (*p && ((*p == ' ') || iscntrl(*p)))
               p++;

         } while (*p != '>');

         return ps;
      }

      while (*p && *p != '>')
         p++;

   } while (1);

}

/*------------------------------------------------------------------*/

void show_custom_page(char *path)
{
   int size, i_edit, i_set, index, n_var;
   char str[TEXT_SIZE], data[TEXT_SIZE], ctext[TEXT_SIZE], keypath[256], *p, *ps;
   HNDLE hDB, hkey;
   KEY key;
   BOOL bedit;

   cm_get_experiment_database(&hDB, NULL);

   /* check if variable to edit */
   i_edit = -1;
   if (equal_ustring(getparam("cmd"), "Edit"))
      i_edit = atoi(getparam("index"));

   /* check if variable to set */
   i_set = -1;
   if (equal_ustring(getparam("cmd"), "Set"))
      i_set = atoi(getparam("index"));

   rsprintf("HTTP/1.0 200 Document follows\r\n");
   rsprintf("Server: MIDAS HTTP %s\r\n", cm_get_version());
   rsprintf("Content-Type: text/html; charset=iso-8859-1\r\n\r\n");

   sprintf(str, "/Custom/%s", path);

   db_find_key(hDB, 0, str, &hkey);
   if (hkey) {
      n_var = 0;
      size = sizeof(ctext);
      db_get_data(hDB, hkey, ctext, &size, TID_STRING);

      /* interprete text, replace <odb> tags with ODB values */
      p = ps = ctext;
      do {
         p = find_odb_tag(ps, keypath, &bedit);
         if (p != NULL)
            *p = 0;
         rsputs(ps);

         if (p == NULL)
            break;
         ps = strchr(p + 1, '>') + 1;

         /* check if path contains index */
         index = 0;

         if (strchr(keypath, '[') && strchr(keypath, ']')) {
            for (p = strchr(keypath, '[') + 1; *p && *p != ']'; p++)
               if (!isdigit(*p))
                  break;

            if (*p && *p == ']') {
               index = atoi(strchr(keypath, '[') + 1);
               *strchr(keypath, '[') = 0;
            }
         }

         db_find_key(hDB, 0, keypath, &hkey);
         if (!hkey)
            rsprintf("<b>Key \"%s\" not found in ODB</b>\n", keypath);
         else {
            db_get_key(hDB, hkey, &key);
            size = sizeof(data);
            db_get_data_index(hDB, hkey, data, &size, index, key.type);
            db_sprintf(str, data, key.item_size, 0, key.type);

            if (bedit) {
               if (n_var == i_set) {
                  /* set value */
                  strcpy(str, getparam("value"));
                  db_sscanf(str, data, &size, 0, key.type);
                  db_set_data_index(hDB, hkey, data, size, index, key.type);

                  /* read back value */
                  size = sizeof(data);
                  db_get_data_index(hDB, hkey, data, &size, index, key.type);
                  db_sprintf(str, data, key.item_size, 0, key.type);
               }

               if (n_var == i_edit) {
                  rsprintf
                      ("<input type=text size=10 maxlength=80 name=value value=\"%s\">\n",
                       str);
                  rsprintf("<input type=submit size=20 name=cmd value=Set>\n");
                  rsprintf("<input type=hidden name=index value=%d>\n", n_var);
                  rsprintf("<input type=hidden name=cmd value=Set>\n");
                  n_var++;
               } else {
                  if (exp_name[0])
                     rsprintf("<a href=\"/CS/%s?exp=%s&cmd=Edit&index=%d\">", path,
                              exp_name, n_var++);
                  else
                     rsprintf("<a href=\"/CS/%s?cmd=Edit&index=%d\">", path, n_var++);

                  rsputs(str);
                  rsprintf("</a>");
               }
            } else
               rsputs(str);
         }


      } while (p != NULL);
   }
}

/*------------------------------------------------------------------*/

void show_cnaf_page()
{
   char *cmd, str[256], *pd;
   int c, n, a, f, d, q, x, r, ia, id, w;
   int i, size, status;
   HNDLE hDB, hrootkey, hsubkey, hkey;

   static char client_name[NAME_LENGTH];
   static HNDLE hconn = 0;

   cm_get_experiment_database(&hDB, NULL);

   /* find FCNA server if not specified */
   if (hconn == 0) {
      /* find client which exports FCNA function */
      status = db_find_key(hDB, 0, "System/Clients", &hrootkey);
      if (status == DB_SUCCESS) {
         for (i = 0;; i++) {
            status = db_enum_key(hDB, hrootkey, i, &hsubkey);
            if (status == DB_NO_MORE_SUBKEYS)
               break;

            sprintf(str, "RPC/%d", RPC_CNAF16);
            status = db_find_key(hDB, hsubkey, str, &hkey);
            if (status == DB_SUCCESS) {
               size = sizeof(client_name);
               db_get_value(hDB, hsubkey, "Name", client_name, &size, TID_STRING, TRUE);
               break;
            }
         }
      }

      if (client_name[0]) {
         status = cm_connect_client(client_name, &hconn);
         if (status != RPC_SUCCESS)
            hconn = 0;
      }
   }

   /* header */
   rsprintf("HTTP/1.0 200 Document follows\r\n");
   rsprintf("Server: MIDAS HTTP %s\r\n", cm_get_version());
   rsprintf("Content-Type: text/html; charset=iso-8859-1\r\n\r\n");

   rsprintf("<html><head><title>MIDAS CAMAC interface</title></head>\n");
   rsprintf("<body><form method=\"GET\" action=\"/CNAF\">\n\n");

   /* title row */

   size = sizeof(str);
   str[0] = 0;
   db_get_value(hDB, 0, "/Experiment/Name", str, &size, TID_STRING, TRUE);

   /* define hidden field for experiment */
   if (exp_name[0])
      rsprintf("<input type=hidden name=exp value=\"%s\">\n", exp_name);

   rsprintf("<table border=3 cellpadding=1>\n");
   rsprintf("<tr><th colspan=3 bgcolor=#A0A0FF>MIDAS experiment \"%s\"", str);

   if (client_name[0] == 0)
      rsprintf("<th colspan=3 bgcolor=#FF0000>No CAMAC server running</tr>\n");
   else if (hconn == 0)
      rsprintf("<th colspan=3 bgcolor=#FF0000>Cannot connect to %s</tr>\n", client_name);
   else
      rsprintf("<th colspan=3 bgcolor=#A0A0FF>CAMAC server: %s</tr>\n", client_name);

   /* default values */
   c = n = 1;
   a = f = d = q = x = 0;
   r = 1;
   ia = id = w = 0;

  /*---- menu buttons ----*/

   rsprintf("<tr><td colspan=3 bgcolor=#C0C0C0>\n");
   rsprintf("<input type=submit name=cmd value=Execute>\n");

   rsprintf("<td colspan=3 bgcolor=#C0C0C0>\n");
   rsprintf("<input type=submit name=cmd value=ODB>\n");
   rsprintf("<input type=submit name=cmd value=Status>\n");
   rsprintf("<input type=submit name=cmd value=Help>\n");
   rsprintf("</tr>\n\n");

   /* header */
   rsprintf("<tr><th bgcolor=#FFFF00>N");
   rsprintf("<th bgcolor=#FF8000>A");
   rsprintf("<th bgcolor=#00FF00>F");
   rsprintf("<th colspan=3 bgcolor=#8080FF>Data");

   /* execute commands */
   size = sizeof(d);

   cmd = getparam("cmd");
   if (equal_ustring(cmd, "C cycle")) {
      rpc_client_call(hconn, RPC_CNAF16, CNAF_CRATE_CLEAR, 0, 0, 0, 0, 0, &d, &size, &x,
                      &q);

      rsprintf("<tr><td colspan=6 bgcolor=#00FF00>C cycle executed sucessfully</tr>\n");
   } else if (equal_ustring(cmd, "Z cycle")) {
      rpc_client_call(hconn, RPC_CNAF16, CNAF_CRATE_ZINIT, 0, 0, 0, 0, 0, &d, &size, &x,
                      &q);

      rsprintf("<tr><td colspan=6 bgcolor=#00FF00>Z cycle executed sucessfully</tr>\n");
   } else if (equal_ustring(cmd, "Clear inhibit")) {
      rpc_client_call(hconn, RPC_CNAF16, CNAF_INHIBIT_CLEAR, 0, 0, 0, 0, 0, &d, &size, &x,
                      &q);

      rsprintf
          ("<tr><td colspan=6 bgcolor=#00FF00>Clear inhibit executed sucessfully</tr>\n");
   } else if (equal_ustring(cmd, "Set inhibit")) {
      rpc_client_call(hconn, RPC_CNAF16, CNAF_INHIBIT_SET, 0, 0, 0, 0, 0, &d, &size, &x,
                      &q);

      rsprintf
          ("<tr><td colspan=6 bgcolor=#00FF00>Set inhibit executed sucessfully</tr>\n");
   } else if (equal_ustring(cmd, "Execute")) {
      c = atoi(getparam("C"));
      n = atoi(getparam("N"));
      a = atoi(getparam("A"));
      f = atoi(getparam("F"));
      r = atoi(getparam("R"));
      w = atoi(getparam("W"));
      id = atoi(getparam("ID"));
      ia = atoi(getparam("IA"));

      pd = getparam("D");
      if (strncmp(pd, "0x", 2) == 0)
         sscanf(pd + 2, "%x", &d);
      else
         d = atoi(getparam("D"));

      /* limit repeat range */
      if (r == 0)
         r = 1;
      if (r > 100)
         r = 100;
      if (w > 1000)
         w = 1000;

      for (i = 0; i < r; i++) {
         status = SUCCESS;

         if (hconn) {
            size = sizeof(d);
            status =
                rpc_client_call(hconn, RPC_CNAF24, CNAF, 0, c, n, a, f, &d, &size, &x,
                                &q);

            if (status == RPC_NET_ERROR) {
               /* try to reconnect */
               cm_disconnect_client(hconn, FALSE);
               status = cm_connect_client(client_name, &hconn);
               if (status != RPC_SUCCESS) {
                  hconn = 0;
                  client_name[0] = 0;
               }

               if (hconn)
                  status =
                      rpc_client_call(hconn, RPC_CNAF24, CNAF, 0, c, n, a, f, &d, &size,
                                      &x, &q);
            }
         }

         if (status != SUCCESS) {
            rsprintf
                ("<tr><td colspan=6 bgcolor=#FF0000>Error executing function, code = %d</tr>",
                 status);
         } else {
            rsprintf("<tr align=center><td bgcolor=#FFFF00>%d", n);
            rsprintf("<td bgcolor=#FF8000>%d", a);
            rsprintf("<td bgcolor=#00FF00>%d", f);
            rsprintf("<td colspan=3 bgcolor=#8080FF>%d / 0x%04X  Q%d X%d", d, d, q, x);
         }

         d += id;
         a += ia;

         if (w > 0)
            ss_sleep(w);
      }
   }

   /* input fields */
   rsprintf
       ("<tr align=center><td bgcolor=#FFFF00><input type=text size=3 name=N value=%d>\n",
        n);
   rsprintf("<td bgcolor=#FF8000><input type=text size=3 name=A value=%d>\n", a);
   rsprintf("<td bgcolor=#00FF00><input type=text size=3 name=F value=%d>\n", f);
   rsprintf
       ("<td bgcolor=#8080FF colspan=3><input type=text size=8 name=D value=%d></tr>\n",
        d);

   /* control fields */
   rsprintf("<tr><td colspan=2 bgcolor=#FF8080>Repeat");
   rsprintf("<td bgcolor=#FF8080><input type=text size=3 name=R value=%d>\n", r);

   rsprintf
       ("<td align=center colspan=3 bgcolor=#FF0000><input type=submit name=cmd value=\"C cycle\">\n");
   rsprintf("<input type=submit name=cmd value=\"Z cycle\">\n");

   rsprintf("<tr><td colspan=2 bgcolor=#FF8080>Repeat delay [ms]");
   rsprintf("<td bgcolor=#FF8080><input type=text size=3 name=W value=%d>\n", w);

   rsprintf
       ("<td align=center colspan=3 bgcolor=#FF0000><input type=submit name=cmd value=\"Set inhibit\">\n");
   rsprintf("<input type=submit name=cmd value=\"Clear inhibit\">\n");

   rsprintf("<tr><td colspan=2 bgcolor=#FF8080>Data increment");
   rsprintf("<td bgcolor=#FF8080><input type=text size=3 name=ID value=%d>\n", id);

   rsprintf
       ("<td colspan=3 align=center bgcolor=#FFFF80>Branch <input type=text size=3 name=B value=0>\n");

   rsprintf("<tr><td colspan=2 bgcolor=#FF8080>A increment");
   rsprintf("<td bgcolor=#FF8080><input type=text size=3 name=IA value=%d>\n", ia);

   rsprintf
       ("<td colspan=3 align=center bgcolor=#FFFF80>Crate <input type=text size=3 name=C value=%d>\n",
        c);

   rsprintf("</table></body>\r\n");
}

/*------------------------------------------------------------------*/

void show_experiment_page(char exp_list[MAX_EXPERIMENT][NAME_LENGTH])
{
   int i;

   rsprintf("HTTP/1.0 200 Document follows\r\n");
   rsprintf("Server: MIDAS HTTP %s\r\n", cm_get_version());
   rsprintf("Content-Type: text/html; charset=iso-8859-1\r\n\r\n");

   rsprintf("<html>\n");
   rsprintf("<head>\n");
   rsprintf("<title>MIDAS Experiment Selection</title>\n");
   rsprintf("</head>\n\n");

   rsprintf("<body>\n\n");
   rsprintf("Several experiments are defined on this host.<BR>\n");
   rsprintf("Please select the one to connect to:<P>");

   for (i = 0; i < MAX_EXPERIMENT; i++) {
      if (exp_list[i][0])
         rsprintf("<a href=\"/?exp=%s\">%s</a><p>", exp_list[i], exp_list[i]);
   }

   rsprintf("</body>\n");
   rsprintf("</html>\r\n");

}

/*------------------------------------------------------------------*/

void show_password_page(char *password, char *experiment)
{
   rsprintf("HTTP/1.0 200 Document follows\r\n");
   rsprintf("Server: MIDAS HTTP %s\r\n", cm_get_version());
   rsprintf("Content-Type: text/html; charset=iso-8859-1\r\n\r\n");

   rsprintf("<html><head><title>Enter password</title></head><body>\n\n");

   rsprintf("<form method=\"GET\" action=\"/\">\n\n");

   /* define hidden fields for current experiment */
   if (experiment[0])
      rsprintf("<input type=hidden name=exp value=\"%s\">\n", experiment);

   rsprintf("<table border=1 cellpadding=5>");

   if (password[0])
      rsprintf("<tr><th bgcolor=#FF0000>Wrong password!</tr>\n");

   rsprintf("<tr><th bgcolor=#A0A0FF>Please enter password</tr>\n");
   rsprintf("<tr><td align=center><input type=password name=pwd></tr>\n");
   rsprintf("<tr><td align=center><input type=submit value=Submit></tr>");

   rsprintf("</table>\n");

   rsprintf("</body></html>\r\n");
}

/*------------------------------------------------------------------*/

BOOL check_web_password(char *password, char *redir, char *experiment)
{
   HNDLE hDB, hkey;
   INT size;
   char str[256];

   cm_get_experiment_database(&hDB, NULL);

   /* check for password */
   db_find_key(hDB, 0, "/Experiment/Security/Web Password", &hkey);
   if (hkey) {
      size = sizeof(str);
      db_get_data(hDB, hkey, str, &size, TID_STRING);
      if (strcmp(password, str) == 0)
         return TRUE;

      /* show web password page */
      rsprintf("HTTP/1.0 200 Document follows\r\n");
      rsprintf("Server: MIDAS HTTP %s\r\n", cm_get_version());
      rsprintf("Content-Type: text/html; charset=iso-8859-1\r\n\r\n");

      rsprintf("<html><head><title>Enter password</title></head><body>\n\n");

      rsprintf("<form method=\"GET\" action=\"/\">\n\n");

      /* define hidden fields for current experiment and destination */
      if (experiment[0])
         rsprintf("<input type=hidden name=exp value=\"%s\">\n", experiment);
      if (redir[0])
         rsprintf("<input type=hidden name=redir value=\"%s\">\n", redir);

      rsprintf("<table border=1 cellpadding=5>");

      if (password[0])
         rsprintf("<tr><th bgcolor=#FF0000>Wrong password!</tr>\n");

      rsprintf
          ("<tr><th bgcolor=#A0A0FF>Please enter password to obtain write access</tr>\n");
      rsprintf("<tr><td align=center><input type=password name=wpwd></tr>\n");
      rsprintf("<tr><td align=center><input type=submit value=Submit></tr>");

      rsprintf("</table>\n");

      rsprintf("</body></html>\r\n");

      return FALSE;
   } else
      return TRUE;
}

/*------------------------------------------------------------------*/

void show_start_page(void)
{
   int rn, i, j, n, size, status, maxlength;
   HNDLE hDB, hkey, hsubkey, hkeycomm, hkeyc;
   KEY key;
   char data[1000], str[32];
   char data_str[256], comment[1000];

   cm_get_experiment_database(&hDB, NULL);

   show_header(hDB, "Start run", "GET", "", 1, 0);

   rsprintf("<tr><th bgcolor=#A0A0FF colspan=2>Start new run</tr>\n");
   rsprintf("<tr><td>Run number");

   /* run number */
   size = sizeof(rn);
   status = db_get_value(hDB, 0, "/Runinfo/Run number", &rn, &size, TID_INT, TRUE);
   assert(status == SUCCESS);

   if (rn < 0)                  // value "zero" is okey
   {
      cm_msg(MERROR, "show_start_page",
             "aborting on attempt to use invalid run number %d", rn);
      abort();
   }

   size = sizeof(i);
   if (db_find_key(hDB, 0, "/Experiment/Edit on start/Edit Run number", &hkey) ==
       DB_SUCCESS && db_get_data(hDB, hkey, &i, &size, TID_BOOL) && i == 0)
      rsprintf("<td><input type=hidden name=value value=%d>%d</tr>\n", rn + 1, rn + 1);
   else
      rsprintf("<td><input type=text size=20 maxlength=80 name=value value=%d></tr>\n",
               rn + 1);

   /* run parameters */
   db_find_key(hDB, 0, "/Experiment/Edit on start", &hkey);
   db_find_key(hDB, 0, "/Experiment/Parameter Comments", &hkeycomm);
   if (hkey) {
      for (i = 0, n = 0;; i++) {
         db_enum_link(hDB, hkey, i, &hsubkey);

         if (!hsubkey)
            break;

         db_get_key(hDB, hsubkey, &key);
         strcpy(str, key.name);

         if (equal_ustring(str, "Edit run number"))
            continue;

         db_enum_key(hDB, hkey, i, &hsubkey);
         db_get_key(hDB, hsubkey, &key);

         size = sizeof(data);
         status = db_get_data(hDB, hsubkey, data, &size, key.type);
         if (status != DB_SUCCESS)
            continue;

         for (j = 0; j < key.num_values; j++) {
            if (key.num_values > 1)
               rsprintf("<tr><td>%s [%d]", str, j);
            else
               rsprintf("<tr><td>%s", str);

            if (j == 0 && hkeycomm) {
               /* look for comment */
               if (db_find_key(hDB, hkeycomm, key.name, &hkeyc) == DB_SUCCESS) {
                  size = sizeof(comment);
                  if (db_get_data(hDB, hkeyc, comment, &size, TID_STRING) == DB_SUCCESS)
                     rsprintf("<br>%s\n", comment);
               }
            }

            db_sprintf(data_str, data, key.item_size, j, key.type);

            maxlength = 80;
            if (key.type == TID_STRING)
               maxlength = key.item_size;

            rsprintf
                ("<td><input type=text size=80 maxlength=%d name=x%d value=\"%s\"></tr>\n",
                 maxlength, n++, data_str);
         }
      }
   }

   rsprintf("<tr><td align=center colspan=2>");
   rsprintf("<input type=submit name=cmd value=Start>");
   rsprintf("<input type=submit name=cmd value=Cancel>");
   rsprintf("</tr>");
   rsprintf("</table>");

   rsprintf("</body></html>\r\n");
}

/*------------------------------------------------------------------*/

void show_odb_page(char *enc_path, char *dec_path)
{
   int i, j, size, status;
   char str[256], tmp_path[256], url_path[256],
       data_str[TEXT_SIZE], hex_str[256], ref[256], keyname[32], link_name[256];
   char *p, *pd;
   char data[TEXT_SIZE];
   HNDLE hDB, hkey, hkeyroot;
   KEY key;

   cm_get_experiment_database(&hDB, NULL);

   if (strcmp(enc_path, "root") == 0) {
      strcpy(enc_path, "");
      strcpy(dec_path, "");
   }

   show_header(hDB, "MIDAS online database", "GET", enc_path, 1, 0);

   /* find key via path */
   status = db_find_key(hDB, 0, dec_path, &hkeyroot);
   if (status != DB_SUCCESS) {
      rsprintf("Error: cannot find key %s<P>\n", dec_path);
      rsprintf("</body></html>\r\n");
      return;
   }

   /* if key is not of type TID_KEY, cut off key name */
   db_get_key(hDB, hkeyroot, &key);
   if (key.type != TID_KEY) {
      /* strip variable name from path */
      p = dec_path + strlen(dec_path) - 1;
      while (*p && *p != '/')
         *p-- = 0;
      if (*p == '/')
         *p = 0;

      strcpy(enc_path, dec_path);
      urlEncode(enc_path);

      status = db_find_key(hDB, 0, dec_path, &hkeyroot);
      if (status != DB_SUCCESS) {
         rsprintf("Error: cannot find key %s<P>\n", dec_path);
         rsprintf("</body></html>\r\n");
         return;
      }
   }

  /*---- menu buttons ----*/

   rsprintf("<tr><td colspan=2 bgcolor=#A0A0A0>\n");
   if (elog_mode) {
      rsprintf("<input type=submit name=cmd value=ELog>\n");
      rsprintf("</tr>\n");
   } else {
      rsprintf("<input type=submit name=cmd value=Find>\n");
      rsprintf("<input type=submit name=cmd value=Create>\n");
      rsprintf("<input type=submit name=cmd value=Delete>\n");
      rsprintf("<input type=submit name=cmd value=Alarms>\n");
      rsprintf("<input type=submit name=cmd value=Programs>\n");
      rsprintf("<input type=submit name=cmd value=Status>\n");
      rsprintf("<input type=submit name=cmd value=Help>\n");
      rsprintf("</tr>\n");

      rsprintf("<tr><td colspan=2 bgcolor=#A0A0A0>\n");
      rsprintf("<input type=submit name=cmd value=\"Create Elog from this page\">\n");
      rsprintf("</tr>\n");
   }

  /*---- ODB display -----------------------------------------------*/

   /* display root key */
   rsprintf("<tr><td colspan=2 align=center><b>");
   if (exp_name[0])
      rsprintf("<a href=\"/root?exp=%s\">/</a> \n", exp_name);
   else
      rsprintf("<a href=\"/root?\">/</a> \n");

   strcpy(tmp_path, "");

   p = dec_path;
   if (*p == '/')
      p++;

  /*---- display path ----*/
   while (*p) {
      pd = str;
      while (*p && *p != '/')
         *pd++ = *p++;
      *pd = 0;

      strcat(tmp_path, str);
      strcpy(url_path, tmp_path);
      urlEncode(url_path);

      if (exp_name[0])
         rsprintf("<a href=\"/%s?&exp=%s\">%s</a>\n / ", url_path, exp_name, str);
      else
         rsprintf("<a href=\"/%s\">%s</a>\n / ", url_path, str);

      strcat(tmp_path, "/");
      if (*p == '/')
         p++;
   }
   rsprintf("</b></tr>\n");

   rsprintf("<tr><th>Key<th>Value</tr>\n");

   /* enumerate subkeys */
   for (i = 0;; i++) {
      db_enum_link(hDB, hkeyroot, i, &hkey);
      if (!hkey)
         break;
      db_get_key(hDB, hkey, &key);

      strcpy(str, dec_path);
      if (str[0] && str[strlen(str) - 1] != '/')
         strcat(str, "/");
      strcat(str, key.name);
      urlEncode(str);
      strcpy(keyname, key.name);

      /* resolve links */
      link_name[0] = 0;
      if (key.type == TID_LINK) {
         size = sizeof(link_name);
         db_get_data(hDB, hkey, link_name, &size, TID_LINK);
         db_enum_key(hDB, hkeyroot, i, &hkey);
         db_get_key(hDB, hkey, &key);
      }

      if (key.type == TID_KEY) {
         /* for keys, don't display data value */
         if (exp_name[0])
            rsprintf
                ("<tr><td colspan=2 bgcolor=#FFD000><a href=\"/%s?exp=%s\">%s</a><br></tr>\n",
                 str, exp_name, keyname);
         else
            rsprintf
                ("<tr><td colspan=2 bgcolor=#FFD000><a href=\"/%s\">%s</a><br></tr>\n",
                 str, keyname);
      } else {
         /* display single value */
         if (key.num_values == 1) {
            size = sizeof(data);
            db_get_data(hDB, hkey, data, &size, key.type);
            db_sprintf(data_str, data, key.item_size, 0, key.type);

            if (key.type != TID_STRING)
               db_sprintfh(hex_str, data, key.item_size, 0, key.type);
            else
               hex_str[0] = 0;

            if (data_str[0] == 0 || equal_ustring(data_str, "<NULL>")) {
               strcpy(data_str, "(empty)");
               hex_str[0] = 0;
            }

            if (exp_name[0])
               sprintf(ref, "/%s?cmd=Set&exp=%s", str, exp_name);
            else
               sprintf(ref, "/%s?cmd=Set", str);

            if (strcmp(data_str, hex_str) != 0 && hex_str[0]) {
               if (link_name[0])
                  rsprintf
                      ("<tr><td bgcolor=#FFFF00>%s <i>-> %s</i><td><a href=\"%s\">%s (%s)</a><br></tr>\n",
                       keyname, link_name, ref, data_str, hex_str);
               else
                  rsprintf
                      ("<tr><td bgcolor=#FFFF00>%s<td><a href=\"%s\">%s (%s)</a><br></tr>\n",
                       keyname, ref, data_str, hex_str);
            } else {
               if (strchr(data_str, '\n')) {
                  if (link_name[0])
                     rsprintf("<tr><td bgcolor=#FFFF00>%s <i>-> %s</i><td>", keyname,
                              link_name);
                  else
                     rsprintf("<tr><td bgcolor=#FFFF00>%s<td>", keyname);
                  rsprintf("\n<pre>");
                  strencode3(data_str);
                  rsprintf("</pre>");
                  rsprintf("<a href=\"%s\">Edit</a></tr>\n", ref);
               } else {
                  if (link_name[0])
                     rsprintf
                         ("<tr><td bgcolor=#FFFF00>%s <i>-> %s</i><td><a href=\"%s\">",
                          keyname, link_name, ref);
                  else
                     rsprintf("<tr><td bgcolor=#FFFF00>%s<td><a href=\"%s\">",
                              keyname, ref);
                  strencode(data_str);
                  rsprintf("</a><br></tr>\n");
               }
            }
         } else {
            /* check for exceeding length */
            if (key.num_values > 1000)
               rsprintf("<tr><td bgcolor=#FFFF00>%s<td><i>... %d values ...</i>\n",
                        keyname, key.num_values);
            else {
               /* display first value */
               if (link_name[0])
                  rsprintf("<tr><td  bgcolor=#FFFF00 rowspan=%d>%s<br><i>-> %s</i>\n",
                           key.num_values, keyname, link_name);
               else
                  rsprintf("<tr><td  bgcolor=#FFFF00 rowspan=%d>%s\n", key.num_values,
                           keyname);

               for (j = 0; j < key.num_values; j++) {
                  size = sizeof(data);
                  db_get_data_index(hDB, hkey, data, &size, j, key.type);
                  db_sprintf(data_str, data, key.item_size, 0, key.type);
                  db_sprintfh(hex_str, data, key.item_size, 0, key.type);

                  if (data_str[0] == 0 || equal_ustring(data_str, "<NULL>")) {
                     strcpy(data_str, "(empty)");
                     hex_str[0] = 0;
                  }

                  if (exp_name[0])
                     sprintf(ref, "/%s?cmd=Set&index=%d&exp=%s", str, j, exp_name);
                  else
                     sprintf(ref, "/%s?cmd=Set&index=%d", str, j);

                  if (j > 0)
                     rsprintf("<tr>");

                  if (strcmp(data_str, hex_str) != 0 && hex_str[0])
                     rsprintf("<td><a href=\"%s\">[%d] %s (%s)</a><br></tr>\n", ref, j,
                              data_str, hex_str);
                  else
                     rsprintf("<td><a href=\"%s\">[%d] %s</a><br></tr>\n", ref, j,
                              data_str);
               }
            }
         }
      }
   }

   rsprintf("</table>\n");
   rsprintf("</body></html>\r\n");
}

/*------------------------------------------------------------------*/

void show_set_page(char *enc_path, char *dec_path, char *group, int index, char *value)
{
   int status, size;
   HNDLE hDB, hkey;
   KEY key;
   char data_str[TEXT_SIZE], str[256], *p, eq_name[NAME_LENGTH];
   char data[TEXT_SIZE];

   cm_get_experiment_database(&hDB, NULL);

   /* show set page if no value is given */
   if (!isparam("value") && !*getparam("text")) {
      status = db_find_key(hDB, 0, dec_path, &hkey);
      if (status != DB_SUCCESS) {
         rsprintf("Error: cannot find key %s<P>\n", dec_path);
         return;
      }
      db_get_key(hDB, hkey, &key);

      show_header(hDB, "Set value", "POST", enc_path, 1, 0);

      if (index > 0)
         rsprintf("<input type=hidden name=index value=\"%d\">\n", index);
      else
         index = 0;

      if (group[0])
         rsprintf("<input type=hidden name=group value=\"%s\">\n", group);

      strcpy(data_str, rpc_tid_name(key.type));
      if (key.num_values > 1) {
         sprintf(str, "[%d]", key.num_values);
         strcat(data_str, str);

         sprintf(str, "%s[%d]", dec_path, index);
      } else
         strcpy(str, dec_path);

      rsprintf("<tr><th bgcolor=#A0A0FF colspan=2>Set new value - type = %s</tr>\n",
               data_str);
      rsprintf("<tr><td bgcolor=#FFFF00>%s<td>\n", str);

      /* set current value as default */
      size = sizeof(data);
      db_get_data(hDB, hkey, data, &size, key.type);
      db_sprintf(data_str, data, key.item_size, index, key.type);

      if (equal_ustring(data_str, "<NULL>"))
         data_str[0] = 0;

      if (strchr(data_str, '\n') != NULL) {
         rsprintf("<textarea rows=20 cols=80 name=\"text\">\n");
         rsputs(data_str);
         rsprintf("</textarea>\n");
      } else {
         size = 20;
         if ((int) strlen(data_str) > size)
            size = strlen(data_str) + 3;
         if (size > 80)
            size = 80;
         rsprintf
             ("<input type=\"text\" size=%d maxlength=256 name=\"value\" value=\"%s\">\n",
              size, data_str);
      }

      rsprintf("</tr>\n");

      rsprintf("<tr><td align=center colspan=2>");
      rsprintf("<input type=submit name=cmd value=Set>");
      rsprintf("<input type=submit name=cmd value=Cancel>");
      rsprintf("</tr>");
      rsprintf("</table>");

      rsprintf("<input type=hidden name=cmd value=Set>\n");

      rsprintf("</body></html>\r\n");
      return;
   } else {
      /* set value */

      status = db_find_key(hDB, 0, dec_path, &hkey);
      if (status != DB_SUCCESS) {
         rsprintf("Error: cannot find key %s<P>\n", dec_path);
         return;
      }
      db_get_key(hDB, hkey, &key);

      memset(data, 0, sizeof(data));

      if (*getparam("text"))
         strncpy(data, getparam("text"), sizeof(data));
      else
         db_sscanf(value, data, &size, 0, key.type);

      if (index < 0)
         index = 0;

      /* extend data size for single string if necessary */
      if ((key.type == TID_STRING || key.type == TID_LINK)
          && (int) strlen(data) + 1 > key.item_size && key.num_values == 1)
         key.item_size = strlen(data) + 1;

      if (key.item_size == 0)
         key.item_size = rpc_tid_size(key.type);

      if (key.num_values > 1)
         status = db_set_data_index(hDB, hkey, data, key.item_size, index, key.type);
      else
         status = db_set_data(hDB, hkey, data, key.item_size, 1, key.type);

      if (status == DB_NO_ACCESS)
         rsprintf("<h2>Write access not allowed</h2>\n");

      /* strip variable name from path */
      p = dec_path + strlen(dec_path) - 1;
      while (*p && *p != '/')
         *p-- = 0;
      if (*p == '/')
         *p = 0;

      strcpy(enc_path, dec_path);
      urlEncode(enc_path);

      /* redirect */

      if (group[0]) {
         /* extract equipment name */
         eq_name[0] = 0;
         if (strncmp(enc_path, "Equipment/", 10) == 0) {
            strcpy(eq_name, enc_path + 10);
            if (strchr(eq_name, '/'))
               *strchr(eq_name, '/') = 0;
         }

         /* back to SC display */
         sprintf(str, "SC/%s/%s", eq_name, group);
         redirect(str);
      } else
         redirect(enc_path);

      return;
   }

}

/*------------------------------------------------------------------*/

void show_find_page(char *enc_path, char *value)
{
   HNDLE hDB, hkey;

   cm_get_experiment_database(&hDB, NULL);

   if (value[0] == 0) {
      /* without value, show find dialog */

      show_header(hDB, "Find value", "GET", enc_path, 1, 0);

      rsprintf("<tr><th bgcolor=#A0A0FF colspan=2>Find string in Online Database</tr>\n");
      rsprintf("<tr><td>Enter substring (case insensitive)\n");

      rsprintf("<td><input type=\"text\" size=\"20\" maxlength=\"80\" name=\"value\">\n");
      rsprintf("</tr>");

      rsprintf("<tr><td align=center colspan=2>");
      rsprintf("<input type=submit name=cmd value=Find>");
      rsprintf("<input type=submit name=cmd value=Cancel>");
      rsprintf("</tr>");
      rsprintf("</table>");

      rsprintf("<input type=hidden name=cmd value=Find>");

      rsprintf("</body></html>\r\n");
   } else {
      show_header(hDB, "Search results", "GET", enc_path, 1, 0);

      rsprintf("<tr><td colspan=2 bgcolor=#A0A0A0>\n");
      rsprintf("<input type=submit name=cmd value=Find>\n");
      rsprintf("<input type=submit name=cmd value=ODB>\n");
      rsprintf("<input type=submit name=cmd value=Help>\n");
      rsprintf("</tr>\n\n");

      rsprintf("<tr><th bgcolor=#A0A0FF colspan=2>");
      rsprintf("Results of search for substring \"%s\"</tr>\n", value);
      rsprintf("<tr><th>Key<th>Value</tr>\n");

      /* start from root */
      db_find_key(hDB, 0, "", &hkey);

      /* scan tree, call "search_callback" for each key */
      db_scan_tree(hDB, hkey, 0, search_callback, (void *) value);

      rsprintf("</table>");
      rsprintf("</body></html>\r\n");
   }
}

/*------------------------------------------------------------------*/

void show_create_page(char *enc_path, char *dec_path, char *value, int index, int type)
{
   char str[256], link[256], *p;
   char data[10000];
   int status;
   HNDLE hDB, hkey;
   KEY key;

   cm_get_experiment_database(&hDB, NULL);

   if (value[0] == 0) {
      /* without value, show create dialog */

      show_header(hDB, "Create ODB entry", "GET", enc_path, 1, 0);

      rsprintf("<tr><th bgcolor=#A0A0FF colspan=2>Create ODB entry</tr>\n");

      rsprintf("<tr><td>Type");
      rsprintf("<td><select type=text size=1 name=type>\n");

      rsprintf("<option value=7> Integer (32-bit)\n");
      rsprintf("<option value=9> Float (4 Bytes)\n");
      rsprintf("<option value=12> String\n");
      rsprintf("<option value=15> Subdirectory\n");

      rsprintf("<option value=1> Byte\n");
      rsprintf("<option value=2> Signed byte\n");
      rsprintf("<option value=3> Character (8-bit)\n");
      rsprintf("<option value=4> Word (16-bit)\n");
      rsprintf("<option value=5> Short integer(16-bit)\n");
      rsprintf("<option value=6> Double Word (32-bit)\n");
      rsprintf("<option value=8> Boolean\n");
      rsprintf("<option value=10> Double float(8 Bytes)\n");
      rsprintf("<option value=16> Symbolic link\n");

      rsprintf("</select></tr>\n");

      rsprintf("<tr><td>Name");
      rsprintf("<td><input type=text size=20 maxlength=80 name=value>\n");
      rsprintf("</tr>");

      rsprintf("<tr><td>Array size");
      rsprintf("<td><input type=text size=20 maxlength=80 name=index value=1>\n");
      rsprintf("</tr>");

      rsprintf("<tr><td align=center colspan=2>");
      rsprintf("<input type=submit name=cmd value=Create>");
      rsprintf("<input type=submit name=cmd value=Cancel>");
      rsprintf("</tr>");
      rsprintf("</table>");

      rsprintf("</body></html>\r\n");
   } else {
      if (type == TID_LINK) {
         /* check if destination exists */
         status = db_find_key(hDB, 0, value, &hkey);
         if (status != DB_SUCCESS) {
            rsprintf("<h1>Error: Link destination \"%s\" does not exist!</h1>", value);
            return;
         }

         /* extract key name from destination */
         strcpy(str, value);
         p = str + strlen(str) - 1;
         while (*p && *p != '/')
            p--;
         p++;

         /* use it as link name */
         strcpy(link, p);

         strcpy(str, dec_path);
         if (str[strlen(str) - 1] != '/')
            strcat(str, "/");
         strcat(str, link);


         status = db_create_link(hDB, 0, str, value);
         if (status != DB_SUCCESS) {
            rsprintf("<h1>Cannot create key %s</h1>\n", str);
            return;
         }

      } else {
         strcpy(str, dec_path);
         if (str[strlen(str) - 1] != '/')
            strcat(str, "/");
         strcat(str, value);

         status = db_create_key(hDB, 0, str, type);
         if (status != DB_SUCCESS) {
            rsprintf("<h1>Cannot create key %s</h1>\n", str);
            return;
         }

         db_find_key(hDB, 0, str, &hkey);
         db_get_key(hDB, hkey, &key);
         memset(data, 0, sizeof(data));
         if (key.type == TID_STRING || key.type == TID_LINK)
            key.item_size = NAME_LENGTH;

         if (index > 1)
            db_set_data_index(hDB, hkey, data, key.item_size, index - 1, key.type);
         else if (key.type == TID_STRING || key.type == TID_LINK)
            db_set_data(hDB, hkey, data, key.item_size, 1, key.type);
      }

      /* redirect */
      redirect(enc_path);
      return;
   }
}

/*------------------------------------------------------------------*/

void show_delete_page(char *enc_path, char *dec_path, char *value, int index)
{
   char str[256];
   int i, status;
   HNDLE hDB, hkeyroot, hkey;
   KEY key;

   cm_get_experiment_database(&hDB, NULL);

   if (value[0] == 0) {
      /* without value, show delete dialog */

      show_header(hDB, "Delete ODB entry", "GET", enc_path, 1, 0);

      rsprintf("<tr><th bgcolor=#A0A0FF colspan=2>Delete ODB entry</tr>\n");

      /* find key via from */
      status = db_find_key(hDB, 0, dec_path, &hkeyroot);
      if (status != DB_SUCCESS) {
         rsprintf("Error: cannot find key %s<P>\n", dec_path);
         rsprintf("</body></html>\r\n");
         return;
      }

      /* count keys */
      for (i = 0;; i++) {
         db_enum_key(hDB, hkeyroot, i, &hkey);
         if (!hkey)
            break;
      }

      rsprintf("<tr><td align=center colspan=2><select type=text size=%d name=value>\n",
               i);

      /* enumerate subkeys */
      for (i = 0;; i++) {
         db_enum_key(hDB, hkeyroot, i, &hkey);
         if (!hkey)
            break;
         db_get_key(hDB, hkey, &key);
         rsprintf("<option> %s\n", key.name);
      }

      rsprintf("</select></tr>\n");

      rsprintf("<tr><td align=center colspan=2>");
      rsprintf("<input type=submit name=cmd value=Delete>");
      rsprintf("<input type=submit name=cmd value=Cancel>");
      rsprintf("</tr>");
      rsprintf("</table>");

      rsprintf("</body></html>\r\n");
   } else {
      strcpy(str, dec_path);
      if (str[strlen(str) - 1] != '/')
         strcat(str, "/");
      strcat(str, value);

      status = db_find_link(hDB, 0, str, &hkey);
      if (status != DB_SUCCESS) {
         rsprintf("<h1>Cannot find key %s</h1>\n", str);
         return;
      }

      status = db_delete_key(hDB, hkey, FALSE);
      if (status != DB_SUCCESS) {
         rsprintf("<h1>Cannot delete key %s</h1>\n", str);
         return;
      }

      /* redirect */
      redirect(enc_path);
      return;
   }
}

/*------------------------------------------------------------------*/

void show_alarm_page()
{
   INT i, size, triggered, type, index, ai;
   BOOL active;
   HNDLE hDB, hkeyroot, hkey;
   KEY key;
   char str[256], ref[256], condition[256], value[256];
   time_t now, last, interval;
   INT al_list[] = { AT_EVALUATED, AT_PROGRAM, AT_INTERNAL, AT_PERIODIC };

   cm_get_experiment_database(&hDB, NULL);

   /* header */
   rsprintf("HTTP/1.0 200 Document follows\r\n");
   rsprintf("Server: MIDAS HTTP %s\r\n", cm_get_version());
   rsprintf("Content-Type: text/html; charset=iso-8859-1\r\n\r\n");

   rsprintf("<html><head>\n");
   rsprintf("<title>Alarms</title></head>\n");
   rsprintf("<body>\n");

   /* title row */
   size = sizeof(str);
   str[0] = 0;
   db_get_value(hDB, 0, "/Experiment/Name", str, &size, TID_STRING, TRUE);
   time(&now);

   rsprintf("<form method=\"GET\" action=\"/\">\n");

   /* define hidden field for experiment */
   if (exp_name[0])
      rsprintf("<input type=hidden name=exp value=\"%s\">\n", exp_name);

   rsprintf("<table border=3 cellpadding=2>\n");
   rsprintf("<tr><th colspan=4 bgcolor=#A0A0FF>MIDAS experiment \"%s\"", str);
   rsprintf("<th colspan=3 bgcolor=#A0A0FF>%s</tr>\n", ctime(&now));

  /*---- menu buttons ----*/

   rsprintf("<tr>\n");
   rsprintf("<td colspan=7 bgcolor=#C0C0C0>\n");

   rsprintf("<input type=submit name=cmd value=\"Reset all alarms\">\n");
   rsprintf("<input type=submit name=cmd value=\"Alarms on/off\">\n");
   rsprintf("<input type=submit name=cmd value=Status>\n");

   rsprintf("</tr></form>\n\n");

  /*---- global flag ----*/

   active = TRUE;
   size = sizeof(active);
   db_get_value(hDB, 0, "/Alarms/Alarm System active", &active, &size, TID_BOOL, TRUE);
   if (!active) {
      if (exp_name[0])
         sprintf(ref, "/Alarms/Alarm System active?cmd=set&exp=%s", exp_name);
      else
         sprintf(ref, "/Alarms/Alarm System active?cmd=set");
      rsprintf
          ("<tr><td align=center colspan=7 bgcolor=#FFC0C0><a href=\"%s\"><h1>Alarm system disabled</h1></a></tr>",
           ref);
   }

  /*---- alarms ----*/

   for (ai = 0; ai < AT_LAST; ai++) {
      index = al_list[ai];

      if (index == AT_EVALUATED) {
         rsprintf
             ("<tr><th align=center colspan=7 bgcolor=#C0C0C0>Evaluated alarms</tr>\n");
         rsprintf
             ("<tr><th>Alarm<th>State<th>First triggered<th>Class<th>Condition<th>Current value<th></tr>\n");
      } else if (index == AT_PROGRAM) {
         rsprintf("<tr><th align=center colspan=7 bgcolor=#C0C0C0>Program alarms</tr>\n");
         rsprintf
             ("<tr><th>Alarm<th>State<th>First triggered<th>Class<th colspan=2>Condition<th></tr>\n");
      } else if (index == AT_INTERNAL) {
         rsprintf
             ("<tr><th align=center colspan=7 bgcolor=#C0C0C0>Internal alarms</tr>\n");
         rsprintf
             ("<tr><th>Alarm<th>State<th>First triggered<th>Class<th colspan=2>Condition/Message<th></tr>\n");
      } else if (index == AT_PERIODIC) {
         rsprintf
             ("<tr><th align=center colspan=7 bgcolor=#C0C0C0>Periodic alarms</tr>\n");
         rsprintf
             ("<tr><th>Alarm<th>State<th>First triggered<th>Class<th colspan=2>Time/Message<th></tr>\n");
      }

      /* go through all alarms */
      db_find_key(hDB, 0, "/Alarms/Alarms", &hkeyroot);
      if (hkeyroot) {
         for (i = 0;; i++) {
            db_enum_link(hDB, hkeyroot, i, &hkey);

            if (!hkey)
               break;

            db_get_key(hDB, hkey, &key);

            /* type */
            size = sizeof(INT);
            db_get_value(hDB, hkey, "Type", &type, &size, TID_INT, TRUE);
            if (type != index)
               continue;

            /* start form for each alarm to make "reset" button work */
            if (exp_name[0])
               sprintf(ref, "/%s?exp=%s", key.name, exp_name);
            else
               sprintf(ref, "/%s", key.name);

            rsprintf("<form method=\"GET\" action=\"%s\">\n", ref);

            /* alarm name */
            if (exp_name[0])
               sprintf(ref, "/Alarms/Alarms/%s?exp=%s", key.name, exp_name);
            else
               sprintf(ref, "/Alarms/Alarms/%s", key.name);

            rsprintf("<tr><td bgcolor=#C0C0FF><a href=\"%s\"><b>%s</b></a>", ref,
                     key.name);

            /* state */
            size = sizeof(BOOL);
            db_get_value(hDB, hkey, "Active", &active, &size, TID_BOOL, TRUE);
            size = sizeof(INT);
            db_get_value(hDB, hkey, "Triggered", &triggered, &size, TID_INT, TRUE);
            if (!active)
               rsprintf("<td bgcolor=#FFFF00 align=center>Disabled");
            else {
               if (!triggered)
                  rsprintf("<td bgcolor=#00FF00 align=center>OK");
               else
                  rsprintf("<td bgcolor=#FF0000 align=center>Triggered");
            }

            /* time */
            size = sizeof(str);
            db_get_value(hDB, hkey, "Time triggered first", str, &size, TID_STRING, TRUE);
            if (!triggered)
               strcpy(str, "-");
            rsprintf("<td align=center>%s", str);

            /* class */
            size = sizeof(str);
            db_get_value(hDB, hkey, "Alarm Class", str, &size, TID_STRING, TRUE);

            if (exp_name[0])
               sprintf(ref, "/Alarms/Classes/%s?exp=%s", str, exp_name);
            else
               sprintf(ref, "/Alarms/Classes/%s", str);
            rsprintf("<td align=center><a href=\"%s\">%s</a>", ref, str);

            /* condition */
            size = sizeof(condition);
            db_get_value(hDB, hkey, "Condition", condition, &size, TID_STRING, TRUE);

            if (index == AT_EVALUATED) {
               /* print condition */
               rsprintf("<td>");
               strencode(condition);

               /* retrieve value */
               al_evaluate_condition(condition, value);
               rsprintf("<td align=center bgcolor=#C0C0FF>%s", value);
            } else if (index == AT_PROGRAM) {
               /* print condition */
               rsprintf("<td colspan=2>");
               strencode(condition);
            } else if (index == AT_INTERNAL) {
               size = sizeof(str);
               if (triggered)
                  db_get_value(hDB, hkey, "Alarm message", str, &size, TID_STRING, TRUE);
               else
                  db_get_value(hDB, hkey, "Condition", str, &size, TID_STRING, TRUE);

               rsprintf("<td colspan=2>%s", str);
            } else if (index == AT_PERIODIC) {
               size = sizeof(str);
               if (triggered)
                  db_get_value(hDB, hkey, "Alarm message", str, &size, TID_STRING, TRUE);
               else {
                  size = sizeof(last);
                  db_get_value(hDB, hkey, "Checked last", &last, &size, TID_DWORD, TRUE);
                  if (last == 0) {
                     last = ss_time();
                     db_set_value(hDB, hkey, "Checked last", &last, size, 1, TID_DWORD);
                  }

                  size = sizeof(interval);
                  db_get_value(hDB, hkey, "Check interval", &interval, &size, TID_INT,
                               TRUE);
                  last += interval;
                  strcpy(value, ctime(&last));
                  value[16] = 0;

                  sprintf(str, "Alarm triggers at %s", value);
               }

               rsprintf("<td colspan=2>%s", str);
            }

            rsprintf("<td>\n");
            if (triggered) {
               if (exp_name[0])
                  rsprintf("<input type=hidden name=exp value=\"%s\">\n", exp_name);
               rsprintf("<input type=submit name=cmd value=\"Reset\">\n");
            } else
               rsprintf(" &nbsp;&nbsp;&nbsp;&nbsp;");

            rsprintf("</tr>\n");
            rsprintf("</form>\n");
         }
      }
   }

   rsprintf("</table>\n");
   rsprintf("</body></html>\r\n");
}

/*------------------------------------------------------------------*/

void show_programs_page()
{
   INT i, j, size, count, status;
   BOOL restart, first, required;
   HNDLE hDB, hkeyroot, hkey, hkey_rc, hkeycl;
   KEY key, keycl;
   char str[256], ref[256], command[256], name[80];

   cm_get_experiment_database(&hDB, NULL);

   /* stop command */
   if (*getparam("Stop")) {
      status = cm_shutdown(getparam("Stop") + 5, FALSE);

      if (status == CM_SUCCESS)
         redirect("?cmd=programs");
      else {
         sprintf(str,
                 "Cannot shut down client \"%s\", please kill manually and do an ODB cleanup",
                 getparam("Stop") + 5);
         show_error(str);
      }

      return;
   }

   /* start command */
   if (*getparam("Start")) {
      /* for NT: close reply socket before starting subprocess */
      redirect2("?cmd=programs");

      strcpy(name, getparam("Start") + 6);
      sprintf(str, "/Programs/%s/Start command", name);
      command[0] = 0;
      size = sizeof(command);
      db_get_value(hDB, 0, str, command, &size, TID_STRING, TRUE);
      if (command[0]) {
         ss_system(command);
         for (i = 0; i < 50; i++) {
            if (cm_exist(name, FALSE) == CM_SUCCESS)
               break;
            ss_sleep(100);
         }
      }

      return;
   }

   show_header(hDB, "Programs", "GET", "", 3, 0);

  /*---- menu buttons ----*/

   rsprintf("<tr><td colspan=6 bgcolor=#C0C0C0>\n");

   rsprintf("<input type=submit name=cmd value=Alarms>\n");
   rsprintf("<input type=submit name=cmd value=Status>\n");
   rsprintf("</tr>\n\n");

   rsprintf("<input type=hidden name=cmd value=Programs>\n");

  /*---- programs ----*/

   rsprintf("<tr><th>Program<th>Running on host<th>Alarm class<th>Autorestart</tr>\n");

   /* go through all programs */
   db_find_key(hDB, 0, "/Programs", &hkeyroot);
   if (hkeyroot) {
      for (i = 0;; i++) {
         db_enum_key(hDB, hkeyroot, i, &hkey);

         if (!hkey)
            break;

         db_get_key(hDB, hkey, &key);

         /* skip "execute on xxxx" */
         if (key.type != TID_KEY)
            continue;

         if (exp_name[0])
            sprintf(ref, "/Programs/%s?exp=%s", key.name, exp_name);
         else
            sprintf(ref, "/Programs/%s", key.name);

         /* required? */
         size = sizeof(required);
         db_get_value(hDB, hkey, "Required", &required, &size, TID_BOOL, TRUE);

         /* running */
         count = 0;
         if (db_find_key(hDB, 0, "/System/Clients", &hkey_rc) == DB_SUCCESS) {
            first = TRUE;
            for (j = 0;; j++) {
               db_enum_key(hDB, hkey_rc, j, &hkeycl);
               if (!hkeycl)
                  break;

               size = sizeof(name);
               db_get_value(hDB, hkeycl, "Name", name, &size, TID_STRING, TRUE);

               db_get_key(hDB, hkeycl, &keycl);
               name[strlen(key.name)] = 0;

               if (equal_ustring(name, key.name)) {
                  size = sizeof(str);
                  db_get_value(hDB, hkeycl, "Host", str, &size, TID_STRING, TRUE);

                  if (first) {
                     rsprintf("<tr><td bgcolor=#C0C0FF><a href=\"%s\"><b>%s</b></a>", ref,
                              key.name);
                     rsprintf("<td align=center bgcolor=#00FF00>");
                  }
                  if (!first)
                     rsprintf("<br>");
                  rsprintf(str);

                  first = FALSE;
                  count++;
               }
            }
         }

         if (count == 0 && required) {
            rsprintf("<tr><td bgcolor=#C0C0FF><a href=\"%s\"><b>%s</b></a>", ref,
                     key.name);
            rsprintf("<td align=center bgcolor=#FF0000>Not running");
         }

         /* dont display non-running programs which are not required */
         if (count == 0 && !required)
            continue;

         /* Alarm */
         size = sizeof(str);
         db_get_value(hDB, hkey, "Alarm Class", str, &size, TID_STRING, TRUE);
         if (str[0]) {
            if (exp_name[0])
               sprintf(ref, "/Alarms/Classes/%s?exp=%s", str, exp_name);
            else
               sprintf(ref, "/Alarms/Classes/%s", str);
            rsprintf("<td bgcolor=#FFFF00 align=center><a href=\"%s\">%s</a>", ref, str);
         } else
            rsprintf("<td align=center>-");

         /* auto restart */
         size = sizeof(restart);
         db_get_value(hDB, hkey, "Auto restart", &restart, &size, TID_BOOL, TRUE);

         if (restart)
            rsprintf("<td align=center>Yes\n");
         else
            rsprintf("<td align=center>No\n");

         /* start/stop button */
         size = sizeof(str);
         db_get_value(hDB, hkey, "Start Command", str, &size, TID_STRING, TRUE);
         if (str[0] && count == 0) {
            sprintf(str, "Start %s", key.name);
            rsprintf("<td align=center><input type=submit name=\"Start\" value=\"%s\">\n",
                     str);
         }

         if (count > 0 && strncmp(key.name, "mhttpd", 6) != 0) {
            sprintf(str, "Stop %s", key.name);
            rsprintf("<td align=center><input type=submit name=\"Stop\" value=\"%s\">\n",
                     str);
         }

         rsprintf("</tr>\n");
      }
   }


   rsprintf("</table>\n");
   rsprintf("</body></html>\r\n");
}

/*------------------------------------------------------------------*/

void show_config_page(int refresh)
{
   char str[80];
   HNDLE hDB;

   cm_get_experiment_database(&hDB, NULL);

   show_header(hDB, "Configure", "GET", "", 1, 0);

   rsprintf("<tr><th bgcolor=#A0A0FF colspan=2>Configure</tr>\n");

   rsprintf("<tr><td bgcolor=#FFFF00>Update period\n");

   sprintf(str, "5");
   rsprintf("<td><input type=text size=5 maxlength=5 name=refr value=%d>\n", refresh);
   rsprintf("</tr>\n");

   rsprintf("<tr><td align=center colspan=2>\n");
   rsprintf("<input type=submit name=cmd value=Accept>\n");
   rsprintf("<input type=submit name=cmd value=Cancel>\n");
   rsprintf("<input type=hidden name=cmd value=Accept>\n");
   rsprintf("</tr>\n");
   rsprintf("</table>\n");

   rsprintf("</body></html>\r\n");
}

/*------------------------------------------------------------------*/

#define LN10 2.302585094
#define LOG2 0.301029996
#define LOG5 0.698970005

void haxis(gdImagePtr im, gdFont * font, int col, int gcol,
           int x1, int y1, int width,
           int minor, int major, int text, int label, int grid, double xmin, double xmax)
{
   double dx, int_dx, frac_dx, x_act, label_dx, major_dx, x_screen, maxwidth;
   int tick_base, major_base, label_base, n_sig1, n_sig2, xs;
   char str[80];
   double base[] = { 1, 2, 5, 10, 20, 50, 100, 200, 500, 1000 };

   if (xmax <= xmin || width <= 0)
      return;

   /* use 5 as min tick distance */
   dx = (xmax - xmin) / (double) (width / 5);

   frac_dx = modf(log(dx) / LN10, &int_dx);
   if (frac_dx < 0) {
      frac_dx += 1;
      int_dx -= 1;
   }

   tick_base = frac_dx < LOG2 ? 1 : frac_dx < LOG5 ? 2 : 3;
   major_base = label_base = tick_base + 1;

   /* rounding up of dx, label_dx */
   dx = pow(10, int_dx) * base[tick_base];
   major_dx = pow(10, int_dx) * base[major_base];
   label_dx = major_dx;

   /* number of significant digits */
   if (xmin == 0)
      n_sig1 = 0;
   else
      n_sig1 = (int) floor(log(fabs(xmin)) / LN10) -
          (int) floor(log(fabs(label_dx)) / LN10) + 1;

   if (xmax == 0)
      n_sig2 = 0;
   else
      n_sig2 = (int) floor(log(fabs(xmax)) / LN10) -
          (int) floor(log(fabs(label_dx)) / LN10) + 1;

   n_sig1 = MAX(n_sig1, n_sig2);
   n_sig1 = MAX(n_sig1, 4);

   /* determination of maximal width of labels */
   sprintf(str, "%1.*lG", n_sig1, floor(xmin / dx) * dx);
   maxwidth = font->h / 2 * strlen(str);
   sprintf(str, "%1.*lG", n_sig1, floor(xmax / dx) * dx);
   maxwidth = MAX(maxwidth, font->h / 2 * strlen(str));
   sprintf(str, "%1.*lG", n_sig1, floor(xmax / dx) * dx + label_dx);
   maxwidth = MAX(maxwidth, font->h / 2 * strlen(str));

   /* increasing label_dx, if labels would overlap */
   while (maxwidth > 0.7 * label_dx / (xmax - xmin) * width) {
      label_base++;
      label_dx = pow(10, int_dx) * base[label_base];
      if (label_base % 3 == 2 && major_base % 3 == 1) {
         major_base++;
         major_dx = pow(10, int_dx) * base[major_base];
      }
   }

   x_act = floor(xmin / dx) * dx;

   gdImageLine(im, x1, y1, x1 + width, y1, col);

   do {
      x_screen = (x_act - xmin) / (xmax - xmin) * width + x1;
      xs = (int) (x_screen + 0.5);

      if (x_screen > x1 + width + 0.001)
         break;

      if (x_screen >= x1) {
         if (fabs(floor(x_act / major_dx + 0.5) - x_act / major_dx) <
             dx / major_dx / 10.0) {

            if (fabs(floor(x_act / label_dx + 0.5) - x_act / label_dx) <
                dx / label_dx / 10.0) {
          /**** label tick mark ****/
               gdImageLine(im, xs, y1, xs, y1 + text, col);

          /**** grid line ***/
               if (grid != 0 && xs > x1 && xs < x1 + width)
                  gdImageLine(im, xs, y1, xs, y1 + grid, col);

          /**** label ****/
               if (label != 0) {
                  sprintf(str, "%1.*lG", n_sig1, x_act);
                  gdImageString(im, font, (int) xs - font->w * strlen(str) / 2,
                                y1 + label, str, col);
               }
            } else {
          /**** major tick mark ****/
               gdImageLine(im, xs, y1, xs, y1 + major, col);

          /**** grid line ****/
               if (grid != 0 && xs > x1 && xs < x1 + width)
                  gdImageLine(im, xs, y1 - 1, xs, y1 + grid, gcol);
            }

         } else
        /**** minor tick mark ****/
            gdImageLine(im, xs, y1, xs, y1 + minor, col);

      }

      x_act += dx;

      /* supress 1.23E-17 ... */
      if (fabs(x_act) < dx / 100)
         x_act = 0;

   } while (1);
}

/*------------------------------------------------------------------*/

void sec_to_label(char *result, int sec, int base, int force_date)
{
   char mon[80];
   struct tm *tms;

   tms = localtime((time_t *) & sec);
   strcpy(mon, mname[tms->tm_mon]);
   mon[3] = 0;

   if (force_date) {
      if (base < 600)
         sprintf(result, "%02d %s %02d %02d:%02d:%02d",
                 tms->tm_mday, mon, tms->tm_year % 100,
                 tms->tm_hour, tms->tm_min, tms->tm_sec);
      else if (base < 3600 * 24)
         sprintf(result, "%02d %s %02d %02d:%02d",
                 tms->tm_mday, mon, tms->tm_year % 100, tms->tm_hour, tms->tm_min);
      else
         sprintf(result, "%02d %s %02d", tms->tm_mday, mon, tms->tm_year % 100);
   } else {
      if (base < 600)
         sprintf(result, "%02d:%02d:%02d", tms->tm_hour, tms->tm_min, tms->tm_sec);
      else if (base < 3600 * 3)
         sprintf(result, "%02d:%02d", tms->tm_hour, tms->tm_min);
      else if (base < 3600 * 24)
         sprintf(result, "%02d %s %02d %02d:%02d",
                 tms->tm_mday, mon, tms->tm_year % 100, tms->tm_hour, tms->tm_min);
      else
         sprintf(result, "%02d %s %02d", tms->tm_mday, mon, tms->tm_year % 100);
   }
}

void taxis(gdImagePtr im, gdFont * font, int col, int gcol,
           int x1, int y1, int width, int xr,
           int minor, int major, int text, int label, int grid, double xmin, double xmax)
{
   int dx, x_act, label_dx, major_dx, x_screen, maxwidth;
   int tick_base, major_base, label_base, xs, xl;
   char str[80];
   int base[] = { 1, 5, 10, 60, 300, 600, 1800, 3600, 3600 * 6, 3600 * 12, 3600 * 24, 0 };
   time_t ltime;
   int force_date, d1, d2;
   struct tm *ptms;

   if (xmax <= xmin || width <= 0)
      return;

   /* force date display if xmax not today */
   ltime = ss_time();
   ptms = localtime(&ltime);
   d1 = ptms->tm_mday;
   ltime = (time_t) xmax;
   ptms = localtime(&ltime);
   d2 = ptms->tm_mday;
   force_date = (d1 != d2);

   /* use 5 pixel as min tick distance */
   dx = (int) ((xmax - xmin) / (double) (width / 5) + 0.5);

   for (tick_base = 0; base[tick_base]; tick_base++) {
      if (base[tick_base] > dx)
         break;
   }
   if (!base[tick_base])
      tick_base--;
   dx = base[tick_base];

   if (base[tick_base + 1])
      major_base = tick_base + 1;
   else
      major_base = tick_base;
   major_dx = base[major_base];

   if (base[major_base + 1])
      label_base = major_base + 1;
   else
      label_base = major_base;
   label_dx = base[label_base];

   do {
      sec_to_label(str, (int) (xmin + 0.5), label_dx, force_date);
      maxwidth = font->h / 2 * strlen(str);

      /* increasing label_dx, if labels would overlap */
      if (maxwidth > 0.7 * label_dx / (xmax - xmin) * width) {
         if (base[label_base + 1])
            label_dx = base[++label_base];
         else
            label_dx += 3600 * 24;
      } else
         break;
   } while (1);

   x_act =
       (int) floor((double) (xmin - ss_timezone()) / label_dx) * label_dx + ss_timezone();

   gdImageLine(im, x1, y1, x1 + width, y1, col);

   do {
      x_screen = (int) ((x_act - xmin) / (xmax - xmin) * width + x1 + 0.5);
      xs = (int) (x_screen + 0.5);

      if (x_screen > x1 + width + 0.001)
         break;

      if (x_screen >= x1) {
         if ((x_act - ss_timezone()) % major_dx == 0) {
            if ((x_act - ss_timezone()) % label_dx == 0) {
          /**** label tick mark ****/
               gdImageLine(im, xs, y1, xs, y1 + text, col);

          /**** grid line ***/
               if (grid != 0 && xs > x1 && xs < x1 + width)
                  gdImageLine(im, xs, y1, xs, y1 + grid, col);

          /**** label ****/
               if (label != 0) {
                  sec_to_label(str, x_act, label_dx, force_date);

                  /* if labels at edge, shift them in */
                  xl = (int) xs - font->w * strlen(str) / 2;
                  if (xl < 0)
                     xl = 0;
                  if (xl + font->w * (int) strlen(str) > xr)
                     xl = xr - font->w * strlen(str);
                  gdImageString(im, font, xl, y1 + label, str, col);
               }
            } else {
          /**** major tick mark ****/
               gdImageLine(im, xs, y1, xs, y1 + major, col);

          /**** grid line ****/
               if (grid != 0 && xs > x1 && xs < x1 + width)
                  gdImageLine(im, xs, y1 - 1, xs, y1 + grid, gcol);
            }

         } else
        /**** minor tick mark ****/
            gdImageLine(im, xs, y1, xs, y1 + minor, col);

      }

      x_act += dx;

      /* supress 1.23E-17 ... */
      if (fabs(x_act) < dx / 100)
         x_act = 0;

   } while (1);
}

/*------------------------------------------------------------------*/

int vaxis(gdImagePtr im, gdFont * font, int col, int gcol,
          int x1, int y1, int width,
          int minor, int major, int text, int label,
          int grid, double ymin, double ymax, BOOL logaxis)
{
   double dy, int_dy, frac_dy, y_act, label_dy, major_dy, y_screen, y_next;
   int tick_base, major_base, label_base, n_sig1, n_sig2, ys, max_width;
   int last_label_y;
   char str[80];
   double base[] = { 1, 2, 5, 10, 20, 50, 100, 200, 500, 1000 };

   if (ymax <= ymin || width <= 0)
      return 0;

   if (logaxis) {
      dy = pow(10, floor(log(ymin) / LN10));
      label_dy = dy;
      major_dy = dy * 10;
      n_sig1 = 4;
   } else {
      dy = (ymax - ymin) / (double) (width / 5);

      frac_dy = modf(log(dy) / LN10, &int_dy);
      if (frac_dy < 0) {
         frac_dy += 1;
         int_dy -= 1;
      }

      tick_base = frac_dy < LOG2 ? 1 : frac_dy < LOG5 ? 2 : 3;
      major_base = label_base = tick_base + 1;

      /* rounding up of dy, label_dy */
      dy = pow(10, int_dy) * base[tick_base];
      major_dy = pow(10, int_dy) * base[major_base];
      label_dy = major_dy;

      /* number of significant digits */
      if (ymin == 0)
         n_sig1 = 0;
      else
         n_sig1 = (int) floor(log(fabs(ymin)) / LN10) -
             (int) floor(log(fabs(label_dy)) / LN10) + 1;

      if (ymax == 0)
         n_sig2 = 0;
      else
         n_sig2 = (int) floor(log(fabs(ymax)) / LN10) -
             (int) floor(log(fabs(label_dy)) / LN10) + 1;

      n_sig1 = MAX(n_sig1, n_sig2);
      n_sig1 = MAX(n_sig1, 4);

      /* increasing label_dy, if labels would overlap */
      while (label_dy / (ymax - ymin) * width < 1.5 * font->h) {
         label_base++;
         label_dy = pow(10, int_dy) * base[label_base];
         if (label_base % 3 == 2 && major_base % 3 == 1) {
            major_base++;
            major_dy = pow(10, int_dy) * base[major_base];
         }
      }
   }

   max_width = 0;
   y_act = floor(ymin / dy) * dy;

   if (x1 != 0 || y1 != 0)
      gdImageLine(im, x1, y1, x1, y1 - width, col);

   last_label_y = y1 + 2 * font->h;

   do {
      if (logaxis)
         y_screen = y1 - (log(y_act) - log(ymin)) / (log(ymax) - log(ymin)) * width;
      else
         y_screen = y1 - (y_act - ymin) / (ymax - ymin) * width;
      ys = (int) (y_screen + 0.5);

      if (y_screen < y1 - width - 0.001)
         break;

      if (y_screen <= y1 + 0.001) {
         if (fabs(floor(y_act / major_dy + 0.5) - y_act / major_dy) <
             dy / major_dy / 10.0) {
            if (fabs(floor(y_act / label_dy + 0.5) - y_act / label_dy) <
                dy / label_dy / 10.0) {
               if (x1 != 0 || y1 != 0) {
            /**** label tick mark ****/
                  gdImageLine(im, x1, ys, x1 + text, ys, col);

            /**** grid line ***/
                  if (grid != 0 && y_screen < y1 && y_screen > y1 - width) {
                     if (grid > 0)
                        gdImageLine(im, x1 + 1, ys, x1 + grid, ys, gcol);
                     else
                        gdImageLine(im, x1 - 1, ys, x1 + grid, ys, gcol);
                  }

            /**** label ****/
                  if (label != 0) {
                     sprintf(str, "%1.*lG", n_sig1, y_act);
                     if (label < 0)
                        gdImageString(im, font, x1 + label - font->w * strlen(str),
                                      ys - font->h / 2, str, col);
                     else
                        gdImageString(im, font, x1 + label, ys - font->h / 2, str, col);

                     last_label_y = ys - font->h / 2;
                  }
               } else {
                  sprintf(str, "%1.*lG", n_sig1, y_act);
                  max_width = MAX(max_width, (int) (font->w * strlen(str)));
               }
            } else {
               if (x1 != 0 || y1 != 0) {
            /**** major tick mark ****/
                  gdImageLine(im, x1, ys, x1 + major, ys, col);

            /**** grid line ***/
                  if (grid != 0 && y_screen < y1 && y_screen > y1 - width)
                     gdImageLine(im, x1, ys, x1 + grid, ys, col);
               }
            }
            if (logaxis) {
               dy *= 10;
               major_dy *= 10;
               label_dy *= 10;
            }

         } else {
            if (x1 != 0 || y1 != 0) {
          /**** minor tick mark ****/
               gdImageLine(im, x1, ys, x1 + minor, ys, col);
            }

            /* for logaxis, also put labes on minor tick marks */
            if (logaxis) {
               if (label != 0) {
                  if (x1 != 0 || y1 != 0) {
                     /* calculate position of next major label */
                     y_next = pow(10, floor(log(y_act) / LN10) + 1);
                     y_screen =
                         (int) (y1 -
                                (log(y_next) - log(ymin)) / (log(ymax) -
                                                             log(ymin)) * width + 0.5);

                     if (ys + font->h / 2 < last_label_y &&
                         ys - font->h / 2 > y_screen + font->h / 2) {
                        sprintf(str, "%1.*lG", n_sig1, y_act);
                        if (label < 0)
                           gdImageString(im, font, x1 + label - font->w * strlen(str),
                                         ys - font->h / 2, str, col);
                        else
                           gdImageString(im, font, x1 + label, ys - font->h / 2, str,
                                         col);
                     }

                     last_label_y = ys - font->h / 2;
                  } else {
                     sprintf(str, "%1.*lG", n_sig1, y_act);
                     max_width = MAX(max_width, (int) (font->w * strlen(str)));
                  }
               }
            }
         }
      }

      y_act += dy;

      /* supress 1.23E-17 ... */
      if (fabs(y_act) < dy / 100)
         y_act = 0;

   } while (1);

   return max_width + abs(label);
}

/*------------------------------------------------------------------*/

int time_to_sec(char *str)
{
   double s;

   s = atof(str);
   switch (str[strlen(str) - 1]) {
   case 'm':
   case 'M':
      s *= 60;
      break;
   case 'h':
   case 'H':
      s *= 3600;
      break;
   case 'd':
   case 'D':
      s *= 3600 * 24;
      break;
   }

   return (int) s;
}

/*------------------------------------------------------------------*/

#define MAX_VARS 10

void generate_hist_graph(char *path, char *buffer, int *buffer_size,
                         int width, int height, int scale, int toffset, int index,
                         int labels)
{
   HNDLE hDB, hkey, hkeypanel, hkeyeq, hkeydvar, hkeyvars, hkeyroot, hkeynames;
   KEY key;
   gdImagePtr im;
   gdGifBuffer gb;
   int i, j, k, l, n_vars, size, status, row, x_marker;
   DWORD bsize, tsize, n_marker, *state, run_number;
   int length, aoffset;
   int flag, x1, y1, x2, y2, xs, xs_old, ys, xold, yold, xmaxm;
   int white, black, grey, ltgrey, red, green, blue, curve_col[MAX_VARS], state_col[3];
   char str[256], panel[NAME_LENGTH], *p, odbpath[256];
   INT var_index[MAX_VARS];
   DWORD type, event_id;
   char event_name[MAX_VARS][NAME_LENGTH];
   char tag_name[MAX_VARS][64], var_name[MAX_VARS][NAME_LENGTH], varname[64],
       key_name[256];
   DWORD n_point[MAX_VARS];
   int x[MAX_VARS][1000];
   float y[MAX_VARS][1000];
   float factor[MAX_VARS], offset[MAX_VARS];
   BOOL logaxis, runmarker;
   float xmin, xmax, ymin, ymax;
   gdPoint poly[3];
   float upper_limit[MAX_VARS], lower_limit[MAX_VARS];
   double yb1, yb2, yf1, yf2, ybase;

   static char *ybuffer;
   static DWORD *tbuffer;
   static int hbuffer_size = 0;

   if (hbuffer_size == 0) {
      hbuffer_size = 1000 * sizeof(DWORD);
      tbuffer = malloc(hbuffer_size);
      ybuffer = malloc(hbuffer_size);
   }

   cm_get_experiment_database(&hDB, NULL);

   /* generate test image */
   im = gdImageCreate(width, height);

   /* First color allocated is background. */
   grey = gdImageColorAllocate(im, 192, 192, 192);
   ltgrey = gdImageColorAllocate(im, 208, 208, 208);
   white = gdImageColorAllocate(im, 255, 255, 255);
   black = gdImageColorAllocate(im, 0, 0, 0);
   red = gdImageColorAllocate(im, 255, 0, 0);
   green = gdImageColorAllocate(im, 0, 255, 0);
   blue = gdImageColorAllocate(im, 0, 0, 255);

   curve_col[0] = gdImageColorAllocate(im, 0, 0, 255);
   curve_col[1] = gdImageColorAllocate(im, 0, 192, 0);
   curve_col[2] = gdImageColorAllocate(im, 255, 0, 0);
   curve_col[3] = gdImageColorAllocate(im, 0, 192, 192);
   curve_col[4] = gdImageColorAllocate(im, 255, 0, 255);
   curve_col[5] = gdImageColorAllocate(im, 192, 192, 0);
   curve_col[6] = gdImageColorAllocate(im, 128, 128, 128);
   curve_col[7] = gdImageColorAllocate(im, 128, 255, 128);
   curve_col[8] = gdImageColorAllocate(im, 255, 128, 128);
   curve_col[9] = gdImageColorAllocate(im, 128, 128, 255);

   state_col[0] = gdImageColorAllocate(im, 255, 0, 0);
   state_col[1] = gdImageColorAllocate(im, 255, 255, 0);
   state_col[2] = gdImageColorAllocate(im, 0, 255, 0);

   /* Set transparent color. */
   gdImageColorTransparent(im, grey);

   /* Title */
   strcpy(panel, path);
   if (strstr(panel, ".gif"))
      *strstr(panel, ".gif") = 0;
   gdImageString(im, gdFontGiant, width / 2 - (strlen(panel) * gdFontGiant->w) / 2, 2,
                 panel, black);

   /* set history path */
   status = db_find_key(hDB, 0, "/Logger/Data dir", &hkey);
   if (status != DB_SUCCESS) {
      sprintf(str, "No data directory defined in ODB");
      gdImageString(im, gdFontGiant, width / 2 - (strlen(str) * gdFontGiant->w) / 2,
                    height / 2, str, red);
      goto error;
   }

   /* check dedicated history path */
   size = sizeof(str);
   memset(str, 0, size);

   status = db_get_value(hDB, 0, "/Logger/History dir", str, &size, TID_STRING, FALSE);
   if (status != DB_SUCCESS)
      db_get_value(hDB, 0, "/Logger/Data dir", str, &size, TID_STRING, TRUE);
   hs_set_path(str);

   /* check panel name in ODB */
   sprintf(str, "/History/Display/%s", panel);
   db_find_key(hDB, 0, str, &hkeypanel);
   if (!hkey) {
      sprintf(str, "Cannot find /History/Display/%s in ODB", panel);
      gdImageString(im, gdFontGiant, width / 2 - (strlen(str) * gdFontGiant->w) / 2,
                    height / 2, str, red);
      goto error;
   }

   db_find_key(hDB, hkeypanel, "Variables", &hkeydvar);
   if (!hkeydvar) {
      sprintf(str, "Cannot find /History/Display/%s/Variables in ODB", panel);
      gdImageString(im, gdFontGiant, width / 2 - (strlen(str) * gdFontGiant->w) / 2,
                    height / 2, str, red);
      goto error;
   }

   db_get_key(hDB, hkeydvar, &key);
   n_vars = key.num_values;

   if (n_vars > MAX_VARS) {
      sprintf(str, "Too many variables in panel %s", panel);
      gdImageString(im, gdFontGiant, width / 2 - (strlen(str) * gdFontGiant->w) / 2,
                    height / 2, str, red);
      goto error;
   }

   ymin = ymax = 0;
   logaxis = runmarker = 0;

   for (i = 0; i < n_vars; i++) {
      if (index != -1 && index != i)
         continue;

      size = sizeof(str);
      db_get_data_index(hDB, hkeydvar, str, &size, i, TID_STRING);
      strcpy(tag_name[i], str);

      /* split varname in event, variable and index */
      if (strchr(tag_name[i], ':')) {
         strcpy(event_name[i], tag_name[i]);
         *strchr(event_name[i], ':') = 0;
         strcpy(var_name[i], strchr(tag_name[i], ':') + 1);
         var_index[i] = 0;
         if (strchr(var_name[i], '[')) {
            var_index[i] = atoi(strchr(var_name[i], '[') + 1);
            *strchr(var_name[i], '[') = 0;
         }
      } else {
         sprintf(str, "Tag \"%s\" has wrong format in panel %s", tag_name[i], panel);
         gdImageString(im, gdFontGiant, width / 2 - (strlen(str) * gdFontGiant->w) / 2,
                       height / 2, str, red);
         goto error;
      }

      /* search event_id */
      status = hs_get_event_id(0, event_name[i], &event_id);

      if (status != HS_SUCCESS) {
         sprintf(str, "Event \"%s\" from panel \"%s\" not found in history",
                 event_name[i], panel);
         gdImageString(im, gdFontGiant, width / 2 - (strlen(str) * gdFontGiant->w) / 2,
                       height / 2, str, red);
         goto error;
      }

      /* get timescale */
      if (scale == 0) {
         strcpy(str, "1h");
         size = NAME_LENGTH;
         status = db_get_value(hDB, hkeypanel, "Timescale", str, &size, TID_STRING, TRUE);
         if (status != DB_SUCCESS) {
            /* delete old integer key */
            db_find_key(hDB, hkeypanel, "Timescale", &hkey);
            if (hkey)
               db_delete_key(hDB, hkey, FALSE);

            strcpy(str, "1h");
            size = NAME_LENGTH;
            status =
                db_get_value(hDB, hkeypanel, "Timescale", str, &size, TID_STRING, TRUE);
         }

         scale = time_to_sec(str);
      }

      for (j = 0; j < MAX_VARS; j++) {
         factor[j] = 1;
         offset[j] = 0;
      }

      /* get factors */
      size = sizeof(float) * n_vars;
      db_get_value(hDB, hkeypanel, "Factor", factor, &size, TID_FLOAT, TRUE);

      /* get offsets */
      size = sizeof(float) * n_vars;
      db_get_value(hDB, hkeypanel, "Offset", offset, &size, TID_FLOAT, TRUE);

      /* get axis type */
      size = sizeof(logaxis);
      logaxis = 0;
      db_get_value(hDB, hkeypanel, "Log axis", &logaxis, &size, TID_BOOL, TRUE);

      /* get runmarker flag */
      size = sizeof(runmarker);
      runmarker = 1;
      db_get_value(hDB, hkeypanel, "Show run markers", &runmarker, &size, TID_BOOL, TRUE);

      /* make ODB path from tag name */
      odbpath[0] = 0;
      db_find_key(hDB, 0, "/Equipment", &hkeyroot);
      if (hkeyroot) {
         for (j = 0;; j++) {
            db_enum_key(hDB, hkeyroot, j, &hkeyeq);

            if (!hkeyeq)
               break;

            db_get_key(hDB, hkeyeq, &key);
            if (equal_ustring(key.name, event_name[i])) {
               /* check if variable is individual key under variabels/ */
               sprintf(str, "Variables/%s", var_name[i]);
               db_find_key(hDB, hkeyeq, str, &hkey);
               if (hkey) {
                  sprintf(odbpath, "/Equipment/%s/Variables/%s", event_name[i],
                          var_name[i]);
                  break;
               }

               /* check if variable is in setttins/names array */
               db_find_key(hDB, hkeyeq, "Settings/Names", &hkeynames);
               if (hkeynames) {
                  /* extract variable name and Variables/<key> */
                  strcpy(str, var_name[i]);
                  p = str + strlen(str) - 1;
                  while (p > str && *p != ' ')
                     p--;
                  strcpy(key_name, p + 1);
                  *p = 0;
                  strcpy(varname, str);

                  /* find key in single name array */
                  db_get_key(hDB, hkeynames, &key);
                  for (k = 0; k < key.num_values; k++) {
                     size = sizeof(str);
                     db_get_data_index(hDB, hkeynames, str, &size, k, TID_STRING);
                     if (equal_ustring(str, varname)) {
                        sprintf(odbpath, "/Equipment/%s/Variables/%s[%d]", event_name[i],
                                key_name, k);
                        break;
                     }
                  }
               } else {
                  /* go through /variables/<name> entries */
                  db_find_key(hDB, hkeyeq, "Variables", &hkeyvars);
                  if (hkeyvars) {
                     for (k = 0;; k++) {
                        db_enum_key(hDB, hkeyvars, k, &hkey);

                        if (!hkey)
                           break;

                        /* find "settins/names <key>" for this key */
                        db_get_key(hDB, hkey, &key);

                        /* find key in key_name array */
                        strcpy(key_name, key.name);
                        sprintf(str, "Settings/Names %s", key_name);

                        db_find_key(hDB, hkeyeq, str, &hkeynames);
                        if (hkeynames) {
                           db_get_key(hDB, hkeynames, &key);
                           for (l = 0; l < key.num_values; l++) {
                              size = sizeof(str);
                              db_get_data_index(hDB, hkeynames, str, &size, l,
                                                TID_STRING);
                              if (equal_ustring(str, var_name[i])) {
                                 sprintf(odbpath, "/Equipment/%s/Variables/%s[%d]",
                                         event_name[i], key_name, l);
                                 break;
                              }
                           }
                        }
                     }
                  }
               }

               break;
            }
         }

         if (!hkeyeq) {
            db_find_key(hDB, 0, "/History/Links", &hkeyroot);
            if (hkeyroot) {
               for (j = 0;; j++) {
                  db_enum_link(hDB, hkeyroot, j, &hkey);

                  if (!hkey)
                     break;

                  db_get_key(hDB, hkey, &key);
                  if (equal_ustring(key.name, event_name[i])) {
                     db_enum_key(hDB, hkeyroot, j, &hkey);
                     db_find_key(hDB, hkey, var_name[i], &hkey);
                     if (hkey) {
                        db_get_key(hDB, hkey, &key);
                        db_get_path(hDB, hkey, odbpath, sizeof(odbpath));
                        if (key.num_values > 1)
                           sprintf(odbpath + strlen(odbpath), "[%d]", var_index[i]);
                        break;
                     }
                  }
               }
            }
         }
      }

      /* search alarm limits */
      upper_limit[i] = lower_limit[i] = -12345;
      db_find_key(hDB, 0, "/Alarms/Alarms", &hkeyroot);
      if (odbpath[0] && hkeyroot) {
         for (j = 0;; j++) {
            db_enum_key(hDB, hkeyroot, j, &hkey);

            if (!hkey)
               break;

            size = sizeof(str);
            db_get_value(hDB, hkey, "Condition", str, &size, TID_STRING, TRUE);

            if (strstr(str, odbpath)) {
               if (strchr(str, '<')) {
                  p = strchr(str, '<') + 1;
                  if (*p == '=')
                     p++;
                  lower_limit[i] = (float) (factor[i] * atof(p) + offset[i]);
               }
               if (strchr(str, '>')) {
                  p = strchr(str, '>') + 1;
                  if (*p == '=')
                     p++;
                  upper_limit[i] = (float) (factor[i] * atof(p) + offset[i]);
               }
            }
         }
      }

      do {
         bsize = tsize = hbuffer_size;
         memset(ybuffer, 0, bsize);
         status =
             hs_read(event_id, ss_time() - scale + toffset, ss_time() + toffset,
                     scale / 1000, var_name[i], var_index[i], tbuffer, &tsize, ybuffer,
                     &bsize, &type, &n_point[i]);

         if (status == HS_TRUNCATED) {
            hbuffer_size *= 2;
            tbuffer = realloc(tbuffer, hbuffer_size);
            assert(tbuffer);
            ybuffer = realloc(ybuffer, hbuffer_size);
            assert(ybuffer);
         }

      } while (status == HS_TRUNCATED);

      if (status == HS_UNDEFINED_VAR) {
         sprintf(str, "Variable \"%s\" not found in history", var_name[i]);
         gdImageString(im, gdFontGiant, width / 2 - (strlen(str) * gdFontGiant->w) / 2,
                       height / 2, str, red);
         goto error;
      }

      for (j = 0; j < (int) n_point[i]; j++) {
         x[i][j] = tbuffer[j] - ss_time();

         /* convert data to float */
         switch (type) {
         case TID_BYTE:
            y[i][j] = (float) *(((BYTE *) ybuffer) + j);
            break;
         case TID_SBYTE:
            y[i][j] = (float) *(((char *) ybuffer) + j);
            break;
         case TID_CHAR:
            y[i][j] = (float) *(((char *) ybuffer) + j);
            break;
         case TID_WORD:
            y[i][j] = (float) *(((WORD *) ybuffer) + j);
            break;
         case TID_SHORT:
            y[i][j] = (float) *(((short *) ybuffer) + j);
            break;
         case TID_DWORD:
            y[i][j] = (float) *(((DWORD *) ybuffer) + j);
            break;
         case TID_INT:
            y[i][j] = (float) *(((INT *) ybuffer) + j);
            break;
         case TID_BOOL:
            y[i][j] = (float) *(((BOOL *) ybuffer) + j);
            break;
         case TID_FLOAT:
            y[i][j] = (float) *(((float *) ybuffer) + j);
            break;
         case TID_DOUBLE:
            y[i][j] = (float) *(((double *) ybuffer) + j);
            break;
         }

         /* avoid overflow */
         if (y[i][j] > 1E30)
            y[i][j] = 1E30f;

         /* apply factor and offset */
         y[i][j] = y[i][j] * factor[i] + offset[i];

         /* calculate ymin and ymax */
         if ((i == 0 || index != -1) && j == 0)
            ymin = ymax = y[i][0];
         else {
            if (y[i][j] > ymax)
               ymax = y[i][j];
            if (y[i][j] < ymin)
               ymin = y[i][j];
         }
      }
   }

   /* check if ylow = 0 */
   if (index == -1) {
      flag = 0;
      size = sizeof(flag);
      db_get_value(hDB, hkeypanel, "Zero ylow", &flag, &size, TID_BOOL, TRUE);
      if (flag && ymin > 0)
         ymin = 0;
   }

   /* if min and max too close together, switch to linear axis */
   if (logaxis && ymin > 0 && ymax > 0) {
      yb1 = pow(10, floor(log(ymin) / LN10));
      yf1 = floor(ymin / yb1);
      yb2 = pow(10, floor(log(ymax) / LN10));
      yf2 = floor(ymax / yb2);

      if (yb1 == yb2 && yf1 == yf2)
         logaxis = 0;
      else {
         /* round down and up ymin and ymax */
         ybase = pow(10, floor(log(ymin) / LN10));
         ymin = (float) (floor(ymin / ybase) * ybase);
         ybase = pow(10, floor(log(ymax) / LN10));
         ymax = (float) ((floor(ymax / ybase) + 1) * ybase);
      }
   }

   /* avoid negative limits for log axis */
   if (logaxis) {
      if (ymax <= 0)
         ymax = 1;
      if (ymin <= 0)
         ymin = 1E-12f;
   }

   /* increase limits by 5% */
   if (ymin == 0 && ymax == 0) {
      ymin = -1;
      ymax = 1;
   } else {
      if (!logaxis) {
         ymax += (ymax - ymin) / 20.f;

         if (ymin != 0)
            ymin -= (ymax - ymin) / 20.f;
      }
   }

   /* avoid ymin == ymax */
   if (ymax == ymin) {
      if (logaxis) {
         ymax *= 2;
         ymin /= 2;
      } else {
         ymax += 1;
         ymin -= 1;
      }
   }

   /* calculate X limits */
   xmin = (float) (-scale / 3600.0 + toffset / 3600.0);
   xmax = (float) (toffset / 3600.0);

   /* caluclate required space for Y-axis */
   aoffset =
       vaxis(im, gdFontSmall, black, ltgrey, 0, 0, height, -3, -5, -7, -8, 0, ymin, ymax,
             logaxis);
   aoffset += 2;

   x1 = aoffset;
   y1 = height - 20;
   x2 = width - 20;
   y2 = 20;

   gdImageFilledRectangle(im, x1, y2, x2, y1, white);

   /* draw axis frame */
   taxis(im, gdFontSmall, black, ltgrey, x1, y1, x2 - x1, width, 3, 5, 9, 10, 0,
         ss_time() - scale + toffset, ss_time() + toffset);

   /* use following line for a X-axis in seconds instead of a time axis */
   //haxis(im, gdFontSmall, black, ltgrey, x1, y1, x2-x1, 3, 5, 9, 10, 0, xmin,  xmax);
   vaxis(im, gdFontSmall, black, ltgrey, x1, y1, y1 - y2, -3, -5, -7, -8, x2 - x1, ymin,
         ymax, logaxis);
   gdImageLine(im, x1, y2, x2, y2, black);
   gdImageLine(im, x2, y2, x2, y1, black);

   xs = ys = xold = yold = 0;

   /* write run markes if selected */
   if (runmarker) {
      bsize = sizeof(ybuffer);
      tsize = sizeof(tbuffer);

      /* read run state */

      status = hs_read(0, ss_time() - scale + toffset, ss_time() + toffset, 0,
                       "State", 0, tbuffer, &tsize, ybuffer, &bsize, &type, &n_marker);

      state = NULL;
      if (status != HS_UNDEFINED_VAR) {
         state = M_MALLOC(sizeof(DWORD) * n_marker);
         for (j = 0; j < (int) n_marker; j++)
            state[j] = *((DWORD *) ybuffer + j);
      }

      bsize = sizeof(ybuffer);
      tsize = sizeof(tbuffer);

      /* read run number */

      status = hs_read(0, ss_time() - scale + toffset, ss_time() + toffset, 0,
                       "Run number", 0, tbuffer, &tsize, ybuffer, &bsize,
                       &type, &n_marker);

      if (status != HS_UNDEFINED_VAR) {
         xs_old = -1;
         xmaxm = x1;
         for (j = 0; j < (int) n_marker; j++) {
            x_marker = tbuffer[j] - ss_time();
            xs = (int) ((x_marker / 3600.0 - xmin) / (xmax - xmin) * (x2 - x1) + x1 +
                        0.5);

            run_number = *((DWORD *) ybuffer + j);

            if (xs <= xs_old)
               xs = xs_old + 1;
            xs_old = xs;

            gdImageDashedLine(im, xs, y1, xs, y2, state_col[state[j] - 1]);

            sprintf(str, "%ld", run_number);

            if (state[j] == STATE_RUNNING) {
               if (xs > xmaxm) {
                  gdImageStringUp(im, gdFontSmall, xs + 0,
                                  y2 + 2 + gdFontSmall->w * strlen(str), str, black);
                  xmaxm = xs - 2 + gdFontSmall->h;
               }
            } else if (state[j] == STATE_STOPPED) {
               if (xs + 2 - gdFontSmall->h > xmaxm) {
                  gdImageStringUp(im, gdFontSmall, xs + 2 - gdFontSmall->h,
                                  y2 + 2 + gdFontSmall->w * strlen(str), str, black);
                  xmaxm = xs - 1;
               }
            }
         }
      }

      if (state)
         M_FREE(state);
   }

   for (i = 0; i < n_vars; i++) {
      if (index != -1 && index != i)
         continue;

      /* draw alarm limits */
      if (lower_limit[i] != -12345) {
         if (logaxis) {
            if (lower_limit[i] <= 0)
               ys = y1;
            else
               ys = (int) (y1 -
                           (log(lower_limit[i]) - log(ymin)) / (log(ymax) -
                                                                log(ymin)) * (y1 - y2) +
                           0.5);
         } else
            ys = (int) (y1 - (lower_limit[i] - ymin) / (ymax - ymin) * (y1 - y2) + 0.5);

         gdImageDashedLine(im, x1, ys, x2, ys, curve_col[i]);

         poly[0].x = x1;
         poly[0].y = ys;
         poly[1].x = x1 + 5;
         poly[1].y = ys;
         poly[2].x = x1;
         poly[2].y = ys - 5;

         gdImageFilledPolygon(im, poly, 3, curve_col[i]);
      }
      if (upper_limit[i] != -12345) {
         if (logaxis) {
            if (upper_limit[i] <= 0)
               ys = y1;
            else
               ys = (int) (y1 -
                           (log(upper_limit[i]) - log(ymin)) / (log(ymax) -
                                                                log(ymin)) * (y1 - y2) +
                           0.5);
         } else
            ys = (int) (y1 - (upper_limit[i] - ymin) / (ymax - ymin) * (y1 - y2) + 0.5);

         gdImageDashedLine(im, x1, ys, x2, ys, curve_col[i]);

         poly[0].x = x1;
         poly[0].y = ys;
         poly[1].x = x1 + 5;
         poly[1].y = ys;
         poly[2].x = x1;
         poly[2].y = ys + 5;

         gdImageFilledPolygon(im, poly, 3, curve_col[i]);
      }

      for (j = 0; j < (int) n_point[i]; j++) {
         xs = (int) ((x[i][j] / 3600.0 - xmin) / (xmax - xmin) * (x2 - x1) + x1 + 0.5);

         if (logaxis) {
            if (y[i][j] <= 0)
               ys = y1;
            else
               ys = (int) (y1 -
                           (log(y[i][j]) - log(ymin)) / (log(ymax) - log(ymin)) * (y1 -
                                                                                   y2) +
                           0.5);
         } else
            ys = (int) (y1 - (y[i][j] - ymin) / (ymax - ymin) * (y1 - y2) + 0.5);

         if (j > 0)
            gdImageLine(im, xold, yold, xs, ys, curve_col[i]);
         xold = xs;
         yold = ys;
      }

      if (n_point[i] > 0) {
         poly[0].x = xs;
         poly[0].y = ys;
         poly[1].x = xs + 12;
         poly[1].y = ys - 6;
         poly[2].x = xs + 12;
         poly[2].y = ys + 6;

         gdImageFilledPolygon(im, poly, 3, curve_col[i]);
      }
   }

   if (labels) {
      for (i = 0; i < n_vars; i++) {
         if (index != -1 && index != i)
            continue;

         if (factor[i] != 1) {
            if (offset[i] == 0)
               sprintf(str, "%s * %1.2lG", strchr(tag_name[i], ':') + 1, factor[i]);
            else
               sprintf(str, "%s * %1.2lG %c %1.5lG", strchr(tag_name[i], ':') + 1,
                       factor[i], offset[i] < 0 ? '-' : '+', fabs(offset[i]));
         } else {
            if (offset[i] == 0)
               sprintf(str, "%s", strchr(tag_name[i], ':') + 1);
            else
               sprintf(str, "%s %c %1.5lG", strchr(tag_name[i], ':') + 1,
                       offset[i] < 0 ? '-' : '+', fabs(offset[i]));
         }

         row = index == -1 ? i : 0;

         gdImageFilledRectangle(im,
                                x1 + 10,
                                y2 + 10 + row * (gdFontMediumBold->h + 10),
                                x1 + 10 + strlen(str) * gdFontMediumBold->w + 10,
                                y2 + 10 + row * (gdFontMediumBold->h + 10) +
                                gdFontMediumBold->h + 2 + 2, white);
         gdImageRectangle(im, x1 + 10, y2 + 10 + row * (gdFontMediumBold->h + 10),
                          x1 + 10 + strlen(str) * gdFontMediumBold->w + 10,
                          y2 + 10 + row * (gdFontMediumBold->h + 10) +
                          gdFontMediumBold->h + 2 + 2, curve_col[i]);

         gdImageString(im, gdFontMediumBold,
                       x1 + 10 + 5,
                       y2 + 10 + 2 + row * (gdFontMediumBold->h + 10), str, curve_col[i]);
      }
   }

   gdImageRectangle(im, x1, y2, x2, y1, black);

 error:

   /* generate GIF */
   gdImageInterlace(im, 1);
   gdImageGif(im, &gb);
   gdImageDestroy(im);
   length = gb.size;

   if (buffer == NULL) {
      rsprintf("HTTP/1.0 200 Document follows\r\n");
      rsprintf("Server: MIDAS HTTP %s\r\n", cm_get_version());

      rsprintf("Content-Type: image/gif\r\n");
      rsprintf("Content-Length: %d\r\n", length);
      rsprintf("Pragma: no-cache\r\n");
      rsprintf("Expires: Fri, 01-Jan-1983 00:00:00 GMT\r\n\r\n");

      if (length > (int) (sizeof(return_buffer) - strlen(return_buffer))) {
         printf("return buffer too small\n");
         return;
      }

      return_length = strlen(return_buffer) + length;
      memcpy(return_buffer + strlen(return_buffer), gb.data, length);
   } else {
      if (length > *buffer_size) {
         printf("return buffer too small\n");
         return;
      }

      memcpy(buffer, gb.data, length);
      *buffer_size = length;
   }
}

/*------------------------------------------------------------------*/

void show_query_page(char *path)
{
   int i;
   time_t ltime_start, ltime_end;
   HNDLE hDB;
   char str[256];
   time_t now;
   struct tm *ptms, tms;

   if (*getparam("m1")) {
      memset(&tms, 0, sizeof(struct tm));
      tms.tm_year = atoi(getparam("y1")) % 100;

      strcpy(str, getparam("m1"));
      for (i = 0; i < 12; i++)
         if (equal_ustring(str, mname[i]))
            break;
      if (i == 12)
         i = 0;

      tms.tm_mon = i;
      tms.tm_mday = atoi(getparam("d1"));
      tms.tm_hour = 0;

      if (tms.tm_year < 90)
         tms.tm_year += 100;

      ltime_start = mktime(&tms);
      memset(&tms, 0, sizeof(struct tm));
      tms.tm_year = atoi(getparam("y2")) % 100;

      strcpy(str, getparam("m2"));
      for (i = 0; i < 12; i++)
         if (equal_ustring(str, mname[i]))
            break;
      if (i == 12)
         i = 0;

      tms.tm_mon = i;
      tms.tm_mday = atoi(getparam("d2"));
      tms.tm_hour = 0;

      if (tms.tm_year < 90)
         tms.tm_year += 100;
      ltime_end = mktime(&tms);
      ltime_end += 3600 * 24;

      sprintf(str, "HS/%s?scale=%d&offset=%d", path, (int) (ltime_end - ltime_start),
              MIN((int) (ltime_end - ss_time()), 0));
      redirect(str);
      return;
   }

   cm_get_experiment_database(&hDB, NULL);

   sprintf(str, "HS/%s", path);
   show_header(hDB, "History", "GET", str, 1, 0);

   /* menu buttons */
   rsprintf("<tr><td colspan=2 bgcolor=#C0C0C0>\n");
   rsprintf("<input type=submit name=cmd value=Query>\n");
   rsprintf("<input type=submit name=cmd value=History>\n");
   rsprintf("<input type=submit name=cmd value=Status></tr>\n");
   rsprintf("</tr>\n\n");

   time(&now);
   now -= 3600 * 24;
   ptms = localtime(&now);
   ptms->tm_year += 1900;

   rsprintf("<tr><td nowrap bgcolor=#CCCCFF>Start date:</td>", "Start date");

   rsprintf("<td bgcolor=#DDEEBB>Month: <select name=\"m1\">\n");
   rsprintf("<option value=\"\">\n");
   for (i = 0; i < 12; i++)
      if (i == ptms->tm_mon)
         rsprintf("<option selected value=\"%s\">%s\n", mname[i], mname[i]);
      else
         rsprintf("<option value=\"%s\">%s\n", mname[i], mname[i]);
   rsprintf("</select>\n");

   rsprintf("&nbsp;Day: <select name=\"d1\">");
   rsprintf("<option selected value=\"\">\n");
   for (i = 0; i < 31; i++)
      if (i + 1 == ptms->tm_mday)
         rsprintf("<option selected value=%d>%d\n", i + 1, i + 1);
      else
         rsprintf("<option value=%d>%d\n", i + 1, i + 1);
   rsprintf("</select>\n");

   rsprintf
       ("&nbsp;Year: <input type=\"text\" size=5 maxlength=5 name=\"y1\" value=\"%d\">",
        ptms->tm_year);
   rsprintf("</td></tr>\n");

   rsprintf("<tr><td nowrap bgcolor=#CCCCFF>End date:</td>");
   time(&now);
   ptms = localtime(&now);
   ptms->tm_year += 1900;

   rsprintf("<td bgcolor=#DDEEBB>Month: <select name=\"m2\">\n");
   rsprintf("<option value=\"\">\n");
   for (i = 0; i < 12; i++)
      if (i == ptms->tm_mon)
         rsprintf("<option selected value=\"%s\">%s\n", mname[i], mname[i]);
      else
         rsprintf("<option value=\"%s\">%s\n", mname[i], mname[i]);
   rsprintf("</select>\n");

   rsprintf("&nbsp;Day: <select name=\"d2\">");
   rsprintf("<option selected value=\"\">\n");
   for (i = 0; i < 31; i++)
      if (i + 1 == ptms->tm_mday)
         rsprintf("<option selected value=%d>%d\n", i + 1, i + 1);
      else
         rsprintf("<option value=%d>%d\n", i + 1, i + 1);
   rsprintf("</select>\n");

   rsprintf
       ("&nbsp;Year: <input type=\"text\" size=5 maxlength=5 name=\"y2\" value=\"%d\">",
        ptms->tm_year);
   rsprintf("</td></tr>\n");

   rsprintf("</table>\n");
   rsprintf("</body></html>\r\n");
}

/*------------------------------------------------------------------*/

void show_hist_config_page(char *path)
{
   int i, j, max_event_id, status, size, index, history, n_names;
   BOOL single_names, flag, is_link;
   HNDLE hDB, hKeyRoot, hKeyEq, hKeyVar, hKey, hKeyNames;
   KEY key, varkey;
   char str[256], eq_name[256], var_name[256], cmd[256], ref[256];
   char display_name[10][2 * NAME_LENGTH];
   float value;
   char *hist_col[] = { "#0000FF", "#00C000", "#FF0000", "#00C0C0", "#FF00FF",
      "#C0C000", "#808080", "#80FF80", "#FF8080", "#8080FF"
   };

   cm_get_experiment_database(&hDB, NULL);
   strcpy(cmd, getparam("cmd"));
   hKeyVar = 0;

   if (cmd[0] && equal_ustring(cmd, "save")) {
      /* copy parameters to their ODB location */
      for (index = 0; index < 10; index++) {
         sprintf(str, "event%d", index);
         if (*getparam(str)) {
            strcpy(var_name, getparam(str));
            strcat(var_name, ":");
            sprintf(str, "var%d", index);
            strcat(var_name, getparam(str));

            sprintf(str, "/History/Display/%s/Variables", path);
            db_find_key(hDB, 0, str, &hKeyVar);
            if (hKeyVar) {
               /* make sure size is correct */
               if (index == 0) {
                  db_delete_key(hDB, hKeyVar, FALSE);
                  db_create_key(hDB, 0, str, TID_STRING);
                  db_find_key(hDB, 0, str, &hKeyVar);
               }
               db_set_data_index(hDB, hKeyVar, var_name, 2 * NAME_LENGTH, index,
                                 TID_STRING);
            }

            sprintf(str, "/History/Display/%s/Factor", path);
            db_find_key(hDB, 0, str, &hKey);
            sprintf(str, "fac%d", index);
            value = (float) atof(getparam(str));
            if (hKey)
               db_set_data_index(hDB, hKey, &value, sizeof(float), index, TID_FLOAT);

            sprintf(str, "/History/Display/%s/Offset", path);
            db_find_key(hDB, 0, str, &hKey);
            sprintf(str, "ofs%d", index);
            value = (float) atof(getparam(str));
            if (hKey)
               db_set_data_index(hDB, hKey, &value, sizeof(float), index, TID_FLOAT);
         } else {
            sprintf(str, "/History/Display/%s/Variables", path);
            db_find_key(hDB, 0, str, &hKey);
            db_set_num_values(hDB, hKey, index);
            sprintf(str, "/History/Display/%s/Factor", path);
            db_find_key(hDB, 0, str, &hKey);
            db_set_num_values(hDB, hKey, index);
            sprintf(str, "/History/Display/%s/Offset", path);
            db_find_key(hDB, 0, str, &hKey);
            db_set_num_values(hDB, hKey, index);
            break;
         }
      }

      if (*getparam("timescale")) {
         sprintf(ref, "/History/Display/%s/Timescale", path);
         strcpy(str, getparam("timescale"));
         db_set_value(hDB, 0, ref, str, NAME_LENGTH, 1, TID_STRING);
      }

      sprintf(ref, "/History/Display/%s/Zero ylow", path);
      flag = *getparam("zero_ylow");
      db_set_value(hDB, 0, ref, &flag, sizeof(flag), 1, TID_BOOL);

      sprintf(ref, "/History/Display/%s/Log Axis", path);
      flag = *getparam("log_axis");
      db_set_value(hDB, 0, ref, &flag, sizeof(flag), 1, TID_BOOL);

      sprintf(ref, "/History/Display/%s/Show run markers", path);
      flag = *getparam("run_markers");
      db_set_value(hDB, 0, ref, &flag, sizeof(flag), 1, TID_BOOL);

      sprintf(str, "HS/%s", path);
      redirect(str);
      return;
   }

   sprintf(str, "HS/%s", path);
   show_header(hDB, "History Config", "GET", str, 3, 0);

   /* menu buttons */
   rsprintf("<tr><td colspan=6 bgcolor=\"#C0C0C0\">\n");
   rsprintf("<input type=submit name=cmd value=Save>\n");
   rsprintf("<input type=submit name=cmd value=Cancel>\n");
   rsprintf("<input type=submit name=cmd value=Refresh>\n");
   rsprintf("<input type=submit name=cmd value=\"Delete Panel\">\n");
   rsprintf("</td></tr>\n");

   rsprintf("<tr><td colspan=6 bgcolor=\"#FFFF00\" align=center><b>Panel \"%s\"</b>\n",
            path);

   /* hidden command for refresh */
   rsprintf("<input type=hidden name=cmd value=Refresh></td></tr>\n");

   /* time scale */
   if (equal_ustring(cmd, "refresh"))
      strcpy(str, getparam("timescale"));
   else {
      sprintf(ref, "/History/Display/%s/Timescale", path);
      size = NAME_LENGTH;
      db_get_value(hDB, 0, ref, str, &size, TID_STRING, TRUE);
   }
   rsprintf
       ("<tr><td bgcolor=\"#E0E0E0\" colspan=6>Time scale: &nbsp;&nbsp;<input type=text name=timescale value=%s></td></tr>\n",
        str);

   /* ylow_zero */
   if (equal_ustring(cmd, "refresh"))
      flag = *getparam("zero_ylow");
   else {
      sprintf(ref, "/History/Display/%s/Zero ylow", path);
      size = sizeof(flag);
      db_get_value(hDB, 0, ref, &flag, &size, TID_BOOL, TRUE);
   }
   if (flag)
      rsprintf
          ("<tr><td bgcolor=\"#E0E0E0\" colspan=6><input type=checkbox checked name=zero_ylow value=1>",
           str);
   else
      rsprintf
          ("<tr><td bgcolor=\"#E0E0E0\" colspan=6><input type=checkbox name=zero_ylow value=1>",
           str);
   rsprintf("&nbsp;&nbsp;Zero Ylow</td></tr>\n");

   /* log_axis */
   if (equal_ustring(cmd, "refresh"))
      flag = *getparam("log_axis");
   else {
      sprintf(ref, "/History/Display/%s/Log axis", path);
      size = sizeof(flag);
      db_get_value(hDB, 0, ref, &flag, &size, TID_BOOL, TRUE);
   }
   if (flag)
      rsprintf
          ("<tr><td bgcolor=\"#E0E0E0\" colspan=6><input type=checkbox checked name=log_axis value=1>",
           str);
   else
      rsprintf
          ("<tr><td bgcolor=\"#E0E0E0\" colspan=6><input type=checkbox name=log_axis value=1>",
           str);
   rsprintf("&nbsp;&nbsp;Logarighmic Y axis</td></tr>\n");

   /* run_markers */
   if (equal_ustring(cmd, "refresh"))
      flag = *getparam("run_markers");
   else {
      sprintf(ref, "/History/Display/%s/Show run markers", path);
      size = sizeof(flag);
      db_get_value(hDB, 0, ref, &flag, &size, TID_BOOL, TRUE);
   }
   if (flag)
      rsprintf
          ("<tr><td bgcolor=\"#E0E0E0\" colspan=6><input type=checkbox checked name=run_markers value=1>",
           str);
   else
      rsprintf
          ("<tr><td bgcolor=\"#E0E0E0\" colspan=6><input type=checkbox name=run_markers value=1>",
           str);
   rsprintf("&nbsp;&nbsp;Show run markers</td></tr>\n");

  /*---- events and variables ----*/

   /* get display event name */

   if (equal_ustring(cmd, "refresh")) {
      /* from parameters in a refresh */
      for (i = 0; i < 10; i++) {
         sprintf(str, "event%d", i);
         strcpy(display_name[i], getparam(str));

         if (display_name[i][0]) {
            sprintf(str, "var%d", i);
            strcat(display_name[i], ":");
            strcat(display_name[i], getparam(str));
         }
      }
   } else {
      /* from ODB else */
      sprintf(str, "/History/Display/%s/Variables", path);
      db_find_key(hDB, 0, str, &hKey);

      memset(display_name, 0, sizeof(display_name));
      if (hKey) {
         db_get_key(hDB, hKey, &key);

         for (i = 0; i < key.num_values; i++) {
            size = 2 * NAME_LENGTH;
            db_get_data_index(hDB, hKey, display_name[i], &size, i, TID_STRING);
         }
      }
   }

   rsprintf("<tr><th>Col<th>Event<th colspan=2>Variable<th>Factor<th>Offset</tr>\n");

   for (index = 0; index < 10; index++) {
      rsprintf("<tr><td bgcolor=\"%s\">&nbsp;<td>\n", hist_col[index]);

      rsprintf("<select name=\"event%d\" size=1 onChange=\"document.form1.submit()\">\n",
               index);

      /* enumerate events */
      max_event_id = 0;

      status = db_find_key(hDB, 0, "/Equipment", &hKeyRoot);
      if (status != DB_SUCCESS) {
         show_error("Cannot find /Equipment entry in database");
         return;
      }

      /* empty option */
      rsprintf("<option value=\"\">&lt;empty&gt;\n");

      /* loop over equipment to display event name */
      for (i = 0;; i++) {
         status = db_enum_key(hDB, hKeyRoot, i, &hKeyEq);
         if (status != DB_SUCCESS)
            break;

         /* check history flag */
         size = sizeof(history);
         db_get_value(hDB, hKeyEq, "Common/Log history", &history, &size, TID_INT, TRUE);

         /* show event only if log history flag is on */
         if (history > 0) {
            /* get equipment name */
            db_get_key(hDB, hKeyEq, &key);

            strcpy(str, display_name[index]);
            str[strlen(key.name)] = 0;
            if (equal_ustring(str, key.name))
               rsprintf("<option selected value=\"%s\">%s\n", key.name, key.name);
            else
               rsprintf("<option value=\"%s\">%s\n", key.name, key.name);
         }
      }

      /* loop over history links to displayh event name */
      status = db_find_key(hDB, 0, "/History/Links", &hKeyRoot);
      if (status == DB_SUCCESS) {
         for (i = 0;; i++) {
            status = db_enum_link(hDB, hKeyRoot, i, &hKey);
            if (status == DB_NO_MORE_SUBKEYS)
               break;

            db_get_key(hDB, hKey, &key);

            if (strncmp(display_name[index], key.name, strlen(key.name)) == 0)
               rsprintf("<option selected value=\"%s\">%s\n", key.name, key.name);
            else
               rsprintf("<option value=\"%s\">%s\n", key.name, key.name);
         }
      }

      rsprintf("</select></td>\n");

      /* display variables for selected event */

      status = db_find_key(hDB, 0, "/Equipment", &hKeyRoot);
      if (status == DB_SUCCESS && display_name[index][0]) {
         strcpy(eq_name, display_name[index]);
         if (strchr(eq_name, ':'))
            *strchr(eq_name, ':') = 0;

         is_link = FALSE;
         db_find_key(hDB, hKeyRoot, eq_name, &hKeyEq);
         if (!hKeyEq) {
            sprintf(str, "/History/Links/%s", eq_name);
            status = db_find_link(hDB, 0, str, &hKeyVar);
            if (status != DB_SUCCESS) {
               sprintf(str, "Cannot find /Equipment/%s or /History/Links/%s in ODB",
                       eq_name, eq_name);
               show_error(str);
               return;
            } else
               is_link = TRUE;
         }

         rsprintf("<td colspan=2><select name=\"var%d\">\n", index);

         /* go through variables for selected event */
         if (!is_link) {
            sprintf(str, "/Equipment/%s/Variables", eq_name);
            status = db_find_key(hDB, 0, str, &hKeyVar);
            if (status != DB_SUCCESS) {
               sprintf(str, "Cannot find /Equipment/%s/Variables in ODB", eq_name);
               show_error(str);
               return;
            }
         }

         for (i = 0;; i++) {
            status = db_enum_link(hDB, hKeyVar, i, &hKey);
            if (status == DB_NO_MORE_SUBKEYS)
               break;

            if (is_link) {
               db_get_key(hDB, hKey, &varkey);

               if (strchr(display_name[index], ':'))
                  strcpy(str, strchr(display_name[index], ':') + 1);
               else
                  str[0] = 0;

               if (equal_ustring(str, varkey.name))
                  rsprintf("<option selected value=\"%s\">%s\n", varkey.name,
                           varkey.name);
               else
                  rsprintf("<option value=\"%s\">%s\n", varkey.name, varkey.name);
            } else {
               /* get variable key */
               db_get_key(hDB, hKey, &varkey);
               n_names = 0;

               /* look for names */
               db_find_key(hDB, hKeyEq, "Settings/Names", &hKeyNames);
               single_names = (hKeyNames > 0);
               if (hKeyNames) {
                  /* get variables from names list */
                  db_get_key(hDB, hKeyNames, &key);
                  n_names = key.num_values;
               } else {
                  sprintf(str, "Settings/Names %s", varkey.name);
                  db_find_key(hDB, hKeyEq, str, &hKeyNames);
                  if (hKeyNames) {
                     /* get variables from names list */
                     db_get_key(hDB, hKeyNames, &key);
                     n_names = key.num_values;
                  }
               }

               if (hKeyNames) {
                  /* loop over array elements */
                  for (j = 0; j < n_names; j++) {
                     /* get name #j */
                     size = NAME_LENGTH;
                     db_get_data_index(hDB, hKeyNames, var_name, &size, j, TID_STRING);

                     /* append variable key name for single name array */
                     if (single_names) {
                        strcat(var_name, " ");
                        strcat(var_name, varkey.name);
                     }

                     /* get name from ODB */
                     if (strchr(display_name[index], ':'))
                        strcpy(str, strchr(display_name[index], ':') + 1);
                     else
                        str[0] = 0;

                     if (equal_ustring(str, var_name))
                        rsprintf("<option selected value=\"%s\">%s\n", var_name,
                                 var_name);
                     else
                        rsprintf("<option value=\"%s\">%s\n", var_name, var_name);
                  }
               } else {
                  if (strchr(display_name[index], ':'))
                     strcpy(str, strchr(display_name[index], ':') + 1);
                  else
                     str[0] = 0;

                  if (varkey.num_values > 0) {
                     for (j = 0; j < varkey.num_values; j++) {
                        sprintf(var_name, "%s[%d]", varkey.name, j);

                        if (equal_ustring(str, var_name))
                           rsprintf("<option selected value=\"%s\">%s\n", var_name,
                                    var_name);
                        else
                           rsprintf("<option value=\"%s\">%s\n", var_name, var_name);
                     }
                  } else {
                     if (equal_ustring(str, var_name))
                        rsprintf("<option selected value=\"%s\">%s\n", varkey.name,
                                 varkey.name);
                     else
                        rsprintf("<option value=\"%s\">%s\n", varkey.name, varkey.name);
                  }
               }
            }
         }

         rsprintf("</select></td>\n");
      } else
         rsprintf("<td colspan=2></td>\n");

      if (equal_ustring(cmd, "refresh")) {
         /* get value from parameters */
         sprintf(str, "fac%d", index);
         value = (float) atof(getparam(str));
      } else {
         /* get value from ODB */
         value = 1;
         sprintf(str, "/History/Display/%s/Factor", path);
         db_find_key(hDB, 0, str, &hKey);
         size = sizeof(float);
         if (hKey) {
            db_get_key(hDB, hKey, &key);
            if (index < key.num_values)
               db_get_data_index(hDB, hKey, &value, &size, index, TID_FLOAT);
         }
      }
      rsprintf
          ("<td><input type=text size=10 maxlength=10 name=\"fac%d\" value=%g></td>\n",
           index, value);

      if (equal_ustring(cmd, "refresh")) {
         /* get value from parameters */
         sprintf(str, "ofs%d", index);
         value = (float) atof(getparam(str));
      } else {
         /* get value from ODB */
         value = 0;
         sprintf(str, "/History/Display/%s/Offset", path);
         db_find_key(hDB, 0, str, &hKey);
         size = sizeof(float);
         if (hKey) {
            db_get_key(hDB, hKey, &key);
            if (index < key.num_values)
               db_get_data_index(hDB, hKey, &value, &size, index, TID_FLOAT);
         }
      }
      rsprintf
          ("<td><input type=text size=10 maxlength=10 name=\"ofs%d\" value=%g></td>\n",
           index, value);

      rsprintf("</tr>\n");
   }

   rsprintf("</table></form>\n");
   rsprintf("</body></html>\r\n");
}

/*------------------------------------------------------------------*/

void show_hist_page(char *path, char *buffer, int *buffer_size, int refresh)
{
   char str[256], ref[256], ref2[256], paramstr[256], scalestr[256], hgroup[256];
   char *poffset, *pscale, *pmag, *pindex;
   HNDLE hDB, hkey, hikeyp, hkeyp, hkeybutton;
   KEY key, ikey;
   int i, j, k, scale, offset, index, width, size, status, labels;
   float factor[2];
   char def_button[][NAME_LENGTH] = { "10m", "1h", "3h", "12h", "24h", "3d", "7d" };

   cm_get_experiment_database(&hDB, NULL);

   if (equal_ustring(getparam("cmd"), "Query")) {
      show_query_page(path);
      return;
   }

   if (equal_ustring(getparam("cmd"), "Config") ||
       equal_ustring(getparam("cmd"), "Save") ||
       equal_ustring(getparam("cmd"), "Refresh")) {
      show_hist_config_page(path);
      /*
         sprintf(str, "History/Display/%s", path);
         redirect(str);
       */

      return;
   }

   if (equal_ustring(getparam("cmd"), "New")) {
      sprintf(str, "HS/%s", path);
      show_header(hDB, "History", "GET", str, 1, 0);

      rsprintf("<tr><td align=center bgcolor=\"#FFFF00\" colspan=2>\n");
      rsprintf("Select group: &nbsp;&nbsp;");
      rsprintf("<select name=\"group\">\n");

      /* list existing groups */
      db_find_key(hDB, 0, "/History/Display", &hkey);
      if (hkey) {
         for (i = 0;; i++) {
            db_enum_link(hDB, hkey, i, &hkeyp);

            if (!hkeyp)
               break;

            db_get_key(hDB, hkeyp, &key);
            rsprintf("<option>%s</option>\n", key.name);
         }
      }
      if (!hkey || i == 0)
         rsprintf("<option>Default</option>\n");
      rsprintf("</select><p>\n");

      rsprintf("Or enter new group name: &nbsp;&nbsp;");
      rsprintf("<input type=text size=15 maxlength=31 name=new_group>\n");

      rsprintf("<tr><td align=center bgcolor=\"#FFFF00\" colspan=2>\n");
      rsprintf("<br>Panel name: &nbsp;&nbsp;");
      rsprintf("<input type=text size=15 maxlength=31 name=panel><br><br>\n");
      rsprintf("</td></tr>\n");

      rsprintf("<tr><td align=center colspan=2>");
      rsprintf("<input type=submit value=Submit>\n");
      rsprintf("</td></tr>\n");

      rsprintf("</table>\r\n");
      rsprintf("</body></html>\r\n");
      return;
   }

   if (equal_ustring(getparam("cmd"), "Delete Panel")) {
      sprintf(str, "/History/Display/%s", path);
      if (db_find_key(hDB, 0, str, &hkey))
         db_delete_key(hDB, hkey, FALSE);

      redirect("HS/");
      return;
   }

   if (*getparam("panel")) {
      strcpy(path, getparam("panel"));
      strcpy(hgroup, getparam("group"));
      /* use new group if present */
      if (isparam("new_group") && *getparam("new_group"))
         strcpy(hgroup, getparam("new_group"));

      /* create new panel */
      sprintf(str, "/History/Display/%s/%s", hgroup, path);
      db_create_key(hDB, 0, str, TID_KEY);
      db_find_key(hDB, 0, str, &hkey);
      db_set_value(hDB, hkey, "Variables", "", NAME_LENGTH * 2, 1, TID_STRING);

      factor[0] = 1;
      db_set_value(hDB, hkey, "Factor", factor, 1 * sizeof(float), 1, TID_FLOAT);
      factor[0] = 0;
      db_set_value(hDB, hkey, "Offset", factor, 1 * sizeof(float), 1, TID_FLOAT);
      db_set_value(hDB, hkey, "Timescale", "1h", NAME_LENGTH, 1, TID_STRING);
      i = 1;
      db_set_value(hDB, hkey, "Zero ylow", &i, sizeof(BOOL), 1, TID_BOOL);
      db_set_value(hDB, hkey, "Show run markers", &i, sizeof(BOOL), 1, TID_BOOL);
      i = 0;
      db_set_value(hDB, hkey, "Log axis", &i, sizeof(BOOL), 1, TID_BOOL);

      /* configure that panel */
      sprintf(str, "%s/%s", hgroup, path);
      show_hist_config_page(str);
      return;
   }

   if (equal_ustring(getparam("cmd"), "Create ELog")) {
      sprintf(str, "\\HS\\%s.gif", path);
      if (getparam("hscale") && *getparam("hscale"))
         sprintf(str + strlen(str), "?scale=%s", getparam("hscale"));
      if (getparam("hoffset") && *getparam("hoffset")) {
         if (strchr(str, '?'))
            strcat(str, "&");
         else
            strcat(str, "?");
         sprintf(str + strlen(str), "offset=%s", getparam("hoffset"));
      }
      if (getparam("hwidth") && *getparam("hwidth")) {
         if (strchr(str, '?'))
            strcat(str, "&");
         else
            strcat(str, "?");
         sprintf(str + strlen(str), "width=%s", getparam("hwidth"));
      }
      if (getparam("hindex") && *getparam("hindex")) {
         if (strchr(str, '?'))
            strcat(str, "&");
         else
            strcat(str, "?");
         sprintf(str + strlen(str), "index=%s", getparam("hindex"));
      }

      show_elog_new(NULL, FALSE, str);
      return;
   }

   pscale = getparam("scale");
   if (pscale == NULL || *pscale == 0)
      pscale = getparam("hscale");
   poffset = getparam("offset");
   if (poffset == NULL || *poffset == 0)
      poffset = getparam("hoffset");
   pmag = getparam("width");
   if (pmag == NULL || *pmag == 0)
      pmag = getparam("hwidth");
   pindex = getparam("index");
   if (pindex == NULL || *pindex == 0)
      pindex = getparam("hindex");

   labels = 1;
   if (*getparam("labels") && atoi(getparam("labels")) == 0)
      labels = 0;

   /* evaluate scale and offset */

   if (poffset && *poffset)
      offset = time_to_sec(poffset);
   else
      offset = 0;

   if (pscale && *pscale)
      scale = time_to_sec(pscale);
   else
      scale = 0;

   if (strstr(path, ".gif")) {
      index = -1;
      if (pindex && *pindex)
         index = atoi(pindex);

      if (equal_ustring(pmag, "Large"))
         generate_hist_graph(path, buffer, buffer_size, 1024, 768, scale, offset, index,
                             labels);
      else if (equal_ustring(pmag, "Small"))
         generate_hist_graph(path, buffer, buffer_size, 320, 200, scale, offset, index,
                             labels);
      else if (atoi(pmag) > 0)
         generate_hist_graph(path, buffer, buffer_size, atoi(pmag),
                             (int) (atoi(pmag) * 0.625), scale, offset, index, labels);
      else
         generate_hist_graph(path, buffer, buffer_size, 640, 400, scale, offset, index,
                             labels);

      return;
   }

   /* evaluate offset shift */
   if (equal_ustring(getparam("shift"), "<"))
      offset -= scale / 2;

   if (equal_ustring(getparam("shift"), ">")) {
      offset += scale / 2;
      if (offset > 0)
         offset = 0;
   }
   if (equal_ustring(getparam("shift"), ">>"))
      offset = 0;

   if (equal_ustring(getparam("shift"), " + ")) {
      offset -= scale / 4;
      scale /= 2;
   }

   if (equal_ustring(getparam("shift"), " - ")) {
      offset += scale / 2;
      if (offset > 0)
         offset = 0;
      scale *= 2;
   }

   sprintf(str, "HS/%s", path);
   show_header(hDB, "History", "GET", str, 1, offset == 0 ? refresh : 0);

   /* menu buttons */
   rsprintf("<tr><td colspan=2 bgcolor=\"#C0C0C0\">\n");
   rsprintf("<input type=submit name=cmd value=ODB>\n");
   rsprintf("<input type=submit name=cmd value=Alarms>\n");
   rsprintf("<input type=submit name=cmd value=Status>\n");

   /* define hidden field for parameters */
   if (pscale && *pscale)
      rsprintf("<input type=hidden name=hscale value=%d>\n", scale);
   else {
      /* if no scale and offset given, get it from default */
      if (path[0] && !equal_ustring(path, "All")) {
         sprintf(str, "/History/Display/%s/Timescale", path);

         strcpy(scalestr, "1h");
         size = NAME_LENGTH;
         status = db_get_value(hDB, 0, str, scalestr, &size, TID_STRING, TRUE);
         if (status != DB_SUCCESS) {
            /* delete old integer key */
            db_find_key(hDB, 0, str, &hkey);
            if (hkey)
               db_delete_key(hDB, hkey, FALSE);

            strcpy(scalestr, "1h");
            size = NAME_LENGTH;
            db_get_value(hDB, 0, str, scalestr, &size, TID_STRING, TRUE);
         }

         rsprintf("<input type=hidden name=hscale value=%s>\n", scalestr);
         scale = time_to_sec(scalestr);
      }
   }

   if (offset != 0)
      rsprintf("<input type=hidden name=hoffset value=%d>\n", offset);
   if (pmag && *pmag)
      rsprintf("<input type=hidden name=hwidth value=%s>\n", pmag);
   if (pindex && *pindex)
      rsprintf("<input type=hidden name=hindex value=%s>\n", pindex);

   rsprintf("</td></tr>\n");

   /* links for history panels */
   rsprintf("<tr><td colspan=2 bgcolor=\"#FFFF00\">\n");
   if (!path[0])
      rsprintf("<b>Please select panel:</b><br>\n");

   /* table for panel selection */
   rsprintf("<table border=1 cellpadding=3 style='text-align: left;'>");

   /* "All" link */
   rsprintf("<tr><td colspan=2>\n");
   if (equal_ustring(path, "All"))
      rsprintf("<b>All</b> &nbsp;&nbsp;");
   else {
      if (exp_name[0])
         rsprintf("<a href=\"/HS/All?exp=%s\">ALL</a> &nbsp;&nbsp;\n", exp_name);
      else
         rsprintf("<a href=\"/HS/All\">ALL</a>\n");
   }
   rsprintf("</td></tr>\n");

   /* Setup History table links */
   db_find_key(hDB, 0, "/History/Display", &hkey);
   if (!hkey) {
      /* create default panel */
      strcpy(str, "System:Trigger per sec.");
      strcpy(str + 2 * NAME_LENGTH, "System:Trigger kB per sec.");
      db_set_value(hDB, 0, "/History/Display/Default/Trigger rate/Variables",
                   str, NAME_LENGTH * 4, 2, TID_STRING);
      strcpy(str, "1h");
      db_set_value(hDB, 0, "/History/Display/Default/Trigger rate/Time Scale",
                   str, NAME_LENGTH, 1, TID_STRING);

      factor[0] = 1;
      factor[1] = 1;
      db_set_value(hDB, 0, "/History/Display/Default/Trigger rate/Factor",
                   factor, 2 * sizeof(float), 2, TID_FLOAT);
      factor[0] = 0;
      factor[1] = 0;
      db_set_value(hDB, 0, "/History/Display/Default/Trigger rate/Offset",
                   factor, 2 * sizeof(float), 2, TID_FLOAT);
      strcpy(str, "1h");
      db_set_value(hDB, 0, "/History/Display/Default/Trigger rate/Timescale",
                   str, NAME_LENGTH, 1, TID_STRING);
      i = 1;
      db_set_value(hDB, 0, "/History/Display/Default/Trigger rate/Zero ylow",
                   &i, sizeof(BOOL), 1, TID_BOOL);
      i = 1;
      db_set_value(hDB, 0, "/History/Display/Default/Trigger rate/Show run markers",
                   &i, sizeof(BOOL), 1, TID_BOOL);
   }

   db_find_key(hDB, 0, "/History/Display", &hkey);
   if (hkey) {
      for (i = 0;; i++) {
         db_enum_link(hDB, hkey, i, &hkeyp);

         if (!hkeyp)
            break;

         // Group key
         db_get_key(hDB, hkeyp, &key);
         rsprintf("<tr><td><b>%s</b></td>\n<td>", key.name);

         for (j = 0;; j++) {
            // scan items 
            db_enum_link(hDB, hkeyp, j, &hikeyp);

            if (!hikeyp) {
               rsprintf("</tr>");
               break;
            }
            // Item key
            db_get_key(hDB, hikeyp, &ikey);

            if (strchr(path, '/'))
               strcpy(str, strchr(path, '/') + 1);
            else
               strcpy(str, path);

            if (equal_ustring(str, ikey.name))
               rsprintf("<small><b>%s</b></small> &nbsp;", ikey.name);
            else {
               if (exp_name[0])
                  rsprintf("<small><a href=\"/HS/%s/%s?exp=%s\">%s</a></small> &nbsp;\n",
                           key.name, ikey.name, exp_name, ikey.name);
               else
                  rsprintf("<small><a href=\"/HS/%s/%s\">%s</a></small> &nbsp;\n",
                           key.name, ikey.name, ikey.name);
            }
         }
      }
   }

   /* "New" button */
   rsprintf("<tr><td colspan=2><input type=submit name=cmd value=New></td></tr>\n");

   rsprintf("</table></tr>\n");

   /* image panel */
   if (path[0] && !equal_ustring(path, "All")) {
      /* navigation links */
      rsprintf("<tr><td bgcolor=\"#A0FFA0\">\n");

      sprintf(str, "/History/Display/%s/Buttons", path);
      db_find_key(hDB, 0, str, &hkeybutton);
      if (hkeybutton == 0) {
         /* create default buttons */
         db_create_key(hDB, 0, str, TID_STRING);
         db_find_key(hDB, 0, str, &hkeybutton);
         db_set_data(hDB, hkeybutton, def_button, sizeof(def_button), 7, TID_STRING);
      }

      db_get_key(hDB, hkeybutton, &key);

      for (i = 0; i < key.num_values; i++) {
         size = sizeof(str);
         db_get_data_index(hDB, hkeybutton, str, &size, i, TID_STRING);
         rsprintf("<input type=submit name=scale value=%s>\n", str);
      }

      rsprintf("<input type=submit name=shift value=\"<\">\n");
      rsprintf("<input type=submit name=shift value=\" + \">\n");
      rsprintf("<input type=submit name=shift value=\" - \">\n");
      if (offset != 0) {
         rsprintf("<input type=submit name=shift value=\">\">\n");
         rsprintf("<input type=submit name=shift value=\">>\">\n");
      }

      rsprintf("<td bgcolor=\"#A0FFA0\">\n");
      rsprintf("<input type=submit name=width value=Large>\n");
      rsprintf("<input type=submit name=width value=Small>\n");
      rsprintf("<input type=submit name=cmd value=\"Create ELog\">\n");
      rsprintf("<input type=submit name=cmd value=Config>\n");
      rsprintf("<input type=submit name=cmd value=Query>\n");

      rsprintf("</tr>\n");

      paramstr[0] = 0;
      sprintf(paramstr + strlen(paramstr), "&scale=%d", scale);
      if (offset != 0)
         sprintf(paramstr + strlen(paramstr), "&offset=%d", offset);
      if (pmag && *pmag)
         sprintf(paramstr + strlen(paramstr), "&width=%s", pmag);

      /* define image map */
      rsprintf("<map name=\"%s\">\r\n", path);

      if (!(pindex && *pindex)) {
         sprintf(str, "/History/Display/%s/Variables", path);
         db_find_key(hDB, 0, str, &hkey);
         if (hkey) {
            db_get_key(hDB, hkey, &key);

            for (i = 0; i < key.num_values; i++) {
               if (paramstr[0]) {
                  if (exp_name[0])
                     sprintf(ref, "/HS/%s?exp=%s%s&index=%d", path, exp_name, paramstr,
                             i);
                  else
                     sprintf(ref, "/HS/%s?%s&index=%d", path, paramstr, i);
               } else {
                  if (exp_name[0])
                     sprintf(ref, "/HS/%s?exp=%s&index=%d", path, exp_name, i);
                  else
                     sprintf(ref, "/HS/%s?index=%d", path, i);
               }

               rsprintf("  <area shape=rect coords=\"%d,%d,%d,%d\" href=\"%s\">\r\n",
                        30, 31 + 23 * i, 150, 30 + 23 * i + 17, ref);
            }
         }
      } else {
         if (paramstr[0]) {
            if (exp_name[0])
               sprintf(ref, "/HS/%s?exp=%s%s", path, exp_name, paramstr);
            else
               sprintf(ref, "/HS/%s?%s", path, paramstr);
         } else {
            if (exp_name[0])
               sprintf(ref, "/HS/%s?exp=%s", path, exp_name);
            else
               sprintf(ref, "/HS/%s", path);
         }

         if (equal_ustring(pmag, "Large"))
            width = 1024;
         else if (equal_ustring(pmag, "Small"))
            width = 320;
         else if (atoi(pmag) > 0)
            width = atoi(pmag);
         else
            width = 640;

         rsprintf("  <area shape=rect coords=\"%d,%d,%d,%d\" href=\"%s\">\r\n",
                  0, 0, width, 20, ref);
      }

      rsprintf("</map>\r\n");

      /* Display individual panels */
      if (pindex && *pindex)
         sprintf(paramstr + strlen(paramstr), "&index=%s", pindex);
      if (paramstr[0]) {
         if (exp_name[0])
            sprintf(ref, "/HS/%s.gif?exp=%s%s", path, exp_name, paramstr);
         else
            sprintf(ref, "/HS/%s.gif?%s", path, paramstr);
      } else {
         if (exp_name[0])
            sprintf(ref, "/HS/%s.gif?exp=%s", path, exp_name);
         else
            sprintf(ref, "/HS/%s.gif", path);
      }

      /* put reference to graph */
      rsprintf("<tr><td colspan=2><img src=\"%s\" alt=\"%s.gif\" usemap=\"#%s\"></tr>\n",
               ref, path, path);
   }

   if (equal_ustring(path, "All")) {
      /* Display all panels */
      db_find_key(hDB, 0, "/History/Display", &hkey);
      if (hkey)
         for (i = 0, k = 0;; i++) {     // scan Groups
            db_enum_link(hDB, hkey, i, &hkeyp);

            if (!hkeyp)
               break;

            db_get_key(hDB, hkeyp, &key);

            for (j = 0;; j++, k++) {
               // scan items 
               db_enum_link(hDB, hkeyp, j, &hikeyp);

               if (!hikeyp)
                  break;

               db_get_key(hDB, hikeyp, &ikey);
               if (exp_name[0]) {
                  sprintf(ref, "/HS/%s/%s.gif?exp=%s&width=Small", key.name, ikey.name,
                          exp_name);
                  sprintf(ref2, "/HS/%s/%s?exp=%s", key.name, ikey.name, exp_name);
               } else {
                  sprintf(ref, "/HS/%s/%s.gif?width=Small", key.name, ikey.name);
                  sprintf(ref2, "/HS/%s/%s", key.name, ikey.name);
               }

               if (k % 2 == 0)
                  rsprintf("<tr><td><a href=\"%s\"><img src=\"%s\" alt=\"%s.gif\"></a>\n",
                           ref2, ref, key.name);
               else
                  rsprintf
                      ("<td><a href=\"%s\"><img src=\"%s\" alt=\"%s.gif\"></a></tr>\n",
                       ref2, ref, key.name);
            }                   // items loop
         }                      // Groups loop
   }                            // All
   rsprintf("</table>\r\n");
   rsprintf("</body></html>\r\n");
}


/*------------------------------------------------------------------*/

void get_password(char *password)
{
   static char last_password[32];

   if (strncmp(password, "set=", 4) == 0)
      strcpy(last_password, password + 4);
   else
      strcpy(password, last_password);
}

/*------------------------------------------------------------------*/

void interprete(char *cookie_pwd, char *cookie_wpwd, char *path, int refresh)
/********************************************************************\

  Routine: interprete

  Purpose: Interprete parametersand generate HTML output from odb.

  Input:
    char *cookie_pwd        Cookie containing encrypted password
    char *path              ODB path "/dir/subdir/key"

  <implicit>
    _param/_value array accessible via getparam()

\********************************************************************/
{
   int i, j, n, status, size, run_state, index;
   WORD event_id;
   HNDLE hkey, hsubkey, hDB, hconn;
   KEY key;
   char str[256], *p;
   char enc_path[256], dec_path[256], eq_name[NAME_LENGTH], fe_name[NAME_LENGTH];
   char data[TEXT_SIZE];
   char *experiment, *password, *wpassword, *command, *value, *group;
   char exp_list[MAX_EXPERIMENT][NAME_LENGTH];
   time_t now;
   struct tm *gmt;

   if (equal_ustring(path, "favicon.ico"))
      return;

   /* encode path for further usage */
   strlcpy(dec_path, path, sizeof(dec_path));
   urlDecode(dec_path);
   // ##urlDecode(dec_path); /* necessary for %2520 -> %20 -> ' ' */
   strcpy(enc_path, dec_path);
   urlEncode(enc_path);

   experiment = getparam("exp");
   password = getparam("pwd");
   wpassword = getparam("wpwd");
   command = getparam("cmd");
   value = getparam("value");
   group = getparam("group");
   index = atoi(getparam("index"));

   /*---- experiment connect ----------------------------------------*/

   /* disconnect from previous experiment if current is different */
   if (connected && !equal_ustring(exp_name, experiment)) {
      /* check if only one experiment defined */
      cm_list_experiments("", exp_list);
      if (exp_list[1][0]) {
         cm_disconnect_experiment();
         connected = FALSE;
      }
   }

   if (!connected) {
      /* connect to experiment */
      if (experiment[0] == 0) {
         cm_list_experiments("", exp_list);
         if (exp_list[1][0]) {
            /* present list of experiments to choose from */
            show_experiment_page(exp_list);
            return;
         }
      }

      if (password)
         sprintf(str, "set=%s", password);
      else
         sprintf(str, "set=");

      get_password(str);
      status = cm_connect_experiment("", experiment, "mhttpd", get_password);
      if (status == CM_WRONG_PASSWORD) {
         show_password_page(password, experiment);
         return;
      }

      if (status != CM_SUCCESS) {
         rsprintf("<H1>Experiment \"%s\" not defined on this host</H1>", experiment);
         return;
      }

      /* remember experiment */
      strcpy(exp_name, experiment);
      connected = TRUE;

      /* place a request for system messages */
      cm_msg_register(receive_message);

      /* redirect message display, turn on message logging */
      cm_set_msg_print(MT_ALL, MT_ALL, print_message);

      /* retrieve last message */
      size = 1000;
      cm_msg_retrieve(1, data, &size);

      receive_message(0, 0, NULL, data+25);
   }

   cm_get_experiment_database(&hDB, NULL);

   /* check for password */
   db_find_key(hDB, 0, "/Experiment/Security/Password", &hkey);
   if (!password[0] && hkey) {
      size = sizeof(str);
      db_get_data(hDB, hkey, str, &size, TID_STRING);

      /* check for excemption */
      db_find_key(hDB, 0, "/Experiment/Security/Allowed programs/mhttpd", &hkey);
      if (hkey == 0 && strcmp(cookie_pwd, str) != 0) {
         show_password_page("", experiment);
         return;
      }
   }

   /* get run state */
   run_state = STATE_STOPPED;
   size = sizeof(run_state);
   db_get_value(hDB, 0, "/Runinfo/State", &run_state, &size, TID_INT, TRUE);

   /*---- redirect with cookie if password given --------------------*/

   if (password[0]) {
      rsprintf("HTTP/1.0 302 Found\r\n");
      rsprintf("Server: MIDAS HTTP %s\r\n", cm_get_version());

      time(&now);
      now += 3600 * 24;
      gmt = gmtime(&now);
      strftime(str, sizeof(str), "%A, %d-%b-%Y %H:00:00 GMT", gmt);

      rsprintf("Set-Cookie: midas_pwd=%s; path=/; expires=%s\r\n",
               ss_crypt(password, "mi"), str);

      if (exp_name[0])
         rsprintf("Location: /?exp=%s\n\n<html>redir</html>\r\n", exp_name);
      else
         rsprintf("Location: /\n\n<html>redir</html>\r\n");
      return;
   }

   if (wpassword[0]) {
      /* check if password correct */
      if (!check_web_password(ss_crypt(wpassword, "mi"), getparam("redir"), experiment))
         return;

      rsprintf("HTTP/1.0 302 Found\r\n");
      rsprintf("Server: MIDAS HTTP %s\r\n", cm_get_version());

      time(&now);
      now += 3600 * 24;
      gmt = gmtime(&now);
      strftime(str, sizeof(str), "%A, %d-%b-%Y %H:%M:%S GMT", gmt);

      rsprintf("Set-Cookie: midas_wpwd=%s; path=/; expires=%s\r\n",
               ss_crypt(wpassword, "mi"), str);

      sprintf(str, "/%s", getparam("redir"));
      if (exp_name[0]) {
         if (strchr(str, '?'))
            rsprintf("Location: %s&exp=%s\n\n<html>redir</html>\r\n", str, exp_name);
         else
            rsprintf("Location: %s?exp=%s\n\n<html>redir</html>\r\n", str, exp_name);
      } else
         rsprintf("Location: %s\n\n<html>redir</html>\r\n", str);
      return;
   }

  /*---- redirect if ODB command -----------------------------------*/

   if (equal_ustring(command, "ODB")) {
      redirect("root");
      return;
   }

  /*---- redirect if SC command ------------------------------------*/

   if (equal_ustring(command, "SC")) {
      redirect("SC/");
      return;
   }

  /*---- redirect if status command --------------------------------*/

   if (equal_ustring(command, "status")) {
      redirect("");
      return;
   }

  /*---- script command --------------------------------------------*/

   if (getparam("script") && *getparam("script")) {
      sprintf(str, "%s?script=%s", path, getparam("script"));
      if (!check_web_password(cookie_wpwd, str, experiment))
         return;

      sprintf(str, "/Script/%s", getparam("script"));

      db_find_key(hDB, 0, str, &hkey);

      if (hkey) {
         /* for NT: close reply socket before starting subprocess */
         redirect2("");
         exec_script(hkey);
      } else
         redirect("");

      return;
   }

  /*---- alarms command --------------------------------------------*/

   if (equal_ustring(command, "alarms")) {
      show_alarm_page();
      return;
   }

  /*---- history command -------------------------------------------*/

   if (equal_ustring(command, "history")) {
      redirect("HS/");
      return;
   }

   if (strncmp(path, "HS/", 3) == 0) {
      if (equal_ustring(command, "config")) {
         sprintf(str, "%s?cmd=%s", path, command);
         if (!check_web_password(cookie_wpwd, str, experiment))
            return;
      }

      show_hist_page(dec_path + 3, NULL, NULL, refresh);
      return;
   }

  /*---- help command ----------------------------------------------*/

   if (equal_ustring(command, "help")) {
      show_help_page();
      return;
   }

  /*---- pause run -------------------------------------------*/

   if (equal_ustring(command, "pause")) {
      if (run_state != STATE_RUNNING) {
         show_error("Run is not running");
         return;
      }

      if (!check_web_password(cookie_wpwd, "?cmd=pause", experiment))
         return;

      status = cm_transition(TR_PAUSE, 0, str, sizeof(str), SYNC, FALSE);
      if (status != CM_SUCCESS && status != CM_DEFERRED_TRANSITION)
         show_error(str);
      else
         redirect("");

      return;
   }

  /*---- resume run ------------------------------------------*/

   if (equal_ustring(command, "resume")) {
      if (run_state != STATE_PAUSED) {
         show_error("Run is not paused");
         return;
      }

      if (!check_web_password(cookie_wpwd, "?cmd=resume", experiment))
         return;

      status = cm_transition(TR_RESUME, 0, str, sizeof(str), SYNC, FALSE);
      if (status != CM_SUCCESS && status != CM_DEFERRED_TRANSITION)
         show_error(str);
      else
         redirect("");

      return;
   }

  /*---- start dialog --------------------------------------------*/

   if (equal_ustring(command, "start")) {
      if (run_state == STATE_RUNNING) {
         show_error("Run is already started");
         return;
      }

      if (value[0] == 0) {
         if (!check_web_password(cookie_wpwd, "?cmd=start", experiment))
            return;
         show_start_page();
      } else {
         /* set run parameters */
         db_find_key(hDB, 0, "/Experiment/Edit on start", &hkey);
         if (hkey) {
            for (i = 0, n = 0;; i++) {
               db_enum_key(hDB, hkey, i, &hsubkey);

               if (!hsubkey)
                  break;

               db_get_key(hDB, hsubkey, &key);

               for (j = 0; j < key.num_values; j++) {
                  size = key.item_size;
                  sprintf(str, "x%d", n++);
                  db_sscanf(getparam(str), data, &size, j, key.type);
                  db_set_data_index(hDB, hsubkey, data, size + 1, j, key.type);
               }
            }
         }

         i = atoi(value);
         if (i <= 0) {
            cm_msg(MERROR, "interprete", "Start run: invalid run number %d", i);
            sprintf(str, "Invalid run number %d", i);
            show_error(str);
            return;
         }

         status = cm_transition(TR_START, i, str, sizeof(str), SYNC, FALSE);
         if (status != CM_SUCCESS && status != CM_DEFERRED_TRANSITION)
            show_error(str);
         else
            redirect("");
      }
      return;
   }

  /*---- stop run --------------------------------------------*/

   if (equal_ustring(command, "stop")) {
      if (run_state != STATE_RUNNING) {
         show_error("Run is not running");
         return;
      }

      if (!check_web_password(cookie_wpwd, "?cmd=stop", experiment))
         return;

      status = cm_transition(TR_STOP, 0, str, sizeof(str), SYNC, FALSE);
      if (status != CM_SUCCESS && status != CM_DEFERRED_TRANSITION)
         show_error(str);
      else
         redirect("");

      return;
   }

  /*---- trigger equipment readout ---------------------------*/

   if (strncmp(command, "Trigger", 7) == 0) {
      sprintf(str, "?cmd=%s", command);
      if (!check_web_password(cookie_wpwd, str, experiment))
         return;

      /* extract equipment name */
      strcpy(eq_name, command + 8);
      if (strchr(eq_name, ' '))
         *strchr(eq_name, ' ') = 0;

      /* get frontend name */
      sprintf(str, "/Equipment/%s/Common/Frontend name", eq_name);
      size = NAME_LENGTH;
      db_get_value(hDB, 0, str, fe_name, &size, TID_STRING, TRUE);

      /* and ID */
      sprintf(str, "/Equipment/%s/Common/Event ID", eq_name);
      size = sizeof(event_id);
      db_get_value(hDB, 0, str, &event_id, &size, TID_WORD, TRUE);

      if (cm_exist(fe_name, FALSE) != CM_SUCCESS) {
         sprintf(str, "Frontend \"%s\" not running!", fe_name);
         show_error(str);
      } else {
         status = cm_connect_client(fe_name, &hconn);
         if (status != RPC_SUCCESS) {
            sprintf(str, "Cannot connect to frontend \"%s\" !", fe_name);
            show_error(str);
         } else {
            status = rpc_client_call(hconn, RPC_MANUAL_TRIG, event_id);
            if (status != CM_SUCCESS)
               show_error("Error triggering event");
            else
               redirect("");

            cm_disconnect_client(hconn, FALSE);
         }
      }

      return;
   }

  /*---- cancel command --------------------------------------------*/

   if (equal_ustring(command, "cancel")) {
      /* strip variable name from path */
      db_find_key(hDB, 0, dec_path, &hkey);
      if (hkey) {
         db_get_key(hDB, hkey, &key);

         if (key.type != TID_KEY) {
            p = enc_path + strlen(enc_path) - 1;
            while (*p && *p != '/')
               *p-- = 0;
            if (*p == '/')
               *p = 0;
         }
      }

      if (group[0]) {
         /* extract equipment name */
         eq_name[0] = 0;
         if (strncmp(enc_path, "Equipment/", 10) == 0) {
            strcpy(eq_name, enc_path + 10);
            if (strchr(eq_name, '/'))
               *strchr(eq_name, '/') = 0;
         }

         /* back to SC display */
         sprintf(str, "SC/%s/%s", eq_name, group);
         redirect(str);
      } else
         redirect(enc_path);

      return;
   }

  /*---- set command -----------------------------------------------*/

   if (equal_ustring(command, "set") &&
       strncmp(path, "SC/", 3) != 0 && strncmp(path, "CS/", 3) != 0) {
      sprintf(str, "%s?cmd=set", enc_path);
      if (!check_web_password(cookie_wpwd, str, experiment))
         return;

      show_set_page(enc_path, dec_path, group, index, value);
      return;
   }

  /*---- find command ----------------------------------------------*/

   if (equal_ustring(command, "find")) {
      show_find_page(enc_path, value);
      return;
   }

  /*---- create command --------------------------------------------*/

   if (equal_ustring(command, "create")) {
      sprintf(str, "%s?cmd=create", enc_path);
      if (!check_web_password(cookie_wpwd, str, experiment))
         return;

      show_create_page(enc_path, dec_path, value, index, atoi(getparam("type")));
      return;
   }

  /*---- CAMAC CNAF command ----------------------------------------*/

   if (equal_ustring(command, "CNAF") || strncmp(path, "CNAF", 4) == 0) {
      if (!check_web_password(cookie_wpwd, "?cmd=CNAF", experiment))
         return;

      show_cnaf_page();
      return;
   }

  /*---- alarms command --------------------------------------------*/

   if (equal_ustring(command, "reset all alarms")) {
      if (!check_web_password(cookie_wpwd, "?cmd=reset%20all%20alarms", experiment))
         return;

      al_reset_alarm(NULL);
      redirect("?cmd=alarms");
      return;
   }

   if (equal_ustring(command, "reset")) {
      if (!check_web_password(cookie_wpwd, "?cmd=reset%20all%20alarms", experiment))
         return;

      al_reset_alarm(dec_path);
      redirect("?cmd=alarms");
      return;
   }

   if (equal_ustring(command, "Alarms on/off")) {
      redirect("Alarms/Alarm system active?cmd=set");
      return;
   }

  /*---- programs command ------------------------------------------*/

   if (equal_ustring(command, "programs")) {
      str[0] = 0;
      if (*getparam("Start"))
         sprintf(str, "?cmd=programs&Start=%s", getparam("Start"));
      if (*getparam("Stop"))
         sprintf(str, "?cmd=programs&Stop=%s", getparam("Stop"));

      if (str[0])
         if (!check_web_password(cookie_wpwd, str, experiment))
            return;

      show_programs_page();
      return;
   }

  /*---- config command --------------------------------------------*/

   if (equal_ustring(command, "config")) {
      show_config_page(refresh);
      return;
   }

  /*---- Messages command ------------------------------------------*/

   if (equal_ustring(command, "messages")) {
      show_messages_page(refresh, 20);
      return;
   }

   if (strncmp(command, "More", 4) == 0 && strncmp(path, "EL/", 3) != 0) {
      i = atoi(command + 4);
      if (i == 0)
         i = 100;
      show_messages_page(0, i);
      return;
   }

  /*---- ELog command ----------------------------------------------*/

   if (equal_ustring(command, "elog")) {
      redirect("EL/");
      return;
   }

   if (strncmp(path, "EL/", 3) == 0) {
      if (equal_ustring(command, "new") ||
          equal_ustring(command, "edit") || equal_ustring(command, "reply")) {
         sprintf(str, "%s?cmd=%s", path, command);
         if (!check_web_password(cookie_wpwd, str, experiment))
            return;
      }

      show_elog_page(dec_path + 3);
      return;
   }

   if (equal_ustring(command, "Create ELog from this page")) {
      show_elog_page(dec_path);
      return;
   }

  /*---- accept command --------------------------------------------*/

   if (equal_ustring(command, "accept")) {
      refresh = atoi(getparam("refr"));

      /* redirect with cookie */
      rsprintf("HTTP/1.0 302 Found\r\n");
      rsprintf("Server: MIDAS HTTP %s\r\n", cm_get_version());
      rsprintf("Content-Type: text/html; charset=iso-8859-1\r\n");

      time(&now);
      now += 3600 * 24 * 365;
      gmt = gmtime(&now);
      strftime(str, sizeof(str), "%A, %d-%b-%Y %H:00:00 GMT", gmt);

      rsprintf("Set-Cookie: midas_refr=%d; path=/; expires=%s\r\n", refresh, str);

      if (exp_name[0])
         rsprintf("Location: /?exp=%s\r\n\r\n<html>redir</html>\r\n", exp_name);
      else
         rsprintf("Location: /\r\n\r\n<html>redir</html>\r\n");

      return;
   }

  /*---- delete command --------------------------------------------*/

   if (equal_ustring(command, "delete")) {
      sprintf(str, "%s?cmd=delete", enc_path);
      if (!check_web_password(cookie_wpwd, str, experiment))
         return;

      show_delete_page(enc_path, dec_path, value, index);
      return;
   }

  /*---- slow control display --------------------------------------*/

   if (strncmp(path, "SC/", 3) == 0) {
      if (equal_ustring(command, "edit")) {
         sprintf(str, "%s?cmd=Edit&index=%d", path, index);
         if (!check_web_password(cookie_wpwd, str, experiment))
            return;
      }

      show_sc_page(dec_path + 3, refresh);
      return;
   }

  /*---- custom page -----------------------------------------------*/

   if (strncmp(path, "CS/", 3) == 0) {
      if (equal_ustring(command, "edit")) {
         sprintf(str, "%s?cmd=Edit&index=%d", path, index);
         if (!check_web_password(cookie_wpwd, str, experiment))
            return;
      }

      show_custom_page(dec_path + 3);
      return;
   }

   if (db_find_key(hDB, 0, "/Custom/Status", &hkey) == DB_SUCCESS && path[0] == 0) {
      if (equal_ustring(command, "edit")) {
         sprintf(str, "%s?cmd=Edit&index=%d", path, index);
         if (!check_web_password(cookie_wpwd, str, experiment))
            return;
      }

      show_custom_page("Status");
      return;
   }

  /*---- show status -----------------------------------------------*/

   if (path[0] == 0) {
      if (elog_mode) {
         redirect("EL/");
         return;
      }

      show_status_page(refresh, cookie_wpwd);
      return;
   }

  /*---- show ODB --------------------------------------------------*/

   if (path[0]) {
      show_odb_page(enc_path, dec_path);
      return;
   }
}

/*------------------------------------------------------------------*/

void decode_get(char *string, char *cookie_pwd, char *cookie_wpwd, int refresh)
{
   char path[256];
   char *p, *pitem;

   initparam();

   strncpy(path, string + 1, sizeof(path));     /* strip leading '/' */
   path[255] = 0;
   if (strchr(path, '?'))
      *strchr(path, '?') = 0;
   setparam("path", path);

   if (strchr(string, '?')) {
      p = strchr(string, '?') + 1;

      /* cut trailing "/" from netscape */
      if (p[strlen(p) - 1] == '/')
         p[strlen(p) - 1] = 0;

      p = strtok(p, "&");
      while (p != NULL) {
         pitem = p;
         p = strchr(p, '=');
         if (p != NULL) {
            *p++ = 0;
            urlDecode(pitem);
            urlDecode(p);

            setparam(pitem, p);

            p = strtok(NULL, "&");
         }
      }
   }

   interprete(cookie_pwd, cookie_wpwd, path, refresh);
}

/*------------------------------------------------------------------*/

void decode_post(char *header, char *string, char *boundary, int length,
                 char *cookie_pwd, char *cookie_wpwd, int refresh)
{
   char *pinit, *p, *pitem, *ptmp, file_name[256], str[256], path[256];
   int n;

   initparam();

   strlcpy(path, header + 1, sizeof(path));     /* strip leading '/' */
   path[255] = 0;
   if (strchr(path, '?'))
      *strchr(path, '?') = 0;
   if (strchr(path, ' '))
      *strchr(path, ' ') = 0;
   setparam("path", path);

   _attachment_size[0] = _attachment_size[1] = _attachment_size[2] = 0;
   pinit = string;

   /* return if no boundary defined */
   if (!boundary[0])
      return;

   if (strstr(string, boundary))
      string = strstr(string, boundary) + strlen(boundary);

   do {
      if (strstr(string, "name=")) {
         pitem = strstr(string, "name=") + 5;
         if (*pitem == '\"')
            pitem++;

         if (strncmp(pitem, "attfile", 7) == 0) {
            n = pitem[7] - '1';

            /* evaluate file attachment */
            if (strstr(pitem, "filename=")) {
               p = strstr(pitem, "filename=") + 9;
               if (*p == '\"')
                  p++;
               if (strstr(p, "\r\n\r\n"))
                  string = strstr(p, "\r\n\r\n") + 4;
               else if (strstr(p, "\r\r\n\r\r\n"))
                  string = strstr(p, "\r\r\n\r\r\n") + 6;
               if (strchr(p, '\"'))
                  *strchr(p, '\"') = 0;

               /* set attachment filename */
               strlcpy(file_name, p, sizeof(file_name));
               sprintf(str, "attachment%d", n);
               setparam(str, file_name);
            } else
               file_name[0] = 0;

            /* find next boundary */
            ptmp = string;
            do {
               while (*ptmp != '-')
                  ptmp++;

               if ((p = strstr(ptmp, boundary)) != NULL) {
                  while (*p == '-')
                     p--;
                  if (*p == 10)
                     p--;
                  if (*p == 13)
                     p--;
                  p++;
                  break;
               } else
                  ptmp += strlen(ptmp);

            } while (TRUE);

            /* save pointer to file */
            if (file_name[0]) {
               _attachment_buffer[n] = string;
               _attachment_size[n] = (INT) p - (INT) string;
            }

            string = strstr(p, boundary) + strlen(boundary);
         } else {
            p = pitem;
            if (strstr(p, "\r\n\r\n"))
               p = strstr(p, "\r\n\r\n") + 4;
            else if (strstr(p, "\r\r\n\r\r\n"))
               p = strstr(p, "\r\r\n\r\r\n") + 6;

            if (strchr(pitem, '\"'))
               *strchr(pitem, '\"') = 0;

            if (strstr(p, boundary)) {
               string = strstr(p, boundary) + strlen(boundary);
               *strstr(p, boundary) = 0;
               ptmp = p + (strlen(p) - 1);
               while (*ptmp == '-' || *ptmp == '\n' || *ptmp == '\r')
                  *ptmp-- = 0;
            }
            setparam(pitem, p);
         }

         while (*string == '-' || *string == '\n' || *string == '\r')
            string++;
      }

   } while ((INT) string - (INT) pinit < length);

   interprete(cookie_pwd, cookie_wpwd, path, refresh);
}

/*------------------------------------------------------------------*/

BOOL _abort = FALSE;

void ctrlc_handler(int sig)
{
   _abort = TRUE;
}

/*------------------------------------------------------------------*/

char net_buffer[WEB_BUFFER_SIZE];

void server_loop(int daemon)
{
   int status, i, refresh, n_error;
   struct sockaddr_in bind_addr, acc_addr;
   char cookie_pwd[256], cookie_wpwd[256], boundary[256];
   int lsock, len, flag, content_length, header_length;
   struct hostent *phe;
   fd_set readfds;
   struct timeval timeout;
   INT last_time = 0;
/*
struct linger        ling;
*/

   /* establish Ctrl-C handler */
   ss_ctrlc_handler(ctrlc_handler);

#ifdef OS_WINNT
   {
      WSADATA WSAData;

      /* Start windows sockets */
      if (WSAStartup(MAKEWORD(1, 1), &WSAData) != 0)
         return;
   }
#endif

   /* create a new socket */
   lsock = socket(AF_INET, SOCK_STREAM, 0);

   if (lsock == -1) {
      printf("Cannot create socket\n");
      return;
   }

   /* bind local node name and port to socket */
   memset(&bind_addr, 0, sizeof(bind_addr));
   bind_addr.sin_family = AF_INET;
   bind_addr.sin_addr.s_addr = htonl(INADDR_ANY);
   bind_addr.sin_port = htons((short) tcp_port);

   /* try reusing address */
   flag = 1;
   setsockopt(lsock, SOL_SOCKET, SO_REUSEADDR, (char *) &flag, sizeof(INT));
   status = bind(lsock, (struct sockaddr *) &bind_addr, sizeof(bind_addr));

   if (status < 0) {
      printf
          ("Cannot bind to port %d.\nPlease try later or use the \"-p\" flag to specify a different port\n",
           tcp_port);
      return;
   }

   /* get host name for mail notification */
   gethostname(host_name, sizeof(host_name));

   phe = gethostbyname(host_name);
   if (phe != NULL)
      phe = gethostbyaddr(phe->h_addr, sizeof(int), AF_INET);

   /* if domain name is not in host name, hope to get it from phe */
   if (strchr(host_name, '.') == NULL)
      strcpy(host_name, phe->h_name);

#ifdef OS_UNIX
   /* give up root privilege */
   setuid(getuid());
   setgid(getgid());
#endif

   if (daemon) {
      printf("Becoming a daemon...\n");
      ss_daemon_init(FALSE);
   }

   /* listen for connection */
   status = listen(lsock, SOMAXCONN);
   if (status < 0) {
      printf("Cannot listen\n");
      return;
   }

   printf("Server listening...\n");
   do {
      FD_ZERO(&readfds);
      FD_SET(lsock, &readfds);

      timeout.tv_sec = 0;
      timeout.tv_usec = 100000;

#ifdef OS_UNIX
      do {
         status = select(FD_SETSIZE, (void *) &readfds, NULL, NULL, (void *) &timeout);
         /* if an alarm signal was cought, restart with reduced timeout */
      } while (status == -1 && errno == EINTR);
#else
      status = select(FD_SETSIZE, (void *) &readfds, NULL, NULL, (void *) &timeout);
#endif

      if (FD_ISSET(lsock, &readfds)) {
         len = sizeof(acc_addr);
         _sock = accept(lsock, (struct sockaddr *) &acc_addr, &len);

         last_time = (INT) ss_time();

         /* turn on lingering (borrowed from NCSA httpd code) */

         /* outcommented, gave occasional hangups on Linux
            ling.l_onoff = 1;
            ling.l_linger = 6;
            setsockopt(_sock, SOL_SOCKET, SO_LINGER, (char *) &ling, sizeof(ling));
          */

         /* save remote host address */
         memcpy(&remote_addr, &(acc_addr.sin_addr), sizeof(remote_addr));

         memset(net_buffer, 0, sizeof(net_buffer));
         len = 0;
         header_length = 0;
         content_length = 0;
         n_error = 0;
         do {
            FD_ZERO(&readfds);
            FD_SET(_sock, &readfds);

            timeout.tv_sec = 6;
            timeout.tv_usec = 0;

#ifdef OS_UNIX
            do {
               status =
                   select(FD_SETSIZE, (void *) &readfds, NULL, NULL, (void *) &timeout);
               /* if an alarm signal was cought, restart with reduced timeout */
            } while (status == -1 && errno == EINTR);
#else
            status = select(FD_SETSIZE, (void *) &readfds, NULL, NULL, (void *) &timeout);
#endif
            if (FD_ISSET(_sock, &readfds))
               i = recv(_sock, net_buffer + len, sizeof(net_buffer) - len, 0);
            else
               goto error;

            /* abort if connection got broken */
            if (i < 0)
               goto error;

            if (i > 0)
               len += i;

            /* check if net_buffer too small */
            if (len >= sizeof(net_buffer)) {
               /* drain incoming remaining data */
               do {
                  FD_ZERO(&readfds);
                  FD_SET(_sock, &readfds);

                  timeout.tv_sec = 2;
                  timeout.tv_usec = 0;

                  status =
                      select(FD_SETSIZE, (void *) &readfds, NULL, NULL,
                             (void *) &timeout);

                  if (FD_ISSET(_sock, &readfds))
                     i = recv(_sock, net_buffer, sizeof(net_buffer), 0);
                  else
                     break;
               } while (i);

               memset(return_buffer, 0, sizeof(return_buffer));
               strlen_retbuf = 0;
               return_length = 0;

               show_error
                   ("Submitted attachment too large, please increase WEB_BUFFER_SIZE in mhttpd.c and recompile");
               send(_sock, return_buffer, strlen_retbuf + 1, 0);
               if (verbose) {
                  printf("==== Return ================================\n");
                  puts(return_buffer);
                  printf("\n\n");
               }
               goto error;
            }

            if (i == 0) {
               n_error++;
               if (n_error == 100)
                  goto error;
            }

            /* finish when empty line received */
            if (strstr(net_buffer, "GET") != NULL && strstr(net_buffer, "POST") == NULL) {
               if (len > 4 && strcmp(&net_buffer[len - 4], "\r\n\r\n") == 0)
                  break;
               if (len > 6 && strcmp(&net_buffer[len - 6], "\r\r\n\r\r\n") == 0)
                  break;
            } else if (strstr(net_buffer, "POST") != NULL) {
               if (header_length == 0) {
                  /* extract header and content length */
                  if (strstr(net_buffer, "Content-Length:"))
                     content_length = atoi(strstr(net_buffer, "Content-Length:") + 15);
                  else if (strstr(net_buffer, "Content-length:"))
                     content_length = atoi(strstr(net_buffer, "Content-length:") + 15);

                  boundary[0] = 0;
                  if (strstr(net_buffer, "boundary=")) {
                     strncpy(boundary, strstr(net_buffer, "boundary=") + 9,
                             sizeof(boundary));
                     if (strchr(boundary, '\r'))
                        *strchr(boundary, '\r') = 0;
                  }

                  if (strstr(net_buffer, "\r\n\r\n"))
                     header_length =
                         (INT) strstr(net_buffer, "\r\n\r\n") - (INT) net_buffer + 4;

                  if (strstr(net_buffer, "\r\r\n\r\r\n"))
                     header_length =
                         (INT) strstr(net_buffer, "\r\r\n\r\r\n") - (INT) net_buffer + 6;

                  if (header_length)
                     net_buffer[header_length - 1] = 0;
               }

               if (header_length > 0 && len >= header_length + content_length)
                  break;
            } else if (strstr(net_buffer, "OPTIONS") != NULL)
               goto error;
            else {
               printf(net_buffer);
               goto error;
            }

         } while (1);

         if (!strchr(net_buffer, '\r'))
            goto error;

         /* extract cookies */
         cookie_pwd[0] = 0;
         cookie_wpwd[0] = 0;
         if (strstr(net_buffer, "midas_pwd=") != NULL) {
            strcpy(cookie_pwd, strstr(net_buffer, "midas_pwd=") + 10);
            cookie_pwd[strcspn(cookie_pwd, " ;\r\n")] = 0;
         }
         if (strstr(net_buffer, "midas_wpwd=") != NULL) {
            strcpy(cookie_wpwd, strstr(net_buffer, "midas_wpwd=") + 11);
            cookie_wpwd[strcspn(cookie_wpwd, " ;\r\n")] = 0;
         }

         refresh = 0;
         if (strstr(net_buffer, "midas_refr=") != NULL)
            refresh = atoi(strstr(net_buffer, "midas_refr=") + 11);
         else
            refresh = DEFAULT_REFRESH;

         memset(return_buffer, 0, sizeof(return_buffer));
         strlen_retbuf = 0;

         if (strncmp(net_buffer, "GET", 3) != 0 && strncmp(net_buffer, "POST", 4) != 0)
            goto error;

         return_length = 0;

         if (verbose)
            printf("\n%s\n", net_buffer);

         if (strncmp(net_buffer, "GET", 3) == 0) {
            /* extract path and commands */
            *strchr(net_buffer, '\r') = 0;

            if (!strstr(net_buffer, "HTTP"))
               goto error;
            *(strstr(net_buffer, "HTTP") - 1) = 0;

            /* decode command and return answer */
            decode_get(net_buffer + 4, cookie_pwd, cookie_wpwd, refresh);
         } else {
            decode_post(net_buffer + 5, net_buffer + header_length, boundary,
                        content_length, cookie_pwd, cookie_wpwd, refresh);
         }

         if (return_length != -1) {
            if (return_length == 0)
               return_length = strlen(return_buffer) + 1;

            if (verbose) {
               printf("==== Return ================================\n");
               puts(return_buffer);
               printf("\n\n");
            }

            i = send_tcp(_sock, return_buffer, return_length, 0);
            if (i != return_length)
               cm_msg(MERROR, "server_loope", "Only sent back %d out of %d bytes", i,
                      return_length);

          error:

            closesocket(_sock);
         }
      }

      /* re-establish ctrl-c handler */
      ss_ctrlc_handler(ctrlc_handler);

      /* check if disconnect from experiment */
      if ((INT) ss_time() - last_time > CONNECT_TIME && connected && !no_disconnect) {
         cm_disconnect_experiment();
         connected = FALSE;
      }

      /* check for shutdown message */
      if (connected) {
         status = cm_yield(0);
         if (status == RPC_SHUTDOWN) {
            cm_disconnect_experiment();
            connected = FALSE;
         }
      }

   } while (!_abort);

   if (connected)
      cm_disconnect_experiment();
}

/*------------------------------------------------------------------*/

int main(int argc, char *argv[])
{
   int i;
   int daemon = FALSE;

   setbuf(stdout, NULL);
   setbuf(stderr, NULL);
#ifdef SIGPIPE
  /* avoid getting killed by "Broken pipe" signals */
  signal(SIGPIPE,SIG_IGN);
#endif

   /* parse command line parameters */
   no_disconnect = FALSE;
   for (i = 1; i < argc; i++) {
      if (argv[i][0] == '-' && argv[i][1] == 'D')
         daemon = TRUE;
      else if (argv[i][0] == '-' && argv[i][1] == 'd')
         verbose = TRUE;
      else if (argv[i][0] == '-' && argv[i][1] == 'E')
         elog_mode = TRUE;
      else if (argv[i][0] == '-' && argv[i][1] == 'c')
         no_disconnect = TRUE;
      else if (argv[i][0] == '-') {
         if (i + 1 >= argc || argv[i + 1][0] == '-')
            goto usage;
         if (argv[i][1] == 'p')
            tcp_port = atoi(argv[++i]);
         else {
          usage:
            printf("usage: %s [-p port] [-d] [-D] [-c]\n\n", argv[0]);
            printf("       -d display HTTP communication\n");
            printf("       -D become a daemon\n");
            printf("       -E only display ELog system\n");
            printf("       -c don't disconnect from experiment\n");
            return 0;
         }
      }
   }

   server_loop(daemon);

   return 0;
}
