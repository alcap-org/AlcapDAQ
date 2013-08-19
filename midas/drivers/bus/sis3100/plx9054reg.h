/* $ZEL: plx9054reg.h,v 1.2 2001/09/04 08:47:53 wuestner Exp $ */

/*
 * Copyright (c) 2001
 * 	Matthias Drochner.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions, and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#ifndef _plx9054reg_h_
#define _plx9054reg_h_

struct plx9045reg {
	U32 LAS0RR, LAS0BA, MARBR, BIGEND_LMISC_PROT_AREA;
	U32 EROMRR, EROMBA, LBRD0, DMRR;
	U32 DMLBAM, DMLDAI, DMPBAM, DMCFGA;
/* 30 */
	U32 OPQIS, OPQIM, _dummy1, _dummy2;
/* 40 */
	U32 IQP, OQP, MBOX2, MBOX3;
	U32 MBOX4, MBOX5, MBOX6, MBOX7;
	U32 P2LDBELL, L2PDBELL, INTCSR, CNTRL;
	U32 PCIHIDR, PCIHREV, MBOX0, MBOX1;
/* 80 */
	U32 DMAMODE0, DMAPADR0, DMALADR0, DMASIZ0;
	U32 DMADPR0, DMAMODE1, DMAPADR1, DMALADR1;
	U32 DMASIZ1, DMADPR1, DMACSR0_DMACSR1, DMAARB;
	U32 DMATHR, DMADAC0, DMADAC1, _dummy3;
/* c0 */
	U32 MQCR, QBAR, IFHPR, IFTPR;
	U32 IPHPR, IPTPR, OFHPR, OFTPR;
	U32 OPHPR, OPTPR, QSR, _dummy4;
/* f0 */
	U32 LAS1RR, LAS1BA, LBRD1, DMDAC;
};

struct plx9054_dmadesc {
	U32 pcistart, localstart, size, next;
};

                               /* to clear the interrupt you must ... */
#define plxirq_mbox     (1<< 3) /* read the mailbox */
#define plxirq_pci      (1<< 8)
#define plxirq_doorbell (1<< 9) /* clear the doorbell bits */
#define plxirq_local    (1<<11) /* clear the local irq source */
#define plxirq_dma0     (1<<18) /* clear the dma status bits */
#define plxirq_dma1     (1<<19) /* clear the dma status bits */

#define plxirq_doorbell_active (1<<20)
#define plxirq_dma0_active     (1<<21)
#define plxirq_dma1_active     (1<<22)

#endif
