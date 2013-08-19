*********************************************************************\
*
*     Name:         MIDAS.INC
*     Created by:   Stefan Ritt
*
*     Contents:     MIDAS Fortran include file. Defining functions
*                   and constants
*
*
*     Revision history
*     ---------------------------------------------------------------
*     date         by    modification
*     ---------    ---   --------------------------------------------
*     14-JAN-98    SR    created
*
*
*********************************************************************/

*     Function declarations
	
	    INTEGER*4 CM_CONNECT_EXPERIMENT,CM_YIELD

*     Constants

	    INTEGER*4 EVENT_BUFFER_SIZE,TRIGGER_ALL,EVENTID_ALL

      PARAMETER (CM_SUCCESS=1,RPC_SHUTDOWN=509,SS_ABORT=415)
      PARAMETER (EVENT_BUFFER_SIZE=1048576) 
      PARAMETER (TRIGGER_ALL=-1,EVENTID_ALL=-1,GET_ALL=1,GET_SOME=2)
