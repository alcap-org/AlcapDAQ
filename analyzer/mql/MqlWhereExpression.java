public class MqlWhereExpression extends SimpleNode {
  public MqlWhereExpression(int id) {
    super(id);
  }

  public MqlWhereExpression(MqlParser p, int id) {
    super(p, id);
  }

  public String get() {
    MqlExpression exp = (MqlExpression) jjtGetChild(0);
    return exp.get();
  }

}
