import java.io.*;

public abstract class MqlStatement extends SimpleNode {

  public MqlStatement(int id) {
    super(id);
  }

  public MqlStatement(MqlParser p, int id) {
    super(p, id);
  }

  public String getTableName(int i) {

    int tablesFound = 0;

    for(int j = 0; j < jjtGetNumChildren(); j++) {
      if(jjtGetChild(j) instanceof MqlTableName) {
        if(tablesFound == i) {
          MqlTableName tn = (MqlTableName) jjtGetChild(j);
          return tn.get();
        }

        tablesFound++;
      }
    }
    return null;

  }

  public int getNumTables() {

    int tablesFound = 0;

    for(int j = 0; j < jjtGetNumChildren(); j++) {
      if(jjtGetChild(j) instanceof MqlTableName) {
        tablesFound++;
      }
    }
    return tablesFound;
  }

  public MqlAxis getAxis(int i) {
    int axesFound = 0;

    for(int j = 0; j < jjtGetNumChildren(); j++) {
      if(jjtGetChild(j) instanceof MqlAxis) {
        if(axesFound == i) {
          return (MqlAxis) jjtGetChild(j);
        }

        axesFound++;
      }
    }
    return null;
  }

  public MqlCoincExpression getCoincidence() {

    for(int j = 0; j < jjtGetNumChildren(); j++) {
      if(jjtGetChild(j) instanceof MqlCoincExpression) {
          return (MqlCoincExpression) jjtGetChild(j);
      }
    }
    return null;
  }

  public String getWhereExpression() {
    for(int i = 0; i < jjtGetNumChildren(); i++) {
      Node c = jjtGetChild(i);
      if(c instanceof MqlWhereExpression) {
        MqlWhereExpression we = (MqlWhereExpression) c;
        return we.get();
      }
    }
    
    return null;
  }

  public String getEnableExpression() {
    for(int i = 0; i < jjtGetNumChildren(); i++) {
      Node c = jjtGetChild(i);
      if(c instanceof MqlEnableExpression) {
        MqlEnableExpression ee = (MqlEnableExpression) c;
        return ee.get();
      }
    }
    
    return null;
  }

  public MqlModuloExpression getModuloExpression(String columnName) {
    for(int i = 0; i < jjtGetNumChildren(); i++) {
      Node c = jjtGetChild(i);
      if(c instanceof MqlModuloExpression) {
        MqlModuloExpression me = (MqlModuloExpression) c;
	if(me.getColumnName().equals(columnName)) {
          return me;
        }
      }
    }
    
    return null;
  }

  public String getExpression() {
    for(int i = 0; i < jjtGetNumChildren(); i++) {
      Node c = jjtGetChild(i);
      if(c instanceof MqlExpression) {
        MqlExpression e = (MqlExpression) c;
        return e.get();
      }
    }
    
    return null;
  }

  public String getSortExpression() {
    for(int i = 0; i < jjtGetNumChildren(); i++) {
      Node c = jjtGetChild(i);
      if(c instanceof MqlSortExpression) {
        MqlSortExpression se = (MqlSortExpression) c;
        return se.get();
      }
    }
    
    return null;
  }

  public String[] getDefaultColumns() 
  {
    return null;
  }

  public int[] getDefaultColumnTypes() 
  {
    return null;
  }

  public MqlHistogramSelector getSelector() {
    for(int i = 0; i < jjtGetNumChildren(); i++) {
      Node c = jjtGetChild(i);
      if(c instanceof MqlHistogramSelector) {
        return (MqlHistogramSelector) c;
      }
    }

    return null;
  }

  public MqlColumnList getColumnList() {
    for(int i = 0; i < jjtGetNumChildren(); i++) {
      Node c = jjtGetChild(i);
      if(c instanceof MqlColumnList) {
        return (MqlColumnList) c;
      }
    }

    return null;
  }

  public String getBankName() {
    for(int i = 0; i < jjtGetNumChildren(); i++) {

      Node c = jjtGetChild(i);
      if(c instanceof MqlBankName) {
        MqlBankName bn = (MqlBankName) c;
        return bn.get();
      }
    }
    
    return null;
  }

  public String getInputTableName() 
  {
    return null;
  }

  public String getOutputTableName() 
  {
    return null;
  }


  public String getHistName() {
    for(int i = 0; i < jjtGetNumChildren(); i++) {
      if(jjtGetChild(i) instanceof MqlHistName) {
        MqlHistName hn = (MqlHistName) jjtGetChild(i);
        return hn.get();
      }
    }

    return null;
  }

  public String getHistTitle() {
    for(int i = 0; i < jjtGetNumChildren(); i++) {
      if(jjtGetChild(i) instanceof MqlHistTitle) {
        MqlHistTitle ht = (MqlHistTitle) jjtGetChild(i);
        return ht.get();
      }
    }

    return getHistName();
  }

  public boolean readyToGenerate() {
    // 1. Don't generate the same statement twice.
    if(getStatus() == IN_PROGRESS || getStatus() == FINISHED) {
      return false;
    }

    // 2. Don't generate a statement if our input table isn't ready
    String inputTableName = getInputTableName();
    if(inputTableName != null) {
      MqlTable inputTable = MqlTable.find(getInputTableName());
      if(inputTable.getStatus() == NOT_READY) {
        return false;
      }
    }

    return true;
  }

  public boolean hasPriority() {
    String outputTableName = getOutputTableName();

    if(outputTableName == null) {
      return false;
    } else {
      MqlTable outputTable = MqlTable.find(getOutputTableName());
      return outputTable.hasPriority();
    }
  }

  public int getStatus() {
    return status;
  }

  public void setStatus(int statusIn) {
    status = statusIn;
  }

  public void declareTables()
    throws MqlException
  {
    // Register as a dependent of our input table
    String inputTableName = getInputTableName();
    if(inputTableName != null) {
      MqlTable inputTable = MqlTable.find(inputTableName);
      inputTable.registerDependent(this);
    }

    // Now generate our output table
    String tableName = getOutputTableName();

    if(tableName != null) {
      MqlColumnList columnList = getColumnList();

      String[] columnNames; 
      String[] expressions = null;
      int[] types = null;
      if(columnList != null) {
        columnNames = columnList.getColumnNames();
        expressions = columnList.getExpressions();
        types = columnList.getTypes();
      } else {
        columnNames = getDefaultColumns();
        types = getDefaultColumnTypes();
      }

      if(expressions == null) {
        expressions = columnNames;
      }

      MqlTable outputTable = new MqlTable(tableName, columnNames, 
        expressions, types);
    }
  }

  public void writeDeclaration(MqlOutput o)
    throws MqlException, IOException
  {
  }
  
  public void writeInitialization(MqlOutput o)
    throws MqlException, IOException
  { 
  }

  public void writePreMain(MqlOutput o)
    throws MqlException, IOException
  {
  } 

  public void writeMain(MqlOutput o)
    throws MqlException, IOException
  { 
  }

  public void writePostMain(MqlOutput o)
    throws MqlException, IOException
  {
  } 

  private boolean priority = false;
  private int status;

  public static final int NOT_READY = 0;
  public static final int IN_PROGRESS = 1;
  public static final int FINISHED = 2;

}
