#ifndef PictureBook_h_
#define PictureBook_h_

#include <fstream>

class PictureBook {
 public:
  PictureBook(std::string filename);
  ~PictureBook();

 private:
  std::ofstream fLatexFile;
};

#endif
