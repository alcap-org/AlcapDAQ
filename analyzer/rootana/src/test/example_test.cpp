#include "tut.h"

#include <vector>

namespace tut
{
  struct data //(1)
  {};

  typedef test_group<data> tg; //(2)
  tg example_group("Example tests"); //(3)
 
  typedef tg::object testobject;
  
  template<> template<>
  void testobject::test<1>()
  {
    //set_test_name("clear");

    std::vector<double> v(3, 1.2);
    ensure_equals("has elements", v.size(),3);
    
    v.clear();
    ensure_equals("has no elements", v.size(),0);
  }

  //these should all suceed
  template<> template<>
  void testobject::test<2>()
  {
    ensure_equals("identity", 1,1);
  }

  template<> template<>
  void testobject::test<3>()
  {
    bool truth_conditon = 1;
    ensure("simple truth", truth_conditon);
  }

  template<> template<>
  void testobject::test<4>()
  {
    int a = 3;
    int max = 5;
    ensure_lessthan("int ordering", a ,max);
  }

  template<> template<>
  void testobject::test<5>()
  {
    double a = 3.14;
    double min = 1./2;
    ensure_greaterthan("double ordering", a , min);
  }

  template<>template<>
  void testobject::test<6>()
  {
    double a =0.5;
    ensure_distance("range", a, 0., 1.);
  }

  //It (mostly) works with anything that can accept equality and
  //ostreamoperators too!
  //[[[[[[
  class partial_order 
  {
  public:
    partial_order(int arg) : fArg(arg) {}
    bool operator<(const partial_order& rhs) const 
    {return (this->fArg < rhs.fArg);}
    bool operator<(const int i)const 
    { return (this->fArg < i);}
    friend std::ostream& operator<<(std::ostream& os, const partial_order& po);
  private: 
    int fArg;
  };
  std::ostream& operator<<(std::ostream& os, const partial_order& po)
  {
    return os << "partial_order(" << po.fArg << ")";
  }
  //]]]]]]

  template<>template<>
  void testobject::test<7>()
  {
    partial_order a(5);
    partial_order b(7);
    
    ensure_lessthan("object order", a, b);
    ensure_lessthan("object-int order", a, 8);
  }
  

  //This should fail
  template<>template<>
  void testobject::test<8>()
  {
    fail("i did this on purpose");
  }

  //it can run past the failure
  template<>template<>
  void testobject::test<9>()
  {
    ensure("this is still run", true);
  }

};
