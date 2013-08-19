/********************************************************************************************

  Qt_hvEdit.cpp

  implementation of the virtual class 'Qt_hvEdit_Base' from the Qt designer
  (see 'Qt_hvEdit_Base.ui')

  Main GUI which handles the interaction between a user and the midas ODB.

*********************************************************************************************

    begin                : Andreas Suter, 2002/11/21
    modfied:             :
    copyright            : (C) 2003 by
    email                : andreas.suter@psi.ch

  $Log: Qt_hvEdit.cpp,v $
  Revision 1.1.1.1  2005/06/20 23:37:12  mucap
  Importing release 1.9.5 of the MIDAS source code as distributed by PSI.
  (Next, I'll commit our local customizations.)

  Revision 1.5  2003/12/30 14:54:26  suter_a
  "doxygenized" code, i.e. added comments which can be handled by doxygen in
  order to generate html- and latex-docu.

  Revision 1.4  2003/10/21 14:39:06  suter_a
  added scaling

  Revision 1.3  2003/05/13 17:29:44  midas
  Fixed compiler warnings

  Revision 1.2  2003/05/09 10:43:41  midas
  Fixed compiler warning

  Revision 1.1  2003/05/09 10:08:09  midas
  Initial revision

  Revision 1.3  2003/03/03 09:33:50  nemu
  add documentation path info to help facility.

  Revision 1.2  2003/02/27 15:22:57  nemu
  fixed restore buffer handling.
  fixed an error in connectToExp().
  initialize keys correctly.


********************************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <stdlib.h>
#include <string.h>

#include <qapplication.h>
#include <qmessagebox.h>
#include <qfiledialog.h>
#include <qlineedit.h>
#include <qlistbox.h>
#include <qtable.h>
#include <qtabwidget.h>
#include <qcombobox.h>
#include <qdialog.h>
#include <qaction.h>
#include <qpushbutton.h>
#include <qdatetime.h>
#include <qprinter.h>
#include <qpainter.h>
#include <qpaintdevicemetrics.h>

#include "midas.h"

#include "Qt_fileList.h"
#include "Qt_hvEditAbout.h"
#include "Qt_Connect.h"
#include "Qt_hvEdit.h"

#include "help/helpwindow.h"

#include "globals.h"

//*******************************************************************************************************
/*!
 * <p>Constructes the hvEdit widget and initializes it properly.
 *
 * \param hvA Pointer to the administration class object.
 * \param cmExp Pointer to the experiment class object.
 * \param parent Pointer to the parent widget.
 * \param name Pointer to the internal name.
 * \param fl The widget flags argument, f, is normally 0, but it can be set to 
 *           customize the window frame of a top-level widget (i.e. parent must be 0). 
 *           To customize the frame, set the WStyle_Customize flag OR'ed with any 
 *           of the Qt::WidgetFlags.  
 */
Qt_hvEdit::Qt_hvEdit(hvAdmin *hvA, cmExperiment *cmExp, QWidget *parent, const char *name,
          WFlags fl) : Qt_hvEdit_Base(parent, name, fl)
{
  Qt_hvEdit::hvA   = hvA;
  Qt_hvEdit::cmExp = cmExp;

  // initialize all working pointers to NULL
  m_Name = NULL;
  m_Demand = NULL;
  m_Measured = NULL;
  m_Restore = NULL;
  m_Current = NULL;
  m_CurrentLimit = NULL;
  m_DemandCache = NULL;
  m_MeasuredCache = NULL;
  m_CurrentCache = NULL;
  m_CurrentLimitCache = NULL;
  m_Selection = NULL;
  m_Group = NULL;

  // initialize keys properly
  rootKey = NULL;
  namesKey = NULL;
  demandKey = NULL;
  measuredKey = NULL;
  currentKey = NULL;
  currentLimitKey = NULL;

  // initialize table cells of the GUI
  hve_HV_table->setColumnReadOnly(HV_NAME, TRUE);
  hve_HV_table->setColumnReadOnly(HV_MEASURED, TRUE);
  hve_HV_table->setColumnReadOnly(HV_CURRENT, TRUE);
  hve_currentLimit_table->setColumnReadOnly(CL_NAME, TRUE);
  hve_currentLimit_table->setColumnReadOnly(CL_CURRENT, TRUE);

  // connect signals with slots
  connect(hve_HV_table, SIGNAL(valueChanged(int, int)), SLOT(hvValueChanged(int, int)));
  connect(hve_currentLimit_table, SIGNAL(valueChanged(int, int)), SLOT(currentLimitValueChanged(int, int)));

  // disable things in the GUI which only make sense if connected
  disableNotConnected();
}

//*******************************************************************************************************
/*!
 * <p>Destroys the object. If not disconnected from the experiment, first disconnects
 * from the experiment.
 */
Qt_hvEdit::~Qt_hvEdit()
{
  disconnectFromExp();
}

//*******************************************************************************************************
/*!
 * <p>Sets the host name in the GUI
 *
 * <p><b>Return:</b> void
 *
 * \param host String holding the host name. 
 */
void Qt_hvEdit::setHostName(QString host)
{
  hve_Host_lineEdit->setText(host);
}

//*******************************************************************************************************
/*!
 * <p>Sets the experiment name in the GUI
 *
 * <p><b>Return:</b> void
 *
 * \param experiment String holding the experiment name. 
 */
void Qt_hvEdit::setExpName(QString experiment)
{
  hve_Exp_lineEdit->setText(experiment);
}

//*******************************************************************************************************
/*!
 * <p>This function establishes all the hot-links between the ODB and the local 
 * structures. In case there are already hot-link established, they will be first
 * closed the memory free'd before they will be newly established.
 *
 * <p><b>Return:</b>
 *   - TRUE if everthing went smooth
 *   - FALSE otherwise
 */
