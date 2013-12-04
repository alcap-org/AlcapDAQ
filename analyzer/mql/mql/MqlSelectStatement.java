import java.io.*;

public class MqlSelectStatement extends MqlStatement 
{
  public MqlSelectStatement(int id) {
    super(id);
  }

  public MqlSelectStatement(MqlParser p, int id) {
    super(p, id);
  }

  public boolean readyToGenerate() {
    if(!super.readyToGenerate()) {
      return false;
    }

    if(getSortExpression() != null) {
      MqlTable inputTable = MqlTable.find(getTableName(0));
      if(inputTable.getStatus() != FINISHED) {
        return false;
      }
    }

    return true;
  }

  public String[] getDefaultColumns() {
    MqlTable inputTable = MqlTable.find(getTableName(0));
    return inputTable.getColumnNames();
  }

  public int[] getDefaultColumnTypes() {
    MqlTable inputTable = MqlTable.find(getTableName(0));
    return inputTable.getColumnTypes();
  }

  public String getInputTableName() {
     return getTableName(0);
  }

  public String getOutputTableName() {
     return getTableName(1);
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
    String inputTableName = getTableName(0);
    String outputTableName = getOutputTableName();
    MqlTable inputTable = MqlTable.find(inputTableName);
    MqlTable outputTable = MqlTable.find(outputTableName);

    // if necessary, write a loop over the input table
    boolean openedLoop = false; 
    String indexVar;
    if(inputTable.getStatus() != MqlTable.IN_PROGRESS && 
       inputTable.getStatus() != MqlTable.LOOPING) {
      indexVar = inputTable.openLoop(o);
      openedLoop = true;
    } else {
      indexVar = inputTable.getIndexVariable();
    }

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

    // Close any braces that we opened
    o.closeBrace();
    if(whereExpression != null) {
      o.closeBrace();
    }
    if(openedLoop) {
      inputTable.closeLoop(o);
    }
    if(enableExpression != null) {
      o.closeBrace();
    }

    // If necessary, sort the table
    if(sortExpression != null) {
      o.o("_t_" + outputTableName + ".sort(" + comparName + ");");
    }
   
    setStatus(FINISHED); 
  }

  private String comparName;
}
