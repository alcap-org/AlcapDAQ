// host.cpp : implementation file
//

#include "stdafx.h"
#include "hvedit.h"
#include "host.h"
#include "midas.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CHost dialog


CHost::CHost(CWnd* pParent /*=NULL*/)
	: CDialog(CHost::IDD, pParent)
{
  m_ValidHost = FALSE;

	//{{AFX_DATA_INIT(CHost)
	m_HostName = _T("");
	//}}AFX_DATA_INIT
}


void CHost::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CHost)
	DDX_Control(pDX, IDC_EXPERIMENT, m_ListExperiment);
	DDX_Text(pDX, IDC_HOST, m_HostName);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CHost, CDialog)
	//{{AFX_MSG_MAP(CHost)
	ON_EN_KILLFOCUS(IDC_HOST, OnKillfocusHost)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CHost message handlers

void CHost::OnKillfocusHost() 
{
char   expts[MAX_EXPERIMENT][NAME_LENGTH];
int    status, i;
static BOOL bActive=FALSE;
	
  /* avoid recursive calls */
  if (bActive)
    return;
  bActive = TRUE;

  /* get list of experiments on host */
  UpdateData(TRUE);

  /* retrieve list of experiments and make selection */
  status = cm_list_experiments((char *) (LPCTSTR) m_HostName, expts);

  if (status != CM_SUCCESS)
    {
    AfxMessageBox(CString("Cannot connect to host ")+m_HostName);
    m_ListExperiment.ResetContent();
    m_ValidHost = FALSE;
    }
  else
    {
    m_ValidHost = TRUE;

    /* fill list box */
    m_ListExperiment.ResetContent();
    for (i=0 ; expts[i][0] ; i++)
      m_ListExperiment.AddString(expts[i]);
    }

  bActive = FALSE;
}

void CHost::OnOK() 
{
  UpdateData(TRUE);

  if (m_ListExperiment.GetCount() == 0 ||
      (m_ListExperiment.GetCurSel() == LB_ERR && 
       m_ListExperiment.GetCount()>1))
    {
    /* if experiment list is missing or not selected get it */
    OnKillfocusHost();
    }
  else
    {
    /* set experiment and return */
    if (m_ListExperiment.GetCurSel() == LB_ERR)
      m_ListExperiment.GetText(0, m_ExperimentName);
    else
      m_ListExperiment.GetText(m_ListExperiment.GetCurSel(), m_ExperimentName);

		CDialog::OnOK();
    }
}
