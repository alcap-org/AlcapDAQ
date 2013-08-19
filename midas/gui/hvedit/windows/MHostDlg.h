// MHostDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CHostDlg dialog

class CMHostDlg:public CDialog {
// Construction
 public:
   CMHostDlg(CWnd * pParent = NULL);    // standard constructor
   CString m_strHostName;
   CString m_strExperimentName;
   BOOL m_ValidHost;

// Dialog Data
   //{{AFX_DATA(CMHostDlg)
   enum { IDD = IDD_HOSTDLG };
   CListBox m_ListExperiment;
   CString m_HostName;
   // NOTE: the ClassWizard will add data members here
   //}}AFX_DATA


// Overrides
   // ClassWizard generated virtual function overrides
   //{{AFX_VIRTUAL(CMHostDlg)
 protected:
    virtual void DoDataExchange(CDataExchange * pDX);   // DDX/DDV support
   //}}AFX_VIRTUAL

// Implementation
 protected:

   // Generated message map functions
   //{{AFX_MSG(CMHostDlg)
    afx_msg void OnKillfocusHost();
   virtual void OnOK();
   //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
};
