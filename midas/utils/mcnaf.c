/********************************************************************\

 Name:         mcnaf.c
 Created by:   Pierre-Andre Amaudruz
 
  Contents:     CAMAC utility

    $Log: mcnaf.c,v $
    Revision 1.1.1.1  2005/06/20 23:37:38  mucap
    Importing release 1.9.5 of the MIDAS source code as distributed by PSI.
    (Next, I'll commit our local customizations.)

    Revision 1.20  2004/01/08 08:40:11  midas
    Implemented standard indentation

    Revision 1.19  2003/05/09 07:40:05  midas
    Added extra parameter to cm_get_environment

    Revision 1.18  2003/04/23 23:08:19  pierre
    Fixed compiler warning

    Revision 1.17  2002/06/05 04:12:23  pierre
    pass cmd_flag arg, remove printf

    Revision 1.16  2002/06/05 04:07:30  pierre
    fix @ job

    Revision 1.15  2001/07/20 00:49:29  pierre
    - Added -c option and -c @filename.

    Revision 1.14  2000/11/02 17:54:58  pierre
    - Fix job repeat, uses command RxWx.

    Revision 1.13  2000/09/14 20:23:45  pierre
    -Add cm_get_environment()

    Revision 1.12  2000/08/10 07:49:25  midas
    Added client name together with frontend name in cam_init_rpc

    Revision 1.11  2000/07/25 14:11:33  midas
    Changed "&" to "&&"

    Revision 1.10  2000/04/18 22:07:31  pierre
    - added cam_lam_clear private function.
    - fix cc_services for N >= 28

    Revision 1.9  1999/12/08 00:04:48  pierre
    - Fix mcstd _r, _sa functions

    Revision 1.8  1998/11/23 16:17:37  pierre
    Fix bug: double CAMAC call since [P] implementation (visible in camacrpc)
    CVS:-----------------------------------------------------------------------

    Revision 1.7  1998/11/20 14:29:31  pierre
    Added [P] cmd for MCStd commands (2nd level)

    Revision 1.5  1998/10/23 14:21:51  midas
    - Modified version scheme from 1.06 to 1.6.0
    - cm_get_version() now returns versino as string

    Revision 1.4  1998/10/13 16:56:53  pierre
    -PAA- Add log header
             
\********************************************************************/
#include <stdio.h>
#include "midas.h"
#include "msystem.h"
#include "mcstd.h"

/* internal definitions */
#define  D16   16
#define  D24   24
#define  LOOP   1
#define  QUIT   2
#define  HELP   3
#define  SKIP   4
#define  JOB    5
#define  CHECK  6
#define  READ   7
#define  MCSTD 77
#define  MAIN  78

#define  CAM_OFF     100
#define  CAM16I      CAM_OFF+29
#define  CAM24I      CAM_OFF+28
#define  CAM16I_Q    CAM_OFF+27
#define  CAM24I_Q    CAM_OFF+26
#define  CAM16I_R    CAM_OFF+25
#define  CAM24I_R    CAM_OFF+24
#define  CAM16I_RQ   CAM_OFF+23
#define  CAM24I_RQ   CAM_OFF+22
#define  CAM16I_SA   CAM_OFF+21
#define  CAM24I_SA   CAM_OFF+20
#define  CAM16I_SN   CAM_OFF+19
#define  CAM24I_SN   CAM_OFF+18

#define  CAM16O      CAM_OFF+17
#define  CAM24O      CAM_OFF+16
#define  CAM16O_Q    CAM_OFF+15
#define  CAM24O_Q    CAM_OFF+14
#define  CAM16O_R    CAM_OFF+13
#define  CAM24O_R    CAM_OFF+12

#define  CAMC        CAM_OFF+11
#define  CAMC_Q      CAM_OFF+10
#define  CAMC_SA     CAM_OFF+9
#define  CAMC_SN     CAM_OFF+8

#define  CAM_LAM_ENABLE     CAM_OFF+7
#define  CAM_LAM_READ       CAM_OFF+6
#define  CAM_LAM_DISABLE    CAM_OFF+5
#define  CAM_LAM_CLEAR      CAM_OFF+4
#define  CAM_INHIBIT_CLEAR  CAM_OFF+3
#define  CAM_INHIBIT_SET    CAM_OFF+2
#define  CAM_CRATE_CLEAR    CAM_OFF+1
#define  CAM_CRATE_ZINIT    CAM_OFF+0

