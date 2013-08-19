// hvconfig.cpp : implementation file
//

#include "stdafx.h"
#include "midas.h"
#include "MExperiment.h"
#include "MKey.h"
#include "hvedit.h"
#include "hvconfig.h"
#include "hvedidlg.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CHvConfig dialog


CHvConfig::CHvConfig(CWnd* pParent /*=NULL*/)
	: CDialog(CHvConfig::IDD, pParent)
{
	//{{AFX_DATA_INIT(CHvConfig)
	//}}AFX_DATA_INIT
}


void CHvConfig::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CHvConfig)
	DDX_Control(pDX, IDC_EDIT, m_ctlInput);
	DDX_Control(pDX, IDC_LIST, m_ctlList);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CHvConfig, CDialog)
	//{{AFX_MSG_MAP(CHvConfig)
	ON_WM_DESTROY()
	ON_LBN_SELCHANGE(IDC_LIST, OnSelchangeList)
	ON_EN_CHANGE(IDC_EDIT, OnChangeEdit)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CHvConfig message handlers

BOOL CHvConfig::OnInitDialog() 
{
int        i, tabs[2], size;
CHveditDlg *dlg;
CString    str;
CMKey*     key;

	CDialog::OnInitDialog();
	
  dlg = (CHveditDlg *) AfxGetMainWnd();

  tabs[0] = 20;
  m_ctlList.SetTabStops(1, tabs);

  key = dlg->GetNameKey();

  m_NameLength = key->GetItemSize();
  size = key->GetNumValues() * m_NameLength;
  m_Name = (char *) malloc(size);

  key->GetData(m_Name, &size, TID_STRING);

  for (i=0 ; i<key->GetNumValues() ; i++)
    {
    str.Format("%1d\t%s", i, m_Name + i*m_NameLength);
    m_ctlList.AddString(str);
    }

	return TRUE;
}

/*----------------------------------------------------------------------------*/

void CHvConfig::OnDestroy() 
{
	CDialog::OnDestroy();
	
  free(m_Name);	
}

/*----------------------------------------------------------------------------*/

void CHvConfig::OnSelchangeList() 
{
int i;

  i = m_ctlList.GetCurSel();
  m_ctlInput.SetWindowText(m_Name + i*m_NameLength);
}

/*----------------------------------------------------------------------------*/

void CHvConfig::OnOK() 
{
CMKey*     key;
CHveditDlg *dlg;
	
  dlg = (CHveditDlg *) AfxGetMainWnd();
                   
  // Set changed names in ODB
  key = dlg->GetNameKey();
  key->SetData(m_Name, key->GetNumValues(), TID_STRING);
	
	CDialog::OnOK();
}

/*----------------------------------------------------------------------------*/

void CHvConfig::OnChangeEdit() 
{
int     i;
CString str;

  i = m_ctlList.GetCurSel();
  m_ctlInput.GetWindowText(str);

  strcpy(m_Name + i*m_NameLength, (LPCTSTR) str);
  str.Format("%1d\t%s", i, m_Name + i*m_NameLength);
  m_ctlList.DeleteString(i);
  m_ctlList.InsertString(i, str);
  m_ctlList.SetCurSel(i);
}
