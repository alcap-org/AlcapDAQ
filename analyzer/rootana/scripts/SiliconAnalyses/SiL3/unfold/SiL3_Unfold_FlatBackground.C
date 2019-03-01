#include "scripts/SiliconAnalyses/Utils/Unfold_FlatBackground.C"

void SiL3_Unfold_FlatBackground(std::string infilename, std::string inhistname, std::string intreename, std::string corrfilename, std::string corrhistname, std::string corrtreename, std::string outfilename, std::string outdirname) {

  Unfold_FlatBackgroundArgs args;
  args.infilename = infilename;
  args.inhistname = inhistname;
  args.intreename = intreename;
    
  args.corrfilename = corrfilename;
  args.corrhistname = corrhistname;
  args.corrtreename = corrtreename;

  args.outfilename = outfilename;
  args.outdirname = outdirname;
  Unfold_FlatBackground(args);
}