BOOL Qt_hvEdit::updateChannelDefinitions()
{
  int  i;
  char str[80];

  QListBoxItem *lBI;

  // free used memory
  freeMem();

  // remove hot links
  namesKey->Unlink();
  demandKey->Unlink();
  measuredKey->Unlink();
  currentKey->Unlink();
  currentLimitKey->Unlink();

  // update keys
  namesKey->UpdateKey();
  demandKey->UpdateKey();
  measuredKey->UpdateKey();
  currentKey->UpdateKey();
  currentLimitKey->UpdateKey();

  // initialize variables
  m_nChannels   = namesKey->GetNumValues();
  m_nNameLength = namesKey->GetItemSize();
  m_nGroups = 0;
  m_iGroupCache = -1;
  m_Selection = (INT *) malloc(m_nChannels * sizeof(INT));
  m_Group = (INT *) calloc(m_nChannels, sizeof(INT));
  m_Restore = (float*) calloc(m_nChannels, sizeof(float));
  m_DemandCache = (float *) calloc(m_nChannels, sizeof(float));
  m_MeasuredCache = (float *) calloc(m_nChannels, sizeof(float));
  m_CurrentCache = (float *) calloc(m_nChannels, sizeof(float));
  m_CurrentLimitCache = (float *) calloc(m_nChannels, sizeof(float));

  // Link names entry to m_Name array
  namesKey->HotLink((void **) &m_Name, MODE_READ | MODE_ALLOC, namesChanged, this);

  // reset group list
  hve_Device_listBox->clear();

  // Analyze names, generate groups
  for (i=0 ; i<m_nChannels; i++) {
    if (strchr((const char *) m_Name+i*m_nNameLength, '%')) {
      strcpy(str, (const char *) m_Name+i*m_nNameLength);
      *strchr((const char *) str, '%') = 0;
      if ((lBI = hve_Device_listBox->findItem(str, Qt::ExactMatch))==0) { // device item new
        hve_Device_listBox->insertItem(str);
        m_Group[i] = m_nGroups;
        m_nGroups++;
      }
      else
        m_Group[i] = hve_Device_listBox->index(lBI); // get index of already existing item
    }
  }
  hve_Device_listBox->setSelected(0, TRUE); // select the first item in the list
  m_iGroup = 0;

  // Link demand entry to m_Demand array
  if (demandKey->GetNumValues() != m_nChannels || demandKey->GetType() != TID_FLOAT) {
    MidasMessage("Wrong 'demand' entry in HV values");
    return FALSE;
  }
  demandKey->HotLink((void **) &m_Demand, MODE_READ | MODE_ALLOC, HVChanged, this);

  // Link measured entry to m_Measured array
  if (measuredKey->GetNumValues() != m_nChannels || measuredKey->GetType() != TID_FLOAT) {
    MidasMessage("Wrong 'measured' entry in HV values");
    return FALSE;
  }
  measuredKey->HotLink((void **) &m_Measured, MODE_READ | MODE_ALLOC, HVChanged, this);

  // Link current entry to m_Current array
  if (currentKey->GetNumValues() != m_nChannels || currentKey->GetType() != TID_FLOAT) {
    MidasMessage("Wrong 'current' entry in HV values");
    return FALSE;
  }
  currentKey->HotLink((void **) &m_Current, MODE_READ | MODE_ALLOC, HVChanged, this);

  // Link current limit entry to m_CurrentLimit array
  if (currentLimitKey->GetNumValues() != m_nChannels || currentLimitKey->GetType() != TID_FLOAT) {
    MidasMessage("Wrong 'current limit' entry in HV values");
    return FALSE;
  }
  currentLimitKey->HotLink((void **) &m_CurrentLimit, MODE_READ | MODE_ALLOC, HVChanged, this);

  return TRUE;
}

//*******************************************************************************************************
/*!
 * <p>This function updates the HV and Current Limit tables.
 *
 * <p><b>Return:</b> void
 *
 * \param ch channel to be updated. If ch < 0 <em>ALL</em> channels are going to be updated at once.
 */
void Qt_hvEdit::updateTable(int ch)
{
  int  i, offset;
  char name[256], demand[256], measured[256], current[256], current_limit[256];
  bool need_update;

  if (ch>=0) { // channel number given
    // get correct index
    i = channelIndex(ch);
    // get name from ODB
    db_sprintf(name, m_Name, namesKey->GetItemSize(), i, TID_STRING);
    // filter out channel name
    if (strchr(name, '%'))
      strcpy(name, strchr(name, '%')+1);
    // overwrite name cell
    hve_HV_table->setText(ch, HV_NAME, name);
    hve_currentLimit_table->setText(ch, CL_NAME, name);

    // get demand HV from ODB
    db_sprintf(demand, m_Demand, sizeof(float), i, TID_FLOAT);
    // overwrite demand cell
    hve_HV_table->setText(ch, HV_DEMAND, demand);

    // get measured HV from ODB
    db_sprintf(measured, m_Measured, sizeof(float), i, TID_FLOAT);
    // overwrite measured cell
    hve_HV_table->setText(ch, HV_MEASURED, measured);

    // get measured current from ODB
    db_sprintf(current, m_Current, sizeof(float), i, TID_FLOAT);
    // overwrite current cell
    hve_HV_table->setText(ch, HV_CURRENT, current);
    hve_currentLimit_table->setText(ch, CL_CURRENT, current);

    // get measured current from ODB
    db_sprintf(current_limit, m_CurrentLimit, sizeof(float), i, TID_FLOAT);
    // overwrite current cell
    hve_currentLimit_table->setText(ch, CL_CURRENT_LIMIT, current_limit);

    // Update cache
    m_DemandCache[i] = m_Demand[i];
    m_MeasuredCache[i] = m_Measured[i];

  } else { // check all channels
    // check if update is needed
    need_update = FALSE;

    if (m_iGroup == m_iGroupCache) {
      for (i=0 ; i<m_nChannels ; i++) {
        if (m_Group[i] == m_iGroup &&
	   (m_Demand[i] != m_DemandCache[i] || m_Measured[i] != m_MeasuredCache[i]) ||
	    m_Current[i] != m_CurrentCache[i] || m_CurrentLimit[i] != m_CurrentLimitCache[i]) {
          need_update = TRUE;
          break;
        }
      }
      if (!need_update)
        return;
    } else {
      need_update = TRUE;
    }

    m_iGroupCache = m_iGroup;

    clearTable(); // remove all items from the table

    offset = 0; // to get offset between devices
    for (i=0 ; i<m_nChannels ; i++) {
      if (m_Group[i] != m_iGroup) {
        offset++;
        continue;
      }
      // Update cache
      m_DemandCache[i] = m_Demand[i];
      m_MeasuredCache[i] = m_Measured[i];

      // get ch name
      db_sprintf(name, m_Name, namesKey->GetItemSize(), i, TID_STRING);
      // filter out ch name
      if (strchr(name, '%'))
        strcpy(name, strchr(name, '%')+1);
      // check if enough rows are available
      if (i-offset>=hve_HV_table->numRows()) {
        // add necessary rows
        hve_HV_table->insertRows(hve_HV_table->numRows());
	hve_currentLimit_table->insertRows(hve_currentLimit_table->numRows());
      }
      // overwrite table cells
      hve_HV_table->setItem(i-offset, HV_NAME, new QTableItem(hve_HV_table, QTableItem::Never, name));
      hve_currentLimit_table->setItem(i-offset, HV_NAME, new QTableItem(hve_HV_table, QTableItem::Never, name));

      // get demand HV from ODB
      db_sprintf(demand, m_Demand, sizeof(float), i, TID_FLOAT);
      // overwrite demand cell
      hve_HV_table->setText(i-offset, HV_DEMAND, demand);

      // get measured HV from ODB
      db_sprintf(measured, m_Measured, sizeof(float), i, TID_FLOAT);
      // overwrite measured cell
      hve_HV_table->setText(i-offset, HV_MEASURED, measured);

      // get measured current from ODB
      db_sprintf(current, m_Current, sizeof(float), i, TID_FLOAT);
      // overwrite current cell
      hve_HV_table->setText(i-offset, HV_CURRENT, current);
      hve_currentLimit_table->setText(i-offset, CL_CURRENT, current);

      // get measured current from ODB
      db_sprintf(current_limit, m_CurrentLimit, sizeof(float), i, TID_FLOAT);
      // overwrite current cell
      hve_currentLimit_table->setText(i-offset, CL_CURRENT_LIMIT, current_limit);
    }
  }

  // display warning message if FE not running
  if (measuredKey->GetKeyTime() > FE_TIMEOUT)
    statusBar()->message("WARNING: FE not running", 2000);
  else
    statusBar()->message("FE is running", 2000);
}

