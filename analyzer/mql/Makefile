# The version = 1.3 stuff is to allow for older JVMs that don't support 
# JDK 1.4 or 1.5. In order for the *.java files to be generated with
# JDK 1.3 compatible code, the JDK_VERSION options must be invoked on 
# jjtree and javacc

all: mql.jar

JAVAC=javac
JAVACC=javacc.sh

mql.jj: mql.jjt
	jjtree -JDK_VERSION=1.3 $<

MqlParser.java: mql.jj
	$(JAVACC) -JDK_VERSION=1.3 $<

mql.jar: MqlParser.java
	$(JAVAC) -source 1.3 -target 1.3 *.java
	jar cfv mql.jar *.class

clean: 
	rm -f mql.jar *.class mql.jj MqlParser.java
	rm -f JJTMqlParserState.java JavaCharStream.java MqlInvertOption.java\
	      MqlNonTransitiveOption.java MqlParser.java MqlParserConstants.java\
	      MqlParserTokenManager.java MqlParserTreeConstants.java MqlRoot.java\
	      MqlUniqueOption.java Node.java ParseException.java SimpleNode.java\
	      Token.java TokenMgrError.java