/* MCStd command list */
char MCStd[31][32] = {
   "CAM_CRATE_ZINIT", "CAM_CRATE_CLEAR", "CAM_INHIBIT_SET", "CAM_INHIBIT_CLEAR",
   "CAM_LAM_CLEAR", "CAM_LAM_DISABLE", "CAM_LAM_READ", "CAM_LAM_ENABLE",
   "CAMC_SN", "CAMC_SA", "CAMC_Q", "CAMC",
   "CAM24O_R", "CAM16O_R", "CAM24O_Q", "CAM16O_Q", "CAM24O", "CAM16O",
   "CAM24I_SN", "CAM16I_SN", "CAM24I_SA", "CAM16I_SA",
   "CAM24I_RQ", "CAM16I_RQ", "CAM24I_R", "CAM16I_R",
   "CAM24I_Q", "CAM16I_Q", "CAM24I", "CAM16I", ""
};

/* structure for CAMAC definition */
typedef struct {
   INT m;
   INT b;
   INT c;
   INT n;
   INT a;
   INT f;
   WORD d16;
   DWORD d24;
   INT r;
   INT w;
   INT q;
   INT x;
} CAMAC;


/* Default CAMAC definition */
CAMAC Prompt[2] = { {24, 0, 0, 1, 2, 0, 0, 0, 1, 0, 0, 0},
{0}
};

/* Default job file name */
char job_name[128] = "cnaf.cnf";

HNDLE hDB, hKey, hConn;
FILE *pF;
BOOL jobflag;
char addr[128];

/* prototype */
INT cnafsub(BOOL, char *);
void help_page(INT which);
INT decode_line(CAMAC * p, char *str);
INT read_job_file(FILE * pF, INT action, void **job, char *name);
void make_display_string(int from, CAMAC * p, char *saddr);
void cc_services(CAMAC * p);
void mcstd_func(CAMAC * PP);

/*--------------------------------------------------------------------*/
void cc_services(CAMAC * p)
{
   if (p->n == 30) {
      if (p->a == 9 && p->f == 24)
         cam_inhibit_clear(p->c);
      else if (p->a == 9 && p->f == 26)
         cam_inhibit_set(p->c);
      else if (p->a >= 0 && p->a < 8 && p->f == 1)
         cam_lam_read(p->c, &p->d24);
      else if (p->a >= 13 && p->f == 17)
         cam_lam_enable(p->c, p->d24);
      else if (p->a >= 12 && p->f == 17)
         cam_lam_disable(p->c, p->d24);
      /*      else if (p->a>=13 && p->f==1)
         we don't support that function "read lam mask" */
   } else if (p->n == 28 && (p->a == 8 || p->a == 9)) {
      if (p->a == 8 && p->f == 26)
         cam_crate_zinit(p->c);
      else if (p->a == 9 && p->f == 26)
         cam_crate_clear(p->c);
   } else if (p->m == 24)       /* Actual 24 bits CAMAC operation */
      if (p->f < 16)
         cam24i_q(p->c, p->n, p->a, p->f, &p->d24, &p->x, &p->q);
      else
         cam24o_q(p->c, p->n, p->a, p->f, p->d24, &p->x, &p->q);
   else /* Actual 16 bits CAMAC operation */ if (p->f < 16)
      cam16i_q(p->c, p->n, p->a, p->f, &p->d16, &p->x, &p->q);
   else
      cam16o_q(p->c, p->n, p->a, p->f, p->d16, &p->x, &p->q);
}

