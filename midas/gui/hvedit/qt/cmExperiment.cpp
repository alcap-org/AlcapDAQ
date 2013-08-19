/*******************************************************************************************

  cmExperiment.cpp

  class to connect to a midas experiment

*********************************************************************************************

    begin                : Andreas Suter, 2002/11/25
    modfied:             :
    copyright            : (C) 2003 by
    email                : andreas.suter@psi.ch

  $Log: cmExperiment.cpp,v $
  Revision 1.1.1.1  2005/06/20 23:37:12  mucap
  Importing release 1.9.5 of the MIDAS source code as distributed by PSI.
  (Next, I'll commit our local customizations.)

  Revision 1.3  2003/12/30 14:54:26  suter_a
  "doxygenized" code, i.e. added comments which can be handled by doxygen in
  order to generate html- and latex-docu.

  Revision 1.2  2003/05/13 17:29:44  midas
  Fixed compiler warnings

  Revision 1.1  2003/05/09 10:08:09  midas
  Initial revision

  Revision 1.1.1.1  2003/02/27 15:26:12  nemu
  added log info


********************************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "Qt_Pwd.h"
#include "globals.h"

#include "cmExperiment.h"

//**********************************************************************
/*!
 * <p>get password callback routine - <em>CANNOT</em> be a class member 
 * since the c-calling midas routine needs a pointer to void which cannot
 * be accomplished be casting!
 *
 * <p><b>return:</b> void
 * \param password is a pointer to the password variable.
 */
void GetPassword(char *password)
{
  // create password dialog
  Qt_Pwd *pwd = new Qt_Pwd(password);
  // execute password dialog
  pwd->exec();
}

//**********************************************************************
/*!
 * <p>Creates an object to a MIDAS experiment.
 *
 * \param strClientName Name of the client which connects to the experiment, e.g. 'hvEdit'
 * \param parent The parent of an object may be viewed as the object's owner. Setting 
 *               <em>parent</em> to 0 constructs an object with no parent. If the object 
 *               is a widget, it will become a top-level window.  
 * \param name   Name of the object. The object name is some text that can be used to 
 *               identify a QObject.
 */
cmExperiment::cmExperiment(QString strClientName, QObject *parent,
                           const char *name) : QObject(parent, name)
{
  // Set empty strings
  m_strHostName       = "";
  m_strExperimentName = "";
  m_strClientName     = strClientName;
  
  m_bConnected = FALSE;
}

//**********************************************************************
/*!
 * <p>Destroys the object.
 */
cmExperiment::~cmExperiment()
{
}

//**********************************************************************
/*!
 * <p>Establishes the connection to a MIDAS experiment, running on the
 * host strHostName.
 *
 * <p><b>return:</b> 
 *   - TRUE, if connection to the experiment is established
 *   - FALSE, otherwise
 *
 * \param strHostName Name of the host on which the MIDAS experiment runs.
 * \param strExperimentName Name of the MIDAS experiment to which one wants to connect
 * \param strClientName Name of the client which connects to the experiment, e.g. 'hvEdit'
 */
BOOL cmExperiment::Connect(QString strHostName, QString strExperimentName, QString strClientName)
{
  int status;

  // initialize variables
  m_bConnected    = FALSE;
  m_bTimerStarted = FALSE;

  // try to connect to midas host
  do {
    status = cm_connect_experiment1((char *)strHostName.ascii(),
                                    (char *)strExperimentName.ascii(),
                                    (char *)strClientName.ascii(), GetPassword,
                                    DEFAULT_ODB_SIZE, 20000); // increased timeout for W95

    if (status == CM_WRONG_PASSWORD) {
      MidasMessage("Password is incorrect");
      return FALSE;
    }

    if (status != CM_SUCCESS) { // midas error
      char str[256];

      cm_get_error(status, str);
      MidasMessage(str);
    }

  } while (status != CM_SUCCESS);

  // disable watchdog during debugging
#ifdef _DEBUG
  cm_set_watchdog_params(TRUE, 0);
#endif

  // route all messages to MidasMessage
  cm_set_msg_print(MT_ALL, MT_ALL, MidasMessage);

  // get handle to ODB
  cm_get_experiment_database(&m_hDB, NULL);

  // set connected flag
  m_bConnected = TRUE;

  return TRUE;
}

//**********************************************************************
/*!
 * <p>Disconnects from an already connected MIDAS experiment.
 *
 * <p><b>return:</b> void
 */
void cmExperiment::Disconnect()
{
  cm_disconnect_experiment();
  m_bConnected = FALSE;
}

//**********************************************************************
/*!
 * <p>Since all MIDAS clients cyclically have to tell that they are still
 * alive, such a cyclical timer is needed, which is implemented here.
 * This cyclic timer works the following way: Once it is started, it
 * runs in the background till timeout is reached. At timeout it calls
 * a time out routine (timeOutProc) and the whole things starts all over
 * again.
 *
 * <p><b>return:</b> 
 *   - TRUE if timer could have been started
 *   - FALSE otherwise
 */
BOOL cmExperiment::StartTimer()
{
  if (m_bTimerStarted)
    return TRUE;

  // create timer
  timer = new QTimer();

  // connect timer to event handler
  connect(timer, SIGNAL(timeout()), SLOT(timeOutProc()));

  // start timer
  if (!timer->start(500)) {
    MidasMessage("Cannot set timer");
    return FALSE;
  }

  m_bTimerStarted = TRUE;

  return TRUE;
}

//**********************************************************************
/*!
 * <p> This routine is the time out routine of StartTimer and is cyclically 
 * called. It self tells MIDAS that hvEdit is still alive and checks
 * if MIDAS itself is alive and if MIDAS (for what ever reason) kick
 * hvEdit out.
 *
 * <p><b>return:</b> void 
 */
void cmExperiment::timeOutProc()
{
  int status;

  // tell MIDAS that hvEdit is still alive
  status = cm_yield(0);

  // check if MIDAS itself is still alive
  // check further if MIDAS did not through hvEdit out
  if (status == SS_ABORT || status == RPC_SHUTDOWN) {
    MidasMessage("Lost connection to the host");
  }
}
