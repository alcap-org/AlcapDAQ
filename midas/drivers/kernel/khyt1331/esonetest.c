#include <stdio.h>
#include "esone.h"

void isr(void)
{
   int ext, q;
   unsigned short d;

   /* toggle output so one can measure interrupt response time */
   cdreg(&ext, 0, 0, 1, 0);
   d = 0xFF;
   cssa(16, ext, &d, &q);
   d = 0;
   cssa(16, ext, &d, &q);

   /* clear lam in unit and controller */
   cdlam(&ext, 0, 0, 1, 0, NULL);
   cclc(ext);

   /* re-enable crate demand */
   ccrgl(ext);

   printf("ISR\n");
}

main()
{
   int ext, q;
   unsigned long d;
   unsigned short sd;

   /* initialize CAMAC */
   if (!fccinit())
      return;

   /* crate zero and remove inhibit */
   cdreg(&ext, 0, 0, 0, 0);
   cccz(ext);
   ccci(ext, 0);

   /* write data and read back */
   cdreg(&ext, 0, 0, 1, 0);
   d = 0x123456;
   cfsa(16, ext, &d, &q);
   d = 0;
   cfsa(0, ext, &d, &q);
   printf("Read back %X, should be 123456\n", d);

   /* enable and clear LAM in unit */
   cdreg(&ext, 0, 0, 1, 0);
   cssa(26, ext, &sd, &q);
   cdlam(&ext, 0, 0, 1, 0, NULL);
   cclc(ext);

   /* register service routine */
   cclnk(ext, isr);

   /* enable crate demand */
   cccd(ext, 1);

   /* go into endless loop */
   while (1);
}
