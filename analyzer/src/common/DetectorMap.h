#ifndef DETECTORMAP_H_MPIL1BS0
#define DETECTORMAP_H_MPIL1BS0

#include <string>
#include <map>

const std::map<std::string, std::string> ChannelToDetectorMap =
{
	{"Na80",  "muSc"},
	{"Nb80",  "muSc-A"},
	{"Nc80",  "ScL"},
	{"Nd80",  "ScR"},
	{"Ne80",  "ScGe"},
	{"Nf80",  "blank"},
	{"Ng80",  "blank"},
	{"Nh80",  "blank"},

	{"Nac0",  "SiL2Slow"},
	{"Nbc0",  "SiR2Slow"},
	{"Ncc0",  "GeSlow"},
	{"Ndc0",  "blank"},
	{"Nec0",  "SiL2Fast"},
	{"Nfc0",  "SiR2Fast"},
	{"Ngc0",  "GeFast"},
	{"Nhc0",  "blank"},

	{"Nae0",  "SiL1_1Fast"},
	{"Nbe0",  "SiL1_2Fast"},
	{"Nce0",  "SiL1_3Fast"},
	{"Nde0",  "SiL1_4Fast"},
	{"Nee0",  "SiR1_1Fast"},
	{"Nfe0",  "SiR1_2Fast"},
	{"Nge0",  "SiR1_3Fast"},
	{"Nhe0",  "SiR1_4Fast"},

	{"Naf0",  "SiL1_1Slow"},
	{"Nbf0",  "SiL1_2Slow"},
	{"Ncf0",  "SiL1_3Slow"},
	{"Ndf0",  "SiL1_4Slow"},
	{"Nef0",  "SiR1_1Slow"},
	{"Nff0",  "SiR1_2Slow"},
	{"Ngf0",  "SiR1_3Slow"},
	{"Nhf0",  "SiR1_4Slow"},
};

#endif /* end of include guard: DETECTORMAP_H_MPIL1BS0 */

