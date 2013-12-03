public class MqlExpression extends SimpleNode {
  public MqlExpression(int id) {
    super(id);
  }

  public MqlExpression(MqlParser p, int id) {
    super(p, id);
  }

  public String get() {
    MqlString s = (MqlString) jjtGetChild(0);
    return s.get();
  }

}
