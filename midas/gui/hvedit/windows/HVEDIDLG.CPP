// hvedidlg.cpp : implementation file
//

#include "stdafx.h"
#include "midas.h"
#include "MExperiment.h"
#include "MKey.h"
#include "hvedit.h"
#include "hvedidlg.h"
#include "hvconfig.h"
#include "hvview.h"
#include "printdlg.h"


#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

#define FE_TIMEOUT 90 // No update of measured values aftet this time (in sec)
                      // displays warning about FE not running

void HVChanged(HNDLE hDB, HNDLE hKey, void *info);

/////////////////////////////////////////////////////////////////////////////
// CHveditDlg dialog

CHveditDlg::CHveditDlg(CString strCmdLine)
	: CDialog(CHveditDlg::IDD, NULL)
{
char *token;
CString str;

	//{{AFX_DATA_INIT(CHveditDlg)
	//}}AFX_DATA_INIT

	// Load icon
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

  // Set default equipment name
  m_EquipmentName = "HV";

  // Overwrite with command line -q flag
  str = strCmdLine;
  token = strtok((char *) str.GetBuffer(80), " ");
  while (token != NULL)
    {
    if (token[0] == '-')
      {
      if (token[1] == 'q')
        m_EquipmentName = strtok(NULL, " ");
      }
    token = strtok(NULL, " ");
    }

  // Connect to experiment
  m_pExperiment = new CMExperiment(strCmdLine, "HVEdit");
  m_HostName = m_pExperiment->GetHostName();
  m_ExperimentName = m_pExperiment->GetExperimentName();

  // init local memory
  m_Name = NULL;
  m_Selection = NULL;
  m_Group = NULL;
  m_Demand = NULL;
  m_Measured = NULL;
  m_Restore = NULL;
  m_DemandCache = NULL;
  m_MeasuredCache = NULL;

  m_pRootKey = NULL;
  m_pNamesKey = NULL;
  m_pDemandKey = NULL;
  m_pMeasuredKey = NULL;

  m_nChannels = m_nGroups = 0; 
  m_iGroup = 0;
  m_LastFileName = CString("");
}

/*----------------------------------------------------------------------------*/

CHveditDlg::~CHveditDlg()
{
  CloseKeys();
  FreeMem();

  delete m_pExperiment;
}

/*----------------------------------------------------------------------------*/

void CHveditDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CHveditDlg)
	DDX_Control(pDX, IDC_MESSAGE, m_ctlMessage);
	DDX_Control(pDX, IDC_COMBO1, m_ctlInput);
	DDX_Control(pDX, IDC_LIST_GROUP, m_ctlGroups);
	DDX_Control(pDX, IDC_LIST_CHANNELS, m_ctlChannels);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CHveditDlg, CDialog)
	//{{AFX_MSG_MAP(CHveditDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_SET, OnSet)
	ON_BN_CLICKED(IDC_SELALL, OnSelall)
	ON_LBN_SELCHANGE(IDC_LIST_CHANNELS, OnSelchangeListChannels)
	ON_BN_CLICKED(IDC_P1, OnP1)
	ON_BN_CLICKED(IDC_P10, OnP10)
	ON_BN_CLICKED(IDC_P100, OnP100)
	ON_BN_CLICKED(IDC_M1, OnM1)
	ON_BN_CLICKED(IDC_M10, OnM10)
	ON_BN_CLICKED(IDC_M100, OnM100)
	ON_BN_CLICKED(IDC_ZERO, OnZero)
	ON_BN_CLICKED(IDC_RESTORE, OnRestore)
	ON_BN_CLICKED(IDC_SAVE, OnSave)
	ON_BN_CLICKED(IDC_LOAD, OnLoad)
	ON_LBN_SELCHANGE(IDC_LIST_GROUP, OnSelchangeListGroup)
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_CONFIG, OnConfig)
	ON_BN_CLICKED(IDC_PRINT, OnPrint)
	ON_BN_CLICKED(IDC_HLP, OnHlp)
	ON_BN_CLICKED(IDC_ALL_OFF, OnAllOff)
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CHveditDlg message handlers

