public class MqlColumnList extends SimpleNode {
  public MqlColumnList(int id) {
    super(id);
  }

  public MqlColumnList(MqlParser p, int id) {
    super(p, id);
  }

  public int[] getTypes()
  {
    int numChildren = jjtGetNumChildren();
    int[] types = new int[numChildren];

    for(int i = 0; i < numChildren; i++) {
      MqlColumnEntry entry = (MqlColumnEntry) jjtGetChild(i);
      types[i] = entry.getType();
    }

    return types;
  }

  public String[] getColumnNames()
  {
    int numChildren = jjtGetNumChildren();
    String[] names = new String[numChildren];

    for(int i = 0; i < numChildren; i++) {
      MqlColumnEntry entry = (MqlColumnEntry) jjtGetChild(i);
      names[i] = entry.getName();
    }

    return names;
  }

  public String[] getExpressions() 
  {
    int numChildren = jjtGetNumChildren();
    String[] expressions = new String[numChildren];

    for(int i = 0; i < numChildren; i++) {
      MqlColumnEntry entry = (MqlColumnEntry) jjtGetChild(i);
      expressions[i] = entry.getExpression();
    }

    return expressions;
  }
}
