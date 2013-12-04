import java.io.*;

public class MqlHistogram1DStatement extends MqlHistogramStatement {
  public MqlHistogram1DStatement(int id) {
    super(id);
  }

  public MqlHistogram1DStatement(MqlParser p, int id) {
    super(p, id);
  }

  public int numDimensions() {
    return 1;
  }
}