/*----------------------------------------------------------------------------*/

BOOL CHveditDlg::OpenKeys()
{
  // Root key
  m_pRootKey = new CMKey(m_pExperiment, "/Equipment/"+m_EquipmentName);
  if (!m_pRootKey->IsValid())
    return FALSE;

  // Names key
  m_pNamesKey = new CMKey(m_pRootKey, "Settings/Names");
  if (!m_pNamesKey->IsValid())
    return FALSE;

  // Demand key
  m_pDemandKey = new CMKey(m_pRootKey, "Variables/Demand");
  if (!m_pDemandKey->IsValid())
    return FALSE;

  // Measured key
  m_pMeasuredKey = new CMKey(m_pRootKey, "Variables/Measured");
  if (!m_pMeasuredKey->IsValid())
    return FALSE;

	return TRUE;
}

/*----------------------------------------------------------------------------*/

void CHveditDlg::CloseKeys()
{
  if (m_pRootKey)
    delete m_pRootKey;
  if (m_pNamesKey)
    delete m_pNamesKey;
  if (m_pDemandKey)
    delete m_pDemandKey;
  if (m_pMeasuredKey)
    delete m_pMeasuredKey;
}

/*----------------------------------------------------------------------------*/

void CHveditDlg::FreeMem()
{
  // free local memory
  if (m_Selection)
    {
    free(m_Selection);
    free(m_Group);
    free(m_Restore);
    free(m_DemandCache);
    free(m_MeasuredCache);
    m_Selection = NULL;
    }
}

/*----------------------------------------------------------------------------*/

BOOL CHveditDlg::OnInitDialog()
{
INT      tabs[5];
CString  str;
int      x, y;

	CDialog::OnInitDialog();

  // Check if connected to MIDAS
  if (!m_pExperiment->IsConnected() || !OpenKeys())
    {
    EndDialog(1);
    return TRUE;
    }

  // Set caption text
  if (m_HostName.IsEmpty())
    str = m_EquipmentName + " - [local]";
  else
    {
    str = m_EquipmentName + " - \\\\" + m_HostName;
    if (!m_ExperimentName.IsEmpty())
       {
      str += "\\";
      str += m_ExperimentName;
      }
    }                           
  SetWindowText(str);

  // Set tabs in listbox
  tabs[0] = 37;
  tabs[1] = 71;
  m_ctlChannels.SetTabStops(2, tabs);

  if (!UpdateChannelDefinition())
    return FALSE;

  UpdateListBox(-1);
  	
 	// Set initial state of radio buttons
  m_ctlInput.SetWindowText("0");
  m_ctlChannels.SetSel(0, TRUE);

  // Move window to previous position
  x = AfxGetApp()->GetProfileInt("Window Position", "Left", 100);
  y = AfxGetApp()->GetProfileInt("Window Position", "Top", 100);
  /*
	SetWindowPos(NULL, x, y, -1, -1,
		SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
  */

	// Add "About..." menu item to system menu.
	CMenu* pSysMenu = GetSystemMenu(FALSE);
	CString strAboutMenu;
	strAboutMenu.LoadString(IDS_ABOUTBOX);
	if (!strAboutMenu.IsEmpty())
    {
		pSysMenu->AppendMenu(MF_SEPARATOR);
		pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
    }

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

  // Start timer for checking FE status
  if (!SetTimer(1176, 10000, NULL))
    MessageBox("Cannot set timer");

	return TRUE;  // return TRUE  unless you set the focus to a control
}

/*----------------------------------------------------------------------------*/

BOOL CHveditDlg::GetInput()
// Read combo box input and convert it to m_Voltage 
{
CString Input;
float   tmp;

  m_ctlInput.GetWindowText(Input);
  tmp = (float) atof(Input);

  if (tmp < 0)
    {
    MessageBox("Input value must be positive");
    return FALSE;
    }

  m_Voltage = tmp;

  // Remember last 10 inputs
  m_ctlInput.InsertString(0, Input);
  m_ctlInput.DeleteString(10);

  return TRUE;
}

