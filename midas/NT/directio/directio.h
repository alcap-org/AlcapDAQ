// directnt.h interface
#define DIRECTIO_TYPE               40000
#define IOCTL_DIRECTIO_CONTROL      CTL_CODE(DIRECTIO_TYPE,0x0800,\
                                    METHOD_BUFFERED, FILE_READ_ACCESS)

#define OP_Check              1 // gibt immer $12345678 zurueck
#define OP_ReadPortByte       2 // gibt Byte-Inhalt von Port #Par1# zurueck
#define OP_WritePortByte      3 // schreibt Byte #Par2# nach Port #Par1#
#define	OP_ReadPhysMemDword   4 // Ein physikalisches Dword vom Speicher lesen
#define	OP_WritePhysMemDword  5 // Ein physikalisches Dword vom Speicher lesen
#define OP_GiveIO             6 // schaltet Ports #Par1# bis #Par2# frei
#define OP_LockIO             7 // blockiert Ports #Par1# bis #Par2#
#define OP_PCIInfo            8 // gibt PCI IO info zurueck
