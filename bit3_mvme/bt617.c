// ****************************************************************************
// *     Filename:  bt617.c
// *  Description:  Midas MVME standard layer, Bit3/SBS 617/618/620
// *   Created by:  Nam Tran (nam at kuno-g.phys.sci.osaka-u.ac.jp)
// *     Time tag:  2013-03-05 18:15:17
// ****************************************************************************
#ifdef __linux__
#ifndef OS_LINUX
#define OS_LINUX
#endif
#endif

#ifdef OS_LINUX
#include "bt617.h"
#endif //OS_LINUX

#define DEV_FILE "/dev/vmedrv" // default access mode 0x09 - A32, data D32

int mvme_open(MVME_INTERFACE **vme, int idx)
{
	*vme = (MVME_INTERFACE *) malloc(sizeof(MVME_INTERFACE));
	if (*vme == NULL)
		return MVME_NO_MEM;
	memset(*vme, 0, sizeof(MVME_INTERFACE));

	int fd;
	if ((fd = open(DEV_FILE, O_RDWR))<0)
	{
		perror("ERROR: fd open()");
		exit(EXIT_FAILURE);
	}

	(*vme)->handle = fd;
	(*vme)->am = MVME_AM_A32_ND;
	(*vme)->dmode = MVME_DMODE_D32;
	(*vme)->initialized = 1;
	bt617_SetAccessMode(*vme);

	(*vme)->blt_mode = MVME_BLT_NONE;
	(*vme)->table = NULL;

	return MVME_SUCCESS;
}

int mvme_close(MVME_INTERFACE *vme)
{
	close(vme->handle);
	return MVME_SUCCESS;
}

void show_mvme(MVME_INTERFACE *vme)
{
	printf("mvme properties:\n");
	printf("init: %d\n",vme->initialized);
	printf("handl: %d\n",vme->handle);
	printf("index: %d\n",vme->index);
	printf("am: %X\n",vme->am);
	printf("dw: %d\n",vme->dmode);
}

vmedrv_access_mode_t bt617_GetAccessMode(MVME_INTERFACE *vme)
{
	int dmode = vme->dmode;
	int am = vme->am;
	int enum_am, enum_dm;
	switch (am)
	{
		case MVME_AM_A32_ND:
			enum_am = 0;
			break;
		case MVME_AM_A24_ND:
			enum_am = 1;
			break;
		case MVME_AM_A16_ND:
			enum_am = 2;
			break;
		default:
			enum_am = 0;
			break;
	}

	switch (dmode)
	{
		case MVME_DMODE_D32:
			enum_dm = 0;
			break;
		case MVME_DMODE_D16:
			enum_dm = 1;
			break;
		default:
			enum_dm = 0;
			break;
	}

	static vmedrv_access_mode_t access_mode_tab[3][2] = {
		{VMEDRV_A32D32,VMEDRV_A32D16},
		{VMEDRV_A24D32,VMEDRV_A24D16},
		{VMEDRV_A16D32,VMEDRV_A16D16}
	};
	return access_mode_tab[enum_am][enum_dm];
}

void bt617_SetAccessMode(MVME_INTERFACE *vme)
{
	vmedrv_access_mode_t amode = bt617_GetAccessMode(vme);
	ioctl(vme->handle,VMEDRV_IOC_SET_ACCESS_MODE,&amode);
}

int mvme_set_am(MVME_INTERFACE *vme, int am)
{
	vme->am = am;
	bt617_SetAccessMode(vme);
	return 0;
}

int mvme_get_am(MVME_INTERFACE *vme, int *am)
{
	*am = vme->am;
	return 0;
}

int mvme_set_dmode(MVME_INTERFACE *vme, int dm)
{
	vme->dmode = dm;
	bt617_SetAccessMode(vme);
	return 0;
}

int mvme_set_blt(MVME_INTERFACE *vme, int mode)
{
	vme->blt_mode = mode;
	if (mode == MVME_BLT_NONE)
	{
		bt617_SetBLT(vme, false);
	}
	else
	{
		bt617_SetBLT(vme, true);
	}
	return 0;
}

int mvme_get_blt(MVME_INTERFACE *vme, int *mode)
{
	*mode = vme->blt_mode;
	return 0;
}

int mvme_get_dmode(MVME_INTERFACE *vme, int *dm)
{
	*dm = vme->dmode;
	return 0;
}

int mvme_read(MVME_INTERFACE *vme, void *dst, 
		mvme_addr_t vme_addr, mvme_size_t n_bytes)
{
	int read_size;
	if(lseek(vme->handle,vme_addr,SEEK_SET) == -1)
	{
		printf("lseek err\n");
		return -1;
	}
	read_size = read(vme->handle,dst,n_bytes);

	if (read_size < 0)
	{
		printf("read err\n");
		return -1;
	}
	else 
		return read_size;
}