void CHveditDlg::SetInput() 
// Convert m_Voltage to string and set it to combo box
{
char  str[80];

  sprintf(str, "%g", m_Voltage);
  m_ctlInput.SetWindowText(str);
}

/*----------------------------------------------------------------------------*/

// A member function cannot be passed to db_open_record. Therefore, NamesChanged
// works as a stub to call UpdateChannelDefinition
void NamesChanged(int,int,void*)
{
CHveditDlg *dlg;
 
  dlg = (CHveditDlg *) AfxGetMainWnd();
  dlg->UpdateChannelDefinition();
  dlg->UpdateListBox(-1);
}

BOOL CHveditDlg::UpdateChannelDefinition()
{
int      i, j;
char     str[80];

  // Free used memory
  FreeMem();

  // Remove hot links
  m_pNamesKey->Unlink();
  m_pDemandKey->Unlink();
  m_pMeasuredKey->Unlink();
  
  // Update keys
  m_pNamesKey->UpdateKey();
  m_pDemandKey->UpdateKey();
  m_pMeasuredKey->UpdateKey();

  m_nChannels   = m_pNamesKey->GetNumValues();
  m_nNameLength = m_pNamesKey->GetItemSize();
  m_nGroups = 0;
  m_iGroupCache = -1;
  m_Selection = (int *) malloc(m_nChannels * sizeof(int));
  m_Group = (int *) calloc(m_nChannels, sizeof(int));
  m_Restore = (float*) calloc(m_nChannels, sizeof(float));
  m_DemandCache = (float *) calloc(m_nChannels, sizeof(float));
  m_MeasuredCache = (float *) calloc(m_nChannels, sizeof(float));
  
  // Link names entry to m_Name array
  m_pNamesKey->HotLink((void **) &m_Name, MODE_READ | MODE_ALLOC, NamesChanged);

  m_ctlGroups.ResetContent();

  // Analyze names, generate groups
  for (i=0 ; i<m_nChannels ; i++)
    {
    if (strchr((const char *) m_Name+i*m_nNameLength, '%'))
      {
      strcpy(str, (const char *) m_Name+i*m_nNameLength);
      *strchr((const char *) str, '%') = 0;
      j = m_ctlGroups.FindStringExact(-1, str);
      if (j == LB_ERR)
        {
        m_ctlGroups.AddString(str);
        m_Group[i] = m_nGroups;
        m_nGroups++;
        }
      else
        m_Group[i] = j;
      }
    }

  m_ctlGroups.SetCurSel(m_iGroup);

  // Link demand entry to m_Demand array
  if (m_pDemandKey->GetNumValues() != m_nChannels || m_pDemandKey->GetType() != TID_FLOAT)
    {
    MessageBox("Wrong 'demand' entry in HV values");
    return FALSE;
    }
  m_pDemandKey->HotLink((void **) &m_Demand, MODE_READ | MODE_ALLOC, HVChanged);

  // Link measured entry to m_Measured array
  if (m_pMeasuredKey->GetNumValues() != m_nChannels || m_pMeasuredKey->GetType() != TID_FLOAT)
    {
    MessageBox("Wrong 'measured' entry in HV values");
    return FALSE;
    }
  m_pMeasuredKey->HotLink((void **) &m_Measured, MODE_READ | MODE_ALLOC, HVChanged);

  return TRUE;
}

/*----------------------------------------------------------------------------*/

int CHveditDlg::ChannelIndex(int ch)
{
int i,j;

  // Find index to m_Demand etc. according to selected
  // channel 'ch' and group 'm_iGroup'

  for (i=0,j=0 ; i<m_nChannels ; i++)
    {
    if (m_Group[i] == m_iGroup)
      j++;
    if (j == ch+1)
      return i;
    }

  return 0;    
}

/*----------------------------------------------------------------------------*/

