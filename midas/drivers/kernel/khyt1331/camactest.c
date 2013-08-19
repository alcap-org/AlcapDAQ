#include <stdio.h>
#include "mcstd.h"

void isr(void)
{
   /* toggle output so one can measure interrupt response time */
   cam16o(0, 1, 0, 16, 0xFF);
   cam16o(0, 1, 0, 16, 0);

   /* clear lam in unit and controller */
   camc(0, 1, 0, 10);
   cam_lam_clear(0, 1);

   /* re-enable crate demand */
   cam_interrupt_enable(0);

   printf("ISR\n");
}

main()
{
   unsigned long d;

   /* initialize CAMAC */
   if (cam_init() < 0)
      return;

   /* crate zero and remove inhibit */
   cam_crate_zinit(0);
   cam_inhibit_clear(0);

   /* write data and read back */
   cam24o(0, 1, 0, 16, 0x123456);
   cam24i(0, 1, 0, 0, &d);
   printf("Read back %X, should be 123456\n", d);

   /* enable and clear LAM in unit */
   camc(0, 1, 0, 26);
   camc(0, 1, 0, 10);

   /* register service routine */
   cam_lam_enable(0, 1);
   cam_lam_clear(0, 1);
   cam_interrupt_attach(0, 1, isr);

   /* enable crate demand */
   cam_interrupt_enable(0);

   /* go into endless loop */
   while (1);
}
