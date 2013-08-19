/*-----------------------------------------------------------------------------
 *  Copyright (c) 1993      TRIUMF Data Acquistion Group
 *  Please leave this header in any reproduction of that distribution
 * 
 *  TRIUMF Data Acquisition Group, 4004 Wesbrook Mall, Vancouver, B.C. V6T 2A3
 *  Email: online@triumf.ca           Tel: (604) 222-1047  Fax: (604) 222-1074
 *         amaudruz@triumf.ca
 * -----------------------------------------------------------------------------
 *  Description	: CAMAC macro for the CES8210.
 *		  The CAMAC controller is addressed through the f0 page.
 * 		  The base address is set to BASE (0x00800000).
 *	 	  for Interrupt vector see cam8210.c
 * 
 * Application : VME
 *
 * contains the ESONE basic command too:
 * void cdreg (int *ext, int b, int c, int n, int a)
 * void cfsa (int f, int ext, int f, int *data, int *q)
 * void cssa (int f, int ext, int f, short *data, int *q)
 * void ctstat(int *istat) 0=QX, 1=nQX, 2=QnX, 3=nQnX
 *
 *
 * CAM_CSRCHK(w)            
 * CAM_CARCHK(w)            
 * CAM_BTBCHK(w)            
 * CAM_QXCHK(q,x)            
 *
 * CAM_BC(&ext,b,c)          
 * CAM_N(&ext,n) 
 * CAM_A(&ext,a) 
 * CAM_F(&ext,f) 
 * CAM_Ncl(&ext,n)
 * CAM_Acl(&ext,a)
 * CAM_Fcl(&ext,f)
 *
 * CAM16_I(&ext,n,a,f,&d)           : read single word (no qx)
 * CAM16_IQX(&ext,n,a,f,&d,q,x)     : read single word + qx check
 * CAM16_IR(&ext,n,a,f,&d,r)        : read r words (no qx)
 * CAM16_IRQ(&ext,n,a,f,&d,r)       : read r words until no Q
 * CAM16_IAS(&ext,n,a,f,&d,r)       : read r word scanning A
 * CAM16_IASQ(&ext,n,a,f,&d,r)      : read max r word or until noQ while scanning A
 *
 * CAM24_I(&ext,n,a,f,&d)
 * CAM24_IQX(&ext,n,a,f,&d,q,x)
 * CAM24_IR(&ext,n,a,f,&d,r)
 * CAM24_IRQ(&ext,n,a,f,&d,r)
 * CAM24_IAS(&ext,n,a,f,&d,r)
 * CAM24_INS(&ext,n,a,f,&d,r)
 *
 * CAM16_O(&ext,n,a,f,d)
 * CAM16_OQX(&ext,n,a,f,d,q,x)
 * CAM24_O(&ext,n,a,f,d)
 * CAM24_OQX(&ext,n,a,f,d,q,x)
 *
 * CAM_COM(b,c,n,a,f)
 * CAM_CMD(&ext,n,a,f)
 * CAM_CMDAS(&ext,n,a,f,r)
 * CAM_CMDNS(&ext,n,a,f,r)
 *
 *  Requires 	: Vxworks lib if sysBusToLocalAdrs() used
 *     
 *  Application : VME
 *
 *  Author:  Pierre-Andre Amaudruz Data Acquisition Group
 * 
 *  Revision 1.0  1996/Oct/2 Pierre	 Initial revision
 *
 *---------------------------------------------------------------------------*/
#ifndef _ces8210_h_
#define _ces8210_h_
#define BASE	       0x00800000       /* camac base address */
#define A32D24	       0xf0000000       /* A32D24 camac base address */
#define A32D16	       0xf0000002       /* A32D16 camac base address */
#define CSR_OFFSET     0x0000e800       /* camac control and status reg */
#define CAR_OFFSET     0x0000e820       /* Crate Address Register */
#define BTB_OFFSET     0x0000e824       /* on-line status (R) */
#define INT_OFFSET     0x0000e810
#define GL             0x0000e828       /* GL register */

