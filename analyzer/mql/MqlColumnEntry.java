public class MqlColumnEntry extends SimpleNode {
  public MqlColumnEntry(int id) {
    super(id);
  }

  public MqlColumnEntry(MqlParser p, int id) {
    super(p, id);
  }

  public int getType() {

    for(int j = 0; j < jjtGetNumChildren(); j++) {
      if(jjtGetChild(j) instanceof MqlType) {
        MqlType type = (MqlType) jjtGetChild(j);
        return type.get();
      }
    }
    return MqlType.DOUBLE;
  }

  public String getName() {

    for(int j = 0; j < jjtGetNumChildren(); j++) {
      if(jjtGetChild(j) instanceof MqlString) {
        MqlString s = (MqlString) jjtGetChild(j);
        return s.get();
      }
    }
    return null;
  }

  public String getExpression() 
  {
    for(int j = 0; j < jjtGetNumChildren(); j++) {
      if(jjtGetChild(j) instanceof MqlExpression) {
        MqlExpression exp = (MqlExpression) jjtGetChild(j);
        return exp.get();
      }
    }
    return null;
  }
}
