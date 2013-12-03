import java.io.*;

public class MqlHistograms1DStatement extends MqlHistogramsStatement {
  public MqlHistograms1DStatement(int id) {
    super(id);
  }

  public MqlHistograms1DStatement(MqlParser p, int id) {
    super(p, id);
  }

  public int numDimensions() {
    return 1;
  }
}
