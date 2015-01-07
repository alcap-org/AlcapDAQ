public class MqlBankName extends SimpleNode {
  public MqlBankName(int id) {
    super(id);
  }

  public MqlBankName(MqlParser p, int id) {
    super(p, id);
  }

  public String get() {
    MqlString sl = (MqlString) jjtGetChild(0);
    return sl.get();
  }
}