/*--------------------------------------------------------------------*/
void mcstd_func(CAMAC * PP)
{
   char paddr[128], pstr[128];
   int i, status, pstatus = 0;
   WORD dd16[100];
   WORD *pdd16;
   DWORD dd24[100];
   DWORD *pdd24;
   DWORD lam;
   CAMAC *p;

   /* Load default CAMAC */
   // PP = &Prompt[1];
   while (1) {
      make_display_string(MCSTD, PP, paddr);
      /* prompt */
      printf("MCStd> [%s] :", paddr);
      ss_gets(pstr, 128);

      /* decode line */
      status = decode_line(PP, pstr);
      p = PP;
      if (status == LOOP)
         status = pstatus;
      if (status != SKIP && status != HELP)
         pstatus = status;
      i = 0;
      pdd16 = dd16;
      pdd24 = dd24;
      switch (status) {
         /* system */
      case CAM_LAM_ENABLE:
         cam_lam_enable(p->c, p->n);
         printf("cam_lam_enable:C%i-N%i\n", p->c, p->n);
         break;
      case CAM_LAM_DISABLE:
         cam_lam_disable(p->c, p->n);
         printf("cam_lam_disable:C%i-N%i\n", p->c, p->n);
         break;
      case CAM_LAM_READ:
         cam_lam_read(p->c, &lam);
         printf("cam_lam_read:C%i-> 0x%lx\n", p->c, lam);
         break;
      case CAM_LAM_CLEAR:
         cam_lam_clear(p->c, p->n);
         printf("cam_lam_clear:C%i \n", p->c);
         break;
      case CAM_INHIBIT_SET:
         cam_inhibit_set(p->c);
         printf("cam_inhibit_set:C%i\n", p->c);
         break;
      case CAM_INHIBIT_CLEAR:
         cam_inhibit_clear(p->c);
         printf("cam_inhibit_clear:C%i\n", p->c);
         break;
      case CAM_CRATE_CLEAR:
         cam_crate_clear(p->c);
         printf("cam_crate_clear:C%i\n", p->c);
         break;
      case CAM_CRATE_ZINIT:
         cam_crate_zinit(p->c);
         printf("cam_crate_zinit:C%i\n", p->c);
         break;
         /* command */
      case CAMC:
         do {
            camc(p->c, p->n, p->a, p->f);
            printf("camc:[R%i]-C%i-N%i-A%i-F%i\n", ++i, p->c, p->n, p->a, p->f);
         } while (i < p->r);
         break;
      case CAMC_Q:
         do {
            camc_q(p->c, p->n, p->a, p->f, &p->q);
            printf("camc_q:[R%i]-C%i-N%i-A%i-F%i -Q:%i\n", ++i, p->c, p->n, p->a, p->f,
                   p->q);
         } while (i < p->r);
         break;
      case CAMC_SA:
         camc(p->c, p->n, p->a, p->f);
         printf("camc_sa:C%i-N%i-A%i-F%i\n", p->c, p->n, p->a, p->f);
         break;
      case CAMC_SN:
         camc(p->c, p->n, p->a, p->f);
         printf("camc_sn:C%i-N%i-A%i-F%i\n", p->c, p->n, p->a, p->f);
         break;
         /* output */
      case CAM16O:
         do {
            cam16o(p->c, p->n, p->a, p->f, p->d16);
            printf("cam16o:[R%i]-C%i-N%i-A%i-F%i <- 0x%x\n", ++i, p->c, p->n, p->a, p->f,
                   p->d16);
         } while (i < p->r);
         break;
      case CAM24O:
         do {
            cam24o(p->c, p->n, p->a, p->f, p->d24);
            printf("cam24o:[R%i]-C%i-N%i-A%i-F%i <- 0x%lx\n", ++i, p->c, p->n, p->a, p->f,
                   p->d24);
         } while (i < p->r);
         break;
      case CAM16O_Q:
         do {
            cam16o_q(p->c, p->n, p->a, p->f, p->d16, &p->x, &p->q);
            printf("cam16o_q:[R%i]-C%i-N%i-A%i-F%i <- 0x%x X:%i-Q:%i\n", ++i, p->c, p->n,
                   p->a, p->f, p->d16, p->x, p->q);
         } while (i < p->r);
         break;
      case CAM24O_Q:
         do {
            cam24o_q(p->c, p->n, p->a, p->f, p->d24, &p->x, &p->q);
            printf("cam24o_q:[R%i]-C%i-N%i-A%i-F%i <- 0x%lx X:%i-Q:%i\n", ++i, p->c, p->n,
                   p->a, p->f, p->d24, p->x, p->q);
         } while (i < p->r);
         break;
      case CAM16O_R:
         cam16o_r(p->c, p->n, p->a, p->f, pdd16, p->r);
         printf("cam16o_r:C%i-N%i-A%i-F%i <- 0x%x\n", p->c, p->n, p->a, p->f, p->d16);
         break;
      case CAM24O_R:
         cam24o_r(p->c, p->n, p->a, p->f, pdd24, p->r);
         printf("cam24o_r:C%i-N%i-A%i-F%i <- 0x%lx\n", p->c, p->n, p->a, p->f, p->d24);
         break;
         /* inputs */
      case CAM16I:
         do {
            cam16i(p->c, p->n, p->a, p->f, &p->d16);
            printf("cam16i:[R%i]-C%i-N%i-A%i-F%i-> 0x%4.4x\n", ++i, p->c, p->n, p->a,
                   p->f, p->d16);
         } while (i < p->r);
         break;
      case CAM24I:
         do {
            cam24i(p->c, p->n, p->a, p->f, &p->d24);
            printf("cam24i:[R%i]-C%i-N%i-A%i-F%i-> 0x%6.6lx\n", ++i, p->c, p->n, p->a,
                   p->f, p->d24);
         } while (i < p->r);
         break;
      case CAM16I_Q:
         do {
            cam16i_q(p->c, p->n, p->a, p->f, &p->d16, &p->x, &p->q);
            printf("cam16i_q:[R%i]-C%i-N%i-A%i-F%i-> 0x%4.4x X:%i-Q:%i\n", ++i, p->c,
                   p->n, p->a, p->f, p->d16, p->x, p->q);
         } while (i < p->r);
         break;
      case CAM24I_Q:
         do {
            cam24i_q(p->c, p->n, p->a, p->f, &p->d24, &p->x, &p->q);
            printf("cam24i_q:[R%i]-C%i-N%i-A%i-F%i-> 0x%6.6lx X:%i-Q:%i\n", ++i, p->c,
                   p->n, p->a, p->f, p->d24, p->x, p->q);
         } while (i < p->r);
         break;
      case CAM16I_R:
         memset(pdd16, 0, sizeof(dd16));
         cam16i_r(p->c, p->n, p->a, p->f, &pdd16, p->r);
         for (i = 0; i < p->r; i++)
            printf("cam16i_r:[R%i]-C%i-N%i-A%i-F%i-> 0x%4.4x\n", i + 1, p->c, p->n, p->a,
                   p->f, dd16[i]);
         break;
      case CAM24I_R:
         memset(pdd24, 0, sizeof(dd24));
         cam24i_r(p->c, p->n, p->a, p->f, &pdd24, p->r);
         for (i = 0; i < p->r; i++)
            printf("cam24i_r:[R%i]-C%i-N%i-A%i-F%i-> 0x%6.6lx\n", i + 1, p->c, p->n, p->a,
                   p->f, dd24[i]);
         break;
      case CAM16I_RQ:
         memset(pdd16, 0, sizeof(dd16));
         cam16i_rq(p->c, p->n, p->a, p->f, &pdd16, p->r);
         for (i = 0; i < p->r; i++)
            printf("cam16i_rq:[R%i]-C%i-N%i-A%i-F%i-> 0x%4.4x\n", i + 1, p->c, p->n, p->a,
                   p->f, dd16[i]);
         break;
      case CAM24I_RQ:
         memset(pdd24, 0, sizeof(dd24));
         cam24i_rq(p->c, p->n, p->a, p->f, &pdd24, p->r);
         for (i = 0; i < p->r; i++)
            printf("cam24i_rq:[R%i]-C%i-N%i-A%i-F%i-> 0x%6.6lx\n", i + 1, p->c, p->n,
                   p->a, p->f, dd24[i]);
         break;
      case CAM16I_SA:
         memset(pdd16, 0, sizeof(dd16));
         cam16i_sa(p->c, p->n, p->a, p->f, &pdd16, p->r);
         for (i = 0; i < p->r; i++)
            printf("cam16i_sa:[R%i]-C%i-N%i-A%i-F%i-> 0x%4.4x\n", i + 1, p->c, p->n,
                   p->a + i, p->f, dd16[i]);
         break;
      case CAM24I_SA:
         memset(pdd24, 0, sizeof(dd24));
         cam24i_sa(p->c, p->n, p->a, p->f, &pdd24, p->r);
         for (i = 0; i < p->r; i++)
            printf("cam24i_sa:[R%i]-C%i-N%i-A%i-F%i-> 0x%6.6lx\n", i + 1, p->c, p->n,
                   p->a + i, p->f, dd24[i]);
         break;
      case CAM16I_SN:
         memset(pdd16, 0, sizeof(dd16));
         cam16i_sa(p->c, p->n, p->a, p->f, &pdd16, p->r);
         for (i = 0; i < p->r; i++)
            printf("cam16i_sn:[R%i]-C%i-N%i-A%i-F%i-> 0x%x\n", i + 1, p->c, p->n + i,
                   p->a, p->f, dd16[i]);
         break;
      case CAM24I_SN:
         memset(pdd24, 0, sizeof(dd24));
         cam24i_sn(p->c, p->n, p->a, p->f, &pdd24, p->r);
         for (i = 0; i < p->r; i++)
            printf("cam24i_sn:[R%i]-C%i-N%i-A%i-F%i-> 0x%lx\n", i + 1, p->c, p->n + i,
                   p->a, p->f, dd24[i]);
         break;
      case QUIT:
         p->r = 1;
         return;
      case HELP:
         help_page(MCSTD);
         break;
      case SKIP:
         break;
      default:
         status = SKIP;
         break;
      }
   }
}

