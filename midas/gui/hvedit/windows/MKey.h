// MKey.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CMKey

class CMKey {
// Construction
 public:
   CMKey(CMExperiment * pExperiment, CString strKeyName);
   CMKey(CMKey * pRootKey, CString strKeyName);

// Attributes
 private:
   HNDLE m_hDB;
   HNDLE m_hKey;
   BOOL m_bValid;
   KEY m_Key;
   CMExperiment *m_pExperiment;

// Operations
 public:
    HNDLE GetHdb() {
      return m_hDB;
   };
   HNDLE GetHkey() {
      return m_hKey;
   };
   BOOL IsValid() {
      return m_bValid;
   };
   INT GetType() {
      return m_Key.type;
   };
   INT GetItemSize() {
      return m_Key.item_size;
   };
   WORD GetAccessMode() {
      return m_Key.access_mode;
   };
   INT GetNumValues() {
      return m_Key.num_values;
   };
   INT UpdateKey();
   INT HotLink(void *addr, WORD access, void (*dispatcher) (INT, INT, void *));
   INT Unlink();

   INT GetData(void *data, INT * size, DWORD type);
   INT SetData(void *data, INT num_values, DWORD type);
   INT SetDataIndex(void *data, INT index, DWORD type);
   INT Load(CString strFileName, BOOL bRemote = FALSE);
   INT Save(CString strFileName, BOOL bRemote = FALSE);
   DWORD GetKeyTime();

// Implementation
 public:
   virtual ~ CMKey();

 protected:
};

/////////////////////////////////////////////////////////////////////////////