int mvme_write(MVME_INTERFACE *vme, mvme_addr_t vme_addr, void *src, mvme_size_t n_bytes)
{
	int write_size;
	if(lseek(vme->handle,vme_addr,SEEK_SET) == -1)
	{
		printf("lseek err\n");
		return -1;
	}
	write_size = write(vme->handle,src,n_bytes);
	if (write_size < 0)
	{
		printf("write err\n");
		return -1;
	}
	else 
		return write_size;
}

unsigned int mvme_read_value(MVME_INTERFACE *vme, mvme_addr_t vme_addr)
{
	int read_size,nbytes;
	unsigned int value;
	switch (vme->dmode)
	{
		case MVME_DMODE_D32:
			nbytes = 4;
			break;
		case MVME_DMODE_D16:
			nbytes = 2;
			break;
		default:
			nbytes = 4;
			break;
	}
	read_size = mvme_read(vme,&value,vme_addr,nbytes);
	return value;
}

int mvme_write_value(MVME_INTERFACE *vme, mvme_addr_t vme_addr, unsigned int value)
{
	int write_size,nbytes;
	switch (vme->dmode)
	{
		case MVME_DMODE_D32:
			nbytes = 4;
			break;
		case MVME_DMODE_D16:
			nbytes = 2;
			break;
		default:
			nbytes = 4;
			break;
	}
	write_size = mvme_write(vme,vme_addr,&value,nbytes);
	if (write_size < 0)
	{
		printf("write_value err \n");
		return -1;
	}
	else 
		return 0;
}

void showbits(uint32_t data)
{
 int ii;
    for (ii=31;ii>=28;ii--)
        (data&(1<<ii))?putchar('1'):putchar('0');
    putchar(' ');

    for (ii=27;ii>=24;ii--) 
        (data&(1<<ii))?putchar('1'):putchar('0');
    putchar(' ');

    for (ii=23;ii>=20;ii--) 
        (data&(1<<ii))?putchar('1'):putchar('0');
    putchar(' ');

    for (ii=19;ii>=16;ii--) 
        (data&(1<<ii))?putchar('1'):putchar('0');
    putchar(' ');

    for (ii=15;ii>=12;ii--) 
        (data&(1<<ii))?putchar('1'):putchar('0');
    putchar(' ');

    for (ii=11;ii>=8;ii--) 
        (data&(1<<ii))?putchar('1'):putchar('0');
    putchar(' ');

    for (ii=7;ii>=4;ii--) 
        (data&(1<<ii))?putchar('1'):putchar('0');
    putchar(' ');

    for (ii=3;ii>=0;ii--) 
        (data&(1<<ii))?putchar('1'):putchar('0');
    putchar('\n');
}

void bt617_SetBLT(MVME_INTERFACE *vme, bool blt)
{
	if (blt)
	{
		int transfer_method = VMEDRV_DMA;
		ioctl(vme->handle, VMEDRV_IOC_SET_TRANSFER_METHOD, &transfer_method);
	}
	else
	{
		int transfer_method = VMEDRV_PIO;
		ioctl(vme->handle, VMEDRV_IOC_SET_TRANSFER_METHOD, &transfer_method);

	}
}
//#define TEST_INTERFACE
#ifdef TEST_INTERFACE
#define V1290N_BASE 0x0040000
#define RPV130_BASE 0x8000
#include "rpv130.h"

int main()
{
	printf("Main: \n");
	MVME_INTERFACE *myvme;
	mvme_open(&myvme, 0);
	//show_mvme(myvme);

	// RPV130, A16 only
	myvme->am = MVME_AM_A16_ND;
	myvme->dmode = MVME_DMODE_D16;
	bt617_SetAccessMode(myvme);
	//show_mvme(myvme);
	unsigned short int reg;
	reg = mvme_read_value(myvme,RPV130_CSR_1 + RPV130_BASE);
	printf("CSR1: %X \n",reg);

	reg = 0x3;
	mvme_write_value(myvme,RPV130_BASE + RPV130_CSR_1,0x3); // reset busy flag
	reg = mvme_read_value(myvme,RPV130_CSR_1 + RPV130_BASE);
	printf("CSR1: %X \n",reg);

	sleep(1);
	mvme_write_value(myvme, RPV130_BASE + RPV130_PULSE,0x1); // pulse to set busy
	reg = mvme_read_value(myvme,RPV130_CSR_1 + RPV130_BASE);
	printf("CSR1: %X \n",reg);

	// Finish
	mvme_close(myvme);
	return 0;
}
#endif

