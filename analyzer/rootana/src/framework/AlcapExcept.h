//////////////////////////////////////////////////////////////
// Shamelessly lifted from COMET (ND280) code
//
#ifndef ALCAPEXCEPT_H_
#define ALCAPEXCEPT_H_

#include <exception>
#include <string>

namespace Except {
    class Base;
}

/// The root exception for all exceptions explicitly thrown by the
/// AlCap offline analysis code.  All exceptions declared by this
/// library should be derived from this class, and not std::exception.
/// New exceptions can be derived from Except::Base by creating a new class
///
/// \code
/// namespace Exception {
///   class Child;
/// };
///
/// class Exception::Child : public Except::Base {
/// public:
///     Child() {AppendWhat("Child");};
/// };
/// \endcode
///
/// But the prefered way to create derived exception classes is to use the
/// MAKE_EXCEPTION macro.  This is used as follows 
/// 
/// \code
/// MAKE_EXCEPTION(Child,Base);
/// MAKE_EXCEPTION(GrandChild,Child);
/// ...
/// try {
///   throw Except::Child();
/// }
/// catch (Except::Base& ex) {
///   std::cout << ex.what << std::endl;
/// }
/// \endcode
///
/// This can add a backtrace at the point of the exception.  The number of
/// backtrace frames to be shown is controlled with the
/// Except::Base::gBacktraceSymbols static variable.
/// 
class Except::Base: public std::exception {
  /// What exception generated this object.
  char fWhat[2048];
  
public:
  /// The number of backtrace symbols to add to the "what" string.
  static unsigned int gBacktraceSymbols;
  
  Base();
  virtual ~Base() throw() {}
  
  /// Inherited from exception to return the name of the exception as a null
  /// terminated string.
  const char* what(void) const throw() {return fWhat;}
  
  /// Used in constructors of classes which inherit from Except::Base
  /// to add text to the What() string.
  void AppendWhat(const char* child);
};

/// A macro to build an exception class __name that is derived from __parent.
/// The __parent class must be derived from Except::Base which provides the
/// AppendWhat method.  This macro builds an exception class that can be used:
/// \verbatim
/// MAKE_EXCEPTION(Child,Base);
/// ...
/// try {
///   throw Except::Child();
/// }
/// catch (Except::Base& ex) {
///   std::cout << ex.what << std::endl;
/// }
/// \endverbatim
#ifndef MAKE_EXCEPTION
#define MAKE_EXCEPTION(NAME, PARENT)               \
  namespace Except {                               \
    class NAME : public PARENT {                   \
    public:                                        \
      NAME() {AppendWhat(#NAME);}                  \
    };                                             \
  };                                               
#endif //MAKE_EXCEPTION
#endif //ALCAPEXCEPT_H_
