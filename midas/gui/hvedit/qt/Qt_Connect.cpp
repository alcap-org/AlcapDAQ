/*******************************************************************************************

  Qt_Dlg_Connect.cpp

  implementation of the virtual class 'Qt_Dlg_Connect_Base' from the Qt designer
  (see 'Qt_Dlg_Connect_Base.ui')

  This GUI handels the pc-name request, displays the corresponding midas experiment list.

*********************************************************************************************

    begin                : Andreas Suter, 2002/11/21
    modfied:             :
    copyright            : (C) 2003 by
    email                : andreas.suter@psi.ch

  $Log: Qt_Connect.cpp,v $
  Revision 1.1.1.1  2005/06/20 23:37:11  mucap
  Importing release 1.9.5 of the MIDAS source code as distributed by PSI.
  (Next, I'll commit our local customizations.)

  Revision 1.2  2003/12/30 14:54:26  suter_a
  "doxygenized" code, i.e. added comments which can be handled by doxygen in
  order to generate html- and latex-docu.

  Revision 1.1  2003/05/09 10:08:09  midas
  Initial revision

  Revision 1.2  2003/02/27 15:19:28  nemu
  fixed minor mistake in getExpList()


********************************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <qmessagebox.h>
#include <qlineedit.h>
#include <qlistbox.h>

#include "midas.h"

#include "Qt_Connect.h"

//*******************************************************************************************************
/*!
 * <p>Shows the 'connect to' dialog which is used to establish the connection
 * to a MIDAS experiment. 
 * 
 * \param cmExp Pointer to a MIDAS experiment object
 * \param parent Pointer to the parent widget.
 * \param name Pointer to the internal name
 * \param modal Flag telling if the widget is going to be modal or not.
 * \param fl The widget flags argument, f, is normally 0, but it can be set to 
 *           customize the window frame of a top-level widget (i.e. parent must be 0). 
 *           To customize the frame, set the WStyle_Customize flag OR'ed with any 
 *           of the Qt::WidgetFlags.  
 */
Qt_Connect::Qt_Connect(cmExperiment *cmExp, QWidget *parent, const char *name,
                       bool modal, WFlags fl) : Qt_Connect_Base(parent, name, modal, fl)
{
  Qt_Connect::cmExp = cmExp; // links local experiment to the global one
  host_lineEdit->setFocus(); // set the focus to the host name field
}

//*******************************************************************************************************
/*!
 * <p>Destroys the object.
 */
Qt_Connect::~Qt_Connect()
{
}

//*******************************************************************************************************
/*!
 * <p>Gets the list of experiments from 'host' and displays them.
 *
 * <p><b>Return:</b> void.
 */
void Qt_Connect::getExpList()
{
  cmExp->SetHostName(host_lineEdit->text());        // gets the host name
  char exp_name_list[MAX_EXPERIMENT][NAME_LENGTH];  // list of all the available experiments
  int  i;

  // try to get list of experiments from host
  if (cm_list_experiments((char *)cmExp->GetHostName().ascii(), exp_name_list) != CM_SUCCESS) {
    QMessageBox::warning(this, "ERROR", "Could not connect to "+cmExp->GetHostName(),
                         QMessageBox::Ok, QMessageBox::Cancel);
    return;
  }

  // generate list of experiments for widget
  exp_listBox->clear(); // deletes a perhaps existing list
  i=0;
  while (strcmp(exp_name_list[i],"")) {
    exp_listBox->insertItem(exp_name_list[i],-1); // insert new item at the end of the list
    i++;
  }
  exp_listBox->setFocus();
}

//*******************************************************************************************************
/*!
 * <p>Gets the selected experiment from the list and terminates the GUI.
 *
 * <p><b>Return:</b> void.
 */
void Qt_Connect::gotExperiment()
{
  cmExp->SetExperimentName(exp_listBox->currentText());
  Qt_Connect::done(0);
}
