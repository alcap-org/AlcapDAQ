/********************************************************************************************

  globals.cpp

*********************************************************************************************

    begin                : Andreas Suter, 2002/11/25
    modfied:             :
    copyright            : (C) 2003 by
    email                : andreas.suter@psi.ch

  $Log: globals.cpp,v $
  Revision 1.1.1.1  2005/06/20 23:37:12  mucap
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

#include "midas.h"
#include <qmessagebox.h>

//****************************************************************************************
/*!
 * <p>MidasMessage translates midas error messages to Qt Dialogs.
 *
 * <p><b>return:</b> 0
 * \param str error message.
 */
int MidasMessage(const char *str)
{
  char name[NAME_LENGTH];
  
  cm_get_client_info(name);
  QMessageBox::warning(0, "MIDAS ERROR", str, QMessageBox::Ok, QMessageBox::NoButton);
  return 0;
}

