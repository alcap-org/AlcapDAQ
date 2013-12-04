import java.io.*;

public class MqlHistograms3DStatement extends MqlHistogramsStatement {
  public MqlHistograms3DStatement(int id) {
    super(id);
  }

  public MqlHistograms3DStatement(MqlParser p, int id) {
    super(p, id);
  }

  public int numDimensions() {
    return 3;
  }
}
