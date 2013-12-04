public class MqlModuloExpression extends SimpleNode {
  public MqlModuloExpression(int id) {
    super(id);
  }

  public MqlModuloExpression(MqlParser p, int id) {
    super(p, id);
  }

  public String getColumnName() {
    MqlString s = (MqlString) jjtGetChild(0);
    return s.get();
  }

  public String getLowerLimit() {
    MqlNumericalExpression s = (MqlNumericalExpression) jjtGetChild(1);
    return s.get();
  }

  public String getUpperLimit() {
    MqlNumericalExpression s = (MqlNumericalExpression) jjtGetChild(2);
    return s.get();
  }

  public String getPeriod() {
    return getUpperLimit() + " - " + getLowerLimit();
  }
}
