#ifndef EVENTNAVIGATOR_H
#define EVENTNAVIGATOR_H

#include "TGlobalData.h"

static TGlobalData *g_event=NULL;


/// The EventNavigator class provides acess to all the collections in
/// the (MIDAS) Event, which corresponds to a single ROOT branch  

class EventNavigator {
 public:
  //The user acessor. Not sure yet if it can be used without
  //providing initalisation info.

  static EventNavigator& Instance();
  ~EventNavigator();
  
 private: 
  EventNavigator();
  EventNavigator(const EventNavigator& src){};
  EventNavigator& operator=(const EventNavigator& rhs){return *this;}

  static EventNavigator* fInstance; 

};



#endif //EVENTNAVIGATOR_H
