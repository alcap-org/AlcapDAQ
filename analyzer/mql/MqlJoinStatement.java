import java.io.*;
import java.util.*;

public class MqlJoinStatement extends MqlStatement 
{
  public MqlJoinStatement(int id) {
    super(id);
  }

  public MqlJoinStatement(MqlParser p, int id) {
    super(p, id);
  }

  public boolean readyToGenerate() {
    if(!super.readyToGenerate()) {
      return false;
    }

//    if(getSortExpression() != null) {
      MqlTable inputTable = MqlTable.find(getTableName(0));
      if(inputTable.getStatus() != FINISHED) {
        return false;
      }
//    }

    MqlTable inputTable2 = MqlTable.find(getTableName(1));
    if(inputTable2.getStatus() != FINISHED) {
      return false;
    }

    return true;
  }

  public void declareTables()
    throws MqlException
  {
    // Register input table 1 and define the output table
    super.declareTables();

    // Mark input table 2 for high-priority creation
    MqlTable inputTable2 = MqlTable.find(getTableName(1));
    inputTable2.registerDependent(this);
    inputTable2.setPriority();
  }

  public void writeDeclaration(MqlOutput o)
    throws IOException
  {
    // declare the output table
    MqlTable outputTable = MqlTable.find(getOutputTableName());
    outputTable.outputStruct(o);

    //
    // declare a comparison function for each of the tables to check
    // whether or not they are sorted properly.
    //
    MqlCoincExpression coinc = getCoincidence();
    if(coinc != null) {
      comparName1 = MqlCompiler.getUniqueID();
      comparName2 = MqlCompiler.getUniqueID();
      outputCompare(o, 1, comparName1);
      outputCompare(o, 2, comparName2);
    }

    // if applicable, write out a sort comparison function for the 
    // output table (in case of an "order by" clause).
    String sortExpression = getSortExpression();
    if(sortExpression != null) {
      outputComparName = MqlCompiler.getUniqueID();
      outputTable.outputCompare(o, sortExpression, outputComparName);
    }
  }

  private void outputCompare(MqlOutput o, int oneOrTwo, String compareName) {

    MqlCoincExpression coinc = getCoincidence();

    String columnName = null;
    if(oneOrTwo == 1) {
      columnName = coinc.getColumn1();
    } else if(oneOrTwo == 2) {
      columnName = coinc.getColumn2();
    }

    MqlTable table = MqlTable.find(getTableName(oneOrTwo-1));

    if(!table.columnExists(columnName)) {
      if(columnName.endsWith("_" + oneOrTwo)) {
         String newColumnName = columnName.substring(0, columnName.length()-2);
          if(table.columnExists(newColumnName)) {
            columnName = newColumnName;
          }
        } 

	String tableName = table.getTableName();
        if(columnName.startsWith(tableName + "_")) {
          String newColumnName = columnName.substring(tableName.length()+1);
          if(table.columnExists(newColumnName)) {
            columnName = newColumnName;
          }
        } 
    }

    table.outputCompare(o, columnName, compareName);

  }

  public void writeInitialization(MqlOutput o)
    throws IOException
  {
  }

  public String getInputTableName() 
  {
     return getTableName(0);
  }

  public String getOutputTableName()
  {
    return getTableName(2);
  }

  public boolean hasUniqueOption()
  {
    for(int j = 0; j < jjtGetNumChildren(); j++) {
      if(jjtGetChild(j) instanceof MqlUniqueOption) {
        return true;
      }
    }

    return false;
  }

  public boolean hasInvertOption()
  {
    for(int j = 0; j < jjtGetNumChildren(); j++) {
      if(jjtGetChild(j) instanceof MqlInvertOption) {
        return true;
      }
    }

    return false;
  }

  public String[] getDefaultColumns()
  {
    // our default column list is the union of the column lists 
    // from the two input tables, with conflicts resolved appropriately

    Vector v = new Vector();
     
    MqlTable table1 = MqlTable.find(getTableName(0));
    MqlTable table2 = MqlTable.find(getTableName(1));

    addColumns(table1, table2, 1, v);
    addColumns(table2, table1, 2, v);

    String[] columns = new String[v.size()];
    for(int i = 0; i < v.size(); i++) {
      columns[i] = (String) v.get(i);
    }

    return columns;
  }

