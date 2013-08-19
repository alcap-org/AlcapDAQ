// MExperiment.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CExperiment window

class CMExperiment {
// Construction
 public:
   CMExperiment(CString strCmdLine, CString strClientName);
   CMExperiment(CString strClientName);

// Attributes
 private:
   CString m_strHostName;
   CString m_strExperimentName;
   CString m_strClientName;
   BOOL m_bConnected;
   BOOL m_bTimerStarted;
   HNDLE m_hDB;
   CWnd *m_pWnd;

// Operations
 public:
    CString GetHostName() {
      return m_strHostName;
   };
   CString GetExperimentName() {
      return m_strExperimentName;
   };
   CString GetClientName() {
      return m_strClientName;
   };
   HNDLE GetHdb() {
      return m_hDB;
   };
   BOOL IsConnected() {
      return m_bConnected;
   };
   BOOL StartTimer();

// Implementation
 public:
   virtual ~ CMExperiment();

 protected:
   BOOL HostDlg(void);
   Connect(CString, CString, CString);
};

/////////////////////////////////////////////////////////////////////////////
