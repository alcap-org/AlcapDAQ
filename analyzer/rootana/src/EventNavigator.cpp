#include "EventNavigator.h"

TGlobalData* TGlobalData::Instance()
{
  return g_event;
}
//======================================================================
EventNavigator* EventNavigator::fInstance = 0x0;

//----------------------------------------------------------------------
EventNavigator& EventNavigator::Instance()
{
  if (!fInstance) fInstance = new EventNavigator();
  return *fInstance;
}

//----------------------------------------------------------------------
EventNavigator::EventNavigator()
{
  //noop
}
