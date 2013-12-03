import java.io.*;

public class MqlOutput
{
  public MqlOutput(String filename) 
    throws FileNotFoundException
  {
    FileOutputStream outputStream = new FileOutputStream(filename);
    outputFile = new PrintStream(outputStream); 
  }

  public void o(String s) {
    outputFile.println(indent + s);
  }

  public void o() {
    o("");
  }

  public void openBrace() {
    o("{");
    indent += "  ";
  }

  public void closeBrace() {
    closeBrace("");
  }

  public void closeBrace(String s) {
    indent = indent.substring(0, indent.length() - 2);
    o("}" + s);
  }

  private String indent = "";
  private PrintStream outputFile;

  public final String QUOTE = "\"";

}
