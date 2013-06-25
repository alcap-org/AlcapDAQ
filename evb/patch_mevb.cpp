--- /home/data/midas-1.9.3/examples/eventbuilder/mevb.c	2002-10-07 19:04:01.000000000 +0200
+++ mevb.cpp	2005-06-18 01:55:31.000000000 +0200
@@ -3,7 +3,17 @@
 Created by:   Pierre-Andre Amaudruz
 
 Contents:     Main Event builder task.
-$Log: mevb.c,v $
+$Log: mevb.cpp,v $
+Revision 1.1  2003/07/17 18:52:54  fegray
+Converted eventbuilder to C++.
+
+Revision 1.4  2003/05/27 16:26:20  mucap
+Update to CVS version of MIDAS.
+
+Revision 1.3  2003/05/13 19:27:04  mucap
+Fixes to allow a subset of the crates to participate in an event.  Also,
+this pulls in Pierre's CVS changes to the generic eventbuilder code.
+
 Revision 1.9  2002/10/07 17:04:01  pierre
 fix tr_stop request
 
@@ -67,7 +77,9 @@
 INT eb_end_of_run(INT, char *);
 INT eb_user(INT, EBUILDER_CHANNEL *, EVENT_HEADER *, void *, INT *);
 
-extern INT  ybos_event_swap   (DWORD * pevt);
+extern "C" {
+  extern INT  ybos_event_swap   (DWORD * pevt);
+};
 
 /*--------------------------------------------------------------------*/
 /* eb_mfragment_add_()
@@ -299,7 +311,7 @@
   if (debug)
     printf("Hand flushing system buffer... \n");
   for (i=0;i<nfragment;i++) {
-    do {  
+    if (ebch[i].set.emask) do {  
       size = max_event_size;
       status = bm_receive_event(ebch[i].hBuf, ebch[i].pfragment, &size, ASYNC);
       if (debug1) {
@@ -407,19 +419,23 @@
 }
 
 /*--------------------------------------------------------------------*/
-INT source_unbooking(nfrag)
+INT source_unbooking(INT nfrag)
 {   
   INT i, status;
 
   /* Skip unbooking if already done */
+#if 0
   if (ebch[0].pfragment == NULL)
     return EB_SUCCESS;
+#endif
 
   /* unbook all source channels */
   for (i=nfrag-1; i>=0 ; i--)
   {
     bm_empty_buffers();
 
+    if (ebch[i].set.emask)
+    {
     /* Remove event ID registration */
     status = bm_delete_request(ebch[i].req_id);
     if (debug)
@@ -435,6 +451,7 @@
       return status;
     }
   }
+  }
 
   /* release local event buffer memory */
   free_event_buffer(nfrag);
@@ -477,6 +494,8 @@
   BOOL   found, event_mismatch;
   BANK_HEADER *psbh;
 
+  status = SUCCESS;
+
   /* Scan all channels at least once */
   for(i=0 ; i<nfragment ; i++) {
     /* Check if current channel needs to be received */
@@ -562,6 +581,9 @@
         sprintf (strsub, "Ser[%d]:%d ", j, ebch[j].serial);
         strcat (str, strsub);
       }
+      if(debug) {
+       printf("%s\n", str);
+      }
     }
     else {  /* serial number match */
 
@@ -590,6 +612,7 @@
       /* Allow bypass of fragment assembly if user wants to do it on its own */
       if (!ebset.user_build) {
         for (j=0 ; j<nfragment ; j++) {
+          if (ebch[j].set.emask) {
           status = meb_fragment_add(dest_event, ebch[j].pfragment, &act_size);
           if (status != EB_SUCCESS) {
             cm_msg(MERROR,"source_scan","compose fragment:%d current size:%d (%d)"
@@ -597,6 +620,7 @@
             return EB_ERROR;
           }
         }
+        }
       } /* skip user_build */
 
       /* Overall event to be sent */
@@ -652,7 +676,8 @@
   memset (&ebch[0], 0, sizeof(ebch));
 
   /* set default */
-  cm_get_environment (host_name, expt_name);
+  cm_get_environment (host_name, sizeof(host_name), 
+		      expt_name, sizeof(expt_name));
 
   /* get parameters */
   for (i=1 ; i<argc ; i++)
@@ -770,9 +795,9 @@
   }
 
   /* Register transition for reset counters */
-  if (cm_register_transition(TR_PRESTART, tr_prestart) != CM_SUCCESS)
+  if (cm_register_transition(TR_START, tr_prestart, 300) != CM_SUCCESS)
     goto error;
-  if (cm_register_transition(TR_STOP, tr_stop) != CM_SUCCESS)
+  if (cm_register_transition(TR_STOP, tr_stop, 700) != CM_SUCCESS)
     goto error;
 
   if (debug)
