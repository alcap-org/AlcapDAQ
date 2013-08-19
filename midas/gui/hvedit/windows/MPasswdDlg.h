// MPasswdDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// MPasswdDlg dialog

class CMPasswdDlg:public CDialog {
// Construction
 public:
   CMPasswdDlg(CWnd * pParent = NULL);  // standard constructor

// Dialog Data
   //{{AFX_DATA(CMPasswdDlg)
   enum { IDD = IDD_PASSDLG };
   CString m_strPassword;
   //}}AFX_DATA


// Overrides
   // ClassWizard generated virtual function overrides
   //{{AFX_VIRTUAL(CMPasswdDlg)
 protected:
    virtual void DoDataExchange(CDataExchange * pDX);   // DDX/DDV support
   //}}AFX_VIRTUAL

// Implementation
 protected:

   // Generated message map functions
   //{{AFX_MSG(CMPasswdDlg)
   // NOTE: the ClassWizard will add member functions here
   //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
};
