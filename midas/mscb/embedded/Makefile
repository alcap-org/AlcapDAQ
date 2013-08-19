#
# Simple makefile
#


CC = c51.exe
LL = bL51.exe

CFLAGS = BROWSE DEBUG OBJECTEXTEND "TABS(3)"
LFLAGS = "RAMSIZE (256)" "DISABLEWARNING (16)" "CODE (?PR?UPGRADE?MSCBMAIN (0X7000))"
IFLAGS = -kr -nut -i3 -l90
EXECS = HVR_200 SCS_210 SCS_300 SCS_310 SCS_400 SCS_520 SCS_600 SCS_700 SCS_800 SCS_900 SCS_910
SRCS = hvr_200.c scs_210.c scs_300.c scs_310.c scs_400.c scs_520.c scs_600.c scs_700.c scs_800.c scs_900.c scs_910.c scs_1000.c

all: $(EXECS) scs_1000.hex subm_250.hex subm_300.hex

$(EXECS): $(SRCS) mscbmain.c mscbutil.c mscb.h
	$(CC) $@.c "DEFINE($@)" $(CFLAGS) 
	$(CC) mscbmain.c "DEFINE($@)" $(CFLAGS) 
	$(CC) mscbutil.c "DEFINE($@)" $(CFLAGS) 
	$(LL) mscbmain.obj, mscbutil.obj, $@.obj TO $@ $(LFLAGS)
	oh51 $@

scs_1000.hex: mscbmain.c mscbutil.c lcd_menu.c mscb.h
	$(CC) mscbmain.c "DEFINE(SCS_1000)" $(CFLAGS) 
	$(CC) mscbutil.c "DEFINE(SCS_1000)" $(CFLAGS) 
	$(CC) lcd_menu.c "DEFINE(SCS_1000)" $(CFLAGS) 
	$(LL) mscbmain.obj, mscbutil.obj, lcd_menu.obj, scs_1000.obj TO SCS_1000 "RAMSIZE (256)" "DISABLEWARNING (16)" 
	oh51 subm_250

subm_250.hex: usb.c mscbutil.c subm_250.c usb.h mscb.h
	$(CC) subm_250.c "DEFINE(SUBM_250)" $(CFLAGS) 
	$(CC) usb.c "DEFINE(SUBM_250)" $(CFLAGS) 
	$(CC) mscbutil.c "DEFINE(SUBM_250)" $(CFLAGS) 
	$(LL) usb.obj, mscbutil.obj, subm_250.obj TO subm_250 "RAMSIZE (256)" "DISABLEWARNING (16)" 
	oh51 subm_250

subm_300.hex: subm_300.c mscbutil.c mscb.h
	$(CC) subm_300.c "DEFINE(SUBM_300)" $(CFLAGS) 
	$(CC) mscbutil.c "DEFINE(SUBM_300)" $(CFLAGS) 
	$(LL) subm_300.obj, mscbutil.obj TO subm_300 "RAMSIZE (256)" "DISABLEWARNING (16)" 
	oh51 subm_300

indent:
	indent $(IFLAGS) *.c

clean:
	rm -f *~ *.OBJ $(EXECS) *.hex
