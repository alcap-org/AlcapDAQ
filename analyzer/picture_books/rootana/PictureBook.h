#ifndef PictureBook_h_
#define PictureBook_h_

#include <fstream>

class PictureBook {
 public:
  PictureBook(std::string filename);
  ~PictureBook();

  void StartNewSection(std::string section_name);
  void InsertFigure(std::string image_file);

 private:
  std::ofstream fLatexFile;

  void WritePreamble();
  void WriteEnd();
};

#endif