//*******************************************************************************************************
/*!
 * <p>Opens a file open dialog and reads an existing high voltage file (extension *.hv).
 * In order to identify for which device and which channels are meant, the following syntax
 * for these files was introduced:
 *
 * <p>SYNTAX of the high voltage files:<br>
 * The high voltage files are ascii files with the extension '.hv'<br>
 * The single entries are organized as following
 *    -# Comment lines start with '!'
 *    -# A line indicating the device group is needed. It has the syntax:<br> 
 *       "D >device_group_name<" <br>
 *       where the " indicate the begin and end of line, respectively.
 *    -# A single device channel has the syntax:<br>
 *       S (CH_No) \$(Voltage)kV %(Current)mA &(Name)<br><br>
 *       - <b>S</b>              keyword at the beginning of the line
 *       - <b>(CH_No)</b>        channel number (1-32 for the Nitronic HVS 132)
 *       - <b>\$(Voltage)</b>    voltage in kV, e.g. $1.34kV
 *       - <b>%(Current)</b>     current limit in mA, e.g. %0.12mA
 *       - <b>&(Name)</b>        name of the channel (no spaces allowed)
 *
 * <p>A typical line would be: S 13 $1.34kV %0.07mA &Anode_MCP2
 * 
 * <p><b>Return:</b> void
 */
void Qt_hvEdit::fileOpen()
{
  FILE *fp;
  char str[81], sub[81], *lp;
  int i, j, max, result, offset;
  QString dev_name, qname, ch[100], name[100], demand[100], current_limit[100];

  Qt_fileList *hv_List = new Qt_fileList();

  QString fln = QFileDialog::getOpenFileName(*hvA->getHVDefaultDir(),
                    "high voltage settings (*.hv)",
                    this,
                    "open file dialog"
                    "Choose a file" );

  // if no file name chosen, i.e. Cancel was selected
  if (fln.isEmpty()) {
    return;
  }

  // open file
  if ((fp = fopen((char *)fln.ascii(), "r"))==NULL) { // file open error
    QMessageBox::information(0, "INFO", "Couldn't open "+fln, QMessageBox::Ok);
    return ;
  }
  i=0; // no of channels counter
  // read file
  while (!feof(fp)) {
    fgets(str, 80, fp);  // read line
    if (str[0] == 'D') { // device name
      if (!findSub(str, ">", "<", &dev_name)) { // get channel number
        QMessageBox::warning(0, "ERROR", "File read error", QMessageBox::Ok, QMessageBox::NoButton);
        return ;
      }
    }
    if (str[0] == 'S') { // device entry
      if (!findSub(str, "S", "$", &ch[i])) { // get channel number
        QMessageBox::warning(0, "ERROR", "File read error", QMessageBox::Ok, QMessageBox::NoButton);
        return ;
      }
      if (!findSub(str, "$", "kV", &demand[i])) { // get hv demand value
        QMessageBox::warning(0, "ERROR", "File read error", QMessageBox::Ok, QMessageBox::NoButton);
        return ;
      }
      if (!findSub(str, "%", "mA", &current_limit[i])) { // get current limit value
        QMessageBox::warning(0, "ERROR", "File read error", QMessageBox::Ok, QMessageBox::NoButton);
        return ;
      }
      if ((lp=strstr(str, "&"))==NULL) { // get channel name
        QMessageBox::warning(0, "ERROR", "File read error", QMessageBox::Ok, QMessageBox::NoButton);
        return ;
      }
      strcpy(sub,lp+strlen("&"));
      sub[strlen(sub)-1] = '\0';
      name[i] = sub;

      i++;
    }
  }
  // close file
  fclose(fp);
  max = i;

  // check if table is large enough
  if (hv_List->hv_table->numRows()<i) { // add rows
    hv_List->hv_table->insertRows(hv_List->hv_table->numRows(), i-hv_List->hv_table->numRows());
  }
  // generate a file list
  for (j=0; j<max; j++) { // row
    hv_List->hv_table->setText(j,0,name[j]);
    hv_List->hv_table->setText(j,1,ch[j]);
    hv_List->hv_table->setText(j,2,demand[j]);
    hv_List->hv_table->setText(j,3,current_limit[j]);
  }

  if (!hv_List->exec()) { // do not load
    QMessageBox::information(0, "INFO", "will NOT load data.", QMessageBox::Ok);
    return;
  }

  // check if device is OK and get ODB device offset
  offset = 0;
  for (i=0; i<m_nChannels; i++) { // parse for device name
    qname = m_Name+i*m_nNameLength;
    if (qname.find(dev_name,0,FALSE)!=-1) { // found
      break;
    }
    offset++;
  }
  if (i==m_nChannels) { // device name couldn't be found
    QMessageBox::warning(0, "ERROR", "device name "+qname+" couldn't be found!", QMessageBox::Ok, QMessageBox::NoButton);
    return;
  }

  // check if names are OK
  for (i=0; i<max; i++) { // compare ch names
    j = offset+ch[i].toInt()-1; // get the right address
    qname = m_Name+j*m_nNameLength;
    if (qname.find(name[i],0,FALSE)==-1) { // ch name load != ch name ODB
      result = QMessageBox::warning(0, "WARNING", "ch name "+name[i]+
                           " different from ODB ch name.\nThough, it is NOT going to be updated!!\nThis Message only appears once.",
			   QMessageBox::Ok, QMessageBox::NoButton);
      break;
    }
  }

  // set values
  for (i=0; i<max; i++) { // update demand and current limit values
    j = offset+ch[i].toInt()-1; // get right address
    m_Restore[j] = m_Demand[j];
    m_Demand[j] = demand[i].toFloat();
    m_CurrentLimit[j] = current_limit[i].toFloat();
  }

  // update ODB and GUI
  updateODB_demand(-1);
  updateODB_currentLimit(-1);
  updateTable(-1);
}

//*******************************************************************************************************
/*!
 * <p>Opens a file save dialog and saves the file in the format as described in
 * Qt_hvEdit::fileOpen()
 *
 * <p><b>Return:</b> void
 */
