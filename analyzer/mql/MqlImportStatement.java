import java.io.*;

public class MqlImportStatement extends MqlStatement 
{
  public MqlImportStatement(int id) {
    super(id);
  }

  public MqlImportStatement(MqlParser p, int id) {
    super(p, id);
  }

  public String getOutputTableName()
  {
    return getTableName(0);
  }

  public void writeDeclaration(MqlOutput o)
    throws IOException
  {
    // include declaration of the output table structure
    String outputTableName = getOutputTableName();
    MqlTable outputTable = MqlTable.find(outputTableName);
    String structName = outputTable.getStructName();
    o.o("struct " + structName);
    o.openBrace();
    o.o("#include " + o.QUOTE + getBankName() + ".h" + o.QUOTE);
    o.closeBrace(";");
    o.o("static MqlArray<struct " + structName + "> _t_" + outputTableName + ";");
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

    // look up the table we're importing
    String outputTableName = getOutputTableName();
    MqlTable outputTable = MqlTable.find(getOutputTableName());

    // Write some code to find the associated bank
    o.o("_t_" + outputTableName + ".fromBank(pevent, " + 
        o.QUOTE + getBankName() + o.QUOTE + ");");
    o.o();

    outputTable.setStatus(MqlTable.FINISHED);

    setStatus(FINISHED);
  }

}
