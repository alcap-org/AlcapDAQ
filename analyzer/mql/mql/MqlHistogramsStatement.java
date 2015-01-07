import java.io.*;

abstract public class MqlHistogramsStatement extends MqlStatement 
{
  public MqlHistogramsStatement(int id) {
    super(id);
  }

  public MqlHistogramsStatement(MqlParser p, int id) {
    super(p, id);
  }

  abstract public int numDimensions();

  public String getLowHist() 
  {
    MqlHistogramSelector selector = getSelector();
    return selector.getLow();
  }

  public String getHighHist() 
  {
    MqlHistogramSelector selector = getSelector();
    return selector.getHigh();
  }

  public String getNumHists() 
  {
    return getHighHist() + "-" + getLowHist() + "+ 1";
  }

  public void writeDeclaration(MqlOutput o)
    throws IOException
  {
      o.o("static TH" + numDimensions() + "D *_h_" + getHistName() + 
          "[" + getNumHists() + "];");
  }

  public String getInputTableName()
  {
    return getTableName(0);
  }

  public void writeInitialization(MqlOutput o)
    throws IOException
  {
    // apply the "enable" condition, if applicable
    String enableExpression = getEnableExpression();
    if(enableExpression != null) {
      o.o("if(" + enableExpression + ")");
      o.openBrace();
    }

    o.o("for(int i = 0; i < " + getNumHists() + "; i++)");
    o.openBrace(); 
    o.o("char histName[256], histTitle[256];");
    o.o("sprintf(histName, " + o.QUOTE + getHistName() + "%d" + o.QUOTE + 
        ", i + " + getLowHist() + ");");
    o.o("sprintf(histTitle, " + o.QUOTE + getHistTitle() + " %d" + o.QUOTE + 
        ", i + " + getLowHist() + ");");
    o.o("_h_" + getHistName() + "[i] = new TH" + numDimensions() + 
        "D(histName, histTitle, ");
    for(int i = 0; i < numDimensions(); i++) {
      o.o("  " + getAxis(i).getNbins() + ", " + getAxis(i).getLow() + ", " + 
          getAxis(i).getHigh() + ((i == numDimensions() - 1) ? ");" : ", "));
    }

    for(int i = 0; i < numDimensions(); i++) {
      o.o("sprintf(histTitle, " + o.QUOTE + getAxis(i).getTitle() + " %d" + 
          o.QUOTE + ", i + " + getLowHist() + ");");

      String axes[] = {"X", "Y", "Z"};
      o.o("_h_" + getHistName() + "[i]->Get" + axes[i] +
          "axis()->SetTitle(histTitle);");
    }
    o.closeBrace();
    if(enableExpression != null) {
      o.closeBrace();
    }
  }

  public void writeMain(MqlOutput o)
    throws IOException, MqlException
  {
    // remember that we're working on this statement
    setStatus(IN_PROGRESS);

    String tableName = getTableName(0);
    MqlTable table = MqlTable.find(tableName);

    if(table == null) {
      throw new MqlException("Unable to locate table " + tableName +
                             " for histogram " + getHistName());
    }

    String enableExpression = getEnableExpression();
    if(enableExpression != null) {
      o.o("if(" + enableExpression + ")");
      o.openBrace();
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

    // now fill the histogram
    MqlHistogramSelector selector = getSelector();
    String selExpression = selector.getExpression() + " - " + getLowHist(); 
    String selVariable = MqlCompiler.getUniqueID();
    o.o("int " + selVariable + " = (int) (" + selExpression + ");");
    o.o("if(" + selVariable + " >= 0 && " + selVariable + " < (" + getNumHists() + "))");
    o.openBrace();
    String fillString = "_h_" + getHistName() + "[" + selVariable  + 
                        "]->Fill(";
    for(int i = 0; i < numDimensions(); i++) {
      fillString += getAxis(i).getExpression() + 
                    ((i == numDimensions() - 1) ? ");" : ", ");
    }
    o.o(fillString);

    // Close any braces that we opened
    o.closeBrace();
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
}
