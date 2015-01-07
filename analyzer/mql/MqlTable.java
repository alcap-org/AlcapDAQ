import java.io.*;
import java.util.*;

public class MqlTable
{
  public MqlTable(String tableNameIn, String columnNamesIn[], 
    String expressionsIn[], int typesIn[])
  {
    tableName = tableNameIn;
    structName = MqlCompiler.getUniqueID();
    columnNames = columnNamesIn;
    expressions = expressionsIn;
    columnTypes = typesIn;

    tables.put(tableName, this);    
  }

  public int getNumColumns() 
  {
    return columnNames.length;
  }

  public String getColumnName(int i)
  {
    return columnNames[i]; 
  }

  public String[] getColumnNames() 
  {
    return columnNames;
  }

  public int[] getColumnTypes() 
  {
    return columnTypes;
  }

  public int getColumnType(int i) 
  {
    return columnTypes[i];
  }

  public boolean columnExists(String s)
  {
    for(int i = 0; i < getNumColumns(); i++) {
      if(s.equals(columnNames[i])) {
        return true;
      }
    }
    return false;
  }

  public int getColumnID(String s) 
  {
    for(int i = 0; i < getNumColumns(); i++) {
      if(s.equals(columnNames[i])) {
        return i;
      }
    }
    return -1;
  }

  public String[] getExpressions()
  {
    return expressions;
  }

  public String getExpression(int i)
  {
    return expressions[i];
  }

  public String getTableName() 
  {
    return tableName;
  }

  public String getStructName()
  {
    return structName;
  }

  public void outputStruct(MqlOutput o) {
    o.o("struct " + structName);
    o.openBrace();
    outputColumns(o);
    o.closeBrace(";");
    o.o("static MqlArray<struct " + structName + "> _t_" + tableName + ";");
    o.o();
  }

  public void outputCompare(MqlOutput o, String sortExpression, String compareName) {
      o.o("// Comparison for " + tableName + " on variable " + sortExpression);
      o.o("static int " + compareName + "(const void *_v1, const void *_v2)");
      o.openBrace();
      o.o("struct " + structName + " *_o1 = (struct " + structName + " *) _v1;");
      o.o("struct " + structName + " *_o2 = (struct " + structName + " *) _v2;");

    // Compute the sort variable for the first element
    o.o("double _sv1, _sv2;");
    o.openBrace();
    for(int i = 0; i < getNumColumns(); i++) {
      String columnName = columnNames[i];
      String columnType = MqlType.toString(columnTypes[i]);
      o.o(columnType + " " + columnName + " = _o1->" + columnName + ";");
    }
    o.o("_sv1 = " + sortExpression + ";");
    o.closeBrace();

    // now with the second element
    o.openBrace();
    for(int i = 0; i < getNumColumns(); i++) {
      String columnName = columnNames[i];
      String columnType = MqlType.toString(columnTypes[i]);
      o.o(columnType + " " + columnName + " = _o2->" + columnName + ";");
    }
    o.o("_sv2 = " + sortExpression + ";");
    o.closeBrace();

    o.o("double _diff = _sv1 - _sv2;");

    o.o("if(_diff < 0) return -1;");
    o.o("if(_diff == 0) return 0;");
    o.o("if(_diff > 0) return  1;");
    o.closeBrace();
  }

  public void outputColumns(MqlOutput o) {
    for(int i = 0; i < getNumColumns(); i++) {
      String columnName = columnNames[i];
      String columnType = MqlType.toString(columnTypes[i]);
      o.o(columnType + " " + columnName + ";"); 
    }
  }

  public void openTable(MqlOutput o)
  {
    o.o("_t_" + getTableName() + ".reset();");
  }

  public static void openTables(MqlOutput o) {
    Object[] t = tables.values().toArray();
    for(int i = 0; i < t.length; i++) {
      MqlTable table = (MqlTable) t[i];
      table.openTable(o);
    }
  }

