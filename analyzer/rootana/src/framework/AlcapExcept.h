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
/// In addition ther are two variant ways to construct an exception
/// \code
/// // Throw an exception with a debugging message 
/// try {
///   throw Except::Child("your message here");
/// }
///  
///  // Throw an exception with the source code location
///  try {
///    throw Except::Child(LOCATION)
///  }
///  \endcode
/// LOCATION is an auxillary macro that evaluates as  `__FILE__ , __LINE__`
/// this form is not usually needed, as it is usually obvious from the backtrace  
class Except::Base: public std::exception {
  /// The backtrace when the exception was thrown;
  char fTrace[2048];
  /// What exception generated this object.
  char fWhat[256];
  
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

  /// Used in constructors of classes which inherit from Except::Base
  /// to add message text to the What() string.
  void AppendWhat(const char* child, const char* message);

  /// Used in conjunction with the LOCATION macro, see documentation
  /// of \ref Except::Base
  void AppendWhat(const char* child, const char* file, int line);
};

#define STRINGIFY_1(x) #x
#define STRINGIFY_2(x) STRINGIFY_1(x)
#define LOCATION __FILE__ ":" STRINGIFY_2(__LINE__)

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
#define MAKE_EXCEPTION(NAME, PARENT)                          \
  namespace Except {                                          \
    class NAME : public PARENT {                              \
    public:                                                   \
      NAME() {AppendWhat(#NAME);}                             \
      NAME(const char* message) {AppendWhat(#NAME, message);} \
      NAME(const char* file, int line) {AppendWhat(#NAME, file, line);}     \
    };                                                        \
  }                                             
#endif //MAKE_EXCEPTION
#endif //ALCAPEXCEPT_H_
