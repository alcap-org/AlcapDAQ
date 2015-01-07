public class MqlHistTitle extends SimpleNode {
  public MqlHistTitle(int id) {
    super(id);
  }

  public MqlHistTitle(MqlParser p, int id) {
    super(p, id);
  }

  public String get() {
    MqlString sl = (MqlString) jjtGetChild(0);
    return sl.get();
  }

}