void Qt_hvEdit::fileSave()
{
  int     i, offset;
  QString str, device, fln;
  char    *sub, line[128];
  FILE    *fp;

  // get active device group
  device = hve_Device_listBox->currentText();

  // open save as dialog
  fln = QFileDialog::getSaveFileName(*(hvA->getHVDefaultDir()), "HV Settings (*.hv)", this,
                                     "Save HV Settings...");
  if (fln.isEmpty()) { // empty file name
    return ;
  }
  // check if file exists
  QFileInfo flnInfo(fln);
  if (flnInfo.size() > 0) { // file exists
    i = QMessageBox::warning(0, "WARNING", "Do you want to overwrite this file?", QMessageBox::Yes, QMessageBox::No);
    if (i == QMessageBox::No)
      return;
  }

  // open file
  if ((fp=fopen(fln, "w"))==NULL) { // couldn't open file
    QMessageBox::warning(0, "ERROR", "Couldn't open file for saving!!", QMessageBox::Ok, QMessageBox::NoButton);
    return;
  }

  // write header
  str = "! "+device+"\n";
  fputs((char *)str.ascii(), fp);
  fputs("! \n", fp);
  fputs("! \n", fp);
  fputs("! '!' is a comment line.\n", fp);
  fputs("! \n", fp);
  fputs("! modified: \n", fp);
  fputs("! \n", fp);
  fputs("!--------------------------------------------------------------------\n", fp);
  fputs("! \n", fp);
  fputs("!         This file is read by program HVEdit to set default things\n", fp);
  fputs("!         to the LEM HV-remote controller.\n", fp);
  fputs("! \n", fp);
  fputs("!         Contains channel encoding.\n", fp);
  fputs("! \n", fp);
  fputs("!         ------------->   D O  N O T change the format !!!!!!!!! <----------\n", fp);
  fputs("! \n", fp);
  fputs("!--------------------------------------------------------------------\n", fp);
  fputs("! \n", fp);
  fputs("!         Define output channels:\n", fp);
  fputs("!         =======================\n", fp);
  fputs("! \n", fp);
  fputs("!         SYNTAX: S (CH_No) $(Voltage)kV %(Current)mA &(Name)\n", fp);
  fputs("! \n", fp);
  fputs("!           S            keyword at the beginning of the line\n", fp);
  fputs("!           (CH_No)      channel number (1-32 for the Nitronic HVS 132)\n", fp);
  fputs("!           $(Voltage)   voltage in kV, e.g. $1.34kV\n", fp);
  fputs("!           %(Current)   current limit in mA, e.g. %0.12mA\n", fp);
  fputs("!           &(Name)      name of the channel (no spaces allowed)\n", fp);
  fputs("! \n", fp);
  fputs("!           typical line would be: S 13 $1.34kV %0.07mA &Anode_MCP2\n", fp);
  fputs("! \n", fp);
  fputs("! \n", fp);
  fputs("!--------------------------------------------------------------------\n", fp);
  fputs("! \n", fp);

  // write data part

  // write device tag
  str = "D >"+device+"<\n";
  fputs((char *)str.ascii(), fp);

  // get device list offset
  offset = 0;
  for (i=0; i<m_nChannels; i++) {
    str = m_Name+i*m_nNameLength;
    if (str.find(device, 0, FALSE) != -1) { // found
      break;
    }
    offset++;
  }

  // write data
  for (i=0; i<m_nChannels; i++) {
    str = m_Name+i*m_nNameLength;
    if ((str.find(device, 0, FALSE) != -1) && (m_Demand[i] != 0)) { // correct device and demand != 0
      sub = strstr(m_Name+i*m_nNameLength,"%");
      sub++;
      sprintf(line, "S %d $%0.2fkV %%%0.3fmA &%s\n", i-offset+1, m_Demand[i], m_CurrentLimit[i], sub);
      fputs(line, fp);
    }
  }
  fputs("! end", fp);

  // close file
  fclose(fp);
}

//*******************************************************************************************************
/*!
 * <p>Opens a file print dialog and when a printer was chosen, the data
 * are going to be sent in the correct format.
 *
 * <p><b>Return:</b> void
 */
void Qt_hvEdit::filePrint()
{
  int     i, offset;
  QString device, str;
  char    *name, line[128];
  QDate   date = QDate::currentDate();
  QTime   time = QTime::currentTime();

  QPrinter *printer = new QPrinter;
  const int Margin = 45;

  // get current device group
  device = hve_Device_listBox->currentText();

  // get device list offset
  offset = 0;
  for (i=0; i<m_nChannels; i++) {
    str = m_Name+i*m_nNameLength;
    if (str.find(device, 0, FALSE) != -1) { // found
      break;
    }
    offset++;
  }

  if (printer->setup(this)) { // printer dialog
     statusBar()->message( "Printing..." );

     QPainter p;
     if(!p.begin(printer))                  // paint on printer
       return;

     QFont f("Courier");
     p.setFont(f);

     int yPos        = 0;                    // y-position for each line
     QFontMetrics fm = p.fontMetrics();
     QPaintDeviceMetrics metrics( printer ); // need width/height

     // write header
     p.drawText(Margin, Margin+yPos, metrics.width(), fm.lineSpacing(),
                ExpandTabs | DontClip, "-------------------------------------------------------------------------");
     yPos += fm.lineSpacing();
     p.drawText(Margin, Margin+yPos, metrics.width(), fm.lineSpacing(),
                ExpandTabs | DontClip, ">> "+time.toString()+", "+date.toString("dddd, MMMM d, yyyy"));
     yPos += fm.lineSpacing();
     p.drawText(Margin, Margin+yPos, metrics.width(), fm.lineSpacing(),
                ExpandTabs | DontClip, "-------------------------------------------------------------------------");
     yPos += fm.lineSpacing();
     p.drawText(Margin, Margin+yPos, metrics.width(), fm.lineSpacing(),
                ExpandTabs | DontClip, "Device Name = "+device);
     yPos += fm.lineSpacing();
     p.drawText(Margin, Margin+yPos, metrics.width(), fm.lineSpacing(),
                ExpandTabs | DontClip, "-------------------------------------------------------------------------");
     yPos += fm.lineSpacing();
     p.drawText(Margin, Margin+yPos, metrics.width(), fm.lineSpacing(),
                ExpandTabs | DontClip, "Ch No,  Demand,  Measured,  Current,  CurrentLimit,  Name");
     yPos += fm.lineSpacing();
     p.drawText(Margin, Margin+yPos, metrics.width(), fm.lineSpacing(),
                ExpandTabs | DontClip, "         (kV)      (kV)       (mA)        (mA)");
     yPos += fm.lineSpacing();

     // write data
     for (i=0; i<m_nChannels; i++) {
       if ( Margin + yPos > metrics.height() - Margin ) { // new page needed?
         printer->newPage();             // no more room on this page
         yPos = 0;                       // back to top of page
       }
       str = m_Name+i*m_nNameLength;
       if (str.find(device, 0, FALSE) != -1) { // found
         name = strstr(m_Name+i*m_nNameLength, "%");
         name++;
         sprintf(line, "%02d,     %0.2f,     %0.2f,      %0.3f,     %0.3f,      %s", i-offset+1,
                 m_Demand[i], m_Measured[i], m_Current[i], m_CurrentLimit[i], name);
         str = line;
         p.drawText(Margin, Margin+yPos, metrics.width(), fm.lineSpacing(),
                    ExpandTabs | DontClip, str);
         yPos += fm.lineSpacing();
       }
     }

     p.end();                                // send job to printer
     statusBar()->message( "Printing completed", 2000 );
  } else {
    statusBar()->message( "Printing aborted", 2000 );
  }
}

