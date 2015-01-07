import java.io.*;

public class MqlLiteralStatement extends MqlStatement 
{
  public MqlLiteralStatement(int id) {
    super(id);
  }

  public MqlLiteralStatement(MqlParser p, int id) {
    super(p, id);
  }

  public void writeDeclaration(MqlOutput o)
    throws IOException
  {
    o.o(getExpression());
  }

  public void writeInitialization(MqlOutput o)
    throws IOException
  {
    // Nothing to put here
  }

  public void writeMain(MqlOutput o)
    throws IOException, MqlException
  {
    setStatus(FINISHED);
  }
}
