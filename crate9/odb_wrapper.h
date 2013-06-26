int odb_connect(char *format, ...);
int odb_find_key(char *format, ...);
int odb_get_int(char *format, ...);
WORD odb_get_word(char *format, ...);
DWORD odb_get_dword(char *format, ...);
BOOL odb_get_bool(char *format, ...);
void odb_get_boolarray(BOOL *array_out, int num_array_elements,
		       char *format, ...);