/*--------------------------------------------------------------------*/
void make_display_string(int from, CAMAC * p, char *saddr)
{
   if (from == MAIN) {
      if (p->m == D24)
         sprintf(saddr, "B%01dC%01dN%02dA%02dF%02d [%ld/0x%06lx Q%01dX%01d] R%dW%dM%2d",
                 p->b, p->c, p->n, p->a, p->f, p->d24, p->d24, p->q, p->x, p->r, p->w,
                 p->m);
      else
         sprintf(saddr, "B%01dC%01dN%02dA%02dF%02d [%d/0x%04x Q%01dX%01d] R%dW%dM%2d",
                 p->b, p->c, p->n, p->a, p->f, p->d16, p->d16, p->q, p->x, p->r, p->w,
                 p->m);
   } else {
      sprintf(saddr, "B%01dC%01dN%02dA%02dF%02d [%ld/0x%06lx] R%d", p->b, p->c, p->n,
              p->a, p->f, p->d24, p->d24, p->r);
   }
}

/*--------------------------------------------------------------------*/
INT read_job_file(FILE * pF, INT action, void **job, char *name)
{
   DWORD n;
   char line[128];
   CAMAC *pjob;

   if (action == CHECK) {
      if (*name == 0)
         sprintf(name, "%s", job_name);
      pF = fopen(name, "r");
      if (pF) {
         fclose(pF);
         return JOB;
      }
      printf("CNAF-I- File not found :%s\n", name);
      return SKIP;
   } else if (action == READ) {
      pF = fopen(name, "r");
      if (pF) {
         n = 0;
         /* count the total number of line */
         while (fgets(line, 128, pF))
            n++;
         /* allocate memory for full job */
         *job = malloc((n + 1) * sizeof(CAMAC));
         pjob = *job;
         /* preset the full job with 0 */
         memset((char *) pjob, 0, (n + 1) * sizeof(CAMAC));
         /* preset the first entry with the predefined CAMAC access */
         memcpy((char *) pjob, (char *) Prompt, sizeof(CAMAC));
         rewind(pF);
         while (fgets(line, 128, pF)) {
            if (pjob->m == 0) {
               /* load previous command before overwriting */
               memcpy((char *) pjob, (char *) (pjob - 1), sizeof(CAMAC));
            }
            decode_line(pjob++, line);
         }
         fclose(pF);
         return JOB;
      }
   } else
      printf("CNAF-I- Cmd not found :%s\n", name);
   return SKIP;
}

