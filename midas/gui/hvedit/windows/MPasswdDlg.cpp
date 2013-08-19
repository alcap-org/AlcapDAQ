// MPasswdDlg.cpp : implementation file
//

#include "stdafx.h"
#include "hvedit.h"
#include "MPasswdDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMPasswdDlg dialog


CMPasswdDlg::CMPasswdDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CMPasswdDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CMPasswdDlg)
	m_strPassword = _T("");
	//}}AFX_DATA_INIT
}


void CMPasswdDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMPasswdDlg)
	DDX_Text(pDX, IDC_EDIT, m_strPassword);
	DDV_MaxChars(pDX, m_strPassword, 31);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CMPasswdDlg, CDialog)
	//{{AFX_MSG_MAP(CMPasswdDlg)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// MPasswdDlg message handlers
