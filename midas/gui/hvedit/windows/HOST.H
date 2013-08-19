// host.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CHost dialog

class CHost : public CDialog
{
// Construction
public:
	CHost(CWnd* pParent = NULL);   // standard constructor
  CString m_ExperimentName;
  BOOL    m_ValidHost;

// Dialog Data
	//{{AFX_DATA(CHost)
	enum { IDD = IDD_HOST };
	CListBox	m_ListExperiment;
	CString	m_HostName;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CHost)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CHost)
	afx_msg void OnKillfocusHost();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
