/*******************************************************************************************

  Qt_Pwd.cpp

  implementation of the virtual class 'Qt_Pwd_Base' from the Qt designer
  (see 'Qt_Pwd_Base.ui').

  GUI for the password handeling with midas.

*********************************************************************************************

    begin                : Andreas Suter, 2002/11/25
    modfied:             :
    copyright            : (C) 2003 by
    email                : andreas.suter@psi.ch

  $Log: Qt_Pwd.cpp,v $
  Revision 1.1.1.1  2005/06/20 23:37:11  mucap
  Importing release 1.9.5 of the MIDAS source code as distributed by PSI.
  (Next, I'll commit our local customizations.)

  Revision 1.2  2003/12/30 14:54:26  suter_a
  "doxygenized" code, i.e. added comments which can be handled by doxygen in
  order to generate html- and latex-docu.

  Revision 1.1  2003/05/09 10:08:09  midas
  Initial revision

  Revision 1.1.1.1  2003/02/27 15:26:12  nemu
  added log info


********************************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <qlineedit.h>
#include "Qt_Pwd.h"

//*******************************************************************************************************
/*!
 * <p>Passes the password from the password dialog to the cmExperiment class.
 *
 * \param password Pointer to the password
 * \param parent Pointer to the parent widget.
 * \param name Pointer to the internal name
 * \param modal Flag telling if the widget is going to be modal or not.
 * \param fl The widget flags argument, f, is normally 0, but it can be set to 
 *           customize the window frame of a top-level widget (i.e. parent must be 0). 
 *           To customize the frame, set the WStyle_Customize flag OR'ed with any 
 *           of the Qt::WidgetFlags.
 */
Qt_Pwd::Qt_Pwd(char *password, QWidget *parent, const char *name,
               bool modal, WFlags fl) : Qt_Pwd_Base(parent, name, modal, fl)
{
  pwd = password;
}

//*******************************************************************************************************
/*!
 * <p>Destroys the object.
 */
Qt_Pwd::~Qt_Pwd()
{
}

//*******************************************************************************************************
/*!
 * <p>Gets the password from the widget and closes it.
 *
 * <p><b>Return:</b> void.
 */
void Qt_Pwd::GetPassword()
{
  strcpy(pwd, (char *)pwd_lineEdit->text().ascii());
  close();
}
