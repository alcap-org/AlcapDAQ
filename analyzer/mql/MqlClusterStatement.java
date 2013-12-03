import java.io.*;
import java.util.*;

public class MqlClusterStatement extends MqlStatement 
{
  public MqlClusterStatement(int id) {
    super(id);
  }

  public MqlClusterStatement(MqlParser p, int id) {
    super(p, id);
  }

  public boolean readyToGenerate() {
    if(!super.readyToGenerate()) {
      return false;
    }

    MqlTable inputTable = MqlTable.find(getTableName(0));
    if(inputTable.getStatus() != FINISHED) {
      return false;
    }

    return true;
  }

  public void declareTables()
    throws MqlException
  {
    // Register input table and define the output table
    super.declareTables();

    // Mark input table for high-priority creation
    MqlTable inputTable = MqlTable.find(getTableName(0));
    inputTable.setPriority();
  }

  public void writeDeclaration(MqlOutput o)
    throws IOException
  {
    // declare the output table
    MqlTable outputTable = MqlTable.find(getOutputTableName());
    outputTable.outputStruct(o);

    //
    // declare a comparison function for each of the tables to check
    // whether or not they are sorted properly.
    //
    MqlCoincExpression coinc = getCoincidence();
    if(coinc != null) {

      MqlTable table = MqlTable.find(getTableName(0));
      inputComparName = MqlCompiler.getUniqueID();

      String columnName1 = coinc.getColumn1();
      columnName1 = columnName1.substring(0, columnName1.length()-2);

      table.outputCompare(o, columnName1, inputComparName);
    }

    // if applicable, write out a sort comparison function for the 
    // output table (in case of an "order by" clause).
    String sortExpression = getSortExpression();
    if(sortExpression != null) {
      outputComparName = MqlCompiler.getUniqueID();
      outputTable.outputCompare(o, sortExpression, outputComparName);
    }
  }

  public void writeInitialization(MqlOutput o)
    throws IOException
  {
  }

  public String getInputTableName() 
  {
     return getTableName(0);
  }

  public String getOutputTableName()
  {
    return getTableName(1);
  }

  public String[] getDefaultColumns()
  {
    // our default column list is a set of variations on the input 
    // column lists, with suffixes like _min, _max, _avg
     
    MqlTable table = MqlTable.find(getInputTableName());

    Vector v = new Vector();
    for(int i = 0 ; i < table.getNumColumns(); i++) {
      String inputName = table.getColumnName(i);
      int columnTypeID = table.getColumnType(i);
      if(columnTypeID < MqlType.STRUCT) {
        v.add(inputName + "_min");
        v.add(inputName + "_max");
        v.add(inputName + "_avg");
      }
    }
    v.add("cluster_size");

    String[] columns = new String[v.size()];
    for(int i = 0; i < v.size(); i++) {
      columns[i] = (String) v.get(i);
    }

    return columns;
  }

  public int[] getDefaultColumnTypes()
  {
    // our default column list is the union of the column lists 
    // from the two input tables, with conflicts resolved appropriately

    MqlTable table = MqlTable.find(getTableName(0));

    int types[] = new int[3*table.getNumColumns() + 1];
    int numTypes = 0;

    for(int i = 0; i < table.getNumColumns(); i++) {
      int columnType = table.getColumnType(i);
      if(columnType < MqlType.STRUCT) {
        for(int j = 0; j < 3; j++) {
          types[numTypes++] = columnType;
        }
      }
    }
    types[numTypes++] = MqlType.INTEGER;
    if(numTypes == types.length) {
      return types;
    }

    int typesOut[] = new int[numTypes];
    for(int i = 0; i < numTypes; i++) {
      typesOut[i] = types[i];
    }
    return typesOut;
  }

  public boolean hasNonTransitiveOption()
  {
    for(int j = 0; j < jjtGetNumChildren(); j++) {
      if(jjtGetChild(j) instanceof MqlNonTransitiveOption) {
        return true;
      }
    }

    return false;
  }

  public void writePreMain(MqlOutput o)
  {
    // If we have a coincidence condition, we will need a variable to 
    // keep track of the beginning of the region of interest.
    MqlCoincExpression coinc = getCoincidence();
    if(coinc != null) {
      firstMatchVar = MqlCompiler.getUniqueID();
      o.o("int " + firstMatchVar + " = 0;");
    }
  }

  private boolean isNameInList(String[] list, String name) {
    for(int i = 0; i < list.length; i++) {
      if(name.equals(list[i])) {
        return true;
      }
    }
    return false;
  }

