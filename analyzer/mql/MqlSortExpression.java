public class MqlSortExpression extends SimpleNode {
  public MqlSortExpression(int id) {
    super(id);
  }

  public MqlSortExpression(MqlParser p, int id) {
    super(p, id);
  }

  public String get() {
    MqlExpression exp = (MqlExpression) jjtGetChild(0);
    return exp.get();
  }

}
