import java.io.*;

public class MqlNtupleImportStatement extends MqlStatement 
{

  public MqlNtupleImportStatement(int id) {
    super(id);
  }

  public MqlNtupleImportStatement(MqlParser p, int id) {
    super(p, id);
  }

  public String getTreeName() 
  {
     return getTableName(0);
  }

  public String getOutputTableName() 
  {
     return getTableName(1);
  }

  public void writeDeclaration(MqlOutput o)
    throws IOException, MqlException
  {
      o.o("TTree *_n_" + getTreeName() + ";");

      sizeVarName = MqlCompiler.getUniqueID();
      o.o("int " + sizeVarName + " = 0;");

      MqlColumnList columnList = getColumnList();
      String[] columnNames = columnList.getColumnNames();
      int[] columnTypes =  columnList.getTypes();
      columnArrayNames = new String[columnNames.length];

      for(int i = 0; i < columnNames.length; i++) {
        columnArrayNames[i] = MqlCompiler.getUniqueID(); 
        o.o("MqlNtupleColumn<" + MqlType.toString(columnTypes[i]) + "> *" + 
          columnArrayNames[i] + ";");
      }

      // declare the output table
      String outputTableName = getOutputTableName();
      MqlTable outputTable = MqlTable.find(outputTableName);
      outputTable.outputStruct(o);
  }

  public void writeInitialization(MqlOutput o)
    throws IOException, MqlException
  {

    o.o("_n_" + getTreeName() + " = MqlRegisterTree(" + 
         o.QUOTE + getTreeName() + o.QUOTE + ");");
    String maxSizeVar = MqlCompiler.getUniqueID();
    o.o("int " + maxSizeVar + " = _n_" + getTreeName() + 
        "->GetBranch(" + o.QUOTE + "_n_" + o.QUOTE +
        ")->GetLeaf(" + o.QUOTE + "_n_" + o.QUOTE + ")->GetMaximum();");

    o.o("_n_" + getTreeName() + "->SetBranchStatus(" + 
        o.QUOTE + "*" + o.QUOTE + ", 0);");
    o.o("_n_" + getTreeName() + "->SetBranchStatus(" + 
        o.QUOTE + "_n_" + o.QUOTE + ", 1);");
    o.o("_n_" + getTreeName() + "->SetBranchAddress(" + 
        o.QUOTE + "_n_" + o.QUOTE + ", &" + sizeVarName + ");");

    MqlColumnList columnList = getColumnList();
    String[] columnNames = columnList.getColumnNames();
    int[] columnTypes =  columnList.getTypes();
    for(int i = 0; i < columnNames.length; i++) {
      o.o(columnArrayNames[i] + " = new MqlNtupleColumn<" + 
          MqlType.toString(columnTypes[i]) + ">;");
      o.o(columnArrayNames[i] + "->realloc(" + maxSizeVar + ");");
      o.o("_n_" + getTreeName() + "->SetBranchStatus(" + 
	     o.QUOTE + columnNames[i]  + o.QUOTE + ", 1);");
      o.o("_n_" + getTreeName() + "->SetBranchAddress(" + 
	     o.QUOTE + columnNames[i]  + o.QUOTE + "," +
	     "&((*" + columnArrayNames[i] + ")[0]));");
    } 
  }

  public void writeMain(MqlOutput o)
    throws IOException, MqlException
  {
    // remember that we're working on this statement
    setStatus(IN_PROGRESS);

    String outputTableName = getOutputTableName();
    MqlTable outputTable = MqlTable.find(outputTableName);
 
    MqlColumnList columnList = getColumnList();
    String[] columnNames = columnList.getColumnNames();
    int[] columnTypes = columnList.getTypes();

    // go through the data one row at a time
    String indexVar = MqlCompiler.getUniqueID();
    o.o("for(int " + indexVar + " = 0; " + indexVar + " < " + sizeVarName + 
		    "; " + indexVar + "++)");
    o.openBrace();
    for(int i = 0; i < columnNames.length; i++) {
      o.o(MqlType.toString(columnTypes[i]) + " "  + columnNames[i] + " = " + 
  	  "(*" + columnArrayNames[i] + ")[" + indexVar + "];");
    }

    // write any statements that can be inserted directly at this point
    outputTable.setStatus(MqlTable.IN_PROGRESS);
    outputTable.writeDependents(o);

    // Finally, if we need to, fill in the new table
    if(outputTable.mustCreateTable()) { 
      String outputIndex = MqlCompiler.getUniqueID();
      o.o("int " + outputIndex + " = _t_" + outputTableName + ".next();");
      for(int i = 0; i < outputTable.getNumColumns(); i++) {
        o.o("_t_" + outputTableName + "[" + outputIndex + "]." +
            outputTable.getColumnName(i) + " = " + columnNames[i] + ";");
      }
      outputTable.setStatus(MqlTable.FINISHED);
    }

    o.closeBrace();

    setStatus(FINISHED);
  }

  String[] columnArrayNames;
  String sizeVarName;
}
