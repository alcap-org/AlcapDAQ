import java.io.*;

abstract public class MqlHistogramStatement extends MqlStatement 
{

  public MqlHistogramStatement(int id) {
    super(id);
  }

  public MqlHistogramStatement(MqlParser p, int id) {
    super(p, id);
  }

  abstract public int numDimensions();

  public String getInputTableName() 
  {
     return getTableName(0);
  }

  public void writeDeclaration(MqlOutput o)
    throws IOException
  {
      o.o("static TH" + numDimensions() + "D *_h_" + getHistName() + ";");
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

    o.o("_h_" + getHistName() + " = new TH" + numDimensions() + "D(" + 
        o.QUOTE + getHistName() + o.QUOTE + ", " +
        o.QUOTE + getHistTitle() + o.QUOTE + ", ");

    for(int i = 0; i < numDimensions(); i++) {
      o.o("  " + getAxis(i).getNbins() + ", " + getAxis(i).getLow() + ", " + 
          getAxis(i).getHigh() + ((i == numDimensions() - 1) ? ");" : ", "));
    }

    for(int i = 0; i < numDimensions(); i++) {
      String axes[] = {"X", "Y", "Z"};
      o.o("_h_" + getHistName() + "->Get" + axes[i] + "axis()->SetTitle(" +
          o.QUOTE + getAxis(i).getTitle() + o.QUOTE + ");");
    }

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

    // apply the "enable" condition, if applicable
    String enableExpression = getEnableExpression();
    if(enableExpression != null) {
      o.o("if(" + enableExpression + ")");
      o.openBrace();
    }

    // apply the "where" condition, if applicable
    String whereExpression = getWhereExpression();
    if(whereExpression != null) {
      o.o("if(" + whereExpression + ")");
      o.openBrace();
    }

    // now fill the histogram
    String fillString = "_h_" + getHistName() + "->Fill(";
    for(int i = 0; i < numDimensions(); i++) {
      fillString += getAxis(i).getExpression() + 
                    ((i == numDimensions() - 1) ? ");" : ", ");
    }
    o.o(fillString);

    // Close any braces that we opened
    if(whereExpression != null) {
      o.closeBrace();
    }
    if(enableExpression != null) {
      o.closeBrace();
    }
    if(openedLoop) {
      table.closeLoop(o);
    }
   
    setStatus(FINISHED); 

  }
}
