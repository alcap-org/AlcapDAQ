import java.io.*;

public class MqlNtupleStatement extends MqlStatement 
{

  public MqlNtupleStatement(int id) {
    super(id);
  }

  public MqlNtupleStatement(MqlParser p, int id) {
    super(p, id);
  }

  public String getInputTableName() 
  {
     return getTableName(0);
  }

  public void writeDeclaration(MqlOutput o)
    throws IOException, MqlException
  {
      o.o("static TTree *_n_" + getHistName() + ";");

      sizeVarName = MqlCompiler.getUniqueID();
      capacityVarName = MqlCompiler.getUniqueID();

      o.o("int " + sizeVarName + " = 0;");
      o.o("int " + capacityVarName + " = 8;");

      MqlColumnList columnList = getColumnList();
      String[] columnNames = columnList.getColumnNames();
      int[] columnTypes =  columnList.getTypes();
      branchNames = new String[columnNames.length];
      columnArrayNames = new String[columnNames.length];

      for(int i = 0; i < columnNames.length; i++) {
        
        branchNames[i] = MqlCompiler.getUniqueID(); 
        columnArrayNames[i] = MqlCompiler.getUniqueID(); 

	o.o("TBranch *" + branchNames[i] + ";");
        switch(columnTypes[i]) {
          case MqlType.BOOLEAN: 
  	     o.o("MqlNtupleColumn<Bool_t> *" + columnArrayNames[i] + ";"); 
	     break;
          case MqlType.BYTE: 
  	     o.o("MqlNtupleColumn<Char_t> *" + columnArrayNames[i] + ";"); 
	     break;
          case MqlType.SHORT: 
  	     o.o("MqlNtupleColumn<Short_t> *" + columnArrayNames[i] + ";"); 
	     break;
          case MqlType.INTEGER: 
  	     o.o("MqlNtupleColumn<Int_t> *" + columnArrayNames[i] + ";"); 
	     break;
          case MqlType.FLOAT: 
  	     o.o("MqlNtupleColumn<Float_t> *" + columnArrayNames[i] + ";"); 
	     break;
          case MqlType.DOUBLE: 
  	     o.o("MqlNtupleColumn<Double_t> *" + columnArrayNames[i] + ";"); 
	     break;
          default: 
            throw new MqlException("Unsupported column type in ntuple " + 
	  			   getHistName());
        }
      }
  }

  public void writeInitialization(MqlOutput o)
    throws IOException, MqlException
  {

    o.o("_n_" + getHistName() + " = new TTree(" + 
        o.QUOTE + getHistName() + o.QUOTE + ", " +
        o.QUOTE + getHistTitle() + o.QUOTE + ", 2);");

    o.o("_n_" + getHistName() + "->Branch(" + o.QUOTE + "_n_" + o.QUOTE +
		    ", &" + sizeVarName + ", " + 
		    o.QUOTE + "_n_/I" + o.QUOTE + ");");


    MqlColumnList columnList = getColumnList();
    String[] columnNames = columnList.getColumnNames();
    int[] columnTypes =  columnList.getTypes();
    for(int i = 0; i < columnNames.length; i++) {
      String typeName, typeCode;
      switch(columnTypes[i]) {
        case MqlType.BOOLEAN: typeName = "Bool_t"; typeCode = "/O"; break;
	case MqlType.BYTE: typeName = "Char_t"; typeCode = "/B"; break;
        case MqlType.SHORT: typeName = "Short_t"; typeCode = "/S"; break;
        case MqlType.INTEGER: typeName = "Int_t"; typeCode = "/I"; break;
        case MqlType.FLOAT: typeName = "Float_t"; typeCode = "/F"; break;
        case MqlType.DOUBLE: typeName = "Double_t"; typeCode = "/D"; break;
        default: 
          throw new MqlException("Unsupported column type in ntuple " + 
				 getHistName());
      }

      o.o(columnArrayNames[i] + 
		      " = new MqlNtupleColumn<" + typeName + ">;");
      o.o(columnArrayNames[i] + "->realloc(" + capacityVarName + ");");
      o.o(branchNames[i] + " = " + "_n_" + getHistName() + "->Branch(" + 
		     o.QUOTE + columnNames[i]  + o.QUOTE + "," +
		     "&((*" + columnArrayNames[i] + ")[0]), " +
		     o.QUOTE + columnNames[i] + "[_n_]" + typeCode + o.QUOTE +
		     ");");
    } 
  }

  public void writePreMain(MqlOutput o)
    throws IOException, MqlException
  {
    o.o(sizeVarName + " = 0;");
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

    String tableName = getTableName(0);
    MqlTable table = MqlTable.find(tableName);
    
    if(table == null) {
      throw new MqlException("Unable to locate table " + tableName +
                             " for ntuple " + getHistName());
    }
  
    // if necessary, write a loop over the input table
    boolean openedLoop = false;
    String indexVar;
    if(table.getStatus() != MqlTable.IN_PROGRESS &&
       table.getStatus() != MqlTable.LOOPING) {
      indexVar = table.openLoop(o);
      openedLoop = true;
    } else {
      indexVar = table.getIndexVariable();
    }
  
    // apply the "where" condition, if applicable
    String whereExpression = getWhereExpression();
    if(whereExpression != null) {
      o.o("if(" + whereExpression + ")");
      o.openBrace();
    }

    MqlColumnList columnList = getColumnList();
    String[] columnNames = columnList.getColumnNames();
    String[] expressions = columnList.getExpressions();

    for(int i = 0; i < expressions.length; i++) {
      o.o( "(*" + columnArrayNames[i] + ") " + 
	  "[" + sizeVarName + "] = " + expressions[i] + ";");
    }
    o.o(sizeVarName + "++;");

    // Resize the column array if necessary
    o.o("if(" + sizeVarName + " >= " + capacityVarName + ")");
    o.o(capacityVarName + " *= 2;");
    o.openBrace();
    for(int i = 0; i < expressions.length; i++) {
      o.o(columnArrayNames[i] + "->realloc(" + capacityVarName + ");");
      o.o(branchNames[i] + "->SetAddress(&((*" + columnArrayNames[i] + ")[0]));");
    }
    o.closeBrace();

    // Close any braces that we opened
    if(whereExpression != null) {
      o.closeBrace();
    }
    if(openedLoop) {
      table.closeLoop(o);
    }
    if(enableExpression != null) {
      o.closeBrace();
    }

    setStatus(FINISHED);
  }

  public void writePostMain(MqlOutput o)
    throws IOException, MqlException
  {
    // now fill the ntuple
    o.o("_n_" + getHistName() + "->Fill();");
  }

  String[] branchNames;
  String[] columnArrayNames;
  String sizeVarName;
  String capacityVarName; 
}
