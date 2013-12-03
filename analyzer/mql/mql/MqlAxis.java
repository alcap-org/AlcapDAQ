public class MqlAxis extends SimpleNode {
  public MqlAxis(int id) {
    super(id);
  }

  public MqlAxis(MqlParser p, int id) {
    super(p, id);
  }

  public String getExpression() {
    MqlExpression exp = (MqlExpression) jjtGetChild(0);
    return exp.get();
  }

  public String getNbins() {
    MqlNumericalExpression in = (MqlNumericalExpression) jjtGetChild(1);
    return in.get();
  }

  public String getLow() {
    MqlNumericalExpression fp = (MqlNumericalExpression) jjtGetChild(2);
    return fp.get();
  }

  public String getHigh() {
    MqlNumericalExpression fp = (MqlNumericalExpression) jjtGetChild(3);
    return fp.get();
  }

  public String getTitle() {
    if(jjtGetNumChildren() >= 5) {
      MqlHistTitle title = (MqlHistTitle) jjtGetChild(4);
      return title.get();
    } else {
      return getExpression(); 
    }
  }
}
