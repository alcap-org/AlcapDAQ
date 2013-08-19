/********************************************************************************************

  hvAdmin.h

*********************************************************************************************

    begin                : Andreas Suter, 2002/11/22
    modfied:             :
    copyright            : (C) 2003 by
    email                : andreas.suter@psi.ch

  $Log: hvAdmin.h,v $
  Revision 1.1.1.1  2005/06/20 23:37:12  mucap
  Importing release 1.9.5 of the MIDAS source code as distributed by PSI.
  (Next, I'll commit our local customizations.)

  Revision 1.4  2004/01/08 08:40:09  midas
  Implemented standard indentation

  Revision 1.3  2003/12/30 14:54:26  suter_a
  "doxygenized" code, i.e. added comments which can be handled by doxygen in
  order to generate html- and latex-docu.

  Revision 1.2  2003/10/27 11:22:59  suter_a
  use new QSettings for default settings. Changed the routines accordingly.

  Revision 1.1  2003/05/09 10:08:09  midas
  Initial revision

  Revision 1.1.1.2  2003/03/03 09:33:50  nemu
  add documentation path info to help facility.

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

#ifndef _HVADMIN_H
#define _HVADMIN_H

#include <qstring.h>

class hvAdmin {
   // construct
 public:
   hvAdmin();
   ~hvAdmin();

   // attributes
 private:
   QString hvA_default_dir_hv_settings; //!< Default directory where the high voltage default files are stored
   QString hvA_default_dir_docu;        //!< Default directory of the documentation
   QString hvA_midas_odb_hv_root;       //!< ODB path for the hvEdit ODB root key
   QString hvA_midas_odb_hv_names;      //!< ODB path for the HV names key
   QString hvA_midas_odb_hv_demand;     //!< ODB path for the HV demand values
   QString hvA_midas_odb_hv_measured;   //!< ODB path for the HV measured values
   QString hvA_midas_odb_hv_current;    //!< ODB path for the HV current values
   QString hvA_midas_odb_hv_current_limit;      //!< ODB path for the HV current limit values
   bool b_hvA_default_dir_hv_settings;  //!< Tag showing if the default directory for the HV settings is defined 
   bool b_hvA_default_dir_docu; //!< Tag showing if the default directory for the docu is defined 
   bool b_hvA_midas_odb_hv_root;        //!< Tag showing if the default root key path is defined
   bool b_hvA_midas_odb_hv_names;       //!< Tag showing if the default names key path is defined
   bool b_hvA_midas_odb_hv_demand;      //!< Tag showing if the default HV demand value key path is defined
   bool b_hvA_midas_odb_hv_measured;    //!< Tag showing if the default HV measured value key path is defined
   bool b_hvA_midas_odb_hv_current;     //!< Tag showing if the default HV current measured key path is defined
   bool b_hvA_midas_odb_hv_current_limit;       //!< Tag showing if the default current limit key path is defined

   // operations
 public:
   //! Returns the directory where the high voltage default files are stored
    QString * getHVDefaultDir() {
      return &hvA_default_dir_hv_settings;
   };
   //! Returns the directory of the documentation
   QString *getHVDefaultDoc() {
      return &hvA_default_dir_docu;
   };
   //! Returns the ODB path for the hvEdit ODB root key
   QString *getHV_ODB_Root() {
      return &hvA_midas_odb_hv_root;
   };
   //! Returns the ODB path for the HV names key
   QString *getHV_ODB_Names() {
      return &hvA_midas_odb_hv_names;
   };
   //! Returns the ODB path for the HV demand values
   QString *getHV_ODB_Demand() {
      return &hvA_midas_odb_hv_demand;
   };
   //! Returns the ODB path for the HV measured values
   QString *getHV_ODB_Measured() {
      return &hvA_midas_odb_hv_measured;
   };
   //! Returns the ODB path for the HV current values
   QString *getHV_ODB_Current() {
      return &hvA_midas_odb_hv_current;
   };
   //! Returns the ODB path for the HV current limit values
   QString *getHV_ODB_Current_Limit() {
      return &hvA_midas_odb_hv_current_limit;
   };
};

#endif                          // _HVADMIN_H
