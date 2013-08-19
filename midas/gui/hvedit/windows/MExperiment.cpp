// MExperiment.cpp : implementation file
//

#include "stdafx.h"
#include "hvedit.h"
#include "midas.h"
#include "MHostDlg.h"
#include "MExperiment.h"
#include "MPasswddlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// Message box for midas routines

int MidasMessage(const char *str)
{
char name[80];

  cm_get_client_info(name);
  MessageBox(GetFocus(), str, name, MB_OK);
  return 0;
}

/////////////////////////////////////////////////////////////////////////////
// Timer callback for cm_yield

void CALLBACK EXPORT TimerProc(HWND hWnd,UINT nMsg,UINT nIDEvent,DWORD dwTime)
{
int   status;
CWnd* pWnd;

  pWnd = AfxGetMainWnd();

  pWnd->KillTimer(1175);

  status = cm_yield(0);

  if (status == SS_ABORT || status == RPC_SHUTDOWN)
    {
    pWnd->PostMessage(WM_CLOSE, 0, 0);
    return;
    }

  if (!pWnd->SetTimer(1175, 100, TimerProc))
    pWnd->MessageBox("Cannot set timer");
}

/////////////////////////////////////////////////////////////////////////////
// callback for password query

void GetPassword(char *password)
{
CMPasswdDlg passwddlg;
  
  if (passwddlg.DoModal() == IDCANCEL)
    {
    password[0] = 0;
    return;
    }

  strcpy(password, passwddlg.m_strPassword);

  return;
}

/////////////////////////////////////////////////////////////////////////////
// CExperiment

BOOL CMExperiment::HostDlg()
{
CMHostDlg hostdlg;
  
  hostdlg.m_strHostName = m_strHostName;

  if (hostdlg.DoModal() == IDCANCEL)
    return FALSE;

  m_strHostName = hostdlg.m_strHostName;
  m_strExperimentName = hostdlg.m_strExperimentName;

  return TRUE;
}

BOOL CMExperiment::Connect(CString strHostName, CString strExperimentName, CString strClientName)
{
int   status;
char  expts[MAX_EXPERIMENT][NAME_LENGTH];

  m_strHostName = strHostName;
  m_strExperimentName = strExperimentName;
  m_strClientName = strClientName;
  m_bConnected = FALSE;
  m_bTimerStarted = FALSE;
  m_pWnd = NULL;

  // start host dialog if more than one experiment defined
  if (m_strExperimentName == "")
    {
    status = cm_list_experiments((char *) (LPCTSTR) strHostName, expts);
    if (status != CM_SUCCESS)
      {
      char str[256];

      cm_get_error(status, str);
      MidasMessage(str);
    
      if (!HostDlg())
        return FALSE;
      }
    else
      {
      // select experiment if more than one
      if (expts[1][0])
        if (!HostDlg())
          return FALSE;
      }
    }

  // try to connect to midas host
  do
    {
    status = cm_connect_experiment1((char *)(LPCTSTR)m_strHostName, 
                                    (char *)(LPCTSTR)m_strExperimentName, 
                                    (char *)(LPCTSTR)m_strClientName, GetPassword,
                                    DEFAULT_ODB_SIZE, 20000); /* increased timeout for W95 */
    
    if (status == CM_WRONG_PASSWORD)
      {
      MidasMessage("Password is incorrect");
      return FALSE;
      }

    if (status != CM_SUCCESS)
      {
      char str[256];

      cm_get_error(status, str);
      MidasMessage(str);
    
      if (!HostDlg())
        return FALSE;
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

BOOL CMExperiment::StartTimer()
{
  if (m_bTimerStarted)
    return TRUE;

  // remember window
  m_pWnd = AfxGetMainWnd();
  if (!m_pWnd)
    return FALSE;

  // start timer to call cm_yield
  if (!m_pWnd->SetTimer(1175, 500, TimerProc))
    {
    MidasMessage("Cannot set timer");
    return FALSE;
    }

  m_bTimerStarted = TRUE;

  return TRUE;
}

CMExperiment::CMExperiment(CString strClientName)
{
  // Set empty strings
  m_strHostName =       "";
  m_strExperimentName = "";

  Connect(m_strHostName, m_strExperimentName, strClientName);
}

CMExperiment::CMExperiment(CString strCmdLine, CString strClientName)
{
char *token;

  // Set empty strings
  m_strHostName =       "";
  m_strExperimentName = "";

  // Overwrite with command line parameters
  token = strtok((char *) strCmdLine.GetBuffer(80), " ");
  while (token != NULL)
    {
    if (token[0] == '-')
      {
      if (token[1] == 'e')
        m_strExperimentName = strtok(NULL, " ");
      else if (token[1] == 'h')
        m_strHostName = strtok(NULL, " ");
      }
    token = strtok(NULL, " ");
    }

  Connect(m_strHostName, m_strExperimentName, strClientName);
}

CMExperiment::~CMExperiment()
{
  // remove message dispatcher
  cm_set_msg_print(MT_ALL, MT_ALL, NULL);

  // Disconnect from MIDAS host
  if (m_bConnected)
    cm_disconnect_experiment();
}
