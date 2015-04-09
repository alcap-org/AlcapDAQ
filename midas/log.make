cc -c -g -O0 -Wall -Wuninitialized -Iinclude  -I./mxml -Idrivers -Llinux64/lib -DINCLUDE_FTPLIB   -D_LARGEFILE64_SOURCE -DHAVE_MYSQL -DHAVE_ROOT -pthread -m64 -I/home/daq/AlcapDAQ/root/include/root -DOS_LINUX -fPIC -Wno-unused-function -o linux64/lib/midas.o src/midas.c
src/midas.c: In function ‘dbg_malloc’:
src/midas.c:919: warning: cast from pointer to integer of different size
src/midas.c: In function ‘dbg_free’:
src/midas.c:954: warning: cast from pointer to integer of different size
src/midas.c: In function ‘bm_add_event_request’:
src/midas.c:5749: warning: cast from pointer to integer of different size
src/midas.c: In function ‘rpc_server_connect’:
src/midas.c:8770: warning: pointer targets in passing argument 3 of ‘getsockname’ differ in signedness
/usr/include/sys/socket.h:119: note: expected ‘socklen_t * __restrict__’ but argument is of type ‘int *’
src/midas.c:8772: warning: pointer targets in passing argument 3 of ‘getsockname’ differ in signedness
/usr/include/sys/socket.h:119: note: expected ‘socklen_t * __restrict__’ but argument is of type ‘int *’
src/midas.c:8774: warning: pointer targets in passing argument 3 of ‘getsockname’ differ in signedness
/usr/include/sys/socket.h:119: note: expected ‘socklen_t * __restrict__’ but argument is of type ‘int *’
src/midas.c:8865: warning: pointer targets in passing argument 3 of ‘accept’ differ in signedness
/usr/include/sys/socket.h:214: note: expected ‘socklen_t * __restrict__’ but argument is of type ‘int *’
src/midas.c:8868: warning: pointer targets in passing argument 3 of ‘accept’ differ in signedness
/usr/include/sys/socket.h:214: note: expected ‘socklen_t * __restrict__’ but argument is of type ‘int *’
src/midas.c:8871: warning: pointer targets in passing argument 3 of ‘accept’ differ in signedness
/usr/include/sys/socket.h:214: note: expected ‘socklen_t * __restrict__’ but argument is of type ‘int *’
src/midas.c: In function ‘rpc_register_server’:
src/midas.c:10975: warning: pointer targets in passing argument 3 of ‘getsockname’ differ in signedness
/usr/include/sys/socket.h:119: note: expected ‘socklen_t * __restrict__’ but argument is of type ‘int *’
src/midas.c: In function ‘rpc_server_accept’:
src/midas.c:11655: warning: pointer targets in passing argument 3 of ‘accept’ differ in signedness
/usr/include/sys/socket.h:214: note: expected ‘socklen_t * __restrict__’ but argument is of type ‘int *’
src/midas.c:11664: warning: pointer targets in passing argument 3 of ‘getpeername’ differ in signedness
/usr/include/sys/socket.h:133: note: expected ‘socklen_t * __restrict__’ but argument is of type ‘int *’
src/midas.c: In function ‘rpc_client_accept’:
src/midas.c:11892: warning: pointer targets in passing argument 3 of ‘accept’ differ in signedness
/usr/include/sys/socket.h:214: note: expected ‘socklen_t * __restrict__’ but argument is of type ‘int *’
src/midas.c: In function ‘bk_delete’:
src/midas.c:12767: warning: cast from pointer to integer of different size
src/midas.c:12768: warning: cast from pointer to integer of different size
src/midas.c:12781: warning: cast from pointer to integer of different size
src/midas.c:12781: warning: cast from pointer to integer of different size
src/midas.c:12792: warning: cast from pointer to integer of different size
src/midas.c:12793: warning: cast from pointer to integer of different size
src/midas.c:12805: warning: cast from pointer to integer of different size
src/midas.c:12805: warning: cast from pointer to integer of different size
src/midas.c: In function ‘bk_close’:
src/midas.c:12834: warning: cast from pointer to integer of different size
src/midas.c:12834: warning: cast from pointer to integer of different size
src/midas.c:12846: warning: cast from pointer to integer of different size
src/midas.c:12846: warning: cast from pointer to integer of different size
src/midas.c: In function ‘bk_locate’:
src/midas.c:12941: warning: cast from pointer to integer of different size
src/midas.c:12941: warning: cast from pointer to integer of different size
src/midas.c:12954: warning: cast from pointer to integer of different size
src/midas.c:12954: warning: cast from pointer to integer of different size
src/midas.c: In function ‘bk_find’:
src/midas.c:12995: warning: cast from pointer to integer of different size
src/midas.c:12995: warning: cast from pointer to integer of different size
src/midas.c:13011: warning: cast from pointer to integer of different size
src/midas.c:13011: warning: cast from pointer to integer of different size
src/midas.c: In function ‘bk_iterate’:
src/midas.c:13064: warning: cast from pointer to integer of different size
src/midas.c:13064: warning: cast from pointer to integer of different size
src/midas.c: In function ‘bk_iterate32’:
src/midas.c:13105: warning: cast from pointer to integer of different size
src/midas.c:13105: warning: cast from pointer to integer of different size
src/midas.c: In function ‘hs_gen_index’:
src/midas.c:13358: warning: format ‘%ld’ expects type ‘long int’, but argument 3 has type ‘DWORD’
src/midas.c: In function ‘hs_dump’:
src/midas.c:14835: warning: format ‘%li’ expects type ‘long int’, but argument 2 has type ‘DWORD’
src/midas.c: In function ‘hs_fdump’:
src/midas.c:14970: warning: format ‘%ld’ expects type ‘long int’, but argument 3 has type ‘DWORD’
src/midas.c:14977: warning: format ‘%li’ expects type ‘long int’, but argument 3 has type ‘DWORD’
src/midas.c:14983: warning: format ‘%ld’ expects type ‘long int’, but argument 2 has type ‘DWORD’
src/midas.c:14983: warning: format ‘%ld’ expects type ‘long int’, but argument 4 has type ‘DWORD’
src/midas.c: In function ‘al_evaluate_condition’:
src/midas.c:16045: warning: format ‘%ld’ expects type ‘long int’, but argument 3 has type ‘DWORD’
src/midas.c: In function ‘dm_area_send’:
src/midas.c:17521: warning: format ‘%li’ expects type ‘long int’, but argument 3 has type ‘BOOL’
src/midas.c:17521: warning: format ‘%li’ expects type ‘long int’, but argument 4 has type ‘BOOL’
cc -c -g -O0 -Wall -Wuninitialized -Iinclude  -I./mxml -Idrivers -Llinux64/lib -DINCLUDE_FTPLIB   -D_LARGEFILE64_SOURCE -DHAVE_MYSQL -DHAVE_ROOT -pthread -m64 -I/home/daq/AlcapDAQ/root/include/root -DOS_LINUX -fPIC -Wno-unused-function -o linux64/lib/system.o src/system.c
src/system.c: In function ‘ss_shell’:
src/system.c:1452: warning: missing sentinel in function call
src/system.c: In function ‘ss_suspend_init_ipc’:
src/system.c:3027: warning: pointer targets in passing argument 3 of ‘getsockname’ differ in signedness
/usr/include/sys/socket.h:119: note: expected ‘socklen_t * __restrict__’ but argument is of type ‘int *’
src/system.c: In function ‘ss_suspend’:
src/system.c:3493: warning: pointer targets in passing argument 6 of ‘recvfrom’ differ in signedness
/usr/include/sys/socket.h:166: note: expected ‘socklen_t * __restrict__’ but argument is of type ‘int *’
src/system.c:3520: warning: pointer targets in passing argument 6 of ‘recvfrom’ differ in signedness
/usr/include/sys/socket.h:166: note: expected ‘socklen_t * __restrict__’ but argument is of type ‘int *’
cc -c -g -O0 -Wall -Wuninitialized -Iinclude  -I./mxml -Idrivers -Llinux64/lib -DINCLUDE_FTPLIB   -D_LARGEFILE64_SOURCE -DHAVE_MYSQL -DHAVE_ROOT -pthread -m64 -I/home/daq/AlcapDAQ/root/include/root -DOS_LINUX -fPIC -Wno-unused-function -o linux64/lib/mrpc.o src/mrpc.c
cc -c -g -O0 -Wall -Wuninitialized -Iinclude  -I./mxml -Idrivers -Llinux64/lib -DINCLUDE_FTPLIB   -D_LARGEFILE64_SOURCE -DHAVE_MYSQL -DHAVE_ROOT -pthread -m64 -I/home/daq/AlcapDAQ/root/include/root -DOS_LINUX -fPIC -Wno-unused-function -o linux64/lib/odb.o src/odb.c
src/odb.c: In function ‘db_sprintf’:
src/odb.c:6330: warning: format ‘%lu’ expects type ‘long unsigned int’, but argument 3 has type ‘DWORD’
src/odb.c: In function ‘db_sprintfh’:
src/odb.c:6405: warning: format ‘%lX’ expects type ‘long unsigned int’, but argument 3 has type ‘DWORD’
src/odb.c: In function ‘db_sscanf’:
src/odb.c:6465: warning: format ‘%lx’ expects type ‘long unsigned int *’, but argument 3 has type ‘DWORD *’
src/odb.c:6493: warning: format ‘%lu’ expects type ‘long unsigned int *’, but argument 3 has type ‘DWORD *’
cc -c -g -O0 -Wall -Wuninitialized -Iinclude  -I./mxml -Idrivers -Llinux64/lib -DINCLUDE_FTPLIB   -D_LARGEFILE64_SOURCE -DHAVE_MYSQL -DHAVE_ROOT -pthread -m64 -I/home/daq/AlcapDAQ/root/include/root -DOS_LINUX -fPIC -Wno-unused-function -o linux64/lib/ybos.o src/ybos.c
src/ybos.c: In function ‘ybk_create_chaos’:
src/ybos.c:485: warning: cast to pointer from integer of different size
src/ybos.c: In function ‘ybk_close_chaos’:
src/ybos.c:525: warning: format ‘%ld’ expects type ‘long int’, but argument 2 has type ‘DWORD’
src/ybos.c: In function ‘ybk_close’:
src/ybos.c:553: warning: cast from pointer to integer of different size
src/ybos.c:557: warning: cast from pointer to integer of different size
src/ybos.c: In function ‘ybos_physrec_skip’:
src/ybos.c:2279: warning: format ‘%ld’ expects type ‘long int’, but argument 2 has type ‘DWORD’
src/ybos.c: In function ‘midas_event_skip’:
src/ybos.c:2314: warning: format ‘%ld’ expects type ‘long int’, but argument 2 has type ‘DWORD’
src/ybos.c: In function ‘yb_any_physrec_display’:
src/ybos.c:2355: warning: format ‘%8.8lx’ expects type ‘long unsigned int’, but argument 2 has type ‘DWORD’
src/ybos.c: In function ‘yb_any_all_info_display’:
src/ybos.c:2390: warning: format ‘%ld’ expects type ‘long int’, but argument 2 has type ‘DWORD’
src/ybos.c:2391: warning: format ‘%5ld’ expects type ‘long int’, but argument 2 has type ‘DWORD’
src/ybos.c:2391: warning: format ‘%5ld’ expects type ‘long int’, but argument 3 has type ‘DWORD’
src/ybos.c:2391: warning: format ‘%5ld’ expects type ‘long int’, but argument 4 has type ‘DWORD’
src/ybos.c:2391: warning: format ‘%5ld’ expects type ‘long int’, but argument 5 has type ‘DWORD’
src/ybos.c:2394: warning: format ‘%ld’ expects type ‘long int’, but argument 2 has type ‘DWORD’
src/ybos.c:2395: warning: format ‘%5ld’ expects type ‘long int’, but argument 2 has type ‘DWORD’
src/ybos.c:2395: warning: format ‘%5ld’ expects type ‘long int’, but argument 3 has type ‘DWORD’
src/ybos.c:2395: warning: format ‘%5ld’ expects type ‘long int’, but argument 4 has type ‘DWORD’
src/ybos.c:2395: warning: format ‘%5ld’ expects type ‘long int’, but argument 5 has type ‘DWORD’
src/ybos.c:2396: warning: format ‘%5ld’ expects type ‘long int’, but argument 2 has type ‘DWORD’
src/ybos.c:2396: warning: format ‘%lx’ expects type ‘long unsigned int’, but argument 3 has type ‘DWORD’
src/ybos.c:2396: warning: format ‘%5ld’ expects type ‘long int’, but argument 4 has type ‘DWORD’
src/ybos.c:2414: warning: format ‘%ld’ expects type ‘long int’, but argument 2 has type ‘DWORD’
src/ybos.c:2415: warning: format ‘%li’ expects type ‘long int’, but argument 2 has type ‘DWORD’
src/ybos.c:2415: warning: format ‘%5ld’ expects type ‘long int’, but argument 5 has type ‘DWORD’
src/ybos.c:2416: warning: format ‘%5ld’ expects type ‘long int’, but argument 2 has type ‘DWORD’
src/ybos.c:2416: warning: format ‘%lx’ expects type ‘long unsigned int’, but argument 3 has type ‘DWORD’
src/ybos.c:2416: warning: format ‘%5ld’ expects type ‘long int’, but argument 4 has type ‘DWORD’
src/ybos.c: In function ‘ybos_event_swap’:
src/ybos.c:2535: warning: format ‘%li’ expects type ‘long int’, but argument 2 has type ‘DWORD’
src/ybos.c: In function ‘ybos_event_get’:
src/ybos.c:2645: warning: format ‘%ld’ expects type ‘long int’, but argument 2 has type ‘DWORD’
src/ybos.c: In function ‘yb_any_raw_event_display’:
src/ybos.c:2808: warning: format ‘%6.0ld’ expects type ‘long int’, but argument 2 has type ‘DWORD’
src/ybos.c:2812: warning: format ‘%8.li’ expects type ‘long int’, but argument 2 has type ‘DWORD’
src/ybos.c:2814: warning: format ‘%8.8lx’ expects type ‘long unsigned int’, but argument 2 has type ‘DWORD’
src/ybos.c: In function ‘yb_any_bank_event_display’:
src/ybos.c:2864: warning: format ‘%li’ expects type ‘long int’, but argument 2 has type ‘DWORD’
src/ybos.c:2864: warning: format ‘%li’ expects type ‘long int’, but argument 3 has type ‘DWORD’
src/ybos.c:2869: warning: format ‘%li’ expects type ‘long int’, but argument 4 has type ‘DWORD’
src/ybos.c:2869: warning: format ‘%lx’ expects type ‘long unsigned int’, but argument 5 has type ‘DWORD’
src/ybos.c:2869: warning: format ‘%li’ expects type ‘long int’, but argument 6 has type ‘DWORD’
src/ybos.c:2869: warning: format ‘%lx’ expects type ‘long unsigned int’, but argument 7 has type ‘DWORD’
src/ybos.c:2889: warning: format ‘%li’ expects type ‘long int’, but argument 4 has type ‘DWORD’
src/ybos.c:2889: warning: format ‘%lx’ expects type ‘long unsigned int’, but argument 5 has type ‘DWORD’
src/ybos.c:2889: warning: format ‘%li’ expects type ‘long int’, but argument 6 has type ‘DWORD’
src/ybos.c:2889: warning: format ‘%lx’ expects type ‘long unsigned int’, but argument 7 has type ‘DWORD’
src/ybos.c: In function ‘yb_any_raw_bank_display’:
src/ybos.c:2979: warning: format ‘%4li’ expects type ‘long int’, but argument 2 has type ‘DWORD’
src/ybos.c:2983: warning: format ‘%8.li’ expects type ‘long int’, but argument 2 has type ‘DWORD’
src/ybos.c:2985: warning: format ‘%8.8lx’ expects type ‘long unsigned int’, but argument 2 has type ‘DWORD’
src/ybos.c:2987: warning: format ‘%8.8lx’ expects type ‘long unsigned int’, but argument 2 has type ‘DWORD’
src/ybos.c: In function ‘ybos_bank_display’:
src/ybos.c:3045: warning: format ‘%li’ expects type ‘long int’, but argument 3 has type ‘DWORD’
src/ybos.c:3045: warning: format ‘%li’ expects type ‘long int’, but argument 4 has type ‘DWORD’
src/ybos.c:3045: warning: format ‘%li’ expects type ‘long int’, but argument 5 has type ‘DWORD’
src/ybos.c:3070: warning: format ‘%8.8lx’ expects type ‘long unsigned int’, but argument 2 has type ‘DWORD’
src/ybos.c:3081: warning: format ‘%8.1li’ expects type ‘long int’, but argument 2 has type ‘DWORD’
src/ybos.c:3083: warning: format ‘%8.8lx’ expects type ‘long unsigned int’, but argument 2 has type ‘DWORD’
src/ybos.c:3129: warning: format ‘%li’ expects type ‘long int’, but argument 2 has type ‘DWORD’
src/ybos.c: In function ‘midas_bank_display’:
src/ybos.c:3212: warning: format ‘%li’ expects type ‘long int’, but argument 3 has type ‘DWORD’
src/ybos.c:3212: warning: format ‘%li’ expects type ‘long int’, but argument 4 has type ‘DWORD’
src/ybos.c:3212: warning: format ‘%li’ expects type ‘long int’, but argument 5 has type ‘DWORD’
src/ybos.c:3236: warning: format ‘%8.8lx’ expects type ‘long unsigned int’, but argument 2 has type ‘DWORD’
src/ybos.c:3247: warning: format ‘%8.1li’ expects type ‘long int’, but argument 2 has type ‘DWORD’
src/ybos.c:3249: warning: format ‘%8.8lx’ expects type ‘long unsigned int’, but argument 2 has type ‘DWORD’
src/ybos.c:3260: warning: format ‘%8.1li’ expects type ‘long int’, but argument 2 has type ‘DWORD’
src/ybos.c:3262: warning: format ‘%8.8lx’ expects type ‘long unsigned int’, but argument 2 has type ‘DWORD’
src/ybos.c: In function ‘midas_bank_display32’:
src/ybos.c:3428: warning: format ‘%li’ expects type ‘long int’, but argument 3 has type ‘DWORD’
src/ybos.c:3428: warning: format ‘%li’ expects type ‘long int’, but argument 4 has type ‘DWORD’
src/ybos.c:3428: warning: format ‘%li’ expects type ‘long int’, but argument 5 has type ‘DWORD’
src/ybos.c:3453: warning: format ‘%8.8lx’ expects type ‘long unsigned int’, but argument 2 has type ‘DWORD’
src/ybos.c:3464: warning: format ‘%8.1li’ expects type ‘long int’, but argument 2 has type ‘DWORD’
src/ybos.c:3466: warning: format ‘%8.8lx’ expects type ‘long unsigned int’, but argument 2 has type ‘DWORD’
src/ybos.c:3477: warning: format ‘%8.1li’ expects type ‘long int’, but argument 2 has type ‘DWORD’
src/ybos.c:3479: warning: format ‘%8.8lx’ expects type ‘long unsigned int’, but argument 2 has type ‘DWORD’
cc -c -g -O0 -Wall -Wuninitialized -Iinclude  -I./mxml -Idrivers -Llinux64/lib -DINCLUDE_FTPLIB   -D_LARGEFILE64_SOURCE -DHAVE_MYSQL -DHAVE_ROOT -pthread -m64 -I/home/daq/AlcapDAQ/root/include/root -DOS_LINUX -fPIC -Wno-unused-function -o linux64/lib/ftplib.o src/ftplib.c
src/ftplib.c: In function ‘ftp_data’:
src/ftplib.c:318: warning: pointer targets in passing argument 3 of ‘getsockname’ differ in signedness
/usr/include/sys/socket.h:119: note: expected ‘socklen_t * __restrict__’ but argument is of type ‘int *’
src/ftplib.c:340: warning: pointer targets in passing argument 3 of ‘accept’ differ in signedness
/usr/include/sys/socket.h:214: note: expected ‘socklen_t * __restrict__’ but argument is of type ‘int *’
cc -c -g -O0 -Wall -Wuninitialized -Iinclude  -I./mxml -Idrivers -Llinux64/lib -DINCLUDE_FTPLIB   -D_LARGEFILE64_SOURCE -DHAVE_MYSQL -DHAVE_ROOT -pthread -m64 -I/home/daq/AlcapDAQ/root/include/root -DOS_LINUX -fPIC -Wno-unused-function -o linux64/lib/strlcpy.o mxml/strlcpy.c
rm -f linux64/lib/libmidas.a
ar -crv linux64/lib/libmidas.a linux64/lib/midas.o linux64/lib/system.o linux64/lib/mrpc.o linux64/lib/odb.o linux64/lib/ybos.o linux64/lib/ftplib.o linux64/lib/strlcpy.o
a - linux64/lib/midas.o
a - linux64/lib/system.o
a - linux64/lib/mrpc.o
a - linux64/lib/odb.o
a - linux64/lib/ybos.o
a - linux64/lib/ftplib.o
a - linux64/lib/strlcpy.o
rm -f linux64/lib/libmidas.so
ld -shared -o linux64/lib/libmidas.so linux64/lib/midas.o linux64/lib/system.o linux64/lib/mrpc.o linux64/lib/odb.o linux64/lib/ybos.o linux64/lib/ftplib.o linux64/lib/strlcpy.o -lutil -lpthread /usr/lib64/mysql/libmysqlclient.so -lz -lc
g++ -c -g -O0 -Wall -Wuninitialized -Iinclude  -I./mxml -Idrivers -Llinux64/lib -DINCLUDE_FTPLIB   -D_LARGEFILE64_SOURCE -DHAVE_MYSQL -DHAVE_ROOT -pthread -m64 -I/home/daq/AlcapDAQ/root/include/root -DOS_LINUX -fPIC -Wno-unused-function -o linux64/lib/mana.o src/mana.cpp
src/mana.cpp:591: warning: deprecated conversion from string constant to ‘char*’
src/mana.cpp:669: warning: non-local variable ‘<anonymous struct> clp’ uses anonymous type
src/mana.cpp:678: warning: non-local variable ‘<anonymous struct> clp_descrip []’ uses anonymous type
src/mana.cpp:759: warning: non-local variable ‘<anonymous struct> out_info’ uses anonymous type
src/mana.cpp: In function ‘void add_data_dir(char*, char*)’:
src/mana.cpp:870: warning: deprecated conversion from string constant to ‘char*’
src/mana.cpp: In function ‘EVENT_DEF* db_get_event_definition(short int)’:
src/mana.cpp:918: warning: deprecated conversion from string constant to ‘char*’
src/mana.cpp:931: warning: deprecated conversion from string constant to ‘char*’
src/mana.cpp:933: warning: deprecated conversion from string constant to ‘char*’
src/mana.cpp:942: warning: deprecated conversion from string constant to ‘char*’
src/mana.cpp:947: warning: deprecated conversion from string constant to ‘char*’
src/mana.cpp:952: warning: deprecated conversion from string constant to ‘char*’
src/mana.cpp:963: warning: deprecated conversion from string constant to ‘char*’
src/mana.cpp:965: warning: deprecated conversion from string constant to ‘char*’
src/mana.cpp:967: warning: deprecated conversion from string constant to ‘char*’
src/mana.cpp:969: warning: deprecated conversion from string constant to ‘char*’
src/mana.cpp:971: warning: deprecated conversion from string constant to ‘char*’
src/mana.cpp:973: warning: deprecated conversion from string constant to ‘char*’
src/mana.cpp:976: warning: deprecated conversion from string constant to ‘char*’
src/mana.cpp:981: warning: deprecated conversion from string constant to ‘char*’
src/mana.cpp: In function ‘INT book_ttree()’:
src/mana.cpp:1653: warning: deprecated conversion from string constant to ‘char*’
src/mana.cpp:1678: warning: deprecated conversion from string constant to ‘char*’
src/mana.cpp:1718: warning: deprecated conversion from string constant to ‘char*’
src/mana.cpp:1724: warning: deprecated conversion from string constant to ‘char*’
src/mana.cpp:1765: warning: deprecated conversion from string constant to ‘char*’
src/mana.cpp: In function ‘INT SaveRootHistograms(TDirectory*, const char*)’:
src/mana.cpp:1804: warning: deprecated conversion from string constant to ‘char*’
src/mana.cpp: In function ‘INT mana_init()’:
src/mana.cpp:1951: warning: deprecated conversion from string constant to ‘char*’
src/mana.cpp: In function ‘INT bor(INT, char*)’:
src/mana.cpp:2147: warning: deprecated conversion from string constant to ‘char*’
src/mana.cpp:2180: warning: deprecated conversion from string constant to ‘char*’
src/mana.cpp:2229: warning: deprecated conversion from string constant to ‘char*’
src/mana.cpp:2242: warning: deprecated conversion from string constant to ‘char*’
src/mana.cpp:2265: warning: deprecated conversion from string constant to ‘char*’
src/mana.cpp:2304: warning: deprecated conversion from string constant to ‘char*’
src/mana.cpp: In function ‘INT eor(INT, char*)’:
src/mana.cpp:2388: warning: deprecated conversion from string constant to ‘char*’
src/mana.cpp: In function ‘INT write_event_ascii(FILE*, EVENT_HEADER*, ANALYZE_REQUEST*)’:
src/mana.cpp:2561: warning: deprecated conversion from string constant to ‘char*’
src/mana.cpp:2617: warning: deprecated conversion from string constant to ‘char*’
src/mana.cpp:2621: warning: deprecated conversion from string constant to ‘char*’
src/mana.cpp:2631: warning: deprecated conversion from string constant to ‘char*’
src/mana.cpp:2641: warning: deprecated conversion from string constant to ‘char*’
src/mana.cpp:2658: warning: deprecated conversion from string constant to ‘char*’
src/mana.cpp:2669: warning: deprecated conversion from string constant to ‘char*’
src/mana.cpp:2689: warning: deprecated conversion from string constant to ‘char*’
src/mana.cpp:2700: warning: deprecated conversion from string constant to ‘char*’
src/mana.cpp:2710: warning: deprecated conversion from string constant to ‘char*’
src/mana.cpp:2718: warning: deprecated conversion from string constant to ‘char*’
src/mana.cpp: In function ‘INT write_event_ttree(FILE*, EVENT_HEADER*, ANALYZE_REQUEST*)’:
src/mana.cpp:3347: warning: deprecated conversion from string constant to ‘char*’
src/mana.cpp:3397: warning: deprecated conversion from string constant to ‘char*’
src/mana.cpp:3409: warning: deprecated conversion from string constant to ‘char*’
src/mana.cpp:3441: warning: deprecated conversion from string constant to ‘char*’
src/mana.cpp:3462: warning: deprecated conversion from string constant to ‘char*’
src/mana.cpp: In function ‘INT write_event_odb(EVENT_HEADER*)’:
src/mana.cpp:3528: warning: deprecated conversion from string constant to ‘char*’
src/mana.cpp:3549: warning: deprecated conversion from string constant to ‘char*’
src/mana.cpp:3563: warning: deprecated conversion from string constant to ‘char*’
src/mana.cpp:3575: warning: deprecated conversion from string constant to ‘char*’
src/mana.cpp: At global scope:
src/mana.cpp:3586: warning: non-local variable ‘<anonymous struct> last_time_event [50]’ uses anonymous type
src/mana.cpp: In function ‘INT process_event(ANALYZE_REQUEST*, EVENT_HEADER*)’:
src/mana.cpp:3636: warning: deprecated conversion from string constant to ‘char*’
src/mana.cpp:3641: warning: deprecated conversion from string constant to ‘char*’
src/mana.cpp:3716: warning: comparison between signed and unsigned integer expressions
src/mana.cpp:3717: warning: deprecated conversion from string constant to ‘char*’
src/mana.cpp:3727: warning: deprecated conversion from string constant to ‘char*’
src/mana.cpp:3757: warning: deprecated conversion from string constant to ‘char*’
src/mana.cpp: In function ‘void receive_event(HNDLE, HNDLE, EVENT_HEADER*, void*)’:
src/mana.cpp:3810: warning: deprecated conversion from string constant to ‘char*’
src/mana.cpp: In function ‘void register_requests()’:
src/mana.cpp:3876: warning: deprecated conversion from string constant to ‘char*’
src/mana.cpp:3890: warning: deprecated conversion from string constant to ‘char*’
src/mana.cpp: In function ‘INT init_module_parameters(BOOL)’:
src/mana.cpp:4182: warning: deprecated conversion from string constant to ‘char*’
src/mana.cpp:4193: warning: deprecated conversion from string constant to ‘char*’
src/mana.cpp: In function ‘INT bevid_2_mheader(EVENT_HEADER*, DWORD*)’:
src/mana.cpp:4213: warning: deprecated conversion from string constant to ‘char*’
src/mana.cpp: In function ‘void odb_load(EVENT_HEADER*)’:
src/mana.cpp:4258: warning: deprecated conversion from string constant to ‘char*’
src/mana.cpp:4269: warning: deprecated conversion from string constant to ‘char*’
src/mana.cpp:4280: warning: deprecated conversion from string constant to ‘char*’
src/mana.cpp: In function ‘MA_FILE* ma_open(char*)’:
src/mana.cpp:4349: warning: deprecated conversion from string constant to ‘char*’
src/mana.cpp:4372: warning: deprecated conversion from string constant to ‘char*’
src/mana.cpp: In function ‘int ma_read_event(MA_FILE*, EVENT_HEADER*, int)’:
src/mana.cpp:4432: warning: deprecated conversion from string constant to ‘char*’
src/mana.cpp:4457: warning: deprecated conversion from string constant to ‘char*’
src/mana.cpp: In function ‘INT analyze_run(INT, char*, char*)’:
src/mana.cpp:4564: warning: deprecated conversion from string constant to ‘char*’
src/mana.cpp:4570: warning: deprecated conversion from string constant to ‘char*’
src/mana.cpp:4626: warning: deprecated conversion from string constant to ‘char*’
src/mana.cpp: In function ‘INT loop_runs_offline()’:
src/mana.cpp:4878: warning: deprecated conversion from string constant to ‘char*’
src/mana.cpp: In function ‘int main(int, char**)’:
src/mana.cpp:5755: warning: deprecated conversion from string constant to ‘char*’
src/mana.cpp:5861: warning: deprecated conversion from string constant to ‘char*’
src/mana.cpp:5928: warning: deprecated conversion from string constant to ‘char*’
g++ -DUSE_ROOT -c -g -O0 -Wall -Wuninitialized -Iinclude  -I./mxml -Idrivers -Llinux64/lib -DINCLUDE_FTPLIB   -D_LARGEFILE64_SOURCE -DHAVE_MYSQL -DHAVE_ROOT -pthread -m64 -I/home/daq/AlcapDAQ/root/include/root -DOS_LINUX -fPIC -Wno-unused-function -pthread -m64 -I/home/daq/AlcapDAQ/root/include/root -o linux64/lib/rmana.o src/mana.cpp
src/mana.cpp:591: warning: deprecated conversion from string constant to ‘char*’
src/mana.cpp:669: warning: non-local variable ‘<anonymous struct> clp’ uses anonymous type
src/mana.cpp:678: warning: non-local variable ‘<anonymous struct> clp_descrip []’ uses anonymous type
src/mana.cpp:759: warning: non-local variable ‘<anonymous struct> out_info’ uses anonymous type
src/mana.cpp: In function ‘void add_data_dir(char*, char*)’:
src/mana.cpp:870: warning: deprecated conversion from string constant to ‘char*’
src/mana.cpp: In function ‘EVENT_DEF* db_get_event_definition(short int)’:
src/mana.cpp:918: warning: deprecated conversion from string constant to ‘char*’
src/mana.cpp:931: warning: deprecated conversion from string constant to ‘char*’
src/mana.cpp:933: warning: deprecated conversion from string constant to ‘char*’
src/mana.cpp:942: warning: deprecated conversion from string constant to ‘char*’
src/mana.cpp:947: warning: deprecated conversion from string constant to ‘char*’
src/mana.cpp:952: warning: deprecated conversion from string constant to ‘char*’
src/mana.cpp:963: warning: deprecated conversion from string constant to ‘char*’
src/mana.cpp:965: warning: deprecated conversion from string constant to ‘char*’
src/mana.cpp:967: warning: deprecated conversion from string constant to ‘char*’
src/mana.cpp:969: warning: deprecated conversion from string constant to ‘char*’
src/mana.cpp:971: warning: deprecated conversion from string constant to ‘char*’
src/mana.cpp:973: warning: deprecated conversion from string constant to ‘char*’
src/mana.cpp:976: warning: deprecated conversion from string constant to ‘char*’
src/mana.cpp:981: warning: deprecated conversion from string constant to ‘char*’
src/mana.cpp: In function ‘INT book_ttree()’:
src/mana.cpp:1653: warning: deprecated conversion from string constant to ‘char*’
src/mana.cpp:1678: warning: deprecated conversion from string constant to ‘char*’
src/mana.cpp:1718: warning: deprecated conversion from string constant to ‘char*’
src/mana.cpp:1724: warning: deprecated conversion from string constant to ‘char*’
src/mana.cpp:1765: warning: deprecated conversion from string constant to ‘char*’
src/mana.cpp: In function ‘INT SaveRootHistograms(TDirectory*, const char*)’:
src/mana.cpp:1804: warning: deprecated conversion from string constant to ‘char*’
src/mana.cpp: In function ‘INT mana_init()’:
src/mana.cpp:1951: warning: deprecated conversion from string constant to ‘char*’
src/mana.cpp: In function ‘INT bor(INT, char*)’:
src/mana.cpp:2147: warning: deprecated conversion from string constant to ‘char*’
src/mana.cpp:2180: warning: deprecated conversion from string constant to ‘char*’
src/mana.cpp:2229: warning: deprecated conversion from string constant to ‘char*’
src/mana.cpp:2242: warning: deprecated conversion from string constant to ‘char*’
src/mana.cpp:2265: warning: deprecated conversion from string constant to ‘char*’
src/mana.cpp:2304: warning: deprecated conversion from string constant to ‘char*’
src/mana.cpp: In function ‘INT eor(INT, char*)’:
src/mana.cpp:2388: warning: deprecated conversion from string constant to ‘char*’
src/mana.cpp: In function ‘INT write_event_ascii(FILE*, EVENT_HEADER*, ANALYZE_REQUEST*)’:
src/mana.cpp:2561: warning: deprecated conversion from string constant to ‘char*’
src/mana.cpp:2617: warning: deprecated conversion from string constant to ‘char*’
src/mana.cpp:2621: warning: deprecated conversion from string constant to ‘char*’
src/mana.cpp:2631: warning: deprecated conversion from string constant to ‘char*’
src/mana.cpp:2641: warning: deprecated conversion from string constant to ‘char*’
src/mana.cpp:2658: warning: deprecated conversion from string constant to ‘char*’
src/mana.cpp:2669: warning: deprecated conversion from string constant to ‘char*’
src/mana.cpp:2689: warning: deprecated conversion from string constant to ‘char*’
src/mana.cpp:2700: warning: deprecated conversion from string constant to ‘char*’
src/mana.cpp:2710: warning: deprecated conversion from string constant to ‘char*’
src/mana.cpp:2718: warning: deprecated conversion from string constant to ‘char*’
src/mana.cpp: In function ‘INT write_event_ttree(FILE*, EVENT_HEADER*, ANALYZE_REQUEST*)’:
src/mana.cpp:3347: warning: deprecated conversion from string constant to ‘char*’
src/mana.cpp:3397: warning: deprecated conversion from string constant to ‘char*’
src/mana.cpp:3409: warning: deprecated conversion from string constant to ‘char*’
src/mana.cpp:3441: warning: deprecated conversion from string constant to ‘char*’
src/mana.cpp:3462: warning: deprecated conversion from string constant to ‘char*’
src/mana.cpp: In function ‘INT write_event_odb(EVENT_HEADER*)’:
src/mana.cpp:3528: warning: deprecated conversion from string constant to ‘char*’
src/mana.cpp:3549: warning: deprecated conversion from string constant to ‘char*’
src/mana.cpp:3563: warning: deprecated conversion from string constant to ‘char*’
src/mana.cpp:3575: warning: deprecated conversion from string constant to ‘char*’
src/mana.cpp: At global scope:
src/mana.cpp:3586: warning: non-local variable ‘<anonymous struct> last_time_event [50]’ uses anonymous type
src/mana.cpp: In function ‘INT process_event(ANALYZE_REQUEST*, EVENT_HEADER*)’:
src/mana.cpp:3636: warning: deprecated conversion from string constant to ‘char*’
src/mana.cpp:3641: warning: deprecated conversion from string constant to ‘char*’
src/mana.cpp:3716: warning: comparison between signed and unsigned integer expressions
src/mana.cpp:3717: warning: deprecated conversion from string constant to ‘char*’
src/mana.cpp:3727: warning: deprecated conversion from string constant to ‘char*’
src/mana.cpp:3757: warning: deprecated conversion from string constant to ‘char*’
src/mana.cpp: In function ‘void receive_event(HNDLE, HNDLE, EVENT_HEADER*, void*)’:
src/mana.cpp:3810: warning: deprecated conversion from string constant to ‘char*’
src/mana.cpp: In function ‘void register_requests()’:
src/mana.cpp:3876: warning: deprecated conversion from string constant to ‘char*’
src/mana.cpp:3890: warning: deprecated conversion from string constant to ‘char*’
src/mana.cpp: In function ‘INT init_module_parameters(BOOL)’:
src/mana.cpp:4182: warning: deprecated conversion from string constant to ‘char*’
src/mana.cpp:4193: warning: deprecated conversion from string constant to ‘char*’
src/mana.cpp: In function ‘INT bevid_2_mheader(EVENT_HEADER*, DWORD*)’:
src/mana.cpp:4213: warning: deprecated conversion from string constant to ‘char*’
src/mana.cpp: In function ‘void odb_load(EVENT_HEADER*)’:
src/mana.cpp:4258: warning: deprecated conversion from string constant to ‘char*’
src/mana.cpp:4269: warning: deprecated conversion from string constant to ‘char*’
src/mana.cpp:4280: warning: deprecated conversion from string constant to ‘char*’
src/mana.cpp: In function ‘MA_FILE* ma_open(char*)’:
src/mana.cpp:4349: warning: deprecated conversion from string constant to ‘char*’
src/mana.cpp:4372: warning: deprecated conversion from string constant to ‘char*’
src/mana.cpp: In function ‘int ma_read_event(MA_FILE*, EVENT_HEADER*, int)’:
src/mana.cpp:4432: warning: deprecated conversion from string constant to ‘char*’
src/mana.cpp:4457: warning: deprecated conversion from string constant to ‘char*’
src/mana.cpp: In function ‘INT analyze_run(INT, char*, char*)’:
src/mana.cpp:4564: warning: deprecated conversion from string constant to ‘char*’
src/mana.cpp:4570: warning: deprecated conversion from string constant to ‘char*’
src/mana.cpp:4626: warning: deprecated conversion from string constant to ‘char*’
src/mana.cpp: In function ‘INT loop_runs_offline()’:
src/mana.cpp:4878: warning: deprecated conversion from string constant to ‘char*’
src/mana.cpp: In function ‘int main(int, char**)’:
src/mana.cpp:5755: warning: deprecated conversion from string constant to ‘char*’
src/mana.cpp:5861: warning: deprecated conversion from string constant to ‘char*’
src/mana.cpp:5928: warning: deprecated conversion from string constant to ‘char*’
cc -c -g -O0 -Wall -Wuninitialized -Iinclude  -I./mxml -Idrivers -Llinux64/lib -DINCLUDE_FTPLIB   -D_LARGEFILE64_SOURCE -DHAVE_MYSQL -DHAVE_ROOT -pthread -m64 -I/home/daq/AlcapDAQ/root/include/root -DOS_LINUX -fPIC -Wno-unused-function -o linux64/lib/mfe.o src/mfe.c
src/mfe.c: In function ‘update_odb’:
src/mfe.c:962: warning: format ‘%li’ expects type ‘long int’, but argument 3 has type ‘DWORD’
src/mfe.c: In function ‘send_event’:
src/mfe.c:1047: warning: pointer targets in assignment differ in signedness
cc -Dextname -c -g -O0 -Wall -Wuninitialized -Iinclude  -I./mxml -Idrivers -Llinux64/lib -DINCLUDE_FTPLIB   -D_LARGEFILE64_SOURCE -DHAVE_MYSQL -DHAVE_ROOT -pthread -m64 -I/home/daq/AlcapDAQ/root/include/root -DOS_LINUX -fPIC -Wno-unused-function -o linux64/lib/fal.o src/fal.c
src/fal.c:317: warning: parameter names (without types) in function declaration
src/fal.c: In function ‘midas_flush_buffer’:
src/fal.c:568: warning: cast from pointer to integer of different size
src/fal.c:568: warning: cast from pointer to integer of different size
src/fal.c: In function ‘midas_write’:
src/fal.c:605: warning: cast from pointer to integer of different size
src/fal.c:605: warning: cast from pointer to integer of different size
src/fal.c: In function ‘dump_write’:
src/fal.c:886: warning: format ‘%ld’ expects type ‘long int’, but argument 3 has type ‘DWORD’
src/fal.c:888: warning: format ‘%ld’ expects type ‘long int’, but argument 3 has type ‘DWORD’
src/fal.c:891: warning: format ‘%ld’ expects type ‘long int’, but argument 5 has type ‘DWORD’
src/fal.c: In function ‘ascii_write’:
src/fal.c:1147: warning: format ‘%ld’ expects type ‘long int’, but argument 3 has type ‘DWORD’
src/fal.c: In function ‘open_history’:
src/fal.c:1829: warning: cast to pointer from integer of different size
src/fal.c:1842: warning: cast to pointer from integer of different size
src/fal.c: In function ‘log_system_history’:
src/fal.c:1950: warning: cast from pointer to integer of different size
src/fal.c: In function ‘cnaf_callback’:
src/fal.c:4515: warning: cast from pointer to integer of different size
src/fal.c:4515: warning: cast from pointer to integer of different size
src/fal.c:4519: warning: format ‘%lX’ expects type ‘long unsigned int’, but argument 2 has type ‘DWORD’
src/fal.c:4522: warning: format ‘%ld’ expects type ‘long int’, but argument 2 has type ‘DWORD’
src/fal.c:4522: warning: format ‘%ld’ expects type ‘long int’, but argument 3 has type ‘DWORD’
src/fal.c:4522: warning: format ‘%ld’ expects type ‘long int’, but argument 4 has type ‘DWORD’
src/fal.c:4522: warning: format ‘%ld’ expects type ‘long int’, but argument 5 has type ‘DWORD’
src/fal.c:4522: warning: format ‘%ld’ expects type ‘long int’, but argument 6 has type ‘DWORD’
src/fal.c:4522: warning: format ‘%lX’ expects type ‘long unsigned int’, but argument 7 has type ‘DWORD’
cc -g -O0 -Wall -Wuninitialized -Iinclude  -I./mxml -Idrivers -Llinux64/lib -DINCLUDE_FTPLIB   -D_LARGEFILE64_SOURCE -DHAVE_MYSQL -DHAVE_ROOT -pthread -m64 -I/home/daq/AlcapDAQ/root/include/root -DOS_LINUX -fPIC -Wno-unused-function -o linux64/bin/mserver src/mserver.c -lmidas -Wl,-rpath,/usr/local/lib -lutil -lpthread /usr/lib64/mysql/libmysqlclient.so -lz
src/mserver.c: In function ‘rpc_server_dispatch’:
src/mserver.c:707: warning: cast to pointer from integer of different size
cc -g -O0 -Wall -Wuninitialized -Iinclude  -I./mxml -Idrivers -Llinux64/lib -DINCLUDE_FTPLIB   -D_LARGEFILE64_SOURCE -DHAVE_MYSQL -DHAVE_ROOT -pthread -m64 -I/home/daq/AlcapDAQ/root/include/root -DOS_LINUX -fPIC -Wno-unused-function -o linux64/bin/mhttpd src/mhttpd.c src/mgd.c -lmidas -Wl,-rpath,/usr/local/lib -lutil -lpthread /usr/lib64/mysql/libmysqlclient.so -lz -lm
src/mhttpd.c: In function ‘sendmail’:
src/mhttpd.c:1320: warning: cast from pointer to integer of different size
src/mhttpd.c:1320: warning: cast from pointer to integer of different size
src/mhttpd.c: In function ‘show_elog_submit_query’:
src/mhttpd.c:3174: warning: passing argument 1 of ‘time’ from incompatible pointer type
/usr/include/time.h:186: note: expected ‘time_t *’ but argument is of type ‘DWORD *’
src/mhttpd.c:3175: warning: passing argument 1 of ‘localtime’ from incompatible pointer type
/usr/include/time.h:237: note: expected ‘const time_t *’ but argument is of type ‘DWORD *’
src/mhttpd.c:3224: warning: passing argument 1 of ‘time’ from incompatible pointer type
/usr/include/time.h:186: note: expected ‘time_t *’ but argument is of type ‘DWORD *’
src/mhttpd.c:3226: warning: passing argument 1 of ‘localtime’ from incompatible pointer type
/usr/include/time.h:237: note: expected ‘const time_t *’ but argument is of type ‘DWORD *’
src/mhttpd.c: In function ‘generate_hist_graph’:
src/mhttpd.c:7846: warning: format ‘%ld’ expects type ‘long int’, but argument 3 has type ‘DWORD’
src/mhttpd.c: In function ‘decode_post’:
src/mhttpd.c:9918: warning: cast from pointer to integer of different size
src/mhttpd.c:9918: warning: cast from pointer to integer of different size
src/mhttpd.c:9946: warning: cast from pointer to integer of different size
src/mhttpd.c:9946: warning: cast from pointer to integer of different size
src/mhttpd.c: In function ‘server_loop’:
src/mhttpd.c:10065: warning: pointer targets in passing argument 3 of ‘accept’ differ in signedness
/usr/include/sys/socket.h:214: note: expected ‘socklen_t * __restrict__’ but argument is of type ‘int *’
src/mhttpd.c:10178: warning: cast from pointer to integer of different size
src/mhttpd.c:10178: warning: cast from pointer to integer of different size
src/mhttpd.c:10182: warning: cast from pointer to integer of different size
src/mhttpd.c:10182: warning: cast from pointer to integer of different size
g++ -g -O0 -Wall -Wuninitialized -Iinclude  -I./mxml -Idrivers -Llinux64/lib -DINCLUDE_FTPLIB   -D_LARGEFILE64_SOURCE -DHAVE_MYSQL -DHAVE_ROOT -pthread -m64 -I/home/daq/AlcapDAQ/root/include/root -DOS_LINUX -fPIC -Wno-unused-function -o linux64/bin/mlogger src/mlogger.c -lmidas -Wl,-rpath,/usr/local/lib -L/home/daq/AlcapDAQ/root/lib/root -lCore -lCint -lRIO -lNet -lHist -lGraf -lGraf3d -lGpad -lTree -lRint -lPostscript -lMatrix -lPhysics -lMathCore -lThread -pthread -lm -ldl -rdynamic -Wl,-rpath,/home/daq/AlcapDAQ/root-install/lib -lutil -lpthread /usr/lib64/mysql/libmysqlclient.so -lz
src/mlogger.c:338: warning: non-local variable ‘<anonymous struct> hist_log [50]’ uses anonymous type
src/mlogger.c:344: warning: deprecated conversion from string constant to ‘char*’
src/mlogger.c:344: warning: deprecated conversion from string constant to ‘char*’
src/mlogger.c:344: warning: deprecated conversion from string constant to ‘char*’
src/mlogger.c:344: warning: deprecated conversion from string constant to ‘char*’
src/mlogger.c:344: warning: deprecated conversion from string constant to ‘char*’
src/mlogger.c:344: warning: deprecated conversion from string constant to ‘char*’
src/mlogger.c:344: warning: deprecated conversion from string constant to ‘char*’
src/mlogger.c:344: warning: deprecated conversion from string constant to ‘char*’
src/mlogger.c:344: warning: deprecated conversion from string constant to ‘char*’
src/mlogger.c:344: warning: deprecated conversion from string constant to ‘char*’
src/mlogger.c:344: warning: deprecated conversion from string constant to ‘char*’
src/mlogger.c:344: warning: deprecated conversion from string constant to ‘char*’
src/mlogger.c:344: warning: deprecated conversion from string constant to ‘char*’
src/mlogger.c:344: warning: deprecated conversion from string constant to ‘char*’
src/mlogger.c:344: warning: deprecated conversion from string constant to ‘char*’
src/mlogger.c:344: warning: deprecated conversion from string constant to ‘char*’
src/mlogger.c:344: warning: deprecated conversion from string constant to ‘char*’
src/mlogger.c:344: warning: deprecated conversion from string constant to ‘char*’
src/mlogger.c:344: warning: deprecated conversion from string constant to ‘char*’
src/mlogger.c:344: warning: deprecated conversion from string constant to ‘char*’
src/mlogger.c:344: warning: deprecated conversion from string constant to ‘char*’
src/mlogger.c:344: warning: deprecated conversion from string constant to ‘char*’
src/mlogger.c:344: warning: deprecated conversion from string constant to ‘char*’
src/mlogger.c: In function ‘void logger_init()’:
src/mlogger.c:374: warning: deprecated conversion from string constant to ‘char*’
src/mlogger.c:378: warning: deprecated conversion from string constant to ‘char*’
src/mlogger.c:382: warning: deprecated conversion from string constant to ‘char*’
src/mlogger.c:385: warning: deprecated conversion from string constant to ‘char*’
src/mlogger.c:389: warning: deprecated conversion from string constant to ‘char*’
src/mlogger.c:393: warning: deprecated conversion from string constant to ‘char*’
src/mlogger.c:396: warning: deprecated conversion from string constant to ‘char*’
src/mlogger.c:399: warning: deprecated conversion from string constant to ‘char*’
src/mlogger.c:402: warning: deprecated conversion from string constant to ‘char*’
src/mlogger.c:404: warning: deprecated conversion from string constant to ‘char*’
src/mlogger.c: In function ‘void log_odb_dump(LOG_CHN*, short int, INT)’:
src/mlogger.c:423: warning: deprecated conversion from string constant to ‘char*’
src/mlogger.c:428: warning: deprecated conversion from string constant to ‘char*’
src/mlogger.c: In function ‘void odb_save(char*)’:
src/mlogger.c:454: warning: deprecated conversion from string constant to ‘char*’
src/mlogger.c: At global scope:
src/mlogger.c:492: warning: deprecated conversion from string constant to ‘char*’
src/mlogger.c:492: warning: deprecated conversion from string constant to ‘char*’
src/mlogger.c:492: warning: deprecated conversion from string constant to ‘char*’
src/mlogger.c:492: warning: deprecated conversion from string constant to ‘char*’
src/mlogger.c:492: warning: deprecated conversion from string constant to ‘char*’
src/mlogger.c:492: warning: deprecated conversion from string constant to ‘char*’
src/mlogger.c:492: warning: deprecated conversion from string constant to ‘char*’
src/mlogger.c:492: warning: deprecated conversion from string constant to ‘char*’
src/mlogger.c:492: warning: deprecated conversion from string constant to ‘char*’
src/mlogger.c:492: warning: deprecated conversion from string constant to ‘char*’
src/mlogger.c:492: warning: deprecated conversion from string constant to ‘char*’
src/mlogger.c:492: warning: deprecated conversion from string constant to ‘char*’
src/mlogger.c: In function ‘int sql_get_columns(HNDLE, SQL_LIST**)’:
src/mlogger.c:604: warning: deprecated conversion from string constant to ‘char*’
src/mlogger.c: In function ‘BOOL sql_create_table(MYSQL*, char*, char*, HNDLE)’:
src/mlogger.c:627: warning: deprecated conversion from string constant to ‘char*’
src/mlogger.c:639: warning: deprecated conversion from string constant to ‘char*’
src/mlogger.c: In function ‘BOOL sql_modify_table(MYSQL*, char*, char*, HNDLE)’:
src/mlogger.c:657: warning: deprecated conversion from string constant to ‘char*’
src/mlogger.c:681: warning: deprecated conversion from string constant to ‘char*’
src/mlogger.c:688: warning: deprecated conversion from string constant to ‘char*’
src/mlogger.c: In function ‘BOOL sql_create_database(MYSQL*, char*)’:
src/mlogger.c:706: warning: deprecated conversion from string constant to ‘char*’
src/mlogger.c:714: warning: deprecated conversion from string constant to ‘char*’
src/mlogger.c: In function ‘int sql_insert(MYSQL*, char*, char*, HNDLE, BOOL)’:
src/mlogger.c:767: warning: deprecated conversion from string constant to ‘char*’
src/mlogger.c:776: warning: deprecated conversion from string constant to ‘char*’
src/mlogger.c:783: warning: deprecated conversion from string constant to ‘char*’
src/mlogger.c: In function ‘int sql_update(MYSQL*, char*, char*, HNDLE, BOOL, char*)’:
src/mlogger.c:830: warning: deprecated conversion from string constant to ‘char*’
src/mlogger.c:836: warning: deprecated conversion from string constant to ‘char*’
src/mlogger.c: In function ‘void create_sql_tree()’:
src/mlogger.c:854: warning: deprecated conversion from string constant to ‘char*’
src/mlogger.c:858: warning: deprecated conversion from string constant to ‘char*’
src/mlogger.c:862: warning: deprecated conversion from string constant to ‘char*’
src/mlogger.c:866: warning: deprecated conversion from string constant to ‘char*’
src/mlogger.c:870: warning: deprecated conversion from string constant to ‘char*’
src/mlogger.c:874: warning: deprecated conversion from string constant to ‘char*’
src/mlogger.c:875: warning: deprecated conversion from string constant to ‘char*’
src/mlogger.c:879: warning: deprecated conversion from string constant to ‘char*’
src/mlogger.c:881: warning: deprecated conversion from string constant to ‘char*’
src/mlogger.c:884: warning: deprecated conversion from string constant to ‘char*’
src/mlogger.c:886: warning: deprecated conversion from string constant to ‘char*’
src/mlogger.c:889: warning: deprecated conversion from string constant to ‘char*’
src/mlogger.c:889: warning: deprecated conversion from string constant to ‘char*’
src/mlogger.c:891: warning: deprecated conversion from string constant to ‘char*’
src/mlogger.c:894: warning: deprecated conversion from string constant to ‘char*’
src/mlogger.c:894: warning: deprecated conversion from string constant to ‘char*’
src/mlogger.c:896: warning: deprecated conversion from string constant to ‘char*’
src/mlogger.c:899: warning: deprecated conversion from string constant to ‘char*’
src/mlogger.c:899: warning: deprecated conversion from string constant to ‘char*’
src/mlogger.c:902: warning: deprecated conversion from string constant to ‘char*’
src/mlogger.c:905: warning: deprecated conversion from string constant to ‘char*’
src/mlogger.c:907: warning: deprecated conversion from string constant to ‘char*’
src/mlogger.c:910: warning: deprecated conversion from string constant to ‘char*’
src/mlogger.c:910: warning: deprecated conversion from string constant to ‘char*’
src/mlogger.c:912: warning: deprecated conversion from string constant to ‘char*’
src/mlogger.c:915: warning: deprecated conversion from string constant to ‘char*’
src/mlogger.c:915: warning: deprecated conversion from string constant to ‘char*’
src/mlogger.c: In function ‘void write_sql(BOOL)’:
src/mlogger.c:936: warning: deprecated conversion from string constant to ‘char*’
src/mlogger.c:942: warning: deprecated conversion from string constant to ‘char*’
src/mlogger.c:954: warning: deprecated conversion from string constant to ‘char*’
src/mlogger.c:956: warning: deprecated conversion from string constant to ‘char*’
src/mlogger.c:960: warning: deprecated conversion from string constant to ‘char*’
src/mlogger.c:962: warning: deprecated conversion from string constant to ‘char*’
src/mlogger.c:969: warning: deprecated conversion from string constant to ‘char*’
src/mlogger.c:973: warning: deprecated conversion from string constant to ‘char*’
src/mlogger.c:977: warning: deprecated conversion from string constant to ‘char*’
src/mlogger.c:981: warning: deprecated conversion from string constant to ‘char*’
src/mlogger.c:985: warning: deprecated conversion from string constant to ‘char*’
src/mlogger.c:989: warning: deprecated conversion from string constant to ‘char*’
src/mlogger.c:990: warning: deprecated conversion from string constant to ‘char*’
src/mlogger.c:994: warning: deprecated conversion from string constant to ‘char*’
src/mlogger.c:1005: warning: deprecated conversion from string constant to ‘char*’
src/mlogger.c:1023: warning: deprecated conversion from string constant to ‘char*’
src/mlogger.c: In function ‘INT tape_open(char*, INT*)’:
src/mlogger.c:1060: warning: deprecated conversion from string constant to ‘char*’
src/mlogger.c:1062: warning: deprecated conversion from string constant to ‘char*’
src/mlogger.c: In function ‘INT ftp_error(char*)’:
src/mlogger.c:1074: warning: deprecated conversion from string constant to ‘char*’
src/mlogger.c: In function ‘INT ftp_open(char*, FTP_CON**)’:
src/mlogger.c:1126: warning: deprecated conversion from string constant to ‘char*’
src/mlogger.c:1130: warning: deprecated conversion from string constant to ‘char*’
src/mlogger.c:1134: warning: deprecated conversion from string constant to ‘char*’
src/mlogger.c:1134: warning: deprecated conversion from string constant to ‘char*’
src/mlogger.c:1139: warning: deprecated conversion from string constant to ‘char*’
src/mlogger.c:1144: warning: deprecated conversion from string constant to ‘char*’
src/mlogger.c: In function ‘EVENT_DEF* db_get_event_definition(short int)’:
src/mlogger.c:1385: warning: deprecated conversion from string constant to ‘char*’
src/mlogger.c:1397: warning: deprecated conversion from string constant to ‘char*’
src/mlogger.c:1399: warning: deprecated conversion from string constant to ‘char*’
src/mlogger.c:1408: warning: deprecated conversion from string constant to ‘char*’
src/mlogger.c:1413: warning: deprecated conversion from string constant to ‘char*’
src/mlogger.c:1423: warning: deprecated conversion from string constant to ‘char*’
src/mlogger.c:1425: warning: deprecated conversion from string constant to ‘char*’
src/mlogger.c:1427: warning: deprecated conversion from string constant to ‘char*’
src/mlogger.c:1429: warning: deprecated conversion from string constant to ‘char*’
src/mlogger.c:1431: warning: deprecated conversion from string constant to ‘char*’
src/mlogger.c:1433: warning: deprecated conversion from string constant to ‘char*’
src/mlogger.c:1436: warning: deprecated conversion from string constant to ‘char*’
src/mlogger.c:1441: warning: deprecated conversion from string constant to ‘char*’
src/mlogger.c: In function ‘INT dump_write(LOG_CHN*, EVENT_HEADER*, INT)’:
src/mlogger.c:1483: warning: deprecated conversion from string constant to ‘char*’
src/mlogger.c:1534: warning: deprecated conversion from string constant to ‘char*’
src/mlogger.c:1562: warning: deprecated conversion from string constant to ‘char*’
src/mlogger.c:1571: warning: deprecated conversion from string constant to ‘char*’
src/mlogger.c:1581: warning: deprecated conversion from string constant to ‘char*’
src/mlogger.c:1589: warning: deprecated conversion from string constant to ‘char*’
src/mlogger.c: In function ‘INT ascii_write(LOG_CHN*, EVENT_HEADER*, INT)’:
src/mlogger.c:1783: warning: deprecated conversion from string constant to ‘char*’
src/mlogger.c:1789: warning: deprecated conversion from string constant to ‘char*’
src/mlogger.c:1798: warning: deprecated conversion from string constant to ‘char*’
src/mlogger.c:1818: warning: deprecated conversion from string constant to ‘char*’
src/mlogger.c:1823: warning: deprecated conversion from string constant to ‘char*’
src/mlogger.c: In function ‘INT root_book_trees(TREE_STRUCT*)’:
src/mlogger.c:1956: warning: deprecated conversion from string constant to ‘char*’
src/mlogger.c:1958: warning: deprecated conversion from string constant to ‘char*’
src/mlogger.c:1984: warning: deprecated conversion from string constant to ‘char*’
src/mlogger.c:1994: warning: deprecated conversion from string constant to ‘char*’
src/mlogger.c:1996: warning: deprecated conversion from string constant to ‘char*’
src/mlogger.c:1999: warning: deprecated conversion from string constant to ‘char*’
src/mlogger.c: In function ‘INT root_book_bank(EVENT_TREE*, HNDLE, int, char*)’:
src/mlogger.c:2024: warning: deprecated conversion from string constant to ‘char*’
src/mlogger.c:2030: warning: deprecated conversion from string constant to ‘char*’
src/mlogger.c:2075: warning: ISO C++ says that these are ambiguous, even though the worst conversion for the first is better than the worst conversion for the second:
/home/daq/AlcapDAQ/root/include/root/TTree.h:296: note: candidate 1: TBranch* TTree::Branch(const char*, Long_t, const char*, Int_t)
/home/daq/AlcapDAQ/root/include/root/TTree.h:309: note: candidate 2: TBranch* TTree::Branch(const char*, const char*, T*, Int_t, Int_t) [with T = char]
src/mlogger.c:2129: warning: ISO C++ says that these are ambiguous, even though the worst conversion for the first is better than the worst conversion for the second:
/home/daq/AlcapDAQ/root/include/root/TTree.h:296: note: candidate 1: TBranch* TTree::Branch(const char*, Long_t, const char*, Int_t)
/home/daq/AlcapDAQ/root/include/root/TTree.h:309: note: candidate 2: TBranch* TTree::Branch(const char*, const char*, T*, Int_t, Int_t) [with T = char]
src/mlogger.c: In function ‘INT root_write(LOG_CHN*, EVENT_HEADER*, INT)’:
src/mlogger.c:2162: warning: deprecated conversion from string constant to ‘char*’
src/mlogger.c:2183: warning: deprecated conversion from string constant to ‘char*’
src/mlogger.c:2249: warning: deprecated conversion from string constant to ‘char*’
src/mlogger.c: In function ‘INT root_log_open(LOG_CHN*, INT)’:
src/mlogger.c:2276: warning: deprecated conversion from string constant to ‘char*’
src/mlogger.c:2295: warning: deprecated conversion from string constant to ‘char*’
src/mlogger.c:2311: warning: deprecated conversion from string constant to ‘char*’
src/mlogger.c: In function ‘INT log_open(LOG_CHN*, INT)’:
src/mlogger.c:2367: warning: deprecated conversion from string constant to ‘char*’
src/mlogger.c:2370: warning: deprecated conversion from string constant to ‘char*’
src/mlogger.c:2373: warning: deprecated conversion from string constant to ‘char*’
src/mlogger.c:2376: warning: deprecated conversion from string constant to ‘char*’
src/mlogger.c:2383: warning: deprecated conversion from string constant to ‘char*’
src/mlogger.c: In function ‘INT log_write(LOG_CHN*, EVENT_HEADER*)’:
src/mlogger.c:2454: warning: deprecated conversion from string constant to ‘char*’
src/mlogger.c:2459: warning: deprecated conversion from string constant to ‘char*’
src/mlogger.c:2461: warning: deprecated conversion from string constant to ‘char*’
src/mlogger.c:2477: warning: deprecated conversion from string constant to ‘char*’
src/mlogger.c:2481: warning: deprecated conversion from string constant to ‘char*’
src/mlogger.c:2487: warning: deprecated conversion from string constant to ‘char*’
src/mlogger.c:2502: warning: deprecated conversion from string constant to ‘char*’
src/mlogger.c:2506: warning: deprecated conversion from string constant to ‘char*’
src/mlogger.c:2512: warning: deprecated conversion from string constant to ‘char*’
src/mlogger.c:2526: warning: deprecated conversion from string constant to ‘char*’
src/mlogger.c:2534: warning: deprecated conversion from string constant to ‘char*’
src/mlogger.c:2539: warning: deprecated conversion from string constant to ‘char*’
src/mlogger.c:2550: warning: deprecated conversion from string constant to ‘char*’
src/mlogger.c:2551: warning: deprecated conversion from string constant to ‘char*’
src/mlogger.c:2553: warning: deprecated conversion from string constant to ‘char*’
src/mlogger.c:2565: warning: deprecated conversion from string constant to ‘char*’
src/mlogger.c:2569: warning: deprecated conversion from string constant to ‘char*’
src/mlogger.c: In function ‘INT open_history()’:
src/mlogger.c:2596: warning: deprecated conversion from string constant to ‘char*’
src/mlogger.c:2598: warning: deprecated conversion from string constant to ‘char*’
src/mlogger.c:2603: warning: deprecated conversion from string constant to ‘char*’
src/mlogger.c:2604: warning: deprecated conversion from string constant to ‘char*’
src/mlogger.c:2606: warning: deprecated conversion from string constant to ‘char*’
src/mlogger.c:2608: warning: deprecated conversion from string constant to ‘char*’
src/mlogger.c:2611: warning: deprecated conversion from string constant to ‘char*’
src/mlogger.c:2611: warning: deprecated conversion from string constant to ‘char*’
src/mlogger.c:2613: warning: deprecated conversion from string constant to ‘char*’
src/mlogger.c:2616: warning: deprecated conversion from string constant to ‘char*’
src/mlogger.c:2616: warning: deprecated conversion from string constant to ‘char*’
src/mlogger.c:2623: warning: deprecated conversion from string constant to ‘char*’
src/mlogger.c:2625: warning: deprecated conversion from string constant to ‘char*’
src/mlogger.c:2637: warning: deprecated conversion from string constant to ‘char*’
src/mlogger.c:2646: warning: deprecated conversion from string constant to ‘char*’
src/mlogger.c:2649: warning: deprecated conversion from string constant to ‘char*’
src/mlogger.c:2654: warning: deprecated conversion from string constant to ‘char*’
src/mlogger.c:2672: warning: deprecated conversion from string constant to ‘char*’
src/mlogger.c:2686: warning: deprecated conversion from string constant to ‘char*’
src/mlogger.c:2714: warning: deprecated conversion from string constant to ‘char*’
src/mlogger.c:2732: warning: deprecated conversion from string constant to ‘char*’
src/mlogger.c:2776: warning: deprecated conversion from string constant to ‘char*’
src/mlogger.c:2785: warning: deprecated conversion from string constant to ‘char*’
src/mlogger.c:2800: warning: deprecated conversion from string constant to ‘char*’
src/mlogger.c:2809: warning: deprecated conversion from string constant to ‘char*’
src/mlogger.c:2822: warning: deprecated conversion from string constant to ‘char*’
src/mlogger.c:2845: warning: deprecated conversion from string constant to ‘char*’
src/mlogger.c:2852: warning: deprecated conversion from string constant to ‘char*’
src/mlogger.c:2909: warning: deprecated conversion from string constant to ‘char*’
src/mlogger.c:2917: warning: deprecated conversion from string constant to ‘char*’
src/mlogger.c:2936: warning: deprecated conversion from string constant to ‘char*’
src/mlogger.c: In function ‘void close_history()’:
src/mlogger.c:2950: warning: deprecated conversion from string constant to ‘char*’
src/mlogger.c: In function ‘INT log_callback(INT, void**)’:
src/mlogger.c:3055: warning: deprecated conversion from string constant to ‘char*’
src/mlogger.c:3057: warning: deprecated conversion from string constant to ‘char*’
src/mlogger.c:3064: warning: deprecated conversion from string constant to ‘char*’
src/mlogger.c:3079: warning: deprecated conversion from string constant to ‘char*’
src/mlogger.c:3083: warning: deprecated conversion from string constant to ‘char*’
src/mlogger.c:3087: warning: deprecated conversion from string constant to ‘char*’
src/mlogger.c:3092: warning: deprecated conversion from string constant to ‘char*’
src/mlogger.c:3101: warning: deprecated conversion from string constant to ‘char*’
src/mlogger.c:3103: warning: deprecated conversion from string constant to ‘char*’
src/mlogger.c:3112: warning: deprecated conversion from string constant to ‘char*’
src/mlogger.c:3114: warning: deprecated conversion from string constant to ‘char*’
src/mlogger.c:3119: warning: deprecated conversion from string constant to ‘char*’
src/mlogger.c: At global scope:
src/mlogger.c:3136: warning: non-local variable ‘<anonymous struct> eb’ uses anonymous type
src/mlogger.c: In function ‘INT tr_start(INT, char*)’:
src/mlogger.c:3164: warning: deprecated conversion from string constant to ‘char*’
src/mlogger.c:3169: warning: deprecated conversion from string constant to ‘char*’
src/mlogger.c:3173: warning: deprecated conversion from string constant to ‘char*’
src/mlogger.c:3176: warning: deprecated conversion from string constant to ‘char*’
src/mlogger.c:3179: warning: deprecated conversion from string constant to ‘char*’
src/mlogger.c:3182: warning: deprecated conversion from string constant to ‘char*’
src/mlogger.c:3186: warning: deprecated conversion from string constant to ‘char*’
src/mlogger.c:3189: warning: deprecated conversion from string constant to ‘char*’
src/mlogger.c:3203: warning: deprecated conversion from string constant to ‘char*’
src/mlogger.c:3214: warning: deprecated conversion from string constant to ‘char*’
src/mlogger.c:3225: warning: deprecated conversion from string constant to ‘char*’
src/mlogger.c:3228: warning: deprecated conversion from string constant to ‘char*’
src/mlogger.c:3233: warning: deprecated conversion from string constant to ‘char*’
src/mlogger.c:3236: warning: deprecated conversion from string constant to ‘char*’
src/mlogger.c:3258: warning: deprecated conversion from string constant to ‘char*’
src/mlogger.c:3268: warning: deprecated conversion from string constant to ‘char*’
src/mlogger.c:3277: warning: deprecated conversion from string constant to ‘char*’
src/mlogger.c:3279: warning: deprecated conversion from string constant to ‘char*’
src/mlogger.c:3281: warning: deprecated conversion from string constant to ‘char*’
src/mlogger.c:3287: warning: deprecated conversion from string constant to ‘char*’
src/mlogger.c:3298: warning: deprecated conversion from string constant to ‘char*’
src/mlogger.c:3344: warning: deprecated conversion from string constant to ‘char*’
src/mlogger.c:3349: warning: deprecated conversion from string constant to ‘char*’
src/mlogger.c:3377: warning: deprecated conversion from string constant to ‘char*’
src/mlogger.c:3393: warning: deprecated conversion from string constant to ‘char*’
src/mlogger.c:3401: warning: deprecated conversion from string constant to ‘char*’
src/mlogger.c:3410: warning: deprecated conversion from string constant to ‘char*’
src/mlogger.c:3423: warning: deprecated conversion from string constant to ‘char*’
src/mlogger.c:3431: warning: deprecated conversion from string constant to ‘char*’
src/mlogger.c:3434: warning: deprecated conversion from string constant to ‘char*’
src/mlogger.c:3447: warning: deprecated conversion from string constant to ‘char*’
src/mlogger.c:3456: warning: deprecated conversion from string constant to ‘char*’
src/mlogger.c:3463: warning: deprecated conversion from string constant to ‘char*’
src/mlogger.c: In function ‘INT tr_stop(INT, char*)’:
src/mlogger.c:3505: warning: deprecated conversion from string constant to ‘char*’
src/mlogger.c:3563: warning: deprecated conversion from string constant to ‘char*’
src/mlogger.c:3568: warning: deprecated conversion from string constant to ‘char*’
src/mlogger.c:3589: warning: deprecated conversion from string constant to ‘char*’
src/mlogger.c: In function ‘int main(int, char**)’:
src/mlogger.c:3672: warning: deprecated conversion from string constant to ‘char*’
src/mlogger.c:3718: warning: deprecated conversion from string constant to ‘char*’
src/mlogger.c:3718: warning: deprecated conversion from string constant to ‘char*’
src/mlogger.c:3723: warning: deprecated conversion from string constant to ‘char*’
src/mlogger.c:3744: warning: deprecated conversion from string constant to ‘char*’
src/mlogger.c:3770: warning: deprecated conversion from string constant to ‘char*’
src/mlogger.c:3777: warning: deprecated conversion from string constant to ‘char*’
src/mlogger.c:3779: warning: deprecated conversion from string constant to ‘char*’
src/mlogger.c:3786: warning: deprecated conversion from string constant to ‘char*’
src/mlogger.c:3788: warning: deprecated conversion from string constant to ‘char*’
src/mlogger.c:3811: warning: deprecated conversion from string constant to ‘char*’
src/mlogger.c:3813: warning: deprecated conversion from string constant to ‘char*’
src/mlogger.c:3820: warning: deprecated conversion from string constant to ‘char*’
src/mlogger.c:3825: warning: deprecated conversion from string constant to ‘char*’
src/mlogger.c:3829: warning: deprecated conversion from string constant to ‘char*’
src/mlogger.c:3832: warning: deprecated conversion from string constant to ‘char*’
cc -g -O0 -Wall -Wuninitialized -Iinclude  -I./mxml -Idrivers -Llinux64/lib -DINCLUDE_FTPLIB   -D_LARGEFILE64_SOURCE -DHAVE_MYSQL -DHAVE_ROOT -pthread -m64 -I/home/daq/AlcapDAQ/root/include/root -DOS_LINUX -fPIC -Wno-unused-function -o linux64/bin/odbedit src/odbedit.c src/cmdedit.c -lmidas -Wl,-rpath,/usr/local/lib -lutil -lpthread /usr/lib64/mysql/libmysqlclient.so -lz
src/odbedit.c: In function ‘print_key’:
src/odbedit.c:537: warning: format ‘%ld’ expects type ‘long int’, but argument 3 has type ‘DWORD’
src/odbedit.c: In function ‘scan_tree’:
src/odbedit.c:721: warning: format ‘%ld’ expects type ‘long int’, but argument 3 has type ‘DWORD’
src/odbedit.c: In function ‘command_loop’:
src/odbedit.c:2341: warning: format ‘%-10ld’ expects type ‘long int’, but argument 2 has type ‘DWORD’
src/odbedit.c:2341: warning: format ‘%-10ld’ expects type ‘long int’, but argument 3 has type ‘DWORD’
cc -g -O0 -Wall -Wuninitialized -Iinclude  -I./mxml -Idrivers -Llinux64/lib -DINCLUDE_FTPLIB   -D_LARGEFILE64_SOURCE -DHAVE_MYSQL -DHAVE_ROOT -pthread -m64 -I/home/daq/AlcapDAQ/root/include/root -DOS_LINUX -fPIC -Wno-unused-function -o linux64/bin/mtape utils/mtape.c -lmidas -Wl,-rpath,/usr/local/lib -lutil -lpthread /usr/lib64/mysql/libmysqlclient.so -lz
utils/mtape.c: In function ‘tape_dir’:
utils/mtape.c:133: warning: passing argument 1 of ‘ctime’ from incompatible pointer type
/usr/include/time.h:258: note: expected ‘const time_t *’ but argument is of type ‘DWORD *’
utils/mtape.c:133: warning: format ‘%ld’ expects type ‘long int’, but argument 2 has type ‘DWORD’
utils/mtape.c:136: warning: passing argument 1 of ‘ctime’ from incompatible pointer type
/usr/include/time.h:258: note: expected ‘const time_t *’ but argument is of type ‘DWORD *’
utils/mtape.c:136: warning: format ‘%ld’ expects type ‘long int’, but argument 2 has type ‘DWORD’
cc -g -O0 -Wall -Wuninitialized -Iinclude  -I./mxml -Idrivers -Llinux64/lib -DINCLUDE_FTPLIB   -D_LARGEFILE64_SOURCE -DHAVE_MYSQL -DHAVE_ROOT -pthread -m64 -I/home/daq/AlcapDAQ/root/include/root -DOS_LINUX -fPIC -Wno-unused-function -o linux64/bin/mhist utils/mhist.c -lmidas -Wl,-rpath,/usr/local/lib -lutil -lpthread /usr/lib64/mysql/libmysqlclient.so -lz
utils/mhist.c: In function ‘write_hist_speed’:
utils/mhist.c:125: warning: format ‘%ld’ expects type ‘long int’, but argument 2 has type ‘DWORD’
utils/mhist.c:130: warning: format ‘%ld’ expects type ‘long int’, but argument 2 has type ‘DWORD’
utils/mhist.c:130: warning: format ‘%ld’ expects type ‘long int’, but argument 3 has type ‘DWORD’
utils/mhist.c: In function ‘generate_hist’:
utils/mhist.c:152: warning: format ‘%ld’ expects type ‘long int’, but argument 2 has type ‘DWORD’
utils/mhist.c: In function ‘query_params’:
utils/mhist.c:182: warning: format ‘%ld’ expects type ‘long int *’, but argument 2 has type ‘DWORD *’
utils/mhist.c:196: warning: format ‘%ld’ expects type ‘long int’, but argument 2 has type ‘DWORD’
utils/mhist.c:196: warning: format ‘%ld’ expects type ‘long int’, but argument 4 has type ‘DWORD’
utils/mhist.c:198: warning: format ‘%ld’ expects type ‘long int’, but argument 2 has type ‘DWORD’
utils/mhist.c:202: warning: format ‘%ld’ expects type ‘long int’, but argument 2 has type ‘DWORD’
utils/mhist.c:210: warning: format ‘%ld’ expects type ‘long int *’, but argument 2 has type ‘DWORD *’
utils/mhist.c:220: warning: format ‘%ld’ expects type ‘long int *’, but argument 2 has type ‘DWORD *’
utils/mhist.c:225: warning: format ‘%ld’ expects type ‘long int *’, but argument 2 has type ‘DWORD *’
utils/mhist.c: In function ‘display_vars’:
utils/mhist.c:281: warning: format ‘%ld’ expects type ‘long int’, but argument 2 has type ‘DWORD’
utils/mhist.c:281: warning: format ‘%ld’ expects type ‘long int’, but argument 4 has type ‘DWORD’
utils/mhist.c:283: warning: format ‘%ld’ expects type ‘long int’, but argument 2 has type ‘DWORD’
utils/mhist.c: In function ‘display_single_hist’:
utils/mhist.c:317: warning: format ‘%li’ expects type ‘long int’, but argument 3 has type ‘DWORD’
utils/mhist.c:319: warning: passing argument 1 of ‘ctime’ from incompatible pointer type
/usr/include/time.h:258: note: expected ‘const time_t *’ but argument is of type ‘DWORD *’
utils/mhist.c: In function ‘display_range_hist’:
utils/mhist.c:374: warning: format ‘%ld’ expects type ‘long int’, but argument 3 has type ‘DWORD’
utils/mhist.c:374: warning: format ‘%ld’ expects type ‘long int’, but argument 4 has type ‘DWORD’
utils/mhist.c:377: warning: format ‘%li’ expects type ‘long int’, but argument 3 has type ‘DWORD’
utils/mhist.c:379: warning: passing argument 1 of ‘ctime’ from incompatible pointer type
/usr/include/time.h:258: note: expected ‘const time_t *’ but argument is of type ‘DWORD *’
utils/mhist.c: In function ‘convert_time’:
utils/mhist.c:430: warning: passing argument 1 of ‘localtime’ from incompatible pointer type
/usr/include/time.h:237: note: expected ‘const time_t *’ but argument is of type ‘DWORD *’
cc -g -O0 -Wall -Wuninitialized -Iinclude  -I./mxml -Idrivers -Llinux64/lib -DINCLUDE_FTPLIB   -D_LARGEFILE64_SOURCE -DHAVE_MYSQL -DHAVE_ROOT -pthread -m64 -I/home/daq/AlcapDAQ/root/include/root -DOS_LINUX -fPIC -Wno-unused-function -o linux64/bin/mstat utils/mstat.c -lmidas -Wl,-rpath,/usr/local/lib -lutil -lpthread /usr/lib64/mysql/libmysqlclient.so -lz
utils/mstat.c: In function ‘compose_status’:
utils/mstat.c:221: warning: format ‘%02ld’ expects type ‘long int’, but argument 3 has type ‘DWORD’
utils/mstat.c:221: warning: format ‘%02ld’ expects type ‘long int’, but argument 4 has type ‘DWORD’
utils/mstat.c:221: warning: format ‘%02ld’ expects type ‘long int’, but argument 5 has type ‘DWORD’
utils/mstat.c:224: warning: format ‘%02ld’ expects type ‘long int’, but argument 3 has type ‘DWORD’
utils/mstat.c:224: warning: format ‘%02ld’ expects type ‘long int’, but argument 4 has type ‘DWORD’
utils/mstat.c:224: warning: format ‘%02ld’ expects type ‘long int’, but argument 5 has type ‘DWORD’
utils/mstat.c:232: warning: format ‘%02ld’ expects type ‘long int’, but argument 3 has type ‘DWORD’
utils/mstat.c:232: warning: format ‘%02ld’ expects type ‘long int’, but argument 4 has type ‘DWORD’
utils/mstat.c:232: warning: format ‘%02ld’ expects type ‘long int’, but argument 5 has type ‘DWORD’
utils/mstat.c:235: warning: format ‘%02ld’ expects type ‘long int’, but argument 3 has type ‘DWORD’
utils/mstat.c:235: warning: format ‘%02ld’ expects type ‘long int’, but argument 4 has type ‘DWORD’
utils/mstat.c:235: warning: format ‘%02ld’ expects type ‘long int’, but argument 5 has type ‘DWORD’
utils/mstat.c:571: warning: format ‘%2.li’ expects type ‘long int’, but argument 3 has type ‘DWORD’
cc -g -O0 -Wall -Wuninitialized -Iinclude  -I./mxml -Idrivers -Llinux64/lib -DINCLUDE_FTPLIB   -D_LARGEFILE64_SOURCE -DHAVE_MYSQL -DHAVE_ROOT -pthread -m64 -I/home/daq/AlcapDAQ/root/include/root -DOS_LINUX -fPIC -Wno-unused-function -o linux64/bin/mcnaf utils/mcnaf.c drivers/bus/camacrpc.c -lmidas -Wl,-rpath,/usr/local/lib -lutil -lpthread /usr/lib64/mysql/libmysqlclient.so -lz
utils/mcnaf.c: In function ‘mcstd_func’:
utils/mcnaf.c:242: warning: format ‘%lx’ expects type ‘long unsigned int’, but argument 3 has type ‘DWORD’
utils/mcnaf.c:298: warning: format ‘%lx’ expects type ‘long unsigned int’, but argument 7 has type ‘DWORD’
utils/mcnaf.c:312: warning: format ‘%lx’ expects type ‘long unsigned int’, but argument 7 has type ‘DWORD’
utils/mcnaf.c:321: warning: format ‘%lx’ expects type ‘long unsigned int’, but argument 6 has type ‘DWORD’
utils/mcnaf.c:335: warning: format ‘%6.6lx’ expects type ‘long unsigned int’, but argument 7 has type ‘DWORD’
utils/mcnaf.c:349: warning: format ‘%6.6lx’ expects type ‘long unsigned int’, but argument 7 has type ‘DWORD’
utils/mcnaf.c:364: warning: format ‘%6.6lx’ expects type ‘long unsigned int’, but argument 7 has type ‘DWORD’
utils/mcnaf.c:378: warning: format ‘%6.6lx’ expects type ‘long unsigned int’, but argument 7 has type ‘DWORD’
utils/mcnaf.c:392: warning: format ‘%6.6lx’ expects type ‘long unsigned int’, but argument 7 has type ‘DWORD’
utils/mcnaf.c:406: warning: format ‘%lx’ expects type ‘long unsigned int’, but argument 7 has type ‘DWORD’
utils/mcnaf.c: In function ‘make_display_string’:
utils/mcnaf.c:430: warning: format ‘%ld’ expects type ‘long int’, but argument 8 has type ‘DWORD’
utils/mcnaf.c:430: warning: format ‘%06lx’ expects type ‘long unsigned int’, but argument 9 has type ‘DWORD’
utils/mcnaf.c:437: warning: format ‘%ld’ expects type ‘long int’, but argument 8 has type ‘DWORD’
utils/mcnaf.c:437: warning: format ‘%06lx’ expects type ‘long unsigned int’, but argument 9 has type ‘DWORD’
cc -g -O0 -Wall -Wuninitialized -Iinclude  -I./mxml -Idrivers -Llinux64/lib -DINCLUDE_FTPLIB   -D_LARGEFILE64_SOURCE -DHAVE_MYSQL -DHAVE_ROOT -pthread -m64 -I/home/daq/AlcapDAQ/root/include/root -DOS_LINUX -fPIC -Wno-unused-function -o linux64/bin/mdump utils/mdump.c -lmidas -Wl,-rpath,/usr/local/lib -lz -lutil -lpthread /usr/lib64/mysql/libmysqlclient.so -lz
utils/mdump.c: In function ‘replog’:
utils/mdump.c:237: warning: format ‘%ld’ expects type ‘long int’, but argument 4 has type ‘DWORD’
utils/mdump.c:237: warning: format ‘%lx’ expects type ‘long unsigned int’, but argument 5 has type ‘DWORD’
utils/mdump.c:237: warning: format ‘%ld’ expects type ‘long int’, but argument 6 has type ‘DWORD’
utils/mdump.c:237: warning: format ‘%lx’ expects type ‘long unsigned int’, but argument 7 has type ‘DWORD’
utils/mdump.c: In function ‘process_event’:
utils/mdump.c:336: warning: format ‘%li’ expects type ‘long int’, but argument 4 has type ‘DWORD’
utils/mdump.c:336: warning: format ‘%lx’ expects type ‘long unsigned int’, but argument 5 has type ‘DWORD’
utils/mdump.c:336: warning: format ‘%li’ expects type ‘long int’, but argument 6 has type ‘DWORD’
utils/mdump.c:336: warning: format ‘%lx’ expects type ‘long unsigned int’, but argument 7 has type ‘DWORD’
utils/mdump.c:340: warning: format ‘%li’ expects type ‘long int’, but argument 4 has type ‘DWORD’
utils/mdump.c:340: warning: format ‘%lx’ expects type ‘long unsigned int’, but argument 5 has type ‘DWORD’
utils/mdump.c:340: warning: format ‘%li’ expects type ‘long int’, but argument 6 has type ‘DWORD’
utils/mdump.c:340: warning: format ‘%lx’ expects type ‘long unsigned int’, but argument 7 has type ‘DWORD’
utils/mdump.c:343: warning: format ‘%li’ expects type ‘long int’, but argument 3 has type ‘DWORD’
utils/mdump.c:373: warning: format ‘%li’ expects type ‘long int’, but argument 3 has type ‘DWORD’
utils/mdump.c:373: warning: format ‘%li’ expects type ‘long int’, but argument 4 has type ‘DWORD’
utils/mdump.c:384: warning: format ‘%ld’ expects type ‘long int’, but argument 4 has type ‘DWORD’
utils/mdump.c:384: warning: format ‘%lx’ expects type ‘long unsigned int’, but argument 5 has type ‘DWORD’
utils/mdump.c:384: warning: format ‘%ld’ expects type ‘long int’, but argument 6 has type ‘DWORD’
utils/mdump.c:384: warning: format ‘%lx’ expects type ‘long unsigned int’, but argument 7 has type ‘DWORD’
utils/mdump.c:411: warning: format ‘%ld’ expects type ‘long int’, but argument 4 has type ‘DWORD’
utils/mdump.c:411: warning: format ‘%lx’ expects type ‘long unsigned int’, but argument 5 has type ‘DWORD’
utils/mdump.c:411: warning: format ‘%ld’ expects type ‘long int’, but argument 6 has type ‘DWORD’
utils/mdump.c:411: warning: format ‘%lx’ expects type ‘long unsigned int’, but argument 7 has type ‘DWORD’
cc -g -O0 -Wall -Wuninitialized -Iinclude  -I./mxml -Idrivers -Llinux64/lib -DINCLUDE_FTPLIB   -D_LARGEFILE64_SOURCE -DHAVE_MYSQL -DHAVE_ROOT -pthread -m64 -I/home/daq/AlcapDAQ/root/include/root -DOS_LINUX -fPIC -Wno-unused-function -o linux64/bin/lazylogger src/lazylogger.c -lmidas -Wl,-rpath,/usr/local/lib -lz -lutil -lpthread /usr/lib64/mysql/libmysqlclient.so -lz
src/lazylogger.c: In function ‘lazy_log_update’:
src/lazylogger.c:416: warning: format ‘%ld’ expects type ‘long int’, but argument 4 has type ‘DWORD’
cc -g -O0 -Wall -Wuninitialized -Iinclude  -I./mxml -Idrivers -Llinux64/lib -DINCLUDE_FTPLIB   -D_LARGEFILE64_SOURCE -DHAVE_MYSQL -DHAVE_ROOT -pthread -m64 -I/home/daq/AlcapDAQ/root/include/root -DOS_LINUX -fPIC -Wno-unused-function -o linux64/bin/mchart utils/mchart.c -lmidas -Wl,-rpath,/usr/local/lib -lutil -lpthread /usr/lib64/mysql/libmysqlclient.so -lz
utils/mchart.c: In function ‘mchart_get_names’:
utils/mchart.c:190: warning: operation on ‘p’ may be undefined
utils/mchart.c: In function ‘mchart_get_array’:
utils/mchart.c:299: warning: format ‘%ld’ expects type ‘long int’, but argument 4 has type ‘DWORD’
cp -f utils/stripchart.tcl linux64/bin/. 
cc -g -O0 -Wall -Wuninitialized -Iinclude  -I./mxml -Idrivers -Llinux64/lib -DINCLUDE_FTPLIB   -D_LARGEFILE64_SOURCE -DHAVE_MYSQL -DHAVE_ROOT -pthread -m64 -I/home/daq/AlcapDAQ/root/include/root -DOS_LINUX -fPIC -Wno-unused-function -o linux64/bin/webpaw utils/webpaw.c -lmidas -Wl,-rpath,/usr/local/lib -lutil -lpthread /usr/lib64/mysql/libmysqlclient.so -lz
utils/webpaw.c: In function ‘server_loop’:
utils/webpaw.c:1542: warning: pointer targets in passing argument 3 of ‘accept’ differ in signedness
/usr/include/sys/socket.h:214: note: expected ‘socklen_t * __restrict__’ but argument is of type ‘int *’
utils/webpaw.c: In function ‘create_password’:
utils/webpaw.c:1729: warning: cast from pointer to integer of different size
utils/webpaw.c:1729: warning: cast from pointer to integer of different size
utils/webpaw.c:1745: warning: cast from pointer to integer of different size
utils/webpaw.c:1745: warning: cast from pointer to integer of different size
cc -g -O0 -Wall -Wuninitialized -Iinclude  -I./mxml -Idrivers -Llinux64/lib -DINCLUDE_FTPLIB   -D_LARGEFILE64_SOURCE -DHAVE_MYSQL -DHAVE_ROOT -pthread -m64 -I/home/daq/AlcapDAQ/root/include/root -DOS_LINUX -fPIC -Wno-unused-function -o linux64/bin/odbhist utils/odbhist.c -lmidas -Wl,-rpath,/usr/local/lib -lutil -lpthread /usr/lib64/mysql/libmysqlclient.so -lz
cc -g -O0 -Wall -Wuninitialized -Iinclude  -I./mxml -Idrivers -Llinux64/lib -DINCLUDE_FTPLIB   -D_LARGEFILE64_SOURCE -DHAVE_MYSQL -DHAVE_ROOT -pthread -m64 -I/home/daq/AlcapDAQ/root/include/root -DOS_LINUX -fPIC -Wno-unused-function -o linux64/bin/melog utils/melog.c -lmidas -Wl,-rpath,/usr/local/lib -lutil -lpthread /usr/lib64/mysql/libmysqlclient.so -lz
cc -g -O0 -Wall -Wuninitialized -Iinclude  -I./mxml -Idrivers -Llinux64/lib -DINCLUDE_FTPLIB   -D_LARGEFILE64_SOURCE -DHAVE_MYSQL -DHAVE_ROOT -pthread -m64 -I/home/daq/AlcapDAQ/root/include/root -DOS_LINUX -fPIC -Wno-unused-function -o linux64/bin/mlxspeaker utils/mlxspeaker.c -lmidas -Wl,-rpath,/usr/local/lib -lutil -lpthread /usr/lib64/mysql/libmysqlclient.so -lz
utils/mlxspeaker.c: In function ‘receive_message’:
utils/mlxspeaker.c:101: warning: format ‘%li’ expects type ‘long int’, but argument 4 has type ‘DWORD’
utils/mlxspeaker.c:101: warning: format ‘%ld’ expects type ‘long int’, but argument 5 has type ‘DWORD’
cc -g -O0 -Wall -Wuninitialized -Iinclude  -I./mxml -Idrivers -Llinux64/lib -DINCLUDE_FTPLIB   -D_LARGEFILE64_SOURCE -DHAVE_MYSQL -DHAVE_ROOT -pthread -m64 -I/home/daq/AlcapDAQ/root/include/root -DOS_LINUX -fPIC -Wno-unused-function -o linux64/bin/dio utils/dio.c
g++ -g -O0 -Wall -Wuninitialized -Iinclude  -I./mxml -Idrivers -Llinux64/lib -DINCLUDE_FTPLIB   -D_LARGEFILE64_SOURCE -DHAVE_MYSQL -DHAVE_ROOT -pthread -m64 -I/home/daq/AlcapDAQ/root/include/root -DOS_LINUX -fPIC -Wno-unused-function -pthread -m64 -I/home/daq/AlcapDAQ/root/include/root -o linux64/bin/rmidas src/rmidas.c -lmidas -Wl,-rpath,/usr/local/lib -L/home/daq/AlcapDAQ/root/lib/root -lGui -lCore -lCint -lRIO -lNet -lHist -lGraf -lGraf3d -lGpad -lTree -lRint -lPostscript -lMatrix -lPhysics -lMathCore -lThread -pthread -lm -ldl -rdynamic -Wl,-rpath,/home/daq/AlcapDAQ/root-install/lib -lutil -lpthread /usr/lib64/mysql/libmysqlclient.so -lz
src/rmidas.c: In member function ‘int RMidasFrame::ConnectServer()’:
src/rmidas.c:308: warning: deprecated conversion from string constant to ‘char*’
src/rmidas.c: In function ‘void rmidas()’:
src/rmidas.c:580: warning: deprecated conversion from string constant to ‘char*’
