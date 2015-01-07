public class MqlEnableExpression extends SimpleNode {
  public MqlEnableExpression(int id) {
    super(id);
  }

  public MqlEnableExpression(MqlParser p, int id) {
    super(p, id);
  }

  public String get() {
    MqlExpression exp = (MqlExpression) jjtGetChild(0);
    return exp.get();
  }
}
