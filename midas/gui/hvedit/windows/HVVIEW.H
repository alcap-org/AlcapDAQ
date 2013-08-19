// hvview.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CHVView view

class CHVView : public CView
{
protected:
	DECLARE_DYNCREATE(CHVView)

// Attributes
public:

// Operations
public:
	CHVView() {};
	~CHVView() {};
  
  void OnFilePrint(CHveditDlg* pHveditDlg);
  CHveditDlg *m_pHveditDlg;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CHVView)
	protected:
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	//}}AFX_VIRTUAL

// Implementation
protected:
	friend class CHveditDlg;

	// Generated message map functions
protected:
	//{{AFX_MSG(CHVView)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
