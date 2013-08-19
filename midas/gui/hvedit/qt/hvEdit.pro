# qmake file for the Qt hvEdit project
#
# Andreas Suter, 2002/11/21

SOURCES = hvEdit.cpp \
          Qt_hvEdit.cpp \
	  hvAdmin.cpp \
	  cmExperiment.cpp \
	  Qt_Connect.cpp \
	  Qt_Pwd.cpp \
	  globals.cpp \
	  cmKey.cpp \
	  help/helpwindow.cpp

HEADERS = Qt_hvEdit.h \
          hvAdmin.h \
	  cmExperiment.h \
	  Qt_Connect.h \
	  Qt_Pwd.h \
	  globals.h \
	  cmKey.h \
	  help/helpwindow.h

FORMS   = Qt_Dlgs/Qt_hvEdit_Base.ui \
          Qt_Dlgs/Qt_Connect_Base.ui \
	  Qt_Dlgs/Qt_Pwd_Base.ui \
	  Qt_Dlgs/Qt_hvEditAbout.ui \
	  Qt_Dlgs/Qt_fileList.ui

CONFIG += qt warn_on debug

# Attention! the variable HOME needs to be set to the 
#            needed path.
HOME = .
INCLUDEPATH += $${HOME}/Qt_Dlgs
INCLUDEPATH += $$(MIDAS_ROOT)/include

#
# linux standard libs
#
unix:LIBS += -lbsd -lm -lutil -lmidas
#
