
/********************************************************************\

  Name:         MRPC.H
  Created by:   Stefan Ritt

  Purpose:      MIDAS RPC function header file
  Contents:     Symbolic constants for internal RPC functions

  $Log: mrpc.h,v $
  Revision 1.1.1.1  2005/06/20 23:37:15  mucap
  Importing release 1.9.5 of the MIDAS source code as distributed by PSI.
  (Next, I'll commit our local customizations.)

  Revision 1.21  2004/07/21 05:37:48  pierre
  fix dox doc

  Revision 1.20  2004/01/28 01:21:44  pierre
  fix double slash for VxWorks

  Revision 1.19  2004/01/08 08:40:09  midas
  Implemented standard indentation

  Revision 1.18  2004/01/08 06:36:53  pierre
  Doxygen the file

  Revision 1.17  2003/11/20 11:28:56  midas
  Added db_check_record

  Revision 1.16  2003/04/15 12:52:23  midas
  Removed db_show_mem

  Revision 1.15  2002/05/10 01:41:19  midas
  Added optional debug output to cm_transition

  Revision 1.14  2000/09/28 13:01:25  midas
  Added manual triggered events

  Revision 1.13  2000/08/21 07:05:47  midas
  Added cm_msg_log1(...,facility) to be compatible with older programs

  Revision 1.12  1999/09/17 11:48:05  midas
  Alarm system half finished

  Revision 1.11  1999/09/15 13:33:32  midas
  Added remote el_submit functionality

  Revision 1.10  1999/08/03 11:15:07  midas
  Added bm_skip_event

  Revision 1.9  1999/05/05 12:01:42  midas
  Added and modified hs_xxx functions

  Revision 1.8  1999/04/19 07:46:43  midas
  Added cm_msg_retrieve

  Revision 1.7  1999/04/15 09:59:22  midas
  Added RPC_CM_EXIST

  Revision 1.6  1999/04/13 12:20:42  midas
  Added db_get_data1 (for Java)

  Revision 1.5  1999/04/08 15:24:13  midas
  Added RPC_CM_TRANSITION and RPC_DB_GET_KEY_INFO

  Revision 1.4  1999/02/11 13:20:06  midas
  Added RPC_CM_CSG

  Revision 1.3  1999/01/13 09:40:48  midas
  Added db_set_data_index2 function

  Revision 1.2  1998/10/12 12:19:01  midas
  Added Log tag in header


\********************************************************************/

/**dox***************************************************************/
/** @file mrpc.h
The mrpc include file
*/

/** @defgroup mrpcincludecode The mrpc.h & mrpc.c
 */
/** @defgroup mrpcdefineh RPC Define 
 */

/**dox***************************************************************/
/** @addtogroup mrpcincludecode
 *  
 *  @{  */

/**dox***************************************************************/
/** @addtogroup mrpcdefineh
 *  
 *  @{  */

/********************************************************************/
/**
routine IDs for RPC calls
*/
#define RPC_CM_SET_CLIENT_INFO          11000 /**< - */
#define RPC_CM_SET_WATCHDOG_PARAMS      11001 /**< - */
#define RPC_CM_CLEANUP                  11002 /**< - */
#define RPC_CM_GET_WATCHDOG_INFO        11003 /**< - */
#define RPC_CM_MSG_LOG                  11004 /**< - */
#define RPC_CM_EXECUTE                  11005 /**< - */
#define RPC_CM_SYNCHRONIZE              11006 /**< - */
#define RPC_CM_ASCTIME                  11007 /**< - */
#define RPC_CM_TIME                     11008 /**< - */
#define RPC_CM_MSG                      11009 /**< - */
#define RPC_CM_EXIST                    11011 /**< - */
#define RPC_CM_MSG_RETRIEVE             11012 /**< - */
#define RPC_CM_MSG_LOG1                 11013 /**< - */

#define RPC_BM_OPEN_BUFFER              11100 /**< - */
#define RPC_BM_CLOSE_BUFFER             11101 /**< - */
#define RPC_BM_CLOSE_ALL_BUFFERS        11102 /**< - */
#define RPC_BM_GET_BUFFER_INFO          11103 /**< - */
#define RPC_BM_GET_BUFFER_LEVEL         11104 /**< - */
#define RPC_BM_INIT_BUFFER_COUNTERS     11105 /**< - */
#define RPC_BM_SET_CACHE_SIZE           11106 /**< - */
#define RPC_BM_ADD_EVENT_REQUEST        11107 /**< - */
#define RPC_BM_REMOVE_EVENT_REQUEST     11108 /**< - */
#define RPC_BM_SEND_EVENT               11109 /**< - */
#define RPC_BM_FLUSH_CACHE              11110 /**< - */
#define RPC_BM_RECEIVE_EVENT            11111 /**< - */
#define RPC_BM_MARK_READ_WAITING        11112 /**< - */
#define RPC_BM_EMPTY_BUFFERS            11113 /**< - */
#define RPC_BM_SKIP_EVENT               11114 /**< - */

