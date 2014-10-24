#include "TMuPCCluster.h"

#include <cmath>
#include <sstream>
#include <cstdlib>
#include <algorithm>

TMuPCCluster::TMuPCCluster() : fTimeStamp(0), fWireNumberX(0), fWireNumberY(0) {
}

TMuPCCluster::TMuPCCluster(double timestamp, double wire_number_x, double wire_number_y):
  fTimeStamp(timestamp), fWireNumberX(wire_number_x), fWireNumberY(wire_number_y){
}

 void TMuPCCluster::Reset(Option_t* o)
{
  fTimeStamp = 0;
  fWireNumberX = 0;
  fWireNumberY = 0;
}
