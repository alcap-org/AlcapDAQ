// ****************************************************************************
// *     Filename:  rpv130.c
// *  Description:  
// *   Created by:  Nam Tran (nam at kuno-g.phys.sci.osaka-u.ac.jp)
// *     Time tag:  2013-04-06 13:55:20
// ****************************************************************************
// ****************************************************************************
#include "rpv130.h"
#include "bt617.h"

int rpv130_Clear(MVME_INTERFACE *vme, mvme_addr_t base_addr)
{
	mvme_set_dmode(vme, RPV130_DATA_SIZE);
	mvme_set_am(vme,RPV130_AM);

	mvme_write_value(vme,base_addr + RPV130_CSR_1, 0x3);
	mvme_write_value(vme,base_addr + RPV130_CSR_2, 0x3);

	return 0;
}

int rpv130_IsBusy1(MVME_INTERFACE *vme, mvme_addr_t base_addr)
{
	mvme_set_dmode(vme, RPV130_DATA_SIZE);
	mvme_set_am(vme,RPV130_AM);

	unsigned short int reg = mvme_read_value(vme,base_addr + RPV130_CSR_1);

	return ((reg & (1<<5))>>5);
}

int rpv130_IsBusy2(MVME_INTERFACE *vme, mvme_addr_t base_addr)
{
	mvme_set_dmode(vme, RPV130_DATA_SIZE);
	mvme_set_am(vme,RPV130_AM);

	unsigned short int reg = mvme_read_value(vme,base_addr + RPV130_CSR_2);

	return ((reg & (1<<5))>>5);
}

int rpv130_IsBusy3(MVME_INTERFACE *vme, mvme_addr_t base_addr)
{
	mvme_set_dmode(vme, RPV130_DATA_SIZE);
	mvme_set_am(vme,RPV130_AM);

	unsigned short int reg = mvme_read_value(vme,base_addr + RPV130_CSR_2);

	return ((reg & (1<<7))>>7);
}

void rpv130_PrintBusyStatus(MVME_INTERFACE *vme, mvme_addr_t base_addr)
{
	printf("Busy1: %d\n",rpv130_IsBusy1(vme,base_addr));
	printf("Busy2: %d\n",rpv130_IsBusy2(vme,base_addr));
	printf("Busy3: %d\n",rpv130_IsBusy3(vme,base_addr));
}

int rpv130_ClearBusy1(MVME_INTERFACE *vme, mvme_addr_t base_addr)
{
	mvme_set_dmode(vme, RPV130_DATA_SIZE);
	mvme_set_am(vme,RPV130_AM);

	mvme_write_value(vme,base_addr + RPV130_CSR_1, 0x2);

	return 0;
}

int rpv130_ClearBusy2(MVME_INTERFACE *vme, mvme_addr_t base_addr)
{
	mvme_set_dmode(vme, RPV130_DATA_SIZE);
	mvme_set_am(vme,RPV130_AM);
	mvme_write_value(vme,base_addr + RPV130_CSR_2, 0x2);

	return 0;
}

int rpv130_ClearBusy3(MVME_INTERFACE *vme, mvme_addr_t base_addr)
{
	mvme_set_dmode(vme, RPV130_DATA_SIZE);
	mvme_set_am(vme,RPV130_AM);
	mvme_write_value(vme,base_addr + RPV130_CSR_2, 0x3);

	return 0;
}

int rpv130_Pulse(MVME_INTERFACE *vme, mvme_addr_t base_addr, int chn) 
{
	if ((chn < 1) || (chn > 8))
		return -1;

	mvme_set_dmode(vme, RPV130_DATA_SIZE);
	mvme_set_am(vme,RPV130_AM);
	mvme_write_value(vme,base_addr + RPV130_PULSE, (0x1<<(chn-1)));

	return 0;
}
/*#define TEST_RPV130*/
#ifdef TEST_RPV130
#define V1290N_BASE 0x0040000
#define RPV130_BASE 0x8000

int main ()
{
	printf("Test program for RPV130: \n");
	MVME_INTERFACE *myvme;
	mvme_open(&myvme, 0);

	rpv130_PrintBusyStatus(myvme,RPV130_BASE);
	printf("\n");

	rpv130_ClearBusy3(myvme,RPV130_BASE);
	rpv130_PrintBusyStatus(myvme,RPV130_BASE);
	printf("\n");

	sleep(1);
	rpv130_Pulse(myvme,RPV130_BASE,1);
	rpv130_PrintBusyStatus(myvme,RPV130_BASE);

	// Finish
	mvme_close(myvme);
	return 0;
}
#endif