void CHveditDlg::UpdateListBox(int ch)
{
char   data_str[256], full_line[256];
int    i, top, n;
BOOL   bNeedUpdate;

  // Transfer radio button setting to dialog
  UpdateData(FALSE);

  top = m_ctlChannels.GetTopIndex();
  n = m_ctlChannels.GetSelItems(m_nChannels, m_Selection);

  if (ch >= 0)
    {
    i = ChannelIndex(ch);

    db_sprintf(data_str, m_Name, m_pNamesKey->GetItemSize(), i, TID_STRING);

    if (strchr(data_str, '%'))
      strcpy(data_str, strchr(data_str, '%')+1);

    strcpy(full_line, data_str);
    strcat(full_line, "\t");
    db_sprintf(data_str, m_Demand, sizeof(float), i, TID_FLOAT);
    strcat(full_line, data_str);
    strcat(full_line, "\t");
    db_sprintf(data_str, m_Measured, sizeof(float), i, TID_FLOAT);
    strcat(full_line, data_str);

    m_ctlChannels.DeleteString(ch);
    m_ctlChannels.InsertString(ch, full_line);
    m_ctlChannels.SetSel(ch, TRUE);
    
    // Update cache
    m_DemandCache[i] = m_Demand[i];
    m_MeasuredCache[i] = m_Measured[i];
    }
  else
    {
    // Check if listbox needs update
    bNeedUpdate = FALSE;

    if (m_iGroup == m_iGroupCache)
      {
      for (i=0 ; i<m_nChannels ; i++)
        {
        if (m_Group[i] == m_iGroup &&
           (m_Demand[i] != m_DemandCache[i] ||
            m_Measured[i] != m_MeasuredCache[i]))
          {
          bNeedUpdate = TRUE;
          break;
          }
        }
      if (!bNeedUpdate)
        return;
      }
    else
      bNeedUpdate = TRUE;

    m_iGroupCache = m_iGroup;

    m_ctlChannels.SetRedraw(FALSE);
    m_ctlChannels.ResetContent();

    for (i=0 ; i<m_nChannels ; i++)
      {
      if (m_Group[i] != m_iGroup)
        continue;

      db_sprintf(data_str, m_Name, m_pNamesKey->GetItemSize(), i, TID_STRING);

      // Update cache
      m_DemandCache[i] = m_Demand[i];
      m_MeasuredCache[i] = m_Measured[i];

      if (strchr(data_str, '%'))
        strcpy(data_str, strchr(data_str, '%')+1);

      strcpy(full_line, data_str);
      strcat(full_line, "\t");
      db_sprintf(data_str, m_Demand, sizeof(float), i, TID_FLOAT);
      strcat(full_line, data_str);
      strcat(full_line, "\t");
      db_sprintf(data_str, m_Measured, sizeof(float), i, TID_FLOAT);
      strcat(full_line, data_str);

      m_ctlChannels.AddString(full_line);
      }

    for (i=0 ; i<n ; i++)
      m_ctlChannels.SetSel(m_Selection[i], TRUE);

    n = m_ctlChannels.GetSelItems(m_nChannels, m_Selection);
    if (n==0)
      m_ctlChannels.SetSel(0, TRUE);

    m_ctlChannels.SetTopIndex(top);

    m_ctlChannels.SetRedraw(TRUE);
    }

  /* display warning message if FE not running */
  if (m_pMeasuredKey->GetKeyTime() > FE_TIMEOUT)
    m_ctlMessage.SetWindowText("WARNING: FE not running");
  else
    m_ctlMessage.SetWindowText("");
}

/*----------------------------------------------------------------------------*/

void CHveditDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
    {
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	  }
	else
	  {
		CDialog::OnSysCommand(nID, lParam);
	  }
}

/*----------------------------------------------------------------------------*/

void CHveditDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CHveditDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

/*----------------------------------------------------------------------------*/

void CHveditDlg::OnPrint() 
{
CHVView hvview;
CDocument doc;
CPrintDlg dlgPrint;

  dlgPrint.m_nWhat = 0;
  dlgPrint.DoModal();
  m_bPrintAll = (dlgPrint.m_nWhat == 1);

  doc.SetTitle("HV Voltages");

  hvview.m_pDocument = &doc;
  hvview.OnFilePrint(this);
  hvview.m_pDocument = NULL;
} 

/*----------------------------------------------------------------------------*/

