public class MqlHistName extends SimpleNode {
  public MqlHistName(int id) {
    super(id);
  }

  public MqlHistName(MqlParser p, int id) {
    super(p, id);
  }

  public String get() {
    MqlString sl = (MqlString) jjtGetChild(0);
    return sl.get();
  }

}