/*--------------------------------------------------------------------*/
INT cnafsub(BOOL cmd_mode, char *cmd)
{
   char str[128], line[128];
   INT status, j;
   CAMAC *P, *p = NULL, *job;

   /* Loop return label */
   if (jobflag) {
      jobflag = FALSE;
   }

   /* Load default CAMAC */
   P = Prompt;
   while (1) {
      if (!cmd_mode) {
         make_display_string(MAIN, P, addr);
         /* prompt */
         printf("mCNAF> [%s] :", addr);
         ss_gets(str, 128);
      } else {
         strcpy(str, cmd);
      }

      /* decode line */
      status = decode_line(P, str);
      if (status == QUIT)
         return status;
      else if (status == MCSTD) {
         mcstd_func(P);
         status = decode_line(P, str);
      } else if (status == HELP)
         help_page(MAIN);
      else if (status == JOB) {
         if (!cmd_mode) {
            /* interactive session, display default job name */
            printf("\nmCNAF> Job file name [%s]:", job_name);
            ss_gets(line, 128);
            if (strlen(line) == 0)
               strcpy(line, job_name);  // Use default
            else {
               strcpy(job_name, line);
            }
         } else {
            /* from command line, skip @ */
            strcpy(line, &str[1]);
         }
         /* Check if file exists */
         status = read_job_file(pF, CHECK, (void **) &job, line);
         if (status == JOB)
            status = read_job_file(pF, READ, (void **) &job, line);
      }

      if (status == LOOP || status == JOB) {
         for (j = 0; j < P->r; j++) {
            if (status == LOOP)
               p = P;
            if (status == JOB)
               p = job;
            while (p->m) {
               if (p->n == 28 || p->n == 29 || p->n == 30)
                  cc_services(p);
               else if (p->m == 24)     /* Actual 24 bits CAMAC operation */
                  if (p->f < 16)
                     cam24i_q(p->c, p->n, p->a, p->f, &p->d24, &p->x, &p->q);
                  else
                     cam24o_q(p->c, p->n, p->a, p->f, p->d24, &p->x, &p->q);
               else /* Actual 16 bits CAMAC operation */ if (p->f < 16)
                  cam16i_q(p->c, p->n, p->a, p->f, &p->d16, &p->x, &p->q);
               else
                  cam16o_q(p->c, p->n, p->a, p->f, p->d16, &p->x, &p->q);
               make_display_string(MAIN, p, addr);

               /* Result display */
               if (p->r > 1) {
                  /* repeat mode */
                  if (status == JOB) {
                     if (!cmd_mode)
                        printf("\nmCNAF> [%s]", addr);
                     if (p->w != 0)
                        ss_sleep(p->w);
                  } else {
                     if (!cmd_mode)
                        printf("mCNAF> [%s] <-%03i\n", addr, j + 1);
                     if (p->w != 0)
                        ss_sleep(p->w);
                     if (j > p->r - 1)
                        break;
                  }
               } else {
                  /* single command */
                  if (status == JOB) {
                     if (!cmd_mode)
                        printf("mCNAF> [%s]\n", addr);
                     if (p->w != 0)
                        ss_sleep(p->w);
                  }
               }
               p++;
            }
         };
         if (status == JOB) {
            free(job);
            if (!cmd_mode)
               printf("\n");
         }
      }
      if (cmd_mode)
         break;
   }
   return status;
}

