public class MqlHistogramSelector extends SimpleNode {
  public MqlHistogramSelector(int id) {
    super(id);
  }

  public MqlHistogramSelector(MqlParser p, int id) {
    super(p, id);
  }

  public String getExpression() {
    MqlExpression ex = (MqlExpression) jjtGetChild(0);
    return ex.get();
  }

  public String getLow() {
     MqlNumericalExpression in = (MqlNumericalExpression) jjtGetChild(1);
     return in.get(); 
  }

  public String getHigh() {
     MqlNumericalExpression in = (MqlNumericalExpression) jjtGetChild(2);
     return in.get(); 
  }

}
