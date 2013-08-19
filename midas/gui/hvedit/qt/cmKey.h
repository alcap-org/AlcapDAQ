/*******************************************************************************************

  cmKey.h

*********************************************************************************************

    begin                : Andreas Suter, 2002/12/02
    modfied:             :
    copyright            : (C) 2003 by
    email                : andreas.suter@psi.ch

  $Log: cmKey.h,v $
  Revision 1.1.1.1  2005/06/20 23:37:12  mucap
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


#ifndef _CMKEY_H
#define _CMKEY_H

#include <qstring.h>

#include "cmExperiment.h"

class cmKey {
   // Construction
 public:
   cmKey(cmExperiment * pExperiment, QString * strKeyName);
   cmKey(cmKey * pRootKey, QString * strKeyName);
   ~cmKey();

   // Attributes
 private:
   HNDLE m_hDB;                 //!< Main handle to the ODB of MIDAS.
   HNDLE m_hKey;                //!< Handle to a key in the ODB.
   BOOL m_bValid;               //!< Validator flag, showing if the searched key is accessible or existing.
   KEY m_Key;                   //!< MIDAS Structure to the key. Holding informations about the content of the key.
   cmExperiment *m_pExperiment; //!< Pointer to a MIDAS experiment object.

   // Operations
 public:
   //! Returns the Main handle of the ODB.
    HNDLE GetHdb() {
      return m_hDB;
   };
   //! Returns the key. 
   HNDLE GetHkey() {
      return m_hKey;
   };
   //! Returns if the searched key is accessible or existing.
   BOOL IsValid() {
      return m_bValid;
   };
   //! Returns the type of the key.
   INT GetType() {
      return m_Key.type;
   };
   //! Returns the item size of the key.
   INT GetItemSize() {
      return m_Key.item_size;
   };
   //! Returns the access mode of the key
   WORD GetAccessMode() {
      return m_Key.access_mode;
   };
   //! Returns the number of elements contained in the key.
   INT GetNumValues() {
      return m_Key.num_values;
   };
   INT UpdateKey();
   INT HotLink(void *addr, WORD access, void (*dispatcher) (INT, INT, void *),
               void *window);
   INT Unlink();

   INT GetData(void *data, INT * size, DWORD type);
   INT SetData(void *data, INT num_values, DWORD type);
   INT SetDataIndex(void *data, INT index, DWORD type);
   DWORD GetKeyTime();
};


#endif                          // _CMKEY_H
