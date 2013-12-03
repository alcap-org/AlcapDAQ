public class MqlModuleDeclaration extends SimpleNode {
  public MqlModuleDeclaration(int id) {
    super(id);
  }

  public MqlModuleDeclaration(MqlParser p, int id) {
    super(p, id);
  }

  public String get() {
    MqlString s = (MqlString) jjtGetChild(0);
    return s.get();
  }

}