//*******************************************************************************************************
/*!
 * <p>Terminates the hvEdit application. In case that there is still an active
 * connection between MIDAS and hvEdit, this connection will be shut down properly
 * first.
 *
 * <p><b>Return:</b> void 
 */
void Qt_hvEdit::fileExit()
{
  int ans;

  if (cmExp->IsConnected()) {
    ans = QMessageBox::warning(0, "WARNING", "You are about to exit hvEdit.\nDo you really want to do that?",
                               QMessageBox::Yes, QMessageBox::No);
    if (ans == QMessageBox::No) {
      return ;
    }
    disconnectFromExp();
  }
  exit(CM_SUCCESS);
}

//*******************************************************************************************************
/*!
 * <p><b>Not implemented yet.</b>
 *
 * <p><b>Return:</b> void 
 */
void Qt_hvEdit::helpIndex()
{
  QMessageBox::information(0, "INFO", "help index not implemented yet.", QMessageBox::Ok);
}

//*******************************************************************************************************
/*!
 * <p>Starts an online help assistant.
 *
 * <p><b>Return:</b> void 
 */
void Qt_hvEdit::helpContents()
{

  QString home = *hvA->getHVDefaultDoc()+"/hvEdit_Help.html";

  HelpWindow *help = new HelpWindow(home, *hvA->getHVDefaultDoc(), 0, "help viewer");
  help->setCaption("hvEdit - Helpviewer");
  help->show();
}

//*******************************************************************************************************
/*!
 * <p>Pops up an about widget.
 *
 * <p><b>Return:</b> void 
 */
void Qt_hvEdit::helpAbout()
{
  Qt_hvEditAbout *hvEditAbout = new Qt_hvEditAbout();
  hvEditAbout->show();
}

//*******************************************************************************************************
/*!
 * <p>Pops up an Qt about widget.
 *
 * <p><b>Return:</b> void 
 */
void Qt_hvEdit::helpAboutQt()
{
  QMessageBox::aboutQt(this, "a Qt application.");
}

//*******************************************************************************************************
/*!
 * <p>Tries to connect to a MIDAS experiment. It creates first an instance from Qt_Connect, which
 * allows to select a MIDAs experiment. Having a valid experiment name and host, it tries
 * to connect to this experiment/host. If successful it updates the GUI, otherwise it
 * displays an error message, cleans up and quit.
 *
 * <p><b>Return:</b> void 
 */
void Qt_hvEdit::connectToExp()
{
  // Dialog for getting the HostName and the ExperimentName
  Qt_Connect *qtc = new Qt_Connect(cmExp, this, "Connect to experiment", TRUE);
  qtc->exec();

  // having the correct experiment, connect to it.
  cmExp->Connect(cmExp->GetHostName(), cmExp->GetExperimentName(), cmExp->GetClientName());

  // set names Host and Experiment in hvEdit GUI
  setHostName(cmExp->GetHostName());
  setExpName(cmExp->GetExperimentName());

  // open keys
  if (!openKeys()) {
    QMessageBox::warning(0, "HVEdit ERROR", "Cannot get requiered Keys!",
                         QMessageBox::Ok, QMessageBox::NoButton);
    // disconnect from experiment
    disconnectFromExp();
    // clear host and experiment name
    cmExp->SetHostName("");
    cmExp->SetExperimentName("");
    setHostName(cmExp->GetHostName());
    setExpName(cmExp->GetExperimentName());
    return;
  }

  if (!updateChannelDefinitions()) return;

  updateTable(-1);
  enableConnected();
}

//*******************************************************************************************************
/*!
 * <p>Disconnects from a MIDAS experiment and cleans up the GUI.
 *
 * <p><b>Return:</b> void 
 */
void Qt_hvEdit::disconnectFromExp()
{
  closeKeys();
  // reset group list of the GUI
  hve_Device_listBox->clear();
  cmExp->Disconnect();
  // remove names Host and Experiment from hvEdit GUI
  setHostName("");
  setExpName("");
  clearTable();
  // remove any selection from table (GUI)
  hve_HV_table->clearSelection();

  disableNotConnected();
}

//*******************************************************************************************************
/*!
 * <p>If the device selection is changed, the table of the tab section is going to
 * switch to the corresponding device.
 *
 * <p><b>Return:</b> void 
 */
void Qt_hvEdit::changedDeviceSelection()
{
  m_iGroup = hve_Device_listBox->currentItem();
  if (m_Demand) { // i.e. hotlink already established
    updateTable(-1);
  }
}

//*******************************************************************************************************
/*!
 * <p>If the 'select all' button is pressed, this function actually selects the 
 * high voltage demand column.
 *
 * <p><b>Return:</b> void 
 */
void Qt_hvEdit::onSelectAll()
{
  //hve_HV_table->selectColumn(HV_DEMAND); // Qt3.1

  // Qt3.0.3 equivalent to the above Qt3.1 command
  QTableSelection s;
  s.init(0,HV_DEMAND); // anchor selection
  s.expandTo(hve_HV_table->numRows(),HV_DEMAND); // expand to last element in the HV_DEMAND column
  hve_HV_table->addSelection(s);
}

//*******************************************************************************************************
/*!
 * <p>If the 'switch all channels off' button is pressed, this function sets all
 * the HV demand values to zero.
 *
 * <p><b>Return:</b> void 
 */
void Qt_hvEdit::onSwitchAllChannelsOff()
{
  int result, i;

  result = QMessageBox::warning(0, "WARNING", "Are you SURE to set all channels in all groups to zero?",
                                QMessageBox::Yes, QMessageBox::No);

  if (result == QMessageBox::Yes) {
    for (i=0 ; i<m_nChannels ; i++) {
      if (m_Demand[i] != 0)
        m_Restore[i] = m_Demand[i];
      m_Demand[i] = 0.f;
    }

    updateODB_demand(-1);
    updateTable(-1);
  }
}

//*******************************************************************************************************
/*!
 * <p>If the 'zero' button is pressed, this function sets the selected channel(s)
 * of the HV demand value to zero.
 *
 * <p><b>Return:</b> void 
 */
void Qt_hvEdit::onZero()
{
  int i, j;
  int device;

  // get device selection
  device = hve_Device_listBox->currentItem();
  j = 0; // device channel counter
  for (i=0; i<m_nChannels; i++) { // i = all over channel counter
    if (m_Group[i] == device) { // check if correct device
      if (hve_HV_tabWidget->currentPageIndex() == 0) { // i.e. HV demand tab
        if (hve_HV_table->isSelected(j,HV_DEMAND)) { // get current selection
          // save voltages in restore buffer
          m_Restore[i] = m_Demand[i];
          // set voltage to zero
          m_Demand[i] = 0.0f;
        }
      } else { // current limit tab
        QMessageBox::warning(0, "WARNING", "This is NOT allowed for current limit", QMessageBox::Ok, QMessageBox::NoButton);
        return ;
      }
      j++;
    }
  }
  updateODB_demand(-1);
  updateTable(-1);
}