  public int[] getDefaultColumnTypes()
  {
    // our default column list is the union of the column lists 
    // from the two input tables, with conflicts resolved appropriately

    MqlTable table1 = MqlTable.find(getTableName(0));
    MqlTable table2 = MqlTable.find(getTableName(1));

    int types[] = new int[table1.getNumColumns() + table2.getNumColumns()];

    for(int i = 0; i < table1.getNumColumns(); i++) {
      types[i] = table1.getColumnType(i);
    }

    for(int i = 0; i < table2.getNumColumns(); i++) {
      types[i + table1.getNumColumns()] = table2.getColumnType(i);
    }

    return types;
  }

  public void addColumns(MqlTable table1, MqlTable table2, int suffix, Vector v)
  {
    String tableName = table1.getTableName();

    for(int i = 0; i < table1.getNumColumns(); i++) {
      String columnName = table1.getColumnName(i);

      // does the other table have a column by this name?
      String outColumnName = columnName;
      if(table2.columnExists(columnName)) {
        if(table1 == table2) {
          outColumnName = columnName + "_" + suffix;
        } else {
          outColumnName = tableName + "_" + columnName;
        }
      }

      v.add(outColumnName);
    }
  }

  public void writePreMain(MqlOutput o)
  {
    // If we have a coincidence condition, we will need a variable to 
    // keep track of the beginning of the region of interest.
    MqlCoincExpression coinc = getCoincidence();
    if(coinc != null) {
      firstMatchVar = MqlCompiler.getUniqueID();
      prevCoinc1Var = MqlCompiler.getUniqueID();
      table2SortDoneVar = MqlCompiler.getUniqueID();
      table1WarningIssuedVar = MqlCompiler.getUniqueID();
      o.o("int " + firstMatchVar + " = 0;");
      o.o("double " + prevCoinc1Var + " = 0;");
      o.o("bool " + table2SortDoneVar + " = false;");
      o.o("bool " + table1WarningIssuedVar + " = false;");
    }
    if(hasInvertOption()) {
      matchCounterVar = MqlCompiler.getUniqueID();
      o.o("int " + matchCounterVar + " = 0;");
    }
  }

  private boolean isNameInList(String[] list, String name) {
    for(int i = 0; i < list.length; i++) {
      if(name.equals(list[i])) {
        return true;
      }
    }
    return false;
  }