#define GLR     BASE |     GL     | A32D24
#define CSR     BASE | CSR_OFFSET | A32D16
#define CAR     BASE | CAR_OFFSET | A32D16
#define BTB     BASE | BTB_OFFSET | A32D16
#define IFR     BASE | INT_OFFSET | A32D16
/* Control and Status Register */
#define IT4	  0x0001        /* front panel int IT4 status */
#define IT2	  0x0002        /* front panel int IT2 status */
#define MIT4	0x0004          /* IT4 mask 0 = enabled */
#define MIT2	0x0008          /* IT2 mask 0 = enabled */
#endif

/*-----------------------------------------------------------------*/
#define CAM_BC(_ext,_b,_c){\
			     DWORD cbd_base = 0xf0000000 | BASE;\
			     {\
				DWORD __w;\
				CAM_BTBCHK (&__w);\
				if ( ((__w >> _c) & 0x1) != 1)\
				{\
				   printf("crate %i not existant\n",_c);\
				   *_ext = 0;\
			       }\
				else\
				*_ext = cbd_base|_b<<19|_c<<16;}\
			 }

/* OR the given field to the original address */
#define CAM_N(_ext,_n)  { *_ext |= _n<<11;}
#define CAM_A(_ext,_a)  { *_ext |= _a<<7;}
#define CAM_F(_ext,_f)  { *_ext |= _f<<2;}

/* update only the given field without touching the rest of the address */
#define CAM_Ncl(_ext,_n){ *_ext = ((*_ext & ~(0x1f << 11)) | (_n << 11));}
#define CAM_Acl(_ext,_a){ *_ext = ((*_ext & ~(0xf  <<  7)) | (_a <<  7));}
#define CAM_Fcl(_ext,_f){ *_ext = ((*_ext & ~(0x1f <<  2)) | (_f <<  2));}

/*-----------------------------------------------------------------*/
#define CAM_CSRCHK(_w){\
			 DWORD cbd_csr = CSR;\
			 *_w = *(WORD *) cbd_csr;}

#define CAM_CARCHK(_w){\
			 DWORD cbd_car = CAR;\
			 *_w = *(WORD *) cbd_car;}

#define CAM_BTBCHK(_w){\
			 DWORD cbd_btb = BTB;\
			 *_w = *(WORD *) cbd_btb;}

#define CAM_GLCHK(_w){\
			 DWORD cbd_gl = GLR;\
			 *_w = *(DWORD *) cbd_gl;}

#define CAM_QXCHK(_q,_x){\
			   DWORD cbd_csr = CSR;\
			   WORD csr = *(WORD *) cbd_csr;\
			   *_q = ((csr & 0x8000)>>15);\
			   *_x = ((csr & 0x4000)>>14);}

#define CAM16_I(_ext,_n,_a,_f,_d){\
				    *_ext &= 0xffff0000;\
				    CAM_N(_ext,_n);\
				    CAM_A(_ext,_a);\
				    CAM_F(_ext,_f);\
				    *(WORD *) _d = *(WORD *) (*_ext | 0x2);}

#define CAM16_IQX(_ext,_n,_a,_f,_d,_q,_x){\
					    CAM16_I(_ext,_n,_a,_f,_d);\
					    CAM_QXCHK(_q,_x);}

#define CAM16_IR(_ext,_n,_a,_f,_d,_r){\
					*_ext &= 0xffff0000;\
					CAM_N(_ext,_n);\
					CAM_F(_ext,_f);\
					CAM_A(_ext,_a);\
					{\
					   DWORD rr;\
					   rr=_r;\
					   do {\
						 *(WORD *) _d = *(WORD *) (*_ext | 0x2);\
						 ((WORD *) _d)++;\
						 rr--;} while (rr > 0);\
				       }\
				    }

