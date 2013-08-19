// hvedidlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CHveditDlg dialog

class CHveditDlg:public CDialog {
// Construction
 public:
   CHveditDlg(CString strCmdLine);      // standard constructor
   ~CHveditDlg();
   BOOL UpdateChannelDefinition();
   CMKey *GetNameKey() {
      return m_pNamesKey;
   }
   int GetNumChannels() {
      return m_nChannels;
   }
   void Print(CDC * pDC);
   void UpdateListBox(int ch);

// Dialog Data
   //{{AFX_DATA(CHveditDlg)
   enum { IDD = IDD_HVEDIT_DIALOG };
   CStatic m_ctlMessage;
   CComboBox m_ctlInput;
   CListBox m_ctlGroups;
   CListBox m_ctlChannels;
   //}}AFX_DATA

   // ClassWizard generated virtual function overrides
   //{{AFX_VIRTUAL(CHveditDlg)
 protected:
   virtual void DoDataExchange(CDataExchange * pDX);    // DDX/DDV support
   //}}AFX_VIRTUAL

// Implementation
 protected:
   CMExperiment * m_pExperiment;
   CMKey *m_pRootKey;
   CMKey *m_pNamesKey;
   CMKey *m_pDemandKey;
   CMKey *m_pMeasuredKey;

   CString m_HostName, m_ExperimentName;
   CString m_EquipmentName;
   CString m_LastFileName;
   HICON m_hIcon;
   INT m_nChannels, m_nNameLength, m_nGroups, m_iGroup, m_iGroupCache;
   char *m_Name;
   float *m_Demand, *m_Measured, *m_Restore;
   float *m_DemandCache, *m_MeasuredCache;
   INT *m_Selection, *m_Group;
   float m_Voltage;
   BOOL m_bPrintAll;

   BOOL OpenKeys();
   void CloseKeys();
   void FreeMem();
   int ChannelIndex(int ch);
   void UpdateODB(int ch);
   void Increment(const float incr);
   BOOL GetInput(void);
   void SetInput(void);

   // Generated message map functions
   //{{AFX_MSG(CHveditDlg)
   virtual BOOL OnInitDialog();
   afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
   afx_msg void OnPaint();
   afx_msg HCURSOR OnQueryDragIcon();
   afx_msg void OnSet();
   afx_msg void OnSelall();
   afx_msg void OnSelchangeListChannels();
   afx_msg void OnP1();
   afx_msg void OnP10();
   afx_msg void OnP100();
   afx_msg void OnM1();
   afx_msg void OnM10();
   afx_msg void OnM100();
   afx_msg void OnZero();
   afx_msg void OnRestore();
   afx_msg void OnSave();
   afx_msg void OnLoad();
   afx_msg void OnSelchangeListGroup();
   afx_msg void OnDestroy();
   afx_msg void OnConfig();
   afx_msg void OnPrint();
   afx_msg void OnHlp();
   afx_msg void OnAllOff();
   afx_msg void OnTimer(UINT nIDEvent);
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg:public CDialog {
 public:
   CAboutDlg();

// Dialog Data
   //{{AFX_DATA(CAboutDlg)
   enum { IDD = IDD_ABOUTBOX };
   //}}AFX_DATA

// Implementation
 protected:
    virtual void DoDataExchange(CDataExchange * pDX);   // DDX/DDV support
   //{{AFX_MSG(CAboutDlg)
   virtual BOOL OnInitDialog();
   //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
};

int DoMessageBox(const char *str);
INT midas_connect(char *host_name, char *experiment_name);
INT midas_disconnect(void);