//*******************************************************************************************************
/*!
 * <p>If the 'restore' button is pressed, this function restores to the last HV setting before.
 *
 * <p><b>Return:</b> void 
 */
void Qt_hvEdit::onRestore()
{
  int i, j;
  int device;

  // get device selection
  device = hve_Device_listBox->currentItem();
  j = 0; // device channel counter
  for (i=0; i<m_nChannels; i++) { // i = all over channel counter
    if (m_Group[i] == device) { // check if correct device
      if (hve_HV_tabWidget->currentPageIndex() == 0) { // i.e. HV demand tab
        if (hve_HV_table->isSelected(j,HV_DEMAND)) { // get current selection
          // retrieve voltages from restore buffer
          m_Demand[i] = m_Restore[i];
        }
      } else { // current limit tab
        return ;
      }
      j++;
    }
  }
  updateODB_demand(-1);
  updateTable(-1);
}

//*******************************************************************************************************
/*!
 * <p>If the '-1.0' button is pressed, this function reduces all the selected HV or Current Limit
 * values by an amount of 1.0. In the case of the HV this means reduce the demand high voltage by
 * 1.0 kV, whereas in the case of the Current Limit it means reduce the current limit value by 
 * 1.0 mA.
 *
 * <p><b>Return:</b> void 
 */
void Qt_hvEdit::onM100()
{
  increment(-1.0f);
}

//*******************************************************************************************************
/*!
 * <p>If the '-0.1' button is pressed, this function reduces all the selected HV or Current Limit
 * values by an amount of 0.1. In the case of the HV this means reduce the demand high voltage by
 * 0.1 kV, whereas in the case of the Current Limit it means reduce the current limit value by 
 * 0.1 mA.
 *
 * <p><b>Return:</b> void 
 */
void Qt_hvEdit::onM010()
{
  increment(-0.1f);
}

//*******************************************************************************************************
/*!
 * <p>If the '-0.01' button is pressed, this function reduces all the selected HV or Current Limit
 * values by an amount of 0.01. In the case of the HV this means reduce the demand high voltage by
 * 0.01 kV, whereas in the case of the Current Limit it means reduce the current limit value by 
 * 0.01 mA.
 *
 * <p><b>Return:</b> void 
 */
void Qt_hvEdit::onM001()
{
  increment(-0.01f);
}

//*******************************************************************************************************
/*!
 * <p>If the '+1.0' button is pressed, this function increases all the selected HV or Current Limit
 * values by an amount of 1.0. In the case of the HV this means increase the demand high voltage by
 * 1.0 kV, whereas in the case of the Current Limit it means increase the current limit value by 
 * 1.0 mA.
 *
 * <p><b>Return:</b> void 
 */
void Qt_hvEdit::onP100()
{
  increment(1.0f);
}

//*******************************************************************************************************
/*!
 * <p>If the '+0.1' button is pressed, this function increases all the selected HV or Current Limit
 * values by an amount of 0.1. In the case of the HV this means increase the demand high voltage by
 * 0.1 kV, whereas in the case of the Current Limit it means increase the current limit value by 
 * 0.1 mA.
 *
 * <p><b>Return:</b> void 
 */
void Qt_hvEdit::onP010()
{
  increment(0.1f);
}

//*******************************************************************************************************
/*!
 * <p>If the '+0.01' button is pressed, this function increases all the selected HV or Current Limit
 * values by an amount of 0.01. In the case of the HV this means increase the demand high voltage by
 * 0.01 kV, whereas in the case of the Current Limit it means increase the current limit value by 
 * 0.01 mA.
 *
 * <p><b>Return:</b> void 
 */
void Qt_hvEdit::onP001()
{
  increment(0.01f);
}

//*******************************************************************************************************
/*!
 * <p>On pressing the 'set' button, this function sets the value from the 'Input:' field
 * to all of the selection (HV Demand or Current Limit). 
 *
 * <p><b>Return:</b> void 
 */
void Qt_hvEdit::onSet()
{
  int i, j;
  int device;
  QString str;

  str = hve_Input_comboBox->currentText();
  // get device selection
  device = hve_Device_listBox->currentItem();
  j = 0; // device channel counter
  for (i=0; i<m_nChannels; i++) { // i = all over channel counter
    if (m_Group[i] == device) { // check if correct device
      if (hve_HV_tabWidget->currentPageIndex() == 0) { // i.e. HV demand tab
        if (hve_HV_table->isSelected(j,HV_DEMAND)) { // get current selection
          // set voltage
          m_Demand[i] = str.toFloat();
        }
      } else { // current limit tab
        if (hve_currentLimit_table->isSelected(j,CL_CURRENT_LIMIT)) { // get current selection
          // set current limit
          m_CurrentLimit[i] = str.toFloat();
        }
      }
      j++;
    }
  }
  if (hve_HV_tabWidget->currentPageIndex() == 0) { // i.e. HV demand tab
    updateODB_demand(-1);
  } else { // current limit tab
    updateODB_currentLimit(-1);
  }

  updateTable(-1);
}

//*******************************************************************************************************
/*!
 * <p>On pressing the 'scale by factor' button, this function multiplies the values of
 * all selections (HV Demand or Current Limit) by this value. 
 *
 * <p><b>Return:</b> void 
 */
void Qt_hvEdit::onScale()
{
  float scale = hve_Scale_comboBox->currentText().toFloat(); // read comboBox value
  int   device, i, j;


  // check if selection is HV and not 'current limit'
  if (hve_HV_tabWidget->currentPageIndex() != 0) { // i.e. not HV demand tab
    QMessageBox::critical(0, "ERROR", "You cannot only scale 'demand HV'.", QMessageBox::Ok, QMessageBox::NoButton);
    return;
  }

  // check if scale is in reasonable bounds
  if (scale < 0.f) {
    QMessageBox::critical(0, "ERROR", "You cannot scale by negative values!", QMessageBox::Ok, QMessageBox::NoButton);
    return;
  }
  if (scale > 2.f) {
    if ( QMessageBox::warning(0, "WARNING", "<b><p>You are going to scale by "
            +hve_Scale_comboBox->currentText()+".<p>Are you serious about it?<\b>",
           QMessageBox::Yes, QMessageBox::No) == QMessageBox::No )
      return;
  }

  // get device selection
  device = hve_Device_listBox->currentItem();
  j = 0; // device channel counter
  for (i=0; i<m_nChannels; i++) { // i = all over channel counter
    if (m_Group[i] == device) { // check if correct device
      if (hve_HV_tabWidget->currentPageIndex() == 0) { // i.e. HV demand tab
        if (hve_HV_table->isSelected(j,HV_DEMAND)) { // get HV selection
	  // save current value into restore buffer
	  m_Restore[i] = m_Demand[i];
          // set voltage
          m_Demand[i] *= scale;
        }
      }
      j++;
    }
  }
  if (hve_HV_tabWidget->currentPageIndex() == 0) { // i.e. HV demand tab
    updateODB_demand(-1);
    updateTable(-1);
  }
}

