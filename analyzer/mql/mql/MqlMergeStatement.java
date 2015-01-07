import java.io.*;
import java.util.*;

public class MqlMergeStatement extends MqlStatement 
{
  public MqlMergeStatement(int id) {
    super(id);
  }

  public MqlMergeStatement(MqlParser p, int id) {
    super(p, id);
  }

  public boolean readyToGenerate() {
    if(!super.readyToGenerate()) {
      return false;
    }

   int numInputTables = getNumTables() - 1;

    for(int i = 0; i < numInputTables; i++) {
      MqlTable inputTable = MqlTable.find(getTableName(i));
      if(inputTable.getStatus() != FINISHED) {
        return false;
      }
    }
    return true;
  }

  public void declareTables()
    throws MqlException
  {
    // Register input table 1 and define the output table
    super.declareTables();

    // Register dependence on other input tables
    int numInputTables = getNumTables() - 1;
    for(int i = 1; i < numInputTables; i++) {
      MqlTable inputTable = MqlTable.find(getTableName(i));
      inputTable.registerDependent(this);
    }
  }

  public String[] getDefaultColumns() {
    // construct the intersection of the columns in the two input
    // tables
    MqlTable inputTable1 = MqlTable.find(getTableName(0));
    int numInputTables = getNumTables() - 1;

    Vector v = new Vector();
    for(int i = 0; i < inputTable1.getNumColumns(); i++) {
      String columnName = inputTable1.getColumnName(i);
      boolean columnExistsEverywhere = true;
      for(int j = 1; j < numInputTables; j++) {
        MqlTable inputTable = MqlTable.find(getTableName(j));
        if(!inputTable.columnExists(columnName)) {
          columnExistsEverywhere = false;
        }
      }
      if(columnExistsEverywhere) {
        v.add(columnName);    
      }
    }

    String[] columns = new String[v.size()];
    for(int i = 0; i < v.size(); i++) {
      columns[i] = (String) v.get(i);
    }

    return columns;
  }

  public int[] getDefaultColumnTypes() {
    // the default columns must exist in both tables; take their types 
    // from table 1.
    MqlTable inputTable1 = MqlTable.find(getTableName(0));
    String[] outputColumns = getDefaultColumns();

    int[] types = new int[outputColumns.length];
    for(int i = 0; i < outputColumns.length; i++) {
      int columnID = inputTable1.getColumnID(outputColumns[i]);
      types[i] = inputTable1.getColumnType(i);
    }

    return types;
  }

  public String getInputTableName() {
     return getTableName(0);
  }

  public String getOutputTableName() {
    return getTableName(getNumTables() - 1);
  }

  public void writeDeclaration(MqlOutput o)
    throws IOException
  {
    // declare the output table
    String outputTableName = getOutputTableName();
    MqlTable outputTable = MqlTable.find(outputTableName);
    outputTable.outputStruct(o);

    // if applicable, write out a sort comparison function
    String sortExpression = getSortExpression();
    if(sortExpression != null) {
      comparName = MqlCompiler.getUniqueID();
      outputTable.outputCompare(o, sortExpression, comparName);
    }
  }

  public void writeInitialization(MqlOutput o)
    throws IOException
  {
    // Nothing to put here
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
    String outputTableName = getOutputTableName();
    MqlTable outputTable = MqlTable.find(outputTableName);

    // write code to deal with each of the input tables in turn.
    int numInputTables = getNumTables() - 1;
    for(int i = 0; i < numInputTables; i++) {
      MqlTable inputTable = MqlTable.find(getTableName(i));
      writeMainForOneTable(o, inputTable);
    }

    // If necessary, sort the table
    String sortExpression = getSortExpression();
    if(sortExpression != null) {
      o.o("_t_" + outputTableName + ".sort(" + comparName + ");");
    }

    if(enableExpression != null) {
      o.closeBrace();
    }
   
    outputTable.setStatus(MqlTable.FINISHED);
    setStatus(FINISHED); 
  }

  private void writeMainForOneTable(MqlOutput o, MqlTable inputTable) 
    throws IOException, MqlException
  {
    // find the output table
    String outputTableName = getOutputTableName();
    MqlTable outputTable = MqlTable.find(outputTableName);

    // write a loop over the input table
    String indexVar = inputTable.openLoop(o);

    // apply the "where" condition, if applicable
    String whereExpression = getWhereExpression();
    if(whereExpression != null) {
      o.o("if(" + whereExpression + ")");
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
      if(inputTable.columnExists(uniqueNames[i])) {
        uniqueNames[i] = uniqueNames[i] + "_0";
	if(inputTable.columnExists(uniqueNames[i])) {
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

    // Finally, if we need to, fill in the new table
    if(outputTable.mustCreateTable()) { 
      String outputIndex = MqlCompiler.getUniqueID();
      o.o("int " + outputIndex + " = _t_" + outputTableName + ".next();");
      for(int i = 0; i < outputTable.getNumColumns(); i++) {
        o.o("_t_" + outputTableName + "[" + outputIndex + "]." +
            outputTable.getColumnName(i) + " = " + uniqueNames[i] + ";");
      }
    }

    // Close any braces that we opened
    o.closeBrace();
    if(whereExpression != null) {
      o.closeBrace();
    }
    inputTable.closeLoop(o);
  }

  private String comparName;
}