void CHveditDlg::UpdateODB(int ch)
{
INT status, size;

  // Update demand values in database
  if (ch >= 0)
    status = m_pDemandKey->SetDataIndex(m_Demand+ch, ch, TID_FLOAT);
  else
    status = m_pDemandKey->SetData(m_Demand, m_nChannels, TID_FLOAT);

  if (status == DB_NO_ACCESS)
    {
    size = m_nChannels * sizeof(float);
    m_pDemandKey->GetData(m_Demand, &size, TID_FLOAT);
    MessageBox("Change of demand values currently not allowed");
    }
}

/*----------------------------------------------------------------------------*/

void CHveditDlg::OnAllOff() 
{
int i;

  if (MessageBox("Are you sure to set all channels in all groups to zero?", 
                "HV Edit", MB_YESNO) == IDYES)
    {
    for (i=0 ; i<m_nChannels ; i++)
      {
      if (m_Demand[i] > 0)
        m_Restore[i] = m_Demand[i];
      m_Demand[i] = 0.f;
      }

    UpdateODB(-1);
    UpdateListBox(-1);
    }
	
}

/*----------------------------------------------------------------------------*/

void CHveditDlg::OnSet() 
{
int n, i;

  // Update m_Voltage
  if (!GetInput())
    return;

  // Get current selection
  n = m_ctlChannels.GetSelItems(m_nChannels, m_Selection);

  // Set voltage to selected channels
  for (i=0 ; i<n ; i++)
    m_Demand[ ChannelIndex(m_Selection[i]) ] = m_Voltage;

	UpdateODB(n == 1 ? ChannelIndex(m_Selection[0]) : -1);
  UpdateListBox(n == 1 ? m_Selection[0] : -1);
}



/*----------------------------------------------------------------------------*/

void CHveditDlg::OnSelall() 
{
	m_ctlChannels.SetSel(-1, TRUE);	
}

/*----------------------------------------------------------------------------*/

void CHveditDlg::OnSelchangeListChannels() 
{
int  n;

  n = m_ctlChannels.GetSelItems(m_nChannels, m_Selection);
  
  if(n>0)
    {
    m_Voltage = m_Demand[ChannelIndex(m_Selection[0])];
    SetInput();
    }
}

/*----------------------------------------------------------------------------*/

void CHveditDlg::OnSelchangeListGroup() 
{
  m_iGroup = m_ctlGroups.GetCurSel();
  UpdateListBox(-1);
}

/*----------------------------------------------------------------------------*/

void CHveditDlg::Increment(const float incr)
{
int   n, i;

  // Get current selection
  n = m_ctlChannels.GetSelItems(m_nChannels, m_Selection);

  // Set voltage to selected channels
  for (i=0 ; i<n ; i++)
    m_Demand[ ChannelIndex(m_Selection[i]) ] =
      max(0, m_Demand[ ChannelIndex(m_Selection[i]) ] + incr);

  if (n>0)
    m_Voltage = m_Demand[ ChannelIndex(m_Selection[0]) ];

  SetInput();

	UpdateODB(n == 1 ? ChannelIndex(m_Selection[0]) : -1);
  UpdateListBox(n == 1 ? m_Selection[0] : -1);
}

void CHveditDlg::OnP1() 
{
	Increment((float) 1);
}

void CHveditDlg::OnP10() 
{
	Increment((float) 10);
}

void CHveditDlg::OnP100() 
{
	Increment((float) 100);
}

void CHveditDlg::OnM1() 
{
	Increment((float) -1);
}

void CHveditDlg::OnM10() 
{
	Increment((float) -10);
}

void CHveditDlg::OnM100() 
{
	Increment((float) -100);
}

/*----------------------------------------------------------------------------*/

void CHveditDlg::OnZero() 
{
int n, i, j;

  // Get current selection
  n = m_ctlChannels.GetSelItems(m_nChannels, m_Selection);

  // Save voltages in Restore buffer, then zero them
  for (i=0 ; i<n ; i++)
    {
    j = ChannelIndex(m_Selection[i]);
    if (m_Demand[j] > 0)
      m_Restore[j] = m_Demand[j];
    m_Demand[j] = 0.f;
    }

	UpdateODB(n == 1 ? ChannelIndex(m_Selection[0]) : -1);
  UpdateListBox(n == 1 ? m_Selection[0] : -1);
}

