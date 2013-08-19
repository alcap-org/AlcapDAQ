// PrintDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CPrintDlg dialog

class CPrintDlg:public CDialog {
// Construction
 public:
   CPrintDlg(CWnd * pParent = NULL);    // standard constructor

// Dialog Data
   //{{AFX_DATA(CPrintDlg)
   enum { IDD = IDD_PRINT };
   int m_nWhat;
   //}}AFX_DATA


// Overrides
   // ClassWizard generated virtual function overrides
   //{{AFX_VIRTUAL(CPrintDlg)
 protected:
    virtual void DoDataExchange(CDataExchange * pDX);   // DDX/DDV support
   //}}AFX_VIRTUAL

// Implementation
 protected:

   // Generated message map functions
   //{{AFX_MSG(CPrintDlg)
   // NOTE: the ClassWizard will add member functions here
   //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
};