  public void writeMain(MqlOutput o)
    throws IOException, MqlException
  {
    // remember that we're working on this statement
    setStatus(IN_PROGRESS);

    // apply the "enable" condition, if applicable
    String enableExpression = getEnableExpression();
    if(enableExpression != null) {
      o.o("if(" + enableExpression + ")");
      o.openBrace();
    }

    // look up the tables that are involved
    String tableName1 = getTableName(0);
    String tableName2 = getTableName(1);
    String outputTableName = getOutputTableName();
    MqlTable table1 = MqlTable.find(tableName1);
    MqlTable table2 = MqlTable.find(tableName2);
    MqlTable outputTable = MqlTable.find(outputTableName);

    // if necessary, write a loop over the first input table
    boolean openedLoop = false; 
    String indexVar1 = null;
    if(table1.getStatus() == MqlTable.FINISHED) {
      indexVar1 = table1.openLoop(o);
      openedLoop = true;
    } else if(table1.getStatus() == MqlTable.LOOPING) {
      indexVar1 = table1.getIndexVariable();
    }  // if IN_PROGRESS, there may be no index variable.

    // We need our own scope, since we want to rename the columns of the
    // two input tables into our own local variables.  This protects us from
    // namespace collisions, since the innermost scope is taken.
    o.openBrace(); 

    // If there is already a loop open, begin by assigning a unique name 
    // to each of the output columns to avoid statements like 
    // "int i = i".  Hint: it doesn't do what you hope it might.
    String[] uniqueColumnNames = null;
    String[] outColumnNames = getDefaultColumns();
    int[] outColumnTypes = getDefaultColumnTypes();
    if(table1.getStatus() == MqlTable.IN_PROGRESS) {
      uniqueColumnNames = new String[table1.getNumColumns()];
      for(int i = 0; i < table1.getNumColumns(); i++) {
        String columnName = table1.getColumnName(i);
        String outColumnType = MqlType.toString(outColumnTypes[i]);
        uniqueColumnNames[i] = MqlCompiler.getUniqueID();
        o.o(outColumnType + " " + uniqueColumnNames[i] + " = " + 
          columnName + ";");
      }
    }
    o.o();

    // now assign a local variable for each of the columns in the first table
    for(int i = 0; i < table1.getNumColumns(); i++) {
      String columnName = table1.getColumnName(i);
      String outColumnName = outColumnNames[i];
      String outColumnType = MqlType.toString(outColumnTypes[i]);
      if(table1.getStatus() == MqlTable.IN_PROGRESS) {
        o.o(outColumnType + " " + outColumnName + " = " + uniqueColumnNames[i]
             + ";");
      } else {
        o.o(outColumnType + " " + outColumnName + " = _t_" + 
            tableName1 + "[" + indexVar1  + "]." + columnName + ";");
      }
    }
    o.o();

    // check that this entry in table 1 is in the proper sequence and 
    // not a NaN.
    MqlCoincExpression coinc = getCoincidence();
    if(coinc != null) {
      o.o("if(!" + table2SortDoneVar +")");
      o.openBrace();
      o.o("if(!_t_" + tableName2 + ".checkSort(" + comparName2 + "))");
      o.openBrace();
      o.o("printf(\"Warning: table " + tableName2 + " is not sorted on " + coinc.getColumn2() + " as required for join with " + tableName1 + " (" + comparName2 + ")\\n\");");
      o.closeBrace();
      o.o(table2SortDoneVar + " = true;");
      o.closeBrace();
      o.o("else if(!" + table1WarningIssuedVar + ")");
      o.openBrace();
      o.o("if(!(" + coinc.getColumn1() + " >= " + prevCoinc1Var + "))");
      o.openBrace();
      o.o("printf(\"Warning: table " + tableName1 + " is not sorted on " + coinc.getColumn1() + " as required for join with " + tableName2 + "\\n\");");
      o.o(table1WarningIssuedVar + " = true;");
      o.closeBrace();
      o.closeBrace();
      o.o(prevCoinc1Var + " = " + coinc.getColumn1() + ";");
    }

    // If we have a coincidence condition, then emit code to search for
    // the first matching entry.  Because both tables are sorted in time 
    // order, we can take the first match from the previous iteration as
    // a first guess.  That is, we only have to search forward from there.
    String coincVar = MqlCompiler.getUniqueID();
    if(coinc != null) {
      String indexVarCoinc = MqlCompiler.getUniqueID();
      o.o("for( ; " + firstMatchVar + " < _t_" + tableName2 + ".size(); " + 
          firstMatchVar + "++)");
      o.openBrace();

      // assign a local variable for each of the columns in the second table
      for(int i = 0; i < table2.getNumColumns(); i++) {
        String columnName = table2.getColumnName(i);
        String outColumnName = outColumnNames[i + table1.getNumColumns()];
        String outColumnType = 
          MqlType.toString(outColumnTypes[i + table1.getNumColumns()]);
        o.o(outColumnType + " " + outColumnName + " = _t_" + 
            table2.getTableName() + "[" + firstMatchVar + "]." + 
            columnName + ";");
      }
      o.o();

      o.o("double " + coincVar + " = " + coinc.getColumn1() + " - " + 
                              coinc.getColumn2() + ";");
      o.o("if(" + coincVar + " < " + coinc.getHigh() + ")");
      o.openBrace(); 
      o.o("break;");
      o.closeBrace(); // if() for coincidence 

      o.closeBrace(); // inner for() loop
    } else {
      o.o("int " + firstMatchVar + " = 0;");
    }
    
    if(hasInvertOption()) {
      o.o(matchCounterVar + " = 0;");
    }

    // Now open the "real" second loop.
    String indexVar2 = MqlCompiler.getUniqueID();
    o.o("for(int " + indexVar2 + " = " + firstMatchVar + "; " + indexVar2 +
        " < _t_" + tableName2 + ".size(); " + indexVar2 + "++)");
    o.openBrace();

    // assign a local variable for each of the columns in the second table
    for(int i = 0; i < table2.getNumColumns(); i++) {
      String columnName = table2.getColumnName(i);
      String outColumnName = outColumnNames[i + table1.getNumColumns()];
      String outColumnType = 
        MqlType.toString(outColumnTypes[i + table1.getNumColumns()]);
      o.o(outColumnType + " " + outColumnName + " = _t_" + 
          table2.getTableName() + "[" + indexVar2 + "]." + columnName + ";");
    }
    o.o();

    // apply a coincidence condition, if applicable
    if(coinc != null) {
      o.o("double " + coincVar + " = " + coinc.getColumn1() + " - " + 
                              coinc.getColumn2() + ";");
      o.o("if(" + coincVar + " < " +  coinc.getLow() + ")");
      o.openBrace();
      o.o("break;");
      o.closeBrace();
    }
 
    // apply a "where" condition, if applicable
    String whereExpression = getWhereExpression();
    if(whereExpression != null) {
      o.o("if(" + whereExpression + ")");
      o.openBrace();
    }

    // if the logic of the join is inverted, then just increment a count
    // of the number of matches, and fill the output table if that count
    // is zero.
    if(hasInvertOption()) {
      o.o(matchCounterVar + "++;");
      if(whereExpression != null) {
        o.closeBrace(); // if() for where expression
      }
      o.closeBrace(); // second inner for() loop
      o.o();
      o.o("if(" + matchCounterVar + " == 0)");
      o.openBrace();
    }

    // OK...we have reached the point that corresponds to a row in the 
    // output table, so this is the chance to insert any statements that
    // depend on the output table.
    o.openBrace();
    String uniqueNames[] = new String[outputTable.getNumColumns()];
    int[] outputColumnTypes = outputTable.getColumnTypes();

    for(int i = 0; i < outputTable.getNumColumns(); i++) {

      // find a unique temporary name for the variable in the output table; 
      // try to make it the same as the final name, or with "_0" added,
      // if possible.
      uniqueNames[i] = outputTable.getColumnName(i);
      if(isNameInList(outColumnNames, uniqueNames[i])) {
        uniqueNames[i] = uniqueNames[i] + "_0";
        if(isNameInList(outColumnNames, uniqueNames[i])) {
          uniqueNames[i] = MqlCompiler.getUniqueID();
        }
      }

      o.o(MqlType.toString(outputColumnTypes[i]) + " " + uniqueNames[i] + 
            " = " + outputTable.getExpression(i) + ";");
    }
    for(int i = 0; i < outputTable.getNumColumns(); i++) {
      if(!uniqueNames[i].equals(outputTable.getColumnName(i))) {
        o.o(MqlType.toString(outputColumnTypes[i]) + " " + 
          outputTable.getColumnName(i) + " = " + uniqueNames[i] + ";");
      }
    }

    String sortExpression = getSortExpression();
    if(sortExpression == null) {
      outputTable.setStatus(MqlTable.IN_PROGRESS);
      outputTable.writeDependents(o);
    }

    // Finally, if we need to, fill in the new table
    if(outputTable.mustCreateTable()) {
      String outputIndex = MqlCompiler.getUniqueID();
      o.o("int " + outputIndex + " = _t_" + outputTableName + ".next();");
      for(int i = 0; i < outputTable.getNumColumns(); i++) {
        o.o("_t_" + outputTableName + "[" + outputIndex + "]." +
            outputTable.getColumnName(i) + " = " + uniqueNames[i] + ";");
      }
      outputTable.setStatus(MqlTable.FINISHED);

    }

    // Impose a uniqueness condition at this point.
    if(hasUniqueOption()) {
      o.o("break;");
    } 

    // Close any braces that we opened
    o.closeBrace(); // scope for filling output table
    if(whereExpression != null && !hasInvertOption()) {
      o.closeBrace(); // if() for where expression
    }
    o.closeBrace(); // second inner for() loop OR inversion if()
    o.closeBrace(); // scope for input variable declarations
    if(openedLoop) {
      table1.closeLoop(o); // outer for() loop
    }
    if(enableExpression != null) {
      o.closeBrace();
    }

    // If necessary, sort the table
    if(sortExpression != null) {
      o.o("_t_" + outputTableName + ".sort(" + outputComparName + ");");
    }

    setStatus(FINISHED);
  }

  String firstMatchVar;
  String matchCounterVar;
  String prevCoinc1Var;
  String table2SortDoneVar;
  String table1WarningIssuedVar;
  String comparName1;
  String comparName2;
  String outputComparName;
}