/*----------------------------------------------------------------------------*/

void CHveditDlg::OnRestore() 
{
int n, i;

  // Get current selection
  n = m_ctlChannels.GetSelItems(m_nChannels, m_Selection);

  // Retrieve voltages from Restore buffer
  for (i=0 ; i<n ; i++)
    m_Demand[ ChannelIndex(m_Selection[i]) ] = m_Restore[ ChannelIndex(m_Selection[i]) ];
	
	UpdateODB(n == 1 ? ChannelIndex(m_Selection[0]) : -1);
  UpdateListBox(n == 1 ? m_Selection[0] : -1);
}

/*----------------------------------------------------------------------------*/

void CHveditDlg::OnSave() 
{
  if (m_LastFileName == CString(""))
    m_LastFileName = CString("*.hv");

	CFileDialog dlg(FALSE, "hv", m_LastFileName, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, 
	  "High Voltage Files (*.hv)|*.hv|All Files (*.*)|*.*||");

  if (dlg.DoModal() == IDOK)
    {
	  m_LastFileName = dlg.GetPathName();

    m_pDemandKey->Save(m_LastFileName);
    }
}

/*----------------------------------------------------------------------------*/

void CHveditDlg::OnLoad() 
{
  /* find out if we have write access */
  m_pDemandKey->UpdateKey();  
  if (!(m_pDemandKey->GetAccessMode() & MODE_WRITE))
    {
    MessageBox("Change of demand values currently not allowed");
    return;
    }

  CFileDialog dlg(TRUE, "hv", "*.hv", OFN_HIDEREADONLY | OFN_FILEMUSTEXIST, 
	  "High Voltage Files (*.hv)|*.hv|All Files (*.*)|*.*||");

  if (dlg.DoModal() == IDOK)
    {
	  m_LastFileName = dlg.GetPathName();

    m_pDemandKey->Load(m_LastFileName);
    UpdateListBox(-1);
    }
}

/*----------------------------------------------------------------------------*/

void HVChanged(HNDLE hDB, HNDLE hKey, void *info)
/* This function gets linked to the open records (demand and measured)
   by the db_open_record calls. Whenever a record changes, cm_yield (below)
   first updates the local copy of the record and then calls odb_update */
{
CHveditDlg *dlg;
 
  dlg = (CHveditDlg *) AfxGetMainWnd();
  dlg->UpdateListBox(-1);
}

/*----------------------------------------------------------------------------*/

void CHveditDlg::OnDestroy() 
{
RECT rect;

  // Save window position
  GetWindowRect(&rect);

  AfxGetApp()->WriteProfileInt("Window Position", "Left", rect.left);
  AfxGetApp()->WriteProfileInt("Window Position", "Top", rect.top);
	
	CDialog::OnDestroy();
}

/*----------------------------------------------------------------------------*/

void CHveditDlg::OnConfig() 
{
CHvConfig dlg;

  if (dlg.DoModal() == IDOK)
    UpdateChannelDefinition();
}

/*----------------------------------------------------------------------------*/