#define CAM16_IRQ(_ext,_n,_a,_f,_d,_r){\
					 *_ext &= 0xffff0000;\
					 CAM_N(_ext,_n);\
					 CAM_A(_ext,_a);\
					 CAM_F(_ext,_f);\
					 {\
					    DWORD rr = _r;\
					    WORD csr;\
					    DWORD cbd_csr = CSR;\
					    do {\
						  *(WORD *) _d = *(WORD *) (*_ext | 0x2);\
						  csr = *(WORD *) cbd_csr;\
						  if ((csr & 0x8000) == 0) {\
									      rr=0;\
									  }\
						  else {\
							  rr--;\
							  ((WORD *) _d)++; }\
					      } while (rr > 0);}\
				     }

#define CAM16_IAS(_ext,_n,_a,_f,_d,_r){\
					 *_ext &= 0xffff0000;\
					 CAM_N(_ext,_n);\
					 CAM_F(_ext,_f);\
					 {\
					    DWORD rr, aa, cbd_csr;\
					    WORD csr;\
					    aa=_a;\
					    rr=_r;\
					    do\
					    {\
					       CAM_Acl(_ext,aa);\
					       *(WORD *) _d = *(WORD *) (*_ext | 0x2);\
					       ((WORD *) _d)++;\
					       rr--;\
					       aa++;\
					   } while (rr > 0);}\
				     }

#define CAM16_IASQ(_ext,_n,_a,_f,_d,_r){\
					  *_ext &= 0xffff0000;\
					  CAM_N(_ext,_n);\
					  CAM_F(_ext,_f);\
					  {\
					     DWORD rr = _r;\
					     DWORD aa;\
					     WORD csr;\
					     DWORD cbd_csr = CSR;\
					     aa=_a;\
					     rr=_r;\
					     do\
					     {\
						CAM_Acl(_ext,aa);\
						*(WORD *) _d = *(WORD *) (*_ext | 0x2);\
						csr = *(WORD *) cbd_csr;\
						if ((csr & 0x8000) == 0) {\
									    rr=0;\
									}\
						else {\
							rr--;\
							aa++;\
							((WORD *) _d)++;}\
					    } while (rr > 0);}\
				      }

#define CAM24_I(_ext,_n,_a,_f,_d){\
				    *_ext &= 0xffff0000;\
				    CAM_N(_ext,_n);\
				    CAM_A(_ext,_a);\
				    CAM_F(_ext,_f);\
	         *(DWORD *) _d = *(DWORD *) *_ext;}

#define CAM24_IQX(_ext,_n,_a,_f,_d,_q,_x){\
					    CAM24_I(_ext,_n,_a,_f,_d);\
					    CAM_QXCHK(_q,_x);}

#define CAM24_IR(_ext,_n,_a,_f,_d,_r){\
					*_ext &= 0xffff0000;\
					CAM_N(_ext,_n);\
					CAM_A(_ext,_a);\
					CAM_F(_ext,_f);\
					{\
					   DWORD rr;\
					   rr=_r;\
					   do {\
						 *((DWORD *)_d)++ = *(DWORD *) *_ext;\
						 rr--;}\
					   while (rr > 0);}\
				    }

#define CAM24_IRQ(_ext,_n,_a,_f,_d,_r){\
					 *_ext &= 0xffff0000;\
					 CAM_N(_ext,_n);\
					 CAM_A(_ext,_a);\
					 CAM_F(_ext,_f);\
					 {\
					    WORD csr;\
					    DWORD cbd_csr = CSR;\
					    DWORD rr=_r;\
					    do {\
						  *((DWORD *)_d) = *(DWORD *) *_ext;\
						  csr = *(WORD *) cbd_csr;\
						  if ((csr & 0x8000) == 0) {\
									      rr=0;\
									  }\
						  else {\
							  rr--;\
							  (DWORD *) _d++; }\
					      } while (rr > 0); }\
				     }

