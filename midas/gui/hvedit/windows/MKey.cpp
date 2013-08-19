// MKey.cpp : implementation file
//

#include "stdafx.h"
#include "hvedit.h"
#include "midas.h"
#include "MExperiment.h"
#include "MKey.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMKey

CMKey::CMKey(CMExperiment *pExperiment, CString strKeyName)
{
  m_pExperiment = pExperiment;
  m_hDB = pExperiment->GetHdb();
  m_bValid = (db_find_key(m_hDB, 0, strKeyName.GetBuffer(256), &m_hKey) == DB_SUCCESS);
  if (m_bValid)
    db_get_key(m_hDB, m_hKey, &m_Key);
  else
    {
    CString msg("Cannot find " + strKeyName + " entry in online database");
    MessageBox(GetFocus(), msg.GetBuffer(256), "MIDAS", MB_OK);
    }
}

CMKey::CMKey(CMKey *pRootKey, CString strKeyName)
{
  m_pExperiment = pRootKey->m_pExperiment;
  m_hDB = pRootKey->GetHdb();
  m_bValid = (db_find_key(m_hDB, pRootKey->GetHkey(), 
                          strKeyName.GetBuffer(256), &m_hKey) == DB_SUCCESS);
  if (m_bValid)
    db_get_key(m_hDB, m_hKey, &m_Key);
  else
    {
    CString msg("Cannot find " + strKeyName + " entry in online database");
    MessageBox(GetFocus(), msg.GetBuffer(256), "MIDAS", MB_OK);
    }
}

CMKey::~CMKey()
{
}

INT CMKey::UpdateKey()
{
  return db_get_key(m_hDB, m_hKey, &m_Key);
}

INT CMKey::HotLink(void *addr, WORD access, void (*dispatcher)(INT,INT,void*))
{
  m_pExperiment->StartTimer();

  return db_open_record(m_hDB, m_hKey, addr, m_Key.total_size, access, dispatcher, this);
}

INT CMKey::Unlink()
{
  return db_close_record(m_hDB, m_hKey);
}

INT CMKey::GetData(void *data, INT *size, DWORD type)
{
  return db_get_data(m_hDB, m_hKey, data, size, type);
}

INT CMKey::SetData(void *data, INT num_values, DWORD type)
{
  return db_set_data(m_hDB, m_hKey, data, num_values*m_Key.item_size, num_values, type);
}

INT CMKey::SetDataIndex(void *data, INT index, DWORD type)
{
  return db_set_data_index(m_hDB, m_hKey, data, m_Key.item_size, index, type);
}

INT CMKey::Load(CString strFileName, BOOL bRemote)
{
  return db_load(m_hDB, m_hKey, (char *) LPCTSTR(strFileName), bRemote);
}

INT CMKey::Save(CString strFileName, BOOL bRemote)
{
  return db_save(m_hDB, m_hKey, (char *) LPCTSTR(strFileName), bRemote);
}

DWORD CMKey::GetKeyTime()
{
DWORD delta;

  db_get_key_time(m_hDB, m_hKey, &delta);
  return delta;
}