/*--------------------------------------------------------------------*/
INT decode_line(CAMAC * P, char *ss)
{
   char empty[16] = { "                " };
   char *c, *cmd;
   char pp[128], *p, *s, *ps;
   DWORD tmp, i;
   BOOL ok = FALSE;

   p = pp;
   s = ss;
   memset(pp, 0, 128);

   /* convert to uppercase */
   c = ss;
   while (*c) {
      *c = toupper(*c);
      c++;
   }

   while (*s) {
      if (*s == 'G') {
         while (*s)
            *p++ = *s++;
         break;
      }
      if ((*s == 'B') || (*s == 'C') ||
          (*s == 'N') || (*s == 'A') ||
          (*s == 'F') || (*s == 'C') ||
          (*s == 'R') || (*s == 'W') ||
          (*s == 'G') || (*s == 'H') || (*s == 'E') || (*s == 'Q'))
         *p++ = ' ';
      else if ((*s == 'X') || (*s == 'D') || (*s == 'O')) {
         *p++ = ' ';
         *p++ = *s++;
         while (*s)
            *p++ = *s++;
         break;
      };
      *p++ = *s++;
   }
   *p++ = ' ';

   p = pp;

   if ((cmd = strpbrk(p, "P")))
      return MCSTD;
   if ((cmd = strpbrk(p, "G"))) {
      for (i = 0;; i++) {
         if (MCStd[i][0] == 0)
            break;
         if (strstr(p + 2, MCStd[i]) != NULL)
            return CAM_OFF + i;
      }
      return LOOP;
   }
   if ((cmd = strpbrk(p, "H")))
      return HELP;
   if ((cmd = strpbrk(p, "J@")))
      return JOB;
   if ((cmd = strpbrk(p, "X"))) {
      ps = strchr(cmd, ' ');
      *ps = 0;
      if (P->m == D24) {
         tmp = strtoul((cmd + 1), NULL, 16);
         if (tmp >= 0x0 && tmp <= 0xffffff) {
            P->d24 = tmp;
            ok = TRUE;
         } else
            printf("mcnaf-E- Data out of range 0x0:0xffffff\n");
      } else {
         tmp = strtoul((cmd + 1), NULL, 16);
         if (tmp >= 0x0 && tmp <= 0xffff) {
            P->d16 = (WORD) tmp;
            ok = TRUE;
         } else
            printf("mcnaf-E- Data out of range 0x0:0xffff\n");
      }
      strncpy(cmd, empty, strlen(cmd));
      *ps = ' ';
   }
   if ((cmd = strpbrk(p, "O"))) {
      ps = strchr(cmd, ' ');
      *ps = 0;
      if (P->m == D24) {
         tmp = strtoul((cmd + 1), NULL, 8);
         if (tmp >= 0 && tmp <= 077777777) {
            P->d24 = tmp;
            ok = TRUE;
         } else
            printf("mcnaf-E- Data out of range O0:O77777777\n");
      } else {
         tmp = strtoul((cmd + 1), NULL, 8);
         if (tmp >= 00 && tmp <= 0177777) {
            P->d16 = (WORD) tmp;
            ok = TRUE;
         } else
            printf("mcnaf-E- Data out of range O0:O177777\n");
      }
      strncpy(cmd, empty, strlen(cmd));
      *ps = ' ';
   }
   if ((cmd = strpbrk(p, "D"))) {
      ps = strchr(cmd, ' ');
      *ps = 0;
      if (P->m == D24) {
         tmp = strtoul((cmd + 1), NULL, 10);
         if (tmp >= 0x0 && tmp <= 0xffffff) {
            P->d24 = tmp;
            ok = TRUE;
         } else
            printf("mcnaf-E- Data out of range 0:16777215\n");
      } else {
         tmp = strtoul((cmd + 1), NULL, 10);
         if (tmp >= 0x0 && tmp <= 0xffff) {
            P->d16 = (WORD) tmp;
            ok = TRUE;
         } else
            printf("mcnaf-E- Data out of range 0:65535\n");
      }
      strncpy(cmd, empty, strlen(cmd));
      *ps = ' ';
   }
   if ((cmd = strchr(p, 'B'))) {
      ps = strchr(cmd, ' ');
      *ps = 0;
      tmp = atoi((cmd + 1));
      if (tmp < 8 && tmp >= 0) {
         P->b = tmp;
         ok = TRUE;
      } else
         printf("mcnaf-E- B out of range 0:7\n");
      *ps = ' ';
   }
   if ((cmd = strchr(p, 'C'))) {
      ps = strchr(cmd, ' ');
      *ps = 0;
      tmp = atoi((cmd + 1));
      if (tmp < 8 && tmp >= 0) {
         P->c = tmp;
         ok = TRUE;
      } else
         printf("mcnaf-E- C out of range 0:7\n");
      *ps = ' ';
   }
   if ((cmd = strchr(p, 'N'))) {
      ps = strchr(cmd, ' ');
      *ps = 0;
      tmp = atoi((cmd + 1));
      if (tmp < 32 && tmp >= 0) {
         P->n = tmp;
         ok = TRUE;
      } else
         printf("mcnaf-E- N out of range 0:31\n");
      *ps = ' ';
   }
   if ((cmd = strchr(p, 'A'))) {
      ps = strchr(cmd, ' ');
      *ps = 0;
      tmp = atoi((cmd + 1));
      if (tmp < 16 && tmp >= 0) {
         P->a = tmp;
         ok = TRUE;
      } else
         printf("mcnaf-E- A out of range 0:15\n");
      *ps = ' ';
   }
   if ((cmd = strchr(p, 'F'))) {
      ps = strchr(cmd, ' ');
      *ps = 0;
      tmp = atoi((cmd + 1));
      if (tmp < 32 && tmp >= 0) {
         P->f = tmp;
         ok = TRUE;
      } else
         printf("mcnaf-E- F out of range 0:31\n");
      *ps = ' ';
   }
   if ((cmd = strchr(p, 'R'))) {
      ps = strchr(cmd, ' ');
      *ps = 0;
      tmp = atoi((cmd + 1));
      if (tmp < 1000 && tmp >= 0) {
         if (tmp == 0)
            tmp = 1;
         P->r = tmp;
      } else
         printf("mcnaf-E- R out of range 1:1000\n");
      *ps = ' ';
   }
   if ((cmd = strchr(p, 'W'))) {
      ps = strchr(cmd, ' ');
      *ps = 0;
      tmp = atoi((cmd + 1));
      if (tmp < 10001 || tmp >= 0)
         P->w = tmp;
      else
         printf("mcnaf-E- W out of range 0:10000\n");
      *ps = ' ';
   }
   if ((cmd = strchr(p, 'M'))) {
      ps = strchr(cmd, ' ');
      *ps = 0;
      tmp = atoi((cmd + 1));
      if (tmp == D16 || tmp == D24)
         P->m = tmp;
      else
         printf("mcnaf-E- M out of range 16,24\n");
      *ps = ' ';
   }
   if (ok)
      return LOOP;
   if ((cmd = strpbrk(p, "QE")))
      return QUIT;
   return SKIP;
}

