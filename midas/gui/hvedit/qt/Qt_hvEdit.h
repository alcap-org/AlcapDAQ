/********************************************************************************************

   Qt_hvEdit.h

*********************************************************************************************

    begin                : Andreas Suter, 2002/11/21
    modfied:             :
    copyright            : (C) 2003 by
    email                : andreas.suter@psi.ch

  $Log: Qt_hvEdit.h,v $
  Revision 1.1.1.1  2005/06/20 23:37:12  mucap
  Importing release 1.9.5 of the MIDAS source code as distributed by PSI.
  (Next, I'll commit our local customizations.)

  Revision 1.4  2004/01/08 08:40:09  midas
  Implemented standard indentation

  Revision 1.3  2003/12/30 14:54:26  suter_a
  "doxygenized" code, i.e. added comments which can be handled by doxygen in
  order to generate html- and latex-docu.

  Revision 1.2  2003/10/21 14:39:06  suter_a
  added scaling

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

#ifndef QT_HVEDIT_H
#define QT_HVEDIT_H

// definitions --------------------------------
#define FE_TIMEOUT 90           //!< Time out in (s) for the frontend to react

#define HV_NAME          0      //!< Table column address for the names (HV Tab)
#define HV_DEMAND        1      //!< Table column address for the HV demand values (HV Tab)
#define HV_MEASURED      2      //!< Table column address for the HV measured values (HV Tab)
#define HV_CURRENT       3      //!< Table column address for the measured currents (HV Tab)

#define CL_NAME          0      //!< Table column address for the names (Current Limit Tab)
#define CL_CURRENT       1      //!< Table column address for the HV measured values (Current Limit Tab)
#define CL_CURRENT_LIMIT 2      //!< Table column address for the measured currents (Current Limit Tab)
// ---------------------------------------------

#include <qstatusbar.h>

#include "hvAdmin.h"
#include "cmExperiment.h"
#include "cmKey.h"

#include "Qt_hvEdit_Base.h"

class Qt_hvEdit:public Qt_hvEdit_Base {
 Q_OBJECT public:
    Qt_hvEdit(hvAdmin * hvA, cmExperiment * cmExp,
              QWidget * parent = 0, const char *name = 0, WFlags fl = WType_TopLevel);
   ~Qt_hvEdit();

 public:
   void setHostName(QString host);
   void setExpName(QString experiment);
   BOOL updateChannelDefinitions();
   void updateTable(int ch);

   public slots:void connectToExp();
   void disconnectFromExp();

   protected slots:void fileOpen();
   void fileSave();
   void filePrint();
   void fileExit();
   void helpIndex();
   void helpContents();
   void helpAboutQt();
   void helpAbout();
   void changedDeviceSelection();
   void onSelectAll();
   void onSwitchAllChannelsOff();
   void onZero();
   void onRestore();
   void onM100();
   void onM010();
   void onM001();
   void onP100();
   void onP010();
   void onP001();
   void onSet();
   void onScale();
   void hvValueChanged(int row, int col);
   void currentLimitValueChanged(int row, int col);

   // Attributes
 private:
    hvAdmin * hvA;              //!< Administration class object
   cmExperiment *cmExp;         //!< Experiment class object
   cmKey *rootKey;              //!< Root key ODB object
   cmKey *namesKey;             //!< Names key ODB object
   cmKey *demandKey;            //!< Demand HV key ODB object
   cmKey *measuredKey;          //!< Measured HV key ODB object
   cmKey *currentKey;           //!< Measured current key ODB object
   cmKey *currentLimitKey;      //!< Current limit key ODB object

   INT m_nChannels;             //!< Number of HV channels
   INT m_nNameLength;           //!< Length of the name strings
   INT m_nGroups;               //!< Number of different groups (HV devices)
   INT m_iGroup;                //!< Group label tag
   INT m_iGroupCache;           //!< Group cache label tag
   char *m_Name;                //!< 'Array' of all the HV channel names
   float *m_Demand;             //!< 'Array' of all the HV demand values
   float *m_Measured;           //!< 'Array' of all the HV measured values
   float *m_Restore;            //!< 'Backup array' of all the HV demand values; used for restore functionality
   float *m_Current;            //!< 'Array' of all the measured current values
   float *m_CurrentLimit;       //!< 'Array' of all the current limit values
   float *m_DemandCache;        //!< 'Cache array' of all the HV demand values
   float *m_MeasuredCache;      //!< 'Cache array' of all the HV measured values
   float *m_CurrentCache;       //!< 'Cache array' of all the measured current values
   float *m_CurrentLimitCache;  //!< 'Cache array' of all the current limit values
   INT *m_Selection;            //!< really needed?? <<-- !!
   INT *m_Group;                //!< tag array which associates for each device a number 

   // Operations
 private:
   void disableNotConnected();
   void enableConnected();
   BOOL openKeys();
   void closeKeys();
   void freeMem();
   int channelIndex(int ch);
   void clearTable();
   void updateODB_demand(int ch);
   void updateODB_currentLimit(int ch);
   void increment(const float incr);
   void scale_hv(const float scale);
   QString *findSub(char str[], char first[], char last[], QString * result);
};

void namesChanged(int, int, void *);
void HVChanged(HNDLE hDB, HNDLE hKey, void *info);

#endif                          // QT_HVEDIT_H
