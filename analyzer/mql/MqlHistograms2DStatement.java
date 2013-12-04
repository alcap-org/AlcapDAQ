import java.io.*;

public class MqlHistograms2DStatement extends MqlHistogramsStatement {
  public MqlHistograms2DStatement(int id) {
    super(id);
  }

  public MqlHistograms2DStatement(MqlParser p, int id) {
    super(p, id);
  }

  public int numDimensions() {
    return 2;
  }
}
