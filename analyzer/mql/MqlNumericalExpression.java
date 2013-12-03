public class MqlNumericalExpression extends SimpleNode {
  public MqlNumericalExpression(int id) {
    super(id);
  }

  public MqlNumericalExpression(MqlParser p, int id) {
    super(p, id);
  }

  public String get() {
    MqlString s = (MqlString) jjtGetChild(0);
    return s.get();
  }
}