#define CAM24_IAS(_ext,_n,_a,_f,_d,_r){\
					 *_ext &= 0xffff0000;\
					 CAM_N(_ext,_n);\
					 CAM_F(_ext,_f);\
					 {\
					    DWORD rr,aa;\
					    WORD csr;\
					    aa=_a;\
					    rr=_r;\
					    do {\
						  CAM_Acl(_ext,aa);\
						  *(DWORD *) _d++ = *(DWORD *) *_ext;\
						  rr--;\
						  aa++;\
					} while (rr > 0); }\
				     }

#define CAM24_INS(_ext,_n,_a,_f,_d,_r){\
					 *_ext &= 0xffff0000;\
					 CAM_A(_ext,_a);\
					 CAM_F(_ext,_f);\
					 {\
					    DWORD rr, nn;\
					    WORD csr;\
					    nn=_n;\
					    rr=_r;\
					    do {\
						  CAM_Ncl(_ext,nn);\
						  *(DWORD *) _d++ = *(DWORD *) *_ext;\
						  rr--;\
						  nn++;\
					} while (rr > 0); }\
				     }

#define CAM16_O(_ext,_n,_a,_f,_d){\
				    *_ext &= 0xffff0000;\
				    CAM_N(_ext,_n);\
				    CAM_A(_ext,_a);\
				    CAM_F(_ext,_f);\
				    *(WORD *) (*_ext | 0x2) =  (WORD) _d;}

#define CAM16_OQX(_ext,_n,_a,_f,_d,_q,_x){\
					    CAM16_O(_ext,_n,_a,_f,_d);\
					    CAM_QXCHK(_q,_x);}

#define CAM24_O(_ext,_n,_a,_f,_d){\
				    *_ext &= 0xffff0000;\
				    CAM_N(_ext,_n);\
				    CAM_A(_ext,_a);\
				    CAM_F(_ext,_f);\
				    *(DWORD *) (*_ext) = (DWORD) _d;}

#define CAM24_OQX(_ext,_n,_a,_f,_d,_q,_x){\
					    CAM24_O(_ext,_n,_a,_f,_d);\
					    CAM_QXCHK(_q,_x);}
#define CAM_COM(_b,_c,_n,_a,_f){\
				  {\
				     static DWORD __ext, __d;\
				     CAM_BC(&__ext,_b,_c);\
				     CAM_N(&__ext,_n);\
				     CAM_A(&__ext,_a);\
				     CAM_F(&__ext,_f);\
				     __d = *(DWORD *) __ext;\
				 }\
			      }

#define CAM_CMD(_ext,_n,_a,_f){\
			       *_ext &= 0xffff0000;\
			       {\
				  static DWORD __d;\
				  CAM_N(_ext,_n);\
				  CAM_A(_ext,_a);\
				  CAM_F(_ext,_f);\
				  __d = *(DWORD *) *_ext;}\
			   }

#define CAM_CMDAS(_ext,_n,_a,_f,_r){\
				      *_ext &= 0xffff0000;\
				      {\
					 static DWORD aa, rr, __d;\
					 CAM_N(_ext,_n);\
					 CAM_F(_ext,_f);\
					 rr = _r;\
					 aa = _a;\
					 do {\
					       CAM_Acl(_ext,aa);\
					       __d = *(DWORD *) *_ext;\
					       rr--;\
					       aa++;} while (rr > 0);\
				     }\
				  }

#define CAM_CMDNS(_ext,_n,_a,_f,_r){\
				      *_ext &= 0xffff0000;\
				      {\
					 static DWORD nn, rr, __d;\
					 CAM_A(_ext,_a);\
					 CAM_F(_ext,_f);\
					 rr = _r;\
					 nn = _n;\
					 do {\
					       CAM_Ncl(_ext,nn);\
					       __d = *(DWORD *) *_ext;\
					       rr--;\
					       nn++;} while (rr > 0);\
				     }\
				  }

/*-----------------------------------------------------------------*/
void cdreg(int *ext, int b, int c, int n, int a);
void cfsa(int f, int ext, int *data, int *q);
void cssa(int f, int ext, short *data, int *q);
void ctstat(int *istat);
