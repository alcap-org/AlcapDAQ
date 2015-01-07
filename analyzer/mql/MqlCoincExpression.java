public class MqlCoincExpression extends SimpleNode {
  public MqlCoincExpression(int id) {
    super(id);
  }

  public MqlCoincExpression(MqlParser p, int id) {
    super(p, id);
  }

  public String getColumn1() {
    MqlString s = (MqlString) jjtGetChild(0);
    return s.get();
  }

  public String getColumn2() {
    MqlString s = (MqlString) jjtGetChild(1);
    return s.get();
  }

  public String getLow() {
    MqlNumericalExpression fp = (MqlNumericalExpression) jjtGetChild(2);
    return fp.get();
  }

  public String getHigh() {
    MqlNumericalExpression fp = (MqlNumericalExpression) jjtGetChild(3);
    return fp.get();
  }
}