void CHveditDlg::Print(CDC *pDC)
{
char       Name[100];
CString    str, GroupName;
CFont      font;
TEXTMETRIC TextMetric;
POINT      PhysPageSize;
int        LineSpace, CharSpace, ColumnWidth, nColumn, nLine, i, j;
int        nChannels, x, y;
unsigned   MaxName;
time_t     acttime;

  /**** find longest string ****/
  MaxName = 0;
  nChannels = 0;
  for (i=0 ; i<m_nChannels ; i++)
    {
    if (!m_bPrintAll && m_Group[i] != m_iGroup)
      continue;

    nChannels++;
    if ( strchr(m_Name+i*m_nNameLength, '%') )
      {
      strcpy(Name, strchr(m_Name+i*m_nNameLength, '%')+1);
      }
    else
      {
      strcpy(Name, m_Name+i*m_nNameLength);
      }

    if (strlen(Name) > MaxName)
      MaxName = strlen(Name);
    }


  /**** set font ****/
  CSize size(0, 400);

  pDC->HIMETRICtoDP(&size);
  font.CreateFont(size.cy, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, "Arial");
  pDC->SelectObject(&font);

  /**** calculate print size ****/
  pDC->GetTextMetrics(&TextMetric);
  pDC->Escape(GETPHYSPAGESIZE, NULL, (LPSTR)NULL, (LPSTR) &PhysPageSize);

  LineSpace = TextMetric.tmHeight + TextMetric.tmExternalLeading;
  CharSpace = (int) (1.2*TextMetric.tmAveCharWidth);
  ColumnWidth = CharSpace * (4 + max(6, (MaxName+1)) + 15);

  nColumn = (PhysPageSize.x / ColumnWidth);
  if (nColumn == 0)
    {
    MessageBox("Page width too small");
    return;
    }
  nLine = (nChannels-1) / nColumn + 1;

  pDC->SetTextAlign(TA_CENTER | TA_TOP);

  if (!m_bPrintAll)
    {
    m_ctlGroups.GetText(m_iGroup, GroupName);
    str = "High Voltages - Group: " + GroupName;
    }
  else
    str = "High Voltages - All groups";

  pDC->TextOut(PhysPageSize.x / 2, LineSpace, str);
  time(&acttime);
  str = CString(ctime(&acttime)).Left(24);
  pDC->TextOut(PhysPageSize.x / 2, 2*LineSpace, str);
  y = 4*LineSpace;

  pDC->SetTextAlign(TA_LEFT | TA_TOP);
  for (i=0 ; i<nColumn ; i++)
    {
    x = PhysPageSize.x/2 - nColumn*ColumnWidth/2 + i*ColumnWidth;

    pDC->TextOut(x, y, "Chn");
    x += 4*CharSpace;

    pDC->TextOut(x, y, "Name");
    x += max(6, (MaxName+1)) * CharSpace;

    pDC->TextOut(x, y, "Demnd");
    x += 7*CharSpace;

    pDC->TextOut(x, y, "Meas");
    x += 7*CharSpace;

    if (i < nColumn-1)
      {
      pDC->MoveTo(x, (int) (y+1.5*LineSpace));
      pDC->LineTo(x, (int) (y+(2+nLine)*LineSpace));
      }
    }

  pDC->MoveTo(PhysPageSize.x/2 - nColumn*ColumnWidth/2, (int) (y+1.5*LineSpace));
  pDC->LineTo(x, (int) (y+1.5*LineSpace));

  for (i=j=0 ; i<m_nChannels ; i++)
    {
    if (!m_bPrintAll && m_Group[i] != m_iGroup)
      continue;

    if ( strchr(m_Name+i*m_nNameLength, '%') )
      strcpy(Name, strchr(m_Name+i*m_nNameLength, '%')+1);
    else
      strcpy(Name, m_Name+i*m_nNameLength);

    x = PhysPageSize.x/2 - nColumn*ColumnWidth/2 +
        (j / nLine)*ColumnWidth;
    y = ((j % nLine)+6)*LineSpace;

    str.Format("%3d", i);
    pDC->TextOut(x, y, str);
    x += 4*CharSpace;

    pDC->TextOut(x, y, Name);
    x += max(6, (MaxName+1)) * CharSpace;

    str.Format("%6g", m_Demand[i]);
    pDC->TextOut(x, y, str);
    x += 7*CharSpace;

    str.Format("%6g", m_Measured[i]);
    pDC->TextOut(x, y, str);
    x += 7*CharSpace;

    j++;
    }
}

/*----------------------------------------------------------------------------*/

void CHveditDlg::OnHlp() 
{
		AfxGetApp()->WinHelp(0, HELP_CONTENTS);
}

/*----------------------------------------------------------------------------*/

void CHveditDlg::OnTimer(UINT nIDEvent) 
{
  /* display warning message if FE not running */
  if (m_pMeasuredKey->GetKeyTime() > FE_TIMEOUT)
    m_ctlMessage.SetWindowText("WARNING: FE not running");
  else
    m_ctlMessage.SetWindowText("");
	
	CDialog::OnTimer(nIDEvent);
}
