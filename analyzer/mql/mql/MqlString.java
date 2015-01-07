public class MqlString extends SimpleNode {
  public MqlString(int id) {
    super(id);
  }

  public MqlString(MqlParser p, int id) {
    super(p, id);
  }

  public void set(String s1) {
    // replace "backslash quote" with just "quote"
    s = s1.replaceAll("\\\\\"", "\"");
  }

  public String get() {
    return s;
  }

  public String toString() {
    return get();
  }

  private String s;
}