/*--------------------------------------------------------------------*/
void help_page(INT which)
{
   if (which == MAIN) {
      printf("\n*-v%s----------- H E L P   C N A F -------------------*\n",
             cm_get_version());
      printf("          Interactive Midas CAMAC command\n");
      printf("          ===============================\n");
      printf(" Output : Data [dec/hex X=0/1 - Q=0/1 ]\n");
      printf(" Inputs : Bx :  Branch   [0 -  7]    Cx :  Crate    [0 -  7]\n");
      printf("          Nx :  Slot     [1 - 31]    Ax :  SubAdd.  [0 - 15]\n");
      printf("          Fx :  Function [0 - 31]    Mx :  Access mode [16,24]\n");
      printf("           H :  HELP on   CNAF       E/Q:  Exit/Quit from CNAF\n");
      printf("          Rx :  Repetition counter               [1 -   999]\n");
      printf("          Wx :  Delay between command (ms)       [0 - 10000]\n");
      printf("           P :  Switch to MCStd CAMAC operation\n");
      printf("           J :  Perform JOB (command list from file )\n");
      printf("                Same syntax as in interactive session\n");
      printf("           G :  PERFORM ACTION of predefine set-up\n");
      printf("           D :  Decimal     Data     [0 - max data_size]\n");
      printf("           O :  Octal       Data     [0 - max data_size]\n");
      printf("           X :  Hexadecimal Data     [0 - max data_size]\n");
      printf("\n");
      printf
          (">>>>>>>>>> Data has to be given LAST if needed in the command string <<<<<\n");
      printf("\n");
      printf
          (" The commands below are following the ESONE standard for a A2 controller\n");
      printf(" N30A9F24 : Crate clear inhibit    N30A9F26 : Crate set inhibit\n");
      printf(" N28A8F26 : Z crate                N28A9F26 : C crate\n");
      printf(" N30A13F17: CC Lam enable          N30A12F17: CC Lam disable\n");
      printf(" N30A0-7F1: CC read Lam\n");
      printf("\n");
      printf(" examples: ");
      printf(">mcnaf -cl\"triggerFE\" -h local -e myexpt\n");
      printf("      CNAF> [B0C1N30A00F00 [0/0x000000 Q0X0] R1W0M24] :n30a9f24\n");
      printf("      CNAF> [B0C1N06A00F24 [0/0x000000 Q1X1] R1W0M24] :n6f9a0\n");
   } else {
      printf("\n*-v%s----------- H E L P   C N A F -------------------*\n",
             cm_get_version());
      printf("          Interactive MCStd CAMAC command\n");
      printf("The action taken is dependent on the driver implementation\n");
      printf("[Q/E]            : Exit this level\n");
      printf("[G] <function>   : Issue the given function on preset CAMAC address\n");
      printf("Possible functions are:\n");
      printf("cam16/24i        : 16/24bit input\n");
      printf("cam16/24i_q      : 16/24bit input with Q response\n");
      printf("cam16/24i_r      : 16/24bit input repeat until R reached (max 100)\n");
      printf
          ("cam16/24i_rq     : 16/24bit input repeat until NoQ or R reached (max 100)\n");
      printf("cam16/24i_sa     : 16/24bit input scan R times over A\n");
      printf("cam16/24i_sn     : 16/24bit input scan R times over N\n");
      printf("cam16/24o        : 16/24bit output\n");
      printf("cam16/24o_q      : 16/24bit output with Q response\n");
      printf("cam16/24o_r      : 16/24bit output repeat R times\n");
      printf("camc             : 16bit command \n");
      printf("camc_q           : 16bit command with Q response\n");
      printf("camc_sa          : 16bit command scan R times over A\n");
      printf("camc_sn          : 16bit command scan R times over N\n");
      printf("cam_lam_enable   : Enable lam in Crate controller ONLY\n");
      printf("cam_lam_disable  : Disable lam in Crate controlled ONLY\n");
      printf("cam_lam_read     : Read lam from Crate controller\n");
      printf("cam_inhibit_clear: Reset Inhibit line in crate\n");
      printf("cam_inhibit_set  : Set Inhibit line in crate\n");
      printf("cam_crate_clear  : Generate a Clear cycle to the crate\n");
      printf("cam_crate_zinit  : Generate a Z cycle to the crate\n");
   }
}

