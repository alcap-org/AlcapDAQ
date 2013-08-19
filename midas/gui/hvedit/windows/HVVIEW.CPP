// hvview.cpp : implementation file
//

#include "stdafx.h"
#include "midas.h"
#include "MExperiment.h"
#include "MKey.h"
#include "hvedit.h"
#include "hvedidlg.h"
#include "hvview.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CHVView

IMPLEMENT_DYNCREATE(CHVView, CView)

BEGIN_MESSAGE_MAP(CHVView, CView)
	//{{AFX_MSG_MAP(CHVView)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Drawing code

BOOL CHVView::OnPreparePrinting(CPrintInfo* pInfo) 
{
  // default preparation
  pInfo->SetMaxPage(1);

	return DoPreparePrinting(pInfo);
}

void CHVView::OnFilePrint(CHveditDlg *pHveditDlg)
{
  m_pHveditDlg = pHveditDlg; 
  CView::OnFilePrint();
}

void CHVView::OnDraw(CDC* pDC)
{
  m_pHveditDlg->Print(pDC);
}