//*******************************************************************************************************
/*!
 * <p>If an element of the HV table is changed within the table, this function is handling it.
 * It updates all the necessary fields and sends the HV demand value to the ODB. 
 *
 * <p><b>Return:</b> void 
 * 
 * \param row Row of the table
 * \param col Column of the table
 */
void Qt_hvEdit::hvValueChanged(int row, int col)
{
  int i, j;
  int device;
  QString str;

  if (col != HV_DEMAND) {
    QMessageBox::warning(0, "WARNING", "Wrong column, somthing is VERY fishy", QMessageBox::Ok, QMessageBox::NoButton);
    return ;
  }
  // get device selection
  device = hve_Device_listBox->currentItem();
  j = 0; // device channel counter
  for (i=0; i<m_nChannels; i++) { // i = all over channel counter
    if (m_Group[i] == device) { // check if correct device
      if (j == row) { // got changed item
        // get value from table
        str = hve_HV_table->text(row, col);
        m_Demand[i] = str.toFloat();
      }
      j++;
    }
  }
  updateODB_demand(-1);
  updateTable(-1);
}

//*******************************************************************************************************
/*!
 * <p>If an element of the Current Limit table is changed within the table, this function is handling it.
 * It updates all the necessary fields and sends the current limit value to the ODB. 
 *
 * <p><b>Return:</b> void 
 * 
 * \param row Row of the table
 * \param col Column of the table
 */
void Qt_hvEdit::currentLimitValueChanged(int row, int col)
{
  int i, j;
  int device;
  QString str;

  if (col != CL_CURRENT_LIMIT) {
    QMessageBox::warning(0, "WARNING", "Wrong column, somthing is VERY fishy", QMessageBox::Ok, QMessageBox::NoButton);
    return ;
  }
  // get device selection
  device = hve_Device_listBox->currentItem();
  j = 0; // device channel counter
  for (i=0; i<m_nChannels; i++) { // i = all over channel counter
    if (m_Group[i] == device) { // check if correct device
      if (j == row) { // got changed item
        // get value from table
        str = hve_currentLimit_table->text(row, col);
        m_CurrentLimit[i] = str.toFloat();
      }
      j++;
    }
  }
  updateODB_currentLimit(-1);
  updateTable(-1);
}

//*******************************************************************************************************
/*!
 * <p>Disables all the fields, buttons and menu entries which make no sence if not connected
 * to an experiment.
 *
 * <p><b>Return:</b> void 
 */
void Qt_hvEdit::disableNotConnected()
{
  connectToExpAction->setEnabled(TRUE);

  fileOpenAction->setEnabled(FALSE);
  fileSaveAction->setEnabled(FALSE);
  filePrintAction->setEnabled(FALSE);
  disconnectFromExpAction->setEnabled(FALSE);

  hve_SwitchAllOff_pushButton->setEnabled(FALSE);
  hve_Set_pushButton->setEnabled(FALSE);
  hve_M100_pushButton->setEnabled(FALSE);
  hve_M010_pushButton->setEnabled(FALSE);
  hve_M001_pushButton->setEnabled(FALSE);
  hve_P100_pushButton->setEnabled(FALSE);
  hve_P010_pushButton->setEnabled(FALSE);
  hve_P001_pushButton->setEnabled(FALSE);
  hve_Zero_pushButton->setEnabled(FALSE);
  hve_Restore_pushButton->setEnabled(FALSE);
  hve_SelectAll_pushButton->setEnabled(FALSE);
  hve_Scale_pushButton->setEnabled(FALSE);
  hve_Scale_comboBox->setEnabled(FALSE);
}

//*******************************************************************************************************
/*!
 * <p>Enables all the fields, buttons and menu entries which make any sence if connected
 * to an experiment.
 *
 * <p><b>Return:</b> void 
 */
void Qt_hvEdit::enableConnected()
{
  connectToExpAction->setEnabled(FALSE);

  fileOpenAction->setEnabled(TRUE);
  fileSaveAction->setEnabled(TRUE);
  filePrintAction->setEnabled(TRUE);
  disconnectFromExpAction->setEnabled(TRUE);

  hve_SwitchAllOff_pushButton->setEnabled(TRUE);
  hve_Set_pushButton->setEnabled(TRUE);
  hve_M100_pushButton->setEnabled(TRUE);
  hve_M010_pushButton->setEnabled(TRUE);
  hve_M001_pushButton->setEnabled(TRUE);
  hve_P100_pushButton->setEnabled(TRUE);
  hve_P010_pushButton->setEnabled(TRUE);
  hve_P001_pushButton->setEnabled(TRUE);
  hve_Zero_pushButton->setEnabled(TRUE);
  hve_Restore_pushButton->setEnabled(TRUE);
  hve_SelectAll_pushButton->setEnabled(TRUE);
  hve_Scale_pushButton->setEnabled(TRUE);
  hve_Scale_comboBox->setEnabled(TRUE);
}

//*******************************************************************************************************
/*!
 * <p>Opens all the necessary keys of the ODB.
 *
 * <p><b>Return:</b>
 *   - TRUE if all the ODB Keys could have been opened.
 *   - FALSE otherwise. 
 */
BOOL Qt_hvEdit::openKeys()
{
  // Root key
  rootKey = new cmKey(cmExp, hvA->getHV_ODB_Root());
  if (!rootKey->IsValid())
    return FALSE;

  // Names key
  namesKey = new cmKey(rootKey, hvA->getHV_ODB_Names());
  if (!namesKey->IsValid())
    return FALSE;

  // Demand key
  demandKey = new cmKey(rootKey, hvA->getHV_ODB_Demand());
  if (!demandKey->IsValid())
    return FALSE;

  // Measured key
  measuredKey = new cmKey(rootKey, hvA->getHV_ODB_Measured());
  if (!measuredKey->IsValid())
    return FALSE;

  // Current key
  currentKey = new cmKey(rootKey, hvA->getHV_ODB_Current());
  if (!currentKey->IsValid())
    return FALSE;

  // Current Limit key
  currentLimitKey = new cmKey(rootKey, hvA->getHV_ODB_Current_Limit());
  if (!currentLimitKey->IsValid())
    return FALSE;

  return TRUE;
}

//*******************************************************************************************************
/*!
 * <p>Closes all the keys of the ODB.
 *
 * <p><b>Return:</b> void
 */
void Qt_hvEdit::closeKeys()
{
  if (rootKey) {
    delete rootKey;
    rootKey = NULL; // reinitialize
  }
  if (namesKey) {
    delete namesKey;
    namesKey = NULL; // reinitialize
  }
  if (demandKey) {
    delete demandKey;
    demandKey = NULL; // reinitialize
  }
  if (measuredKey) {
    delete measuredKey;
    measuredKey = NULL; // reinitialize
  }
  if (currentKey) {
    delete currentKey;
    currentKey = NULL; // reinitialize
  }
  if (currentLimitKey) {
    delete currentLimitKey;
    currentLimitKey = NULL; // reinitialize
  }
}