#define RPC_DB_OPEN_DATABASE            11200 /**< - */
#define RPC_DB_CLOSE_DATABASE           11201 /**< - */
#define RPC_DB_CLOSE_ALL_DATABASES      11202 /**< - */
#define RPC_DB_CREATE_KEY               11203 /**< - */
#define RPC_DB_CREATE_LINK              11204 /**< - */
#define RPC_DB_SET_VALUE                11205 /**< - */
#define RPC_DB_GET_VALUE                11206 /**< - */
#define RPC_DB_FIND_KEY                 11207 /**< - */
#define RPC_DB_FIND_LINK                11208 /**< - */
#define RPC_DB_GET_PATH                 11209 /**< - */
#define RPC_DB_DELETE_KEY               11210 /**< - */
#define RPC_DB_ENUM_KEY                 11211 /**< - */
#define RPC_DB_GET_KEY                  11212 /**< - */
#define RPC_DB_GET_DATA                 11213 /**< - */
#define RPC_DB_SET_DATA                 11214 /**< - */
#define RPC_DB_SET_DATA_INDEX           11215 /**< - */
#define RPC_DB_SET_MODE                 11216 /**< - */
#define RPC_DB_GET_RECORD_SIZE          11219 /**< - */
#define RPC_DB_GET_RECORD               11220 /**< - */
#define RPC_DB_SET_RECORD               11221 /**< - */
#define RPC_DB_ADD_OPEN_RECORD          11222 /**< - */
#define RPC_DB_REMOVE_OPEN_RECORD       11223 /**< - */
#define RPC_DB_SAVE                     11224 /**< - */
#define RPC_DB_LOAD                     11225 /**< - */
#define RPC_DB_SET_CLIENT_NAME          11226 /**< - */
#define RPC_DB_RENAME_KEY               11227 /**< - */
#define RPC_DB_ENUM_LINK                11228 /**< - */
#define RPC_DB_REORDER_KEY              11229 /**< - */
#define RPC_DB_CREATE_RECORD            11230 /**< - */
#define RPC_DB_GET_DATA_INDEX           11231 /**< - */
#define RPC_DB_GET_KEY_TIME             11232 /**< - */
#define RPC_DB_GET_OPEN_RECORDS         11233 /**< - */
#define RPC_DB_FLUSH_DATABASE           11235 /**< - */
#define RPC_DB_SET_DATA_INDEX2          11236 /**< - */
#define RPC_DB_GET_KEY_INFO             11237 /**< - */
#define RPC_DB_GET_DATA1                11238 /**< - */
#define RPC_DB_SET_NUM_VALUES           11239 /**< - */
#define RPC_DB_CHECK_RECORD             11240 /**< - */
#define RPC_DB_GET_NEXT_LINK            11241 /**< - */

#define RPC_HS_SET_PATH                 11300 /**< - */
#define RPC_HS_DEFINE_EVENT             11301 /**< - */
#define RPC_HS_WRITE_EVENT              11302 /**< - */
#define RPC_HS_COUNT_EVENTS             11303 /**< - */
#define RPC_HS_ENUM_EVENTS              11304 /**< - */
#define RPC_HS_COUNT_VARS               11305 /**< - */
#define RPC_HS_ENUM_VARS                11306 /**< - */
#define RPC_HS_READ                     11307 /**< - */
#define RPC_HS_GET_VAR                  11308 /**< - */
#define RPC_HS_GET_EVENT_ID             11309 /**< - */

#define RPC_EL_SUBMIT                   11400 /**< - */

#define RPC_AL_CHECK                    11500 /**< - */
#define RPC_AL_TRIGGER_ALARM            11501 /**< - */

#define RPC_RC_TRANSITION               12000 /**< - */

#define RPC_ANA_CLEAR_HISTOS            13000 /**< - */

#define RPC_LOG_REWIND                  14000 /**< - */

#define RPC_TEST                        15000 /**< - */

#define RPC_CNAF16                      16000 /**< - */
#define RPC_CNAF24                      16001 /**< - */

#define RPC_MANUAL_TRIG                 17000 /**< - */

#define RPC_ID_WATCHDOG                 99997 /**< - */
#define RPC_ID_SHUTDOWN                 99998 /**< - */
#define RPC_ID_EXIT                     99999 /**< - */

/*------------------------------------------------------------------*/

/**dox***************************************************************/
/** @} *//* end of rpcdefineh */

/**dox***************************************************************/
/** @} *//* end of rpcincludecode */
