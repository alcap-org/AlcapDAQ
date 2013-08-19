// hvconfig.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CHvConfig dialog

class CHvConfig:public CDialog {
// Construction
 public:
   CHvConfig(CWnd * pParent = NULL);    // standard constructor

// Dialog Data
   //{{AFX_DATA(CHvConfig)
   enum { IDD = IDD_CONFIG };
   CEdit m_ctlInput;
   CListBox m_ctlList;
   //}}AFX_DATA


// Overrides
   // ClassWizard generated virtual function overrides
   //{{AFX_VIRTUAL(CHvConfig)
 protected:
    virtual void DoDataExchange(CDataExchange * pDX);   // DDX/DDV support
   //}}AFX_VIRTUAL

// Implementation
 protected:
   char *m_Name;
   int m_NameLength;

   // Generated message map functions
   //{{AFX_MSG(CHvConfig)
   virtual BOOL OnInitDialog();
   afx_msg void OnDestroy();
   afx_msg void OnSelchangeList();
   virtual void OnOK();
   afx_msg void OnChangeEdit();
   //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
};
