#include "../..//Utils/Unfold_FlatBackground.C"

void Si16b_Unfold_FlatBackground(std::string infilename, std::string inhistname, std::string cutfilename, std::string cuttreename,
				 std::string corrfilename, std::string corrhistname, std::string corrtreename, std::string outfilename, std::string outdirname) {

  Unfold_FlatBackgroundArgs args;
  args.infilename = infilename;
  args.inhistname = inhistname;
  args.cutfilename = cutfilename;
  args.cuttreename = cuttreename;
    
  args.corrfilename = corrfilename;
  args.corrhistname = corrhistname;
  args.corrtreename = corrtreename;

  args.outfilename = outfilename;
  args.outdirname = outdirname;
  Unfold_FlatBackground(args);
}
