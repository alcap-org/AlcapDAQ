/********************************************************************************************

  hvEdit.cpp

*********************************************************************************************

    begin                : Andreas Suter, 2002/11/19
    modfied:             :
    copyright            : (C) 2003 by
    email                : andreas.suter@psi.ch

  $Log: hvEdit.cpp,v $
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

/*!
 * \mainpage Qt hvEdit for MIDAS
 *
 * <p>hvEdit is a GUI based on the Qt libraries of trolltech (http://www.trolltech.com).
 * It is interfacing MIDAS. The Maximum Integrated Data Acquisition System (MIDAS) is a 
 * general-purpose system for event based data acquisition in small and medium scale physics 
 * experiments. It has been developed at the Paul Scherrer Institute (Switzerland) and at 
 * TRIUMF (Canada) starting 1993 (http://midas.psi.ch and http://midas.triumf.ca).
 *
 * <p>The purpose of hvEdit is to simplify the handling of high voltages in our experiment, 
 * (low energy muon spin rotation \f$\mathrm{LE-\mu SR}\f$ http://lmu.web.psi.ch/lem/index.html).
 * The GUI, however is set up in a way, that it is quite generally usable without any changes
 * on the source code.
 *
 * \section hvEdit How does hvEdit works?
 *
 * <p>The functionality of hvEdit is shortly described in it's one help system 
 * (<a HREF="../hvEdit_Help.html">hvEdit Help</a>). The idea of hvEdit is the following:
 * hvEdit uses the hot-link mechanism of MIDAS. This allows to link a C structure to the online 
 * database (ODB) of MIDAS. Whenever something changes, either in the ODB or in the C structure,
 * this is propagated to the other side. On the C side, a dispatcher function takes care of
 * what needs to be done with the updated data. This ansatz is very nice in the sence that it
 * is very simple to interface to any kind of device. Still, there is a price to be paied as well:
 * Since one is only communicating with the ODB with its one scheduling system, sometimes the
 * responds times are somewhat slowly.
 */ 
 
 
/* Qt stuff */
#include <qapplication.h>

#include "Qt_hvEdit.h"

/*!
 * <p>Main function, which calls the hvEdit Qt widget. 
 *
 * <p><b>Return:</b> The return value of the Qt Application
 *
 * \param argc dummy
 * \param argv dummy
 */
int main(int argc, char *argv[])
{
  // create administration class
  hvAdmin *hvA = new hvAdmin();
  // create experiment class
  cmExperiment *mExp = new cmExperiment("HVEdit");
  
  QApplication app(argc, argv); // create main Qt application entry point
  Qt_hvEdit    *dlg_hvEdit = new Qt_hvEdit(hvA, mExp);  // create Qt hvEdit Dialog

  dlg_hvEdit->show(); // show widget
  return app.exec();
}