  public void writeMain(MqlOutput o)
    throws IOException, MqlException
  {
    // This code gets a little hairy.  Here is an outline of the
    // intended output:
    //
    // loop(i1 from 0 to table_length) {
    //
    //   if(we've already used event i1 in a cluster) {
    //     continue;
    //   }
    //
    //   add event i1 to cluster;
    //   
    //   loop(i2 from i1 to table_length) {
    //
    //     we_saw_a_coincidence = false;
    //
    //     loop(i3 from 0 to number_of_events_in_cluster) {
    //
    //       // IF NON_TRANSITIVE OPTION IS SET
    //       if(i3 != 0) break;
    //
    //       i4 = cluster_member[i3];
    //
    //       if(!(event i2 in coincidence with event i4)) {
    //         continue; // next iteration of loop over i3
    //       }
    //
    //       we_saw_a_coincidence = true;
    //       
    //       if(event i2 matches "where" condition for event i4) {
    //         add event i2 to cluster;
    //         i2 = i1;  // go back to beginning
    //       }
    //     }
    //
    //     if(!we_saw_a_coincidence) {
    //       break; // from loop over i2
    //     }
    //  }
    //
    //  fill in output table with this cluster;
    // }

    // remember that we're working on this statement
    setStatus(IN_PROGRESS);

    // apply the "enable" condition, if applicable
    String enableExpression = getEnableExpression();
    if(enableExpression != null) {
      o.o("if(" + enableExpression + ")");
      o.openBrace();
    }

    // look up the tables that are involved
    String tableName = getTableName(0);
    String outputTableName = getOutputTableName();
    MqlTable table = MqlTable.find(tableName);
    MqlTable outputTable = MqlTable.find(outputTableName);

    // a variable to keep track of which events have been used as part
    // of a cluster already.
    String usedVar = MqlCompiler.getUniqueID();
    o.o("bool " + usedVar + "[_t_" +  table.getTableName() + ".size()];");
    o.o("memset(" + usedVar + ", 0, sizeof(" + usedVar + "));");

    // Check that the table is sorted correctly
    MqlCoincExpression coinc = getCoincidence();
    if(coinc != null) {
      o.o("if(!_t_" + tableName + ".checkSort(" + inputComparName + "))");
      o.openBrace();
      o.o("printf(\"Warning: table " + tableName + " is not sorted on " + coinc.getColumn1() + " as required for cluster.\\n\");");
      o.closeBrace();
    }

    // Open the outer loop over the input table.
    String indexVar1 = table.openLoop(o);

    // Check that we haven't used this event as part of another cluster 
    // already.
    o.o("if(" + usedVar + "[" + indexVar1 + "])");
    o.openBrace();
    o.o("continue;");
    o.closeBrace();

    // And remember now that it has been used.
    o.o(usedVar + "[" + indexVar1 + "] = true;");

    // Establish an array to hold the indices of the events that are
    // part of the cluster.
    o.o("int cluster_size = 1;");
    o.o("int cluster_members[_t_" + table.getTableName() + ".size()];");
    o.o("cluster_members[0] = " + indexVar1 + ";");
    o.o();

    // Open a pair of nested loops.  We need to test each event within the
    // the coincidence window against every event that is already in the
    // cluster.
    String indexVar2 = MqlCompiler.getUniqueID();
    o.o("for(int " + indexVar2 + " = " + indexVar1 + " + 1; " + indexVar2 +
        " < _t_" + tableName + ".size(); " + indexVar2 + "++)");
    o.openBrace();

    // check that this hit has not been included in another cluster already
    o.o("if(" + usedVar + "[" + indexVar2 + "])");
    o.openBrace();
    o.o("continue;");
    o.closeBrace();

    o.o("bool coincidence_seen = false;");

    String indexVar3 = MqlCompiler.getUniqueID();
    o.o("for(int " + indexVar3 + " = 0; " + indexVar3 + " < cluster_size; " +
         indexVar3 + "++)");
    o.openBrace();

    if(hasNonTransitiveOption()) {
      o.o("if(" + indexVar3 + " > 0) break;");
    }

    String indexVar4 = MqlCompiler.getUniqueID();
    o.o("int " + indexVar4 + " = cluster_members[" + indexVar3 + "];");

    // assign a local variable for each of the columns in both events.
    for(int i = 0; i < table.getNumColumns(); i++) {
        String columnName = table.getColumnName(i); 
        String columnType = MqlType.toString(table.getColumnType(i));
        o.o(columnType + " " + columnName + "_1 = _t_" + 
            table.getTableName() + "[" + indexVar4 + "]." + columnName + ";");
        o.o(columnType + " " + columnName + "_2 = _t_" + 
            table.getTableName() + "[" + indexVar2 + "]." + 
	    columnName + ";");

        // correct the values for the modulo condition
        MqlModuloExpression modulo = getModuloExpression(columnName);
        if(modulo != null) {
          o.o("if(" + columnName + "_1 - " + columnName + "_2 > (" + modulo.getPeriod() + ")/2.0)");
	  o.openBrace();
	  o.o(columnName + "_1 = " + columnName + "_1 - (" + columnType + ")(" + modulo.getPeriod() + ");");
  	  o.closeBrace();
          o.o("else if(" + columnName + "_2 - " + columnName + "_1 > (" + modulo.getPeriod() + ")/2.0)");
	  o.openBrace();
	  o.o(columnName + "_2 = " + columnName + "_2 - (" + columnType + ")(" + modulo.getPeriod() + ");");
  	  o.closeBrace();
        } 
    }
    o.o();

    // check the coincidence condition, if there is one
    String coincVar = MqlCompiler.getUniqueID();
    if(coinc != null) {
      o.o("double " + coincVar + " = " + coinc.getColumn1() + " - " + 
                              coinc.getColumn2() + ";");
      o.o("if(" + coincVar + " < " +  coinc.getLow() + ")");
      o.openBrace();
      o.o("continue;");
      o.closeBrace();
    }
    o.o("coincidence_seen = true;");

 
    // apply a "where" condition, if applicable
    String whereExpression = getWhereExpression();
    if(whereExpression != null) {
      o.o("if(!(" + whereExpression + "))");
      o.openBrace();
      o.o("continue;");
      o.closeBrace();
    }

    // if it matches, add this event to the cluster and break
    // out of the third loop.
    o.o("cluster_members[cluster_size++] = " + indexVar2 + ";");
    o.o(usedVar + "[" + indexVar2 + "] = true;");
    if(!hasNonTransitiveOption()) {
      o.o(indexVar2 + " = " + indexVar1 + ";"); 
    }
    o.o("break;");

    // close the innermost loop over the cluster members
    o.closeBrace();

    // check whether a coincidence was found with any cluster member
    o.o("if(!coincidence_seen)");
    o.openBrace();
    o.o("break;");
    o.closeBrace();

    // close the middle loop
    o.closeBrace();

    // At this point, we've decided which events will be elements of this
    // cluster.  Now we will go through those elements again to form the
    // entry in the output table.
    for(int i = 0; i < table.getNumColumns(); i++) {
      String columnName = table.getColumnName(i);
      int columnTypeID = table.getColumnType(i);
      String columnType = MqlType.toString(columnTypeID);

      if(columnTypeID < MqlType.STRUCT) {
        o.o(columnType + " " + columnName + "_min = _t_" +
          table.getTableName() + "[" + indexVar1 + "]." + columnName + ";");
        o.o(columnType + " " + columnName + "_max = " + columnName + "_min;");
        o.o(columnType + " " + columnName + "_sum = 0;");
      }
    }

    o.o("for(int " + indexVar3 + " = 0; " + indexVar3 + " < cluster_size; " +
         indexVar3 + "++)");
    o.openBrace();
     
    o.o("int " + indexVar4 + " = cluster_members[" + indexVar3 + "];");
    for(int i = 0; i < table.getNumColumns(); i++) {
      String columnName = table.getColumnName(i);
      int columnTypeID = table.getColumnType(i);
      String columnType = MqlType.toString(columnTypeID);

      // First pick a value for this parameter.  If there is a modulo
      // period, then we have to adjust all values to be on the same side
      // of the dividing line.
      o.o(columnName + " = _t_" + getInputTableName() + "[" + indexVar4 + 
		      "]." + columnName + ";");
      MqlModuloExpression modulo = getModuloExpression(columnName);
      if(modulo != null) {
	o.o("if(" + indexVar3  + " != 0)");
	o.openBrace();
        o.o("if(" + columnName + " - (double)(" + columnName + "_sum)/" + indexVar3 +
			" > (" + modulo.getPeriod() + ")/2.0)");
	o.openBrace();
	o.o(columnName + " = " + columnName + " - (" + columnType + ")(" + modulo.getPeriod() + ");");
	o.closeBrace();
	o.o("else if((double)(" + columnName + "_sum)/" + indexVar3 + " - " + 
			 columnName + " > (" + modulo.getPeriod() + ")/2.0)");
	o.openBrace();
	o.o(columnName + " = " + columnName + " + (" + columnType + ")(" + modulo.getPeriod() + ");");
	o.closeBrace();
	o.closeBrace();
      }
      if(columnTypeID < MqlType.STRUCT) {
        o.o(columnName + "_min = MIN(" + columnName + "_min, " + 
              columnName + ");");
        o.o(columnName + "_max = MAX(" + columnName + "_max, " + 
              columnName + ");");
        o.o(columnName + "_sum = " + columnName + "_sum + " + 
              columnName + ";");
      }
    }

    o.closeBrace();

    // OK...now we know the sum, minimum, maximum, etc. of the hits that
    // form the cluster.  We have therefore reached the point that corresponds 
    // to a row in the output table, so this is the chance to insert any 
    // statements that depend on the output table.
    for(int i = 0; i < table.getNumColumns(); i++) {
      String columnName = table.getColumnName(i);
      int columnTypeID = table.getColumnType(i);
      String columnType = MqlType.toString(columnTypeID);
      if(columnTypeID < MqlType.STRUCT) {
        o.o(columnType + " " + columnName + "_1 = _t_" + getInputTableName() +
  	    "[" + indexVar1 + "]." + columnName + ";");
        o.o("double " + columnName + "_avg = " 
             + columnName + "_sum/((double)cluster_size);"); 
      } else {
        o.o(columnType + " " + columnName + "_1 = _t_" + getInputTableName() +
  	    "[" + indexVar1 + "]." + columnName + ";");
      }

      MqlModuloExpression modulo = getModuloExpression(columnName);
      if(columnTypeID < MqlType.STRUCT) {
        if(modulo != null) {
          o.o(columnName + "_avg = safe_fmod(" + columnName + "_avg - " + 
  	    modulo.getLowerLimit() + ", " + modulo.getPeriod() + ") + " + 
	    modulo.getLowerLimit() + ";");
	  o.o(columnType + " " + columnName + "_width = " + 
  			columnName + "_max - " + columnName + "_min + 1;");
          o.o(columnName + "_min = (" + columnType + ")(safe_fmod(" + columnName + "_min -" + 
   	    modulo.getLowerLimit() + ", " + modulo.getPeriod() + ") + " + 
	    modulo.getLowerLimit() + ");");
          o.o(columnName + "_max = (" + columnType + ")(safe_fmod(" + columnName + "_max - " + 
   	    modulo.getLowerLimit() + ", " + modulo.getPeriod() + ") + " + 
	    modulo.getLowerLimit() + ");");
        } else {
	  o.o(columnType + " " + columnName + "_width = " + 
	  		  columnName + "_max - " + columnName + "_min + 1;");
        }
      }
    }
    o.openBrace();
    String uniqueNames[] = new String[outputTable.getNumColumns()];
    int[] outputColumnTypes = outputTable.getColumnTypes();

    String[] defaultColumns = getDefaultColumns();
    for(int i = 0; i < outputTable.getNumColumns(); i++) {

      // find a unique temporary name for the variable in the output table; 
      // try to make it the same as the final name, or with "_0" added,
      // if possible.
      uniqueNames[i] = outputTable.getColumnName(i);
      if(isNameInList(defaultColumns, uniqueNames[i])) {
        uniqueNames[i] = uniqueNames[i] + "_0";
        if(isNameInList(defaultColumns, uniqueNames[i])) {
          uniqueNames[i] = MqlCompiler.getUniqueID();
        }
      }

      o.o(MqlType.toString(outputColumnTypes[i]) + " " + uniqueNames[i] + 
            " = " + outputTable.getExpression(i) + ";");
    }
    for(int i = 0; i < outputTable.getNumColumns(); i++) {
      if(!uniqueNames[i].equals(outputTable.getColumnName(i))) {
        o.o(MqlType.toString(outputColumnTypes[i]) + " " + 
          outputTable.getColumnName(i) + " = " + uniqueNames[i] + ";");
      }
    }

    outputTable.setStatus(MqlTable.IN_PROGRESS);
    outputTable.writeDependents(o);

    // Finally, if we need to, fill in the new table
    if(outputTable.mustCreateTable()) {
      String outputIndex = MqlCompiler.getUniqueID();
      o.o("int " + outputIndex + " = _t_" + outputTableName + ".next();");
      for(int i = 0; i < outputTable.getNumColumns(); i++) {
        o.o("_t_" + outputTableName + "[" + outputIndex + "]." +
            outputTable.getColumnName(i) + " = " + uniqueNames[i] + ";");
      }
      outputTable.setStatus(MqlTable.FINISHED);

    }
    o.closeBrace();
    table.closeLoop(o);
    if(enableExpression != null) {
      o.closeBrace();
    }

    String sortExpression = getSortExpression();
    if(sortExpression != null) {
      o.o("_t_" + outputTableName + ".sort(" + outputComparName + ");");
    }

    setStatus(FINISHED);
  }

  String inputComparName;
  String outputComparName;
  String firstMatchVar;
}
