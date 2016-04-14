#!/usr/bin/env python
import sys
import getpass
import os
content = ""
def appendToContent(line):
  global content
  content += str(line) + "\n"
def constructDescriptor(module_name, author_name):
  part = "ANA_MODULE " + module_name + "_module =\n"
  part += "{\n"
  part += "\t\"" + module_name + "\",\t\t/* module name */\n"
  part += "\t\"" + author_name + "\",\t\t/* author name*/\n"
  part += "\t" + module_name + "_event,\t\t/* event routine*/\n"
  part += "\t" + module_name + "_bor,\t\t/* BOR routine*/\n"
  part += "\t" + module_name + "_eor,\t\t/* EOR routine*/\n"
  part += "\t" + module_name + "_init,\t\t/* init routine*/\n"
  part += "\t" + module_name + "_exit,\t\t/* exit routine*/\n"
  part += "\t" + "NULL,\t\t/* parameter structure*/\n"
  part += "\t" + "0,\t\t/* structure size*/\n"
  part += "\t" + "NULL,\t\t/* initial parameters*/\n"
  part += "};\n"
  return part
def constructEmptyFunctions(module_name, suffix):
  appendToContent("\nINT " + module_name + "_" + suffix)
  appendToContent("{")
  appendToContent("\treturn SUCCESS;")
  appendToContent("}")
def constructIncludes(items, comments):
  appendToContent("/*" + comments + "*/")
  for item in items:
    appendToContent("#include " + item + "")
  appendToContent("\n")

author_name = raw_input("Name of author:") or getpass.getuser()
module_name = raw_input("Name of MODULE:") or "default"
module_description = raw_input("Description:") or ""

function_list = ("init(void)", "exit(void)", "bor(INT)", "eor(INT)", "event(EVENT_HEADER*, void*)")
include_standard_libraries = ("<stdio.h>", "<stdlib.h>", "<string>", "<map>",)
include_midas_libraries = ("\"midas.h\"",)
include_alcap_libraries = ("\"TGlobalData.h\"", "\"TPulseIsland.h\"",)
using_namespaces = ("std::string", "std::map", "std::vector", "std::pair",)

constructIncludes(include_standard_libraries, "Standard includes");
constructIncludes(include_midas_libraries, "MIDAS includes");
constructIncludes(include_alcap_libraries, "AlCap includes");

appendToContent("/* namespaces */\n")
for item in using_namespaces:
  appendToContent("using " + item + ";")
for item in function_list:
  appendToContent("static INT " + module_name + "_" + item + ";")
appendToContent(constructDescriptor(module_name, author_name) )

for item in function_list:
  constructEmptyFunctions(module_name, item)

source_directory = os.environ['DAQdir'] + "/analyzer/src/" +  author_name + "/"
modulefile = open(source_directory + module_name + ".cpp", 'w+')
modulefile.write(content)
modulefile.close()
print module_name + " generated in " + source_directory + "\n"
