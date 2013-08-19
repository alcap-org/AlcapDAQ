/*******************************************************************************************

  hvAdmin.cpp

  Administration class to encapsulate midas online data base (ODB) settings.

  The constructor tries to read the file '$HOME/.qt/hveditrc' in which all the relevant
  ODB pathes can be given. If '$HOME/.qt/hveditrc' does not exist, some default settings
  will be tried.
  
  Sample 'hveditrc' file:
    # hveditrc
    # Adminstration file for hvEdit
    #
    # this file should be placed in $HOME/.qt,
    #
    # Andreas Suter, 2003/10/27
    #

    [General]
    DEFAULT_SETTING_DIR=.
    DEFAULT_DOC_DIR=gui/hvedit/qt/doc

    [ODB Keys]
    MIDAS_KEY_HV_ROOT=/Equipment/HV
    MIDAS_KEY_HV_NAMES=Settings/Names
    MIDAS_KEY_HV_DEMAND=Variables/Demand
    MIDAS_KEY_HV_MEASURED=Variables/Measured
    MIDAS_KEY_HV_CURRENT=Variables/Current
    MIDAS_KEY_HV_CURRENT_LIMIT=Settings/Current Limit
    
    # end of hveditrc

*********************************************************************************************

    begin                : Andreas Suter, 2002/11/22
    modfied:             :
    copyright            : (C) 2003 by
    email                : andreas.suter@psi.ch

  $Log: hvAdmin.cpp,v $
  Revision 1.1.1.1  2005/06/20 23:37:12  mucap
  Importing release 1.9.5 of the MIDAS source code as distributed by PSI.
  (Next, I'll commit our local customizations.)

  Revision 1.4  2003/12/30 14:54:26  suter_a
  "doxygenized" code, i.e. added comments which can be handled by doxygen in
  order to generate html- and latex-docu.

  Revision 1.3  2003/10/27 11:22:59  suter_a
  use new QSettings for default settings. Changed the routines accordingly.

  Revision 1.2  2003/05/12 16:19:09  suter_a
  fixed problem with white spaces in '.hvEdit'

  Revision 1.1  2003/05/09 10:08:09  midas
  Initial revision

  Revision 1.1.1.2  2003/03/03 09:33:50  nemu
  add documentation path info to help facility.

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

//#include <stdlib.h>

#include <qstring.h>
#include <qsettings.h>

#include "midas.h"

#include "hvAdmin.h"

//**********************************************************************
/*!
 * <p>Tries to read '$HOME/.qt/hveditrc' file in order to get the correct ODB pathes.
 * If '$HOME/.qt/hveditrc' doesn't exist, default pathes are tried, and a warning
 * is sent to the MIDAS message system.
 */
hvAdmin::hvAdmin()
{
  QSettings settings;
  QString   midas_root;

  // initialize flags for ODB pathes
  b_hvA_default_dir_hv_settings = FALSE;
  b_hvA_default_dir_docu = FALSE;
  b_hvA_midas_odb_hv_root = FALSE;
  b_hvA_midas_odb_hv_names = FALSE;
  b_hvA_midas_odb_hv_demand = FALSE;
  b_hvA_midas_odb_hv_measured = FALSE;
  b_hvA_midas_odb_hv_current = FALSE;
  b_hvA_midas_odb_hv_current_limit = FALSE;

  // read all the setting keys
  hvA_default_dir_hv_settings = settings.readEntry("/hvEdit/General/DEFAULT_SETTING_DIR", ".", &b_hvA_default_dir_hv_settings);
  hvA_default_dir_docu = settings.readEntry("/hvEdit/General/DEFAULT_DOC_DIR", "gui/hvedit/qt/doc", &b_hvA_default_dir_docu);
  hvA_midas_odb_hv_root = settings.readEntry("/hvEdit/ODB Keys/MIDAS_KEY_HV_ROOT", "/Equipment/HV", &b_hvA_midas_odb_hv_root);
  hvA_midas_odb_hv_names = settings.readEntry("/hvEdit/ODB Keys/MIDAS_KEY_HV_NAMES", "Settings/Names", &b_hvA_midas_odb_hv_names);
  hvA_midas_odb_hv_demand = settings.readEntry("/hvEdit/ODB Keys/MIDAS_KEY_HV_DEMAND", "Variables/Demand", &b_hvA_midas_odb_hv_demand);
  hvA_midas_odb_hv_measured = settings.readEntry("/hvEdit/ODB Keys/MIDAS_KEY_HV_MEASURED", "Variables/Measured", &b_hvA_midas_odb_hv_measured);
  hvA_midas_odb_hv_current = settings.readEntry("/hvEdit/ODB Keys/MIDAS_KEY_HV_CURRENT", "Variables/Current", &b_hvA_midas_odb_hv_current);
  hvA_midas_odb_hv_current_limit = settings.readEntry("/hvEdit/ODB Keys/MIDAS_KEY_HV_CURRENT_LIMIT", "Settings/Current Limit", &b_hvA_midas_odb_hv_current_limit);

  // add $MIDAS_ROOT to hvA_default_dir_docu
  midas_root = getenv("MIDAS_ROOT");
  if (midas_root.isNull()) { // haven't found $MIDAS_ROOT use current directory
    cm_msg(MINFO, "hvAdmin", "hvEdit: Warning: Haven't found $MIDAS_ROOT use current directory.");
    midas_root = ".";
  }
  hvA_default_dir_docu = midas_root+"/"+hvA_default_dir_docu;

  // check if '$HOME/.qt/hvEdit' could be read
  if (!b_hvA_midas_odb_hv_root) {
    cm_msg(MINFO, "hvAdmin", "hvEdit: Warning: Couldn't read ODB HV root path settings, will try '/Equipment/HV' instead.");
  }
  if (!b_hvA_midas_odb_hv_names) {
    cm_msg(MINFO, "hvAdmin", "hvEdit: Warning: Couldn't read ODB HV names path settings, will try 'Settings/Names' instead.");
  }
  if (!b_hvA_midas_odb_hv_demand) {
    cm_msg(MINFO, "hvAdmin", "hvEdit: Warning: Couldn't read ODB HV demand path settings, will try 'Variables/Demand' instead.");
  }
  if (!b_hvA_midas_odb_hv_measured) {
    cm_msg(MINFO, "hvAdmin", "hvEdit: Warning: Couldn't read ODB HV measured path settings, will try 'Variables/Measured' instead.");
  }
  if (!b_hvA_midas_odb_hv_current) {
    cm_msg(MINFO, "hvAdmin", "hvEdit: Warning: Couldn't read ODB HV current path settings, will try 'Variables/Current' instead.");
  }
  if (!b_hvA_midas_odb_hv_current_limit) {
    cm_msg(MINFO, "hvAdmin", "hvEdit: Warning: Couldn't read ODB HV demand path settings, will try 'Settings/Current Limit' instead.");
  }
}

//**********************************************************************
/*!
 * <p>Destroys the object.
 */
hvAdmin::~hvAdmin()
{
}
