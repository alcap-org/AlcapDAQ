/********************************************************************************************

  cmKey.cpp

  class to handle a single midas ODB key.

*********************************************************************************************

    begin                : Andreas Suter, 2002/11/25
    modfied:             :
    copyright            : (C) 2003 by
    email                : andreas.suter@psi.ch

  $Log: cmKey.cpp,v $
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

#include <qmessagebox.h>

#include "midas.h"

#include "cmKey.h"

//**********************************************************************
/*!
 * <p>Constructs a cmKey object, linked to a MIDAS experiment. It is
 * used to establish the main entry key for all the other ones.
 *
 * \param pExperiment Pointer to a MIDAS experiment object, i.e. the connection 
 *                    to this experiment is already established (see cmExperiment).
 * \param strKeyName  Name of the key in the ODB, which one wants to link to.
 */
cmKey::cmKey(cmExperiment *pExperiment, QString *strKeyName)
{
  m_pExperiment = pExperiment;   // pointer to the experiment
  m_hDB = pExperiment->GetHdb(); // handle to the ODB
  m_bValid = (db_find_key(m_hDB, 0, (char *)strKeyName->ascii(), &m_hKey) == DB_SUCCESS);
  if (m_bValid) {
    db_get_key(m_hDB, m_hKey, &m_Key);
  } else {
    QMessageBox::warning(0, "MIDAS", "Cannot find " + *strKeyName + " entry in online database",
                         QMessageBox::Ok, QMessageBox::NoButton);
  }
}

/*!
 * <p>Constructs a cmKey object, linked to a MIDAS experiment. It is 
 * used to establish a hot-link to a <em>specific</em> sub-key.  
 *
 * \param pRootKey   Pointer to the root key.
 * \param strKeyName Name of the key in the ODB, which one wants to link to.
 */
cmKey::cmKey(cmKey *pRootKey, QString *strKeyName)
{
  m_pExperiment = pRootKey->m_pExperiment; // pointer to the experiment
  m_hDB = pRootKey->GetHdb();              // handle to the ODB
  m_bValid = (db_find_key(m_hDB, pRootKey->GetHkey(),
                          (char *)strKeyName->ascii(), &m_hKey) == DB_SUCCESS);
  if (m_bValid) {
    db_get_key(m_hDB, m_hKey, &m_Key);
  } else {
    QMessageBox::warning(0, "MIDAS", "Cannot find " + *strKeyName + " entry in online database",
                         QMessageBox::Ok, QMessageBox::NoButton);
  }
}

//**********************************************************************
/*!
 * <p>Destroys the object.
 */
cmKey::~cmKey()
{
}

//**********************************************************************
/*!
 * <p>Updates the key handle and the related info structure. 
 * 
 * <p><b>Return:</b> 
 *   - DB_SUCCESS on successful completion.
 *   - DB_INVALID_HANDLE if ODB or key is invalid. 
 */
INT cmKey::UpdateKey()
{
  return db_get_key(m_hDB, m_hKey, &m_Key);
}


//**********************************************************************
/*!
 * <p>Establishes a hot-link between a local data structure and the ODB, i.e.
 * everytime this structure is changed in the ODB, it is automatically
 * propageted to the structure and vice versa. Further is starts the cyclic
 * timer needed for the MIDAS watch-dog process.
 *
 * <p><b>Return:</b> 
 *   - DB_SUCCESS on successful completion.
 *   - DB_INVALID_HANDLE if ODB or key is invalid. 
 *   - DB_STRUCT_SIZE_MISMATCH if the structure size doesn't match the sub-tree size
 *   - DB_NO_ACCESS if there is no READ/WRITE access to the sub-tree
 * 
 * \param addr Address of the local structure to which a hot-link shall be established.
 * \param access Access flags for this hot-link.
 * \param dispatcher Callback function which is called, if the entries in the ODB have 
 *                   changed. 
 * \param window Pointer to the ...
 */
INT cmKey::HotLink(void *addr, WORD access, void (*dispatcher)(INT,INT,void*), void *window)
{
  m_pExperiment->StartTimer(); // start a cylcic timer event

  // establishes a hotlink
  return db_open_record(m_hDB, m_hKey, addr, m_Key.total_size, access, dispatcher, window);
}

//**********************************************************************
/*!
 * <p>Closes the hot-link, i.e. disconnects the tight link between the local
 * structure and the ODB.
 * 
 * <p><b>Return:</b> 
 *   - DB_SUCCESS on successful completion.
 *   - DB_INVALID_HANDLE if ODB or key is invalid. 
 */
INT cmKey::Unlink()
{
  return db_close_record(m_hDB, m_hKey);
}

//**********************************************************************
/*!
 * <p>The function returns single values or whole arrays which are contained in an ODB key. 
 * Since the data buffer is of type void, no type checking can be performed by the compiler. 
 * Therefore the type has to be explicitly supplied, which is checked against the type 
 * stored in the ODB.
 * 
 * <p><b>Return:</b> 
 *   - DB_SUCCESS on successful completion.
 *   - DB_INVALID_HANDLE if ODB or key is invalid. 
 *   - DB_TRUNCATED if there is a size mismatch between the local structure and the ODB.
 *   - DB_TYPE_MISMATCH if there is a type discrepancy between the local structure and the ODB.
 *
 * \param data Pointer to the data structure. 
 * \param size Size of data buffer.
 * \param type Type of key, one of TID_xxx (see MIDAS documentation)
 */
INT cmKey::GetData(void *data, INT *size, DWORD type)
{
  return db_get_data(m_hDB, m_hKey, data, size, type);
}

//**********************************************************************
/*!
 * <p>Set key data from a handle. Adjust number of values if previous data has different size.
 *
 * <p><b>Return:</b> 
 *   - DB_SUCCESS on successful completion.
 *   - DB_INVALID_HANDLE if ODB or key is invalid. 
 *   - DB_TRUNCATED if there is a size mismatch between the local structure and the ODB.
 *
 * \param data Pointer to the data structure. 
 * \param num_values Number of data elements.
 * \param type Type of key, one of TID_xxx (see MIDAS documentation)
 */
INT cmKey::SetData(void *data, INT num_values, DWORD type)
{
  return db_set_data(m_hDB, m_hKey, data, num_values*m_Key.item_size, num_values, type);
}

//**********************************************************************
/*!
 * <p>Set key data for a key which contains an array of values. 
 * This function sets individual values of a key containing an array. 
 * If the index is larger than the array size, the array is extended and 
 * the intermediate values are set to zero.
 *
 * <p><b>Return:</b> 
 *   - DB_SUCCESS on successful completion.
 *   - DB_INVALID_HANDLE if ODB or key is invalid. 
 *   - DB_TRUNCATED if there is a size mismatch between the local structure and the ODB.
 *
 * \param data Pointer to the data structure. 
 * \param index Index of the data element.
 * \param type Type of key, one of TID_xxx (see MIDAS documentation)
 */
INT cmKey::SetDataIndex(void *data, INT index, DWORD type)
{
  return db_set_data_index(m_hDB, m_hKey, data, m_Key.item_size, index, type);
}

//**********************************************************************
/*!
 * <p>Returns the time in seconds, when the key was last time modified.
 *
 * <p><b>Return:</b> Time when key was last time modified. 
 */
DWORD cmKey::GetKeyTime()
{
  DWORD delta;

  db_get_key_time(m_hDB, m_hKey, &delta);
  return delta;
}
