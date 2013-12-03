public class MqlTableName extends SimpleNode {
  public MqlTableName(int id) {
    super(id);
  }

  public MqlTableName(MqlParser p, int id) {
    super(p, id);
  }

  public String get() {
    MqlString sl = (MqlString) jjtGetChild(0);
    return sl.get();
  } 
}
