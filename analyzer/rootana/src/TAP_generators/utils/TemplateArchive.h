#ifndef TemplateArchive_h__
#define TemplateArchive_h__

#include "TFile.h"
#include "TH1.h"

////////////////////////////////////////////////////////////////////////////////
/// \ingroup rootana_modules
/// \author Andrew Edmonds
///
/// \brief
/// Utility class that manages reading and writing template histograms to a ROOT file
///
////////////////////////////////////////////////////////////////////////////////
class TemplateArchive {
  
 private:
  /// \brief
  /// The ROOT file that contains the templates
  TFile* fTemplateFile;

 public:
  /// \brief
  /// The constructor
  ///
  /// \detailed
  /// Given a file name and a standard ROOT TFile option (e.g. "READ", "RECREATE" etc.)
  /// and opens the ROOT file ready for reading and writing
  TemplateArchive(const char* filename, const char* option);

  /// \brief
  /// Empty destructor
  ~TemplateArchive();

 public:
  /// \brief
  /// Gets the template histogram of the given name and returns it
  TH1D* GetTemplate(const char* template_name);

  /// \brief
  /// Takes the given histogram and saves it to the ROOT file
  void SaveTemplate(TH1* hTemplate);

};

#endif