/*--------------------------------------------------------------------*/

int main(int argc, char **argv)
{
   char host_name[HOST_NAME_LENGTH], exp_name[NAME_LENGTH], fe_name[256], rpc_server[256];
   char cmd[256];
   INT i, status;
   BOOL debug, cmd_mode = FALSE;

   /* set default */
   host_name[0] = '\0';
   exp_name[0] = '\0';
   fe_name[0] = '\0';
   rpc_server[0] = '\0';
   debug = FALSE;

   /* get parameters */
   cm_get_environment(host_name, sizeof(host_name), exp_name, sizeof(exp_name));

   /* parse command line parameters */
   for (i = 1; i < argc; i++) {
      if (argv[i][0] == '-' && argv[i][1] == 'd')
         debug = TRUE;
      else if (argv[i][0] == '-') {
         if (i + 1 >= argc || argv[i + 1][0] == '-')
            goto usage;
         if (strncmp(argv[i], "-e", 3) == 0)
            strcpy(exp_name, argv[++i]);
         else if (strncmp(argv[i], "-h", 3) == 0)
            strcpy(host_name, argv[++i]);
         else if (strncmp(argv[i], "-f", 3) == 0)
            strcpy(fe_name, argv[++i]);
         else if (strncmp(argv[i], "-s", 3) == 0)
            strcpy(rpc_server, argv[++i]);
         else if (argv[i][1] == 'c') {
            if (strlen(argv[i]) >= 256) {
               printf("error: command line too long (>256).\n");
               return 0;
            }
            strcpy(cmd, argv[++i]);
            cmd_mode = TRUE;
         } else {
          usage:
            printf
                ("usage: mcnaf [-f Frontend] [-h Hostname] [-e Experiment] [-s RPC server]\n");
            printf("             [-c Command] [-c @CommandFile] \n\n");
            return 0;
         }
      }
   }

   if (rpc_server[0])
      status = cam_init_rpc("", "", "", "mcnaf", rpc_server);
   else
      status = cam_init_rpc(host_name, exp_name, fe_name, "mcnaf", "");
   if (status == SUCCESS) {
      status = cam_init();
      if (status == SUCCESS)
         status = cnafsub(cmd_mode, cmd);
   }
   cam_exit();
//  printf("status:%d\n", status);
   return status;
}
