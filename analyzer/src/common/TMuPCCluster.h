#ifndef TMuPCCluster_h
#define TMuPCCluster_h

#include <TObject.h>

////////////////////////////////////////////////////////////////////////////////
/// \brief
/// Simple format for delivering muPC data.  Included are the timestamp and x, y wire numbers.  
/// Wire numbers are doubles bc they represent the average wire number in each dimension
/// found from a clustering algorithm.
///
////////////////////////////////////////////////////////////////////////////////
class TMuPCCluster : public TObject {
 private:
  /// The time, in sample clock ticks, of the hit
  double fTimeStamp;
  /// The (average) wire number hit
  double fWireNumberX;
  double fWireNumberY;

 public:
  /// This default constructor I believe exists so ROOT can load these from file and is not used explcitly.
  TMuPCCluster();

  TMuPCCluster(double timestamp, double wire_number_x, double wire_number_y);

  /// Goes to the state the default constructor would set it to. Not generally used.
  void Reset(Option_t* o = "");

  double GetTimeStamp() const { return fTimeStamp; }
  double GetWireNumberX() const { return fWireNumberX; }
  double GetWireNumberY() const { return fWireNumberY; }

  void SetTimeStamp(double t ){fTimeStamp=t;}
  void SetWireNumberX(double wx ){fWireNumberX=wx;}
  void SetWireNumberY(double wy ){fWireNumberY=wy;}

 private:
  /// Copying is made explicitly private since we do not need it yet.
  TMuPCCluster(const TMuPCCluster& src);
  /// Assignment is made explicitly private since we do not need it yet.
  TMuPCCluster operator=(const TMuPCCluster& rhs); 

  ClassDef(TMuPCCluster, 1);
};

#endif

/** @}*/
