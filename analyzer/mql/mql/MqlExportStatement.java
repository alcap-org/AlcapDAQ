import java.io.*;

public class MqlExportStatement extends MqlStatement {
  public MqlExportStatement(int id) {
    super(id);
  }

  public MqlExportStatement(MqlParser p, int id) {
    super(p, id);
  }

  public void writeMain(MqlOutput o)
    throws IOException, MqlException
  {
    // write the header file
    MqlOutput header = new MqlOutput(getBankName() + ".h");
    MqlTable table = MqlTable.find(getTableName(0));
    table.outputColumns(header);

    setStatus(FINISHED);
  }

  public void declareTables()
    throws MqlException
  {   
    // Mark the table as being exported; it will happen as part of the
    // creation of the table.
    MqlTable table = MqlTable.find(getTableName(0));
    table.setExport(getBankName()); 
  }
}