  public void closeTable(MqlOutput o) {
    // If the table is to be exported, do so
    if(isExported()) {
      o.o("_t_" + getTableName() + ".toBank(pevent, " + 
            o.QUOTE + exportBank + o.QUOTE + ");");
    }
    o.o("_t_" + getTableName() + ".finish();");
  }

  public static void closeTables(MqlOutput o) {
    Object[] t = tables.values().toArray();
    for(int i = 0; i < t.length; i++) {
      MqlTable table = (MqlTable) t[i];
      table.closeTable(o);
    }
  }

  public String openLoop(MqlOutput o) 
  {
    // Get a loop index variable
    String indexVar = MqlCompiler.getUniqueID();

    // write the loop prologue
    o.o("for(int " + indexVar + " = 0; " + indexVar + " < _t_" +
        getTableName() + ".size(); " + indexVar + "++)");
    o.openBrace();

    // assign local variables named after the columns
    for(int i = 0; i < getNumColumns(); i++) {
      String columnName = columnNames[i];
      String columnType = MqlType.toString(columnTypes[i]);
      o.o(columnType + " " + columnName + " = _t_" + tableName + "[" + 
          indexVar + "]." + columnName + ";");
    }

    // remember that we're looping
    setStatus(MqlTable.LOOPING);
    setIndexVariable(indexVar);

    return indexVar;
  }

  public void closeLoop(MqlOutput o) 
    throws MqlException, IOException
  {
    // can anyone "piggyback" on this loop?
    writeDependents(o);

    o.closeBrace();
    setIndexVariable(null);
    setStatus(FINISHED);
  }

  public void registerDependent(MqlStatement s) 
  {
    dependents.add(s);
  }

  public void writeDependents(MqlOutput o) 
    throws MqlException, IOException
  {

    // First go for high-priority dependents
    boolean anotherPass;
    do {
      anotherPass = false;

      // look for high-priority dependents
      for(int i = 0; i < dependents.size(); i++) {
        MqlStatement s = (MqlStatement) dependents.get(i);
        if(s.readyToGenerate() && s.hasPriority()) {
          s.writeMain(o); 
          anotherPass = true;
        }
      }

      // otherwise, look for ordinary dependents
      if(!anotherPass) {
        for(int i = 0; i < dependents.size(); i++) {
          MqlStatement s = (MqlStatement) dependents.get(i);
          if(s.readyToGenerate()) {
            s.writeMain(o); 
            anotherPass = true;
          }
        }
      }
    } while(anotherPass);
  }

  public void setExport(String bankName) {
    exportBank = bankName; 
  }

  public boolean isExported() {
    return (exportBank != null);
  }

  public String getExportBankName() {
    return exportBank;
  }

  public boolean mustCreateTable() {
    // We don't need to actually create the table if it isn't exported and
    // there are no more dependent statements

    if(isExported()) {
      return true;
    }

    for(int i = 0; i < dependents.size(); i++) {
      MqlStatement s = (MqlStatement) dependents.get(i);
      if(s.getStatus() != FINISHED) {
        return true;
      }
    }
   
    return false; 
  }

  public int getStatus() 
  {
    return status;
  }

  public void setStatus(int statusIn)
  {
    status = statusIn;
  }

  public boolean hasPriority() 
  {
    return priority;
  }

  public void setPriority()
  {
    priority = true;
  }

  public void setIndexVariable(String indexVariableIn)
  {
    indexVariable = indexVariableIn;
  }

  public String getIndexVariable() 
  {
    return indexVariable;
  }

  private String tableName;
  private String structName;
  private String columnNames[];
  private String expressions[];
  private int columnTypes[];

  private Vector dependents = new Vector();
  private String exportBank;
  private String indexVariable;
  private int status = NOT_READY;
  private boolean priority = false;

  public static final int NOT_READY = 0;
  public static final int IN_PROGRESS = 1;
  public static final int FINISHED = 2; 
  public static final int LOOPING = 3; 

  public static MqlTable find(String name) {
    MqlTable result = (MqlTable) tables.get(name);
    if(result == null) {
       System.out.println("Failed to locate table " + name);
    }
    return result;
  }

  private static Hashtable tables = new Hashtable();
}
