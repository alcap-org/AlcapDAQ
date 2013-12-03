import java.io.*;

public class MqlHistogram2DStatement extends MqlHistogramStatement {
  public MqlHistogram2DStatement(int id) {
    super(id);
  }

  public MqlHistogram2DStatement(MqlParser p, int id) {
    super(p, id);
  }

  public int numDimensions() {
    return 2;
  }
}
