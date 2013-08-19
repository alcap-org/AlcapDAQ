/********************************************************************************************

   Qt_Dlg_Connect.h

   implementation of the virtual class 'Qt_Dlg_Connect_Base' from the Qt designer
   (see 'Qt_Dlg_Connect_Base.ui').

   For details see the implementation: Qt_Dlg_Connect.cpp

*********************************************************************************************

    begin                : Andreas Suter, 2002/11/21
    modfied:             :
    copyright            : (C) 2003 by
    email                : andreas.suter@psi.ch

  $Log: Qt_Connect.h,v $
  Revision 1.1.1.1  2005/06/20 23:37:11  mucap
  Importing release 1.9.5 of the MIDAS source code as distributed by PSI.
  (Next, I'll commit our local customizations.)

  Revision 1.3  2004/01/08 08:40:09  midas
  Implemented standard indentation

  Revision 1.2  2003/12/30 14:54:26  suter_a
  "doxygenized" code, i.e. added comments which can be handled by doxygen in
  order to generate html- and latex-docu.

  Revision 1.1  2003/05/09 10:08:09  midas
  Initial revision

  Revision 1.1.1.1  2003/02/27 15:24:37  nemu
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

#ifndef QT_CONNECT_H
#define QT_CONNECT_H

#include "cmExperiment.h"
#include "Qt_Connect_Base.h"

class Qt_Connect:public Qt_Connect_Base {
   Q_OBJECT
       // Constructor/Destructor
 public:
   Qt_Connect(cmExperiment * cmExp,
              QWidget * parent = 0, const char *name = 0, bool modal = FALSE, WFlags f =
              WDestructiveClose);
   ~Qt_Connect();

   // Attributes
 private:
    cmExperiment * cmExp;       //!< Pointer to a MIDAS experiment object.

   // Slots
   public slots:void gotExperiment();

   private slots:void getExpList();
};

#endif                          // QT_CONNECT_H