//*******************************************************************************************************
/*!
 * <p>Free's all the local memory.
 *
 * <p><b>Return:</b> void
 */
void Qt_hvEdit::freeMem()
{
  // free local memory
  if (m_Selection) {
    free(m_Selection);
    free(m_Group);
    free(m_Restore);
    free(m_DemandCache);
    free(m_MeasuredCache);
    free(m_CurrentCache);
    free(m_CurrentLimitCache);
    m_Selection = NULL;
  }
}

//*******************************************************************************************************
/*!
 * <p>Translates a channel index of a device to a channel index
 * of HV ODB.
 *
 * <p><b>Return:</b> channel index of the HV ODB
 *
 * \param ch channel index of a HV device.
 */
int Qt_hvEdit::channelIndex(int ch)
{
  int i,j;

  // Find index to m_Demand etc. according to selected
  // channel 'ch' and group 'm_iGroup'

  for (i=0,j=0 ; i<m_nChannels ; i++) {
    if (m_Group[i] == m_iGroup)
      j++;
    if (j == ch+1)
      return i;
  }

  return 0;
}

//*******************************************************************************************************
/*!
 * <p>Clears the HV/Current Limit tables.
 *
 * <p><b>Return:</b> void
 */
void Qt_hvEdit::clearTable()
{
  int i, j;
  for (i=0; i<hve_HV_table->numCols(); i++) {
    for (j=0; j<hve_HV_table->numRows(); j++) {
      hve_HV_table->clearCell(j,i);
    }
  }
  for (i=0; i<hve_currentLimit_table->numCols(); i++) {
    for (j=0; j<hve_currentLimit_table->numRows(); j++) {
      hve_currentLimit_table->clearCell(j,i);
    }
  }
}

//*******************************************************************************************************
/*!
 * <p>Sets the HV demand value in the ODB. If 'ch' is larger than zero,
 * only a single channel is written, otherwise the whole array is written.
 *
 * <p><b>Return:</b> void
 *
 * \param ch channel of the HV ODB
 */
void Qt_hvEdit::updateODB_demand(int ch)
{
  int status, size;

  // Update demand values in database
  if (ch >= 0)
    status = demandKey->SetDataIndex(m_Demand+ch, ch, TID_FLOAT);
  else
    status = demandKey->SetData(m_Demand, m_nChannels, TID_FLOAT);

  if (status == DB_NO_ACCESS) {
    size = m_nChannels * sizeof(float);
    demandKey->GetData(m_Demand, &size, TID_FLOAT);
    MidasMessage("Change of demand values currently not allowed");
  }
}

//*******************************************************************************************************
/*!
 * <p>Sets the Current Limit value in the ODB. If 'ch' is larger than zero,
 * only a single channel is written, otherwise the whole array is written.
 *
 * <p><b>Return:</b> void
 *
 * \param ch channel of the HV ODB
 */
void Qt_hvEdit::updateODB_currentLimit(int ch)
{
  int status, size;

  // Update current limit values in database
  if (ch >= 0)
    status = currentLimitKey->SetDataIndex(m_CurrentLimit+ch, ch, TID_FLOAT);
  else
    status = currentLimitKey->SetData(m_CurrentLimit, m_nChannels, TID_FLOAT);

  if (status == DB_NO_ACCESS) {
    size = m_nChannels * sizeof(float);
    currentLimitKey->GetData(m_CurrentLimit, &size, TID_FLOAT);
    MidasMessage("Change of current limit values currently not allowed");
  }
}

//*******************************************************************************************************
/*!
 * <p>Increments the current selectio of the active tab by the amount 'incr'.
 *
 * <p><b>Return:</b> void
 *
 * \param incr Value by which the selected cells shall be incremented.
 */
void Qt_hvEdit::increment(const float incr)
{
  int i, j;
  int device;

  // get device selection
  device = hve_Device_listBox->currentItem();
  j = 0; // device channel counter
  for (i=0; i<m_nChannels; i++) { // i = all over channel counter
    if (m_Group[i] == device) { // check if correct device
      if (hve_HV_tabWidget->currentPageIndex() == 0) { // i.e. HV demand tab
        if (hve_HV_table->isSelected(j,HV_DEMAND)) { // get HV selection
	  // save current value into restore buffer
	  m_Restore[i] = m_Demand[i];
          // set voltage
          m_Demand[i] += incr;
        }
      } else { // current limit tab
        if (hve_currentLimit_table->isSelected(j,CL_CURRENT_LIMIT)) { // get current selection
          // set current limit
          m_CurrentLimit[i] += incr;
        }
      }
      j++;
    }
  }
  if (hve_HV_tabWidget->currentPageIndex() == 0) { // i.e. HV demand tab
    updateODB_demand(-1);
  } else { // current limit tab
    updateODB_currentLimit(-1);
  }

  updateTable(-1);
}

//*******************************************************************************************************
/*!
 * <p>Searches a sub-string sandwiched between 'first' and 'last'.
 *
 * <p><b>Return:</b> Pointer to the sub-string
 * 
 * \param str    haystack, i.e. long string containing sub-string
 * \param first  sub-string in front of the searched one
 * \param last   sub-string following of the searched one
 * \param result pointer to the result string
 */
QString *Qt_hvEdit::findSub(char str[], char first[], char last[], QString *result)
{
   char sub[80], *lp;

   if ((lp=strstr(str, first)) == NULL) 
     return NULL;
   strcpy(sub, lp+strlen(first));
   if ((lp=strstr(sub, last)) == NULL)
     return NULL;
   *lp = '\0';
   *result = sub;

   return result;
}

//*******************************************************************************************************
/*!
 * <p>A member function cannot be passed to db_open_record. Therefore, NamesChanged
 *    works as a stub to call Qt_hvEdit::updateChannelDefinition.
 *
 * <p><b>Return:</b> void
 */
void namesChanged(int,int,void*)
{
  Qt_hvEdit *dlg;

  dlg = (Qt_hvEdit *) qApp->activeWindow();
  dlg->updateChannelDefinitions();
  dlg->updateTable(-1);
}

//*******************************************************************************************************
/*!
 * <p>This function gets linked to the open records (demand and measured)
 *    by the db_open_record calls. Whenever a record changes, cm_yield
 *    first updates the local copy of the record and then calls odb_update
 *
 * <p><b>Return:</b> void
 *
 * \param hDB  Handle to the ODB.
 * \param hKey Handle to the corresponding key.
 * \param info Pointer to the GUI.
 */
void HVChanged(HNDLE hDB, HNDLE hKey, void *info)
{
  Qt_hvEdit  *dlg;

  if (hDB);  // only there to cheat the compiler warning hDB never used ...
  if (hKey); // only there to cheat the compiler warning hKey never used ...

  dlg = (Qt_hvEdit *) info;
  if (dlg == NULL) {
    MidasMessage(">> HVChanges: Couldn't get Main Window!!");
  } else {
    dlg->updateTable(-1);
  }
}

