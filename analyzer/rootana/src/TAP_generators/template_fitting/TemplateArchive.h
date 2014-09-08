#ifndef TemplateArchive_h__
#define TemplateArchive_h__

#include <TFile.h>
#include "TTemplate.h"

////////////////////////////////////////////////////////////////////////////////
/// \ingroup rootana_modules
/// \author Andrew Edmonds 
/// \author Ben Krikler
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
  TDirectory* fDirectory;

 public:
  /// \brief
  ///  constructor to make separate template files
  ///
  /// \detailed
  /// Given a file name and a standard ROOT TFile option (e.g. "READ", "RECREATE" etc.)
  /// and opens the ROOT file ready for reading and writing
  TemplateArchive(const char* filename, const char* option);

  /// \brief
  ///  constructor to use a directory in an existing file
  TemplateArchive(TDirectory* dir);

  /// \brief
  /// Empty destructor
  ~TemplateArchive();

 public:
  /// \brief
  /// Gets the template histogram of the given name and returns it
  const TTemplate* GetTemplate(const char* template_name);

  /// \brief
  /// Takes the given histogram and saves it to the ROOT file
  void SaveTemplate(const TTemplate* hTemplate);

};

#endif