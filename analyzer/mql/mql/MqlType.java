import java.util.*;

public class MqlType extends SimpleNode {
  public MqlType(int id) {
    super(id);
  }

  public MqlType(MqlParser p, int id) {
    super(p, id);
  }

  public void set(int typeIn) {
    type = typeIn;

    // If it's a structure, give it a number and put the number in 
    // a look-up table.
    if(type == STRUCT) {
      String structName = getStructName();
      Integer structIDInteger = (Integer) structNameToID.get(structName);
      if(structIDInteger == null) {
        int structID = nextStructID++;
        structIDInteger = new Integer(structID);
        structNameToID.put(structName, structIDInteger);
	structIDToName.ensureCapacity(structID+1);
	structIDToName.setSize(structID+1);
        structIDToName.set(structID, structName);
      } 
      type = structIDInteger.intValue();
    }
  }

  public String getStructName() {
    Node nameNode = jjtGetChild(0);
    if(type != STRUCT || nameNode == null) {
      return null;
    }
    return jjtGetChild(0).toString().intern();
  }

  public int get() {
    return type;
  }

  public static String toString(int i) {
    if(i > STRUCT && i < nextStructID) {
      return "struct " + structIDToName.get(i);
    } else {
      switch(i) {
        case BOOLEAN: return "bool";
        case BYTE: return "signed char";
        case SHORT: return "short";
        case INTEGER: return "int";
        case FLOAT: return "float";
        case DOUBLE: return "double";
        default: return "unknown";
      }
    }
  }

  public String toString() {
    return toString(type);
  }

  private int type;

  public static final int BOOLEAN = 0;
  public static final int BYTE = 1;
  public static final int SHORT = 2;
  public static final int INTEGER = 3;
  public static final int FLOAT = 4;
  public static final int DOUBLE = 5;
  public static final int STRUCT = 6;

  private static HashMap structNameToID = new HashMap();
  private static Vector structIDToName = new Vector();
  private static int nextStructID = STRUCT + 1;
}
