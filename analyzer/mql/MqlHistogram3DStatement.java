import java.io.*;

public class MqlHistogram3DStatement extends MqlHistogramStatement {
  public MqlHistogram3DStatement(int id) {
    super(id);
  }

  public MqlHistogram3DStatement(MqlParser p, int id) {
    super(p, id);
  }

  public int numDimensions() {
    return 3;
  }
}
