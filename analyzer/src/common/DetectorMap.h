#ifndef DETECTORMAP_H_MPIL1BS0
#define DETECTORMAP_H_MPIL1BS0

#include <string>
#include <map>

template <typename T, typename U> class create_map
{
	private:
		std::map<T, U> m_map;
	public:
		create_map(const T& key, const U& val) { m_map[key] = val; }
		create_map<T, U>& operator()(const T& key, const U& val)
		{
			m_map[key] = val;
			return *this;
		}
		operator std::map<T, U>() { return m_map; }
};

typedef std::map<std::string, std::string> bank2detMap;
typedef std::map<std::string, TH1I *> det2histMap;

typedef bank2detMap::iterator bankIter;
typedef det2histMap::iterator detIter;

static bank2detMap ChannelToDetectorMap = create_map<std::string, std::string>
	("Na80",  "muSc")
	("Nb80",  "muScA")
	("Nc80",  "ScL")
	("Nd80",  "ScR")
	("Ne80",  "ScGe")
	//("Nf80",  "blank")
	//("Ng80",  "blank")
	//("Nh80",  "blank")

	("Nac0",  "SiL2Slow")
	("Nbc0",  "SiR2Slow")
	("Ncc0",  "GeSlow")
	//("Ndc0",  "blank")
	("Nec0",  "SiL2Fast")
	("Nfc0",  "SiR2Fast")
	("Ngc0",  "GeFast")
	//("Nhc0",  "blank")

	("Nae0",  "SiL1_1Fast")
	("Nbe0",  "SiL1_2Fast")
	("Nce0",  "SiL1_3Fast")
	("Nde0",  "SiL1_4Fast")
	("Nee0",  "SiR1_1Fast")
	("Nfe0",  "SiR1_2Fast")
	("Nge0",  "SiR1_3Fast")
	("Nhe0",  "SiR1_4Fast")

	("Naf0",  "SiL1_1Slow")
	("Nbf0",  "SiL1_2Slow")
	("Ncf0",  "SiL1_3Slow")
	("Ndf0",  "SiL1_4Slow")
	("Nef0",  "SiR1_1Slow")
	("Nff0",  "SiR1_2Slow")
	("Ngf0",  "SiR1_3Slow")
	("Nhf0",  "SiR1_4Slow");


static TH1I *hmuSc_Raw;   // Bank: Na80
static TH1I *hmuScA_Raw;  // Bank: Nb80
static TH1I *hScL_Raw; 		// Bank: Nc80
static TH1I *hScR_Raw; 		// Bank: Nd80
static TH1I *hScGe_Raw;		// Bank: Ne80

static TH1I *hSiL2Slow_Raw; // Bank: Nac0
static TH1I *hSiR2Slow_Raw; // Bank: Nbc0
static TH1I *hGeSlow_Raw;  // Bank: Ncc0

static TH1I *hSiL2Fast_Raw; // Bank: Nec0
static TH1I *hSiR2Fast_Raw; // Bank: Nfc0
static TH1I *hGeFast_Raw;  // Bank: Ngc0

static TH1I *hSiL1_1Fast_Raw; // Bank: Nae0
static TH1I *hSiL1_2Fast_Raw; // Bank: Nbe0
static TH1I *hSiL1_3Fast_Raw; // Bank: Nce0
static TH1I *hSiL1_4Fast_Raw; // Bank: Nde0
static TH1I *hSiR1_1Fast_Raw; // Bank: Nee0
static TH1I *hSiR1_2Fast_Raw; // Bank: Nfe0
static TH1I *hSiR1_3Fast_Raw; // Bank: Nge0
static TH1I *hSiR1_4Fast_Raw; // Bank: Nhe0

static TH1I *hSiL1_1Slow_Raw; // Bank: Naf0
static TH1I *hSiL1_2Slow_Raw; // Bank: Nbf0
static TH1I *hSiL1_3Slow_Raw; // Bank: Ncf0
static TH1I *hSiL1_4Slow_Raw; // Bank: Ndf0
static TH1I *hSiR1_1Slow_Raw; // Bank: Nef0
static TH1I *hSiR1_2Slow_Raw; // Bank: Nff0
static TH1I *hSiR1_3Slow_Raw; // Bank: Ngf0
static TH1I *hSiR1_4Slow_Raw; // Bank: Nhf0

static det2histMap DetectorToRawHistMap =
	create_map<std::string, TH1I *>
	("muSc", hmuSc_Raw)
	("muScA", hmuScA_Raw)
	("ScL", hScL_Raw)
	("ScR", hScR_Raw)
	("ScGe", hScGe_Raw)
	//("blank", hBlank_Raw)
	//("blank", hBlank_Raw)
	//("blank", hBlank_Raw)

	("SiL2Slow", hSiL2Slow_Raw)
	("SiR2Slow", hSiR2Slow_Raw)
	("GeSlow", hGeSlow_Raw)
	//("blank", hBlank_Raw)

	("SiL2Fast", hSiL2Fast_Raw)
	("SiR2Fast", hSiR2Fast_Raw)
	("GeFast", hGeFast_Raw)
	//("blank", hBlank_Raw)


	("SiL1_1Fast", hSiL1_1Fast_Raw)
	("SiL1_2Fast", hSiL1_2Fast_Raw)
	("SiL1_3Fast", hSiL1_3Fast_Raw)
	("SiL1_4Fast", hSiL1_4Fast_Raw)
	("SiR1_1Fast", hSiR1_1Fast_Raw)
	("SiR1_2Fast", hSiR1_2Fast_Raw)
	("SiR1_3Fast", hSiR1_3Fast_Raw)
	("SiR1_4Fast", hSiR1_4Fast_Raw)

	("SiL1_1Slow", hSiL1_1Slow_Raw)
	("SiL1_2Slow", hSiL1_2Slow_Raw)
	("SiL1_3Slow", hSiL1_3Slow_Raw)
	("SiL1_4Slow", hSiL1_4Slow_Raw)
	("SiR1_1Slow", hSiR1_1Slow_Raw)
	("SiR1_2Slow", hSiR1_2Slow_Raw)
	("SiR1_3Slow", hSiR1_3Slow_Raw)
	("SiR1_4Slow", hSiR1_4Slow_Raw);

static TH1I *hmuSc_Heights;   // Bank: Na80
static TH1I *hmuScA_Heights;  // Bank: Nb80
static TH1I *hScL_Heights; 		// Bank: Nc80
static TH1I *hScR_Heights; 		// Bank: Nd80
static TH1I *hScGe_Heights;		// Bank: Ne80

static TH1I *hSiL2Slow_Heights; // Bank: Nac0
static TH1I *hSiR2Slow_Heights; // Bank: Nbc0
static TH1I *hGeSlow_Heights;  // Bank: Ncc0

static TH1I *hSiL2Fast_Heights; // Bank: Nec0
static TH1I *hSiR2Fast_Heights; // Bank: Nfc0
static TH1I *hGeFast_Heights;  // Bank: Ngc0

static TH1I *hSiL1_1Fast_Heights; // Bank: Nae0
static TH1I *hSiL1_2Fast_Heights; // Bank: Nbe0
static TH1I *hSiL1_3Fast_Heights; // Bank: Nce0
static TH1I *hSiL1_4Fast_Heights; // Bank: Nde0
static TH1I *hSiR1_1Fast_Heights; // Bank: Nee0
static TH1I *hSiR1_2Fast_Heights; // Bank: Nfe0
static TH1I *hSiR1_3Fast_Heights; // Bank: Nge0
static TH1I *hSiR1_4Fast_Heights; // Bank: Nhe0

static TH1I *hSiL1_1Slow_Heights; // Bank: Naf0
static TH1I *hSiL1_2Slow_Heights; // Bank: Nbf0
static TH1I *hSiL1_3Slow_Heights; // Bank: Ncf0
static TH1I *hSiL1_4Slow_Heights; // Bank: Ndf0
static TH1I *hSiR1_1Slow_Heights; // Bank: Nef0
static TH1I *hSiR1_2Slow_Heights; // Bank: Nff0
static TH1I *hSiR1_3Slow_Heights; // Bank: Ngf0
static TH1I *hSiR1_4Slow_Heights; // Bank: Nhf0

static det2histMap DetectorToHeightHistMap =
	create_map<std::string, TH1I *>
	("muSc", hmuSc_Heights)
	("muScA", hmuScA_Heights)
	("ScL", hScL_Heights)
	("ScR", hScR_Heights)
	("ScGe", hScGe_Heights)
	//("blank", hBlank_Heights)
	//("blank", hBlank_Heights)
	//("blank", hBlank_Heights)

	("SiL2Slow", hSiL2Slow_Heights)
	("SiR2Slow", hSiR2Slow_Heights)
	("GeSlow", hGeSlow_Heights)
	//("blank", hBlank_Heights)

	("SiL2Fast", hSiL2Fast_Heights)
	("SiR2Fast", hSiR2Fast_Heights)
	("GeFast", hGeFast_Heights)
	//("blank", hBlank_Heights)


	("SiL1_1Fast", hSiL1_1Fast_Heights)
	("SiL1_2Fast", hSiL1_2Fast_Heights)
	("SiL1_3Fast", hSiL1_3Fast_Heights)
	("SiL1_4Fast", hSiL1_4Fast_Heights)
	("SiR1_1Fast", hSiR1_1Fast_Heights)
	("SiR1_2Fast", hSiR1_2Fast_Heights)
	("SiR1_3Fast", hSiR1_3Fast_Heights)
	("SiR1_4Fast", hSiR1_4Fast_Heights)

	("SiL1_1Slow", hSiL1_1Slow_Heights)
	("SiL1_2Slow", hSiL1_2Slow_Heights)
	("SiL1_3Slow", hSiL1_3Slow_Heights)
	("SiL1_4Slow", hSiL1_4Slow_Heights)
	("SiR1_1Slow", hSiR1_1Slow_Heights)
	("SiR1_2Slow", hSiR1_2Slow_Heights)
	("SiR1_3Slow", hSiR1_3Slow_Heights)
	("SiR1_4Slow", hSiR1_4Slow_Heights);
	
	
static TH1I *hmuSc_Times;   // Bank: Na80
static TH1I *hmuScA_Times;  // Bank: Nb80
static TH1I *hScL_Times; 		// Bank: Nc80
static TH1I *hScR_Times; 		// Bank: Nd80
static TH1I *hScGe_Times;		// Bank: Ne80

static TH1I *hSiL2Slow_Times; // Bank: Nac0
static TH1I *hSiR2Slow_Times; // Bank: Nbc0
static TH1I *hGeSlow_Times;  // Bank: Ncc0

static TH1I *hSiL2Fast_Times; // Bank: Nec0
static TH1I *hSiR2Fast_Times; // Bank: Nfc0
static TH1I *hGeFast_Times;  // Bank: Ngc0

static TH1I *hSiL1_1Fast_Times; // Bank: Nae0
static TH1I *hSiL1_2Fast_Times; // Bank: Nbe0
static TH1I *hSiL1_3Fast_Times; // Bank: Nce0
static TH1I *hSiL1_4Fast_Times; // Bank: Nde0
static TH1I *hSiR1_1Fast_Times; // Bank: Nee0
static TH1I *hSiR1_2Fast_Times; // Bank: Nfe0
static TH1I *hSiR1_3Fast_Times; // Bank: Nge0
static TH1I *hSiR1_4Fast_Times; // Bank: Nhe0

static TH1I *hSiL1_1Slow_Times; // Bank: Naf0
static TH1I *hSiL1_2Slow_Times; // Bank: Nbf0
static TH1I *hSiL1_3Slow_Times; // Bank: Ncf0
static TH1I *hSiL1_4Slow_Times; // Bank: Ndf0
static TH1I *hSiR1_1Slow_Times; // Bank: Nef0
static TH1I *hSiR1_2Slow_Times; // Bank: Nff0
static TH1I *hSiR1_3Slow_Times; // Bank: Ngf0
static TH1I *hSiR1_4Slow_Times; // Bank: Nhf0

static det2histMap DetectorToTimeHistMap =
	create_map<std::string, TH1I *>
	("muSc", hmuSc_Times)
	("muScA", hmuScA_Times)
	("ScL", hScL_Times)
	("ScR", hScR_Times)
	("ScGe", hScGe_Times)
	//("blank", hBlank_Times)
	//("blank", hBlank_Times)
	//("blank", hBlank_Times)

	("SiL2Slow", hSiL2Slow_Times)
	("SiR2Slow", hSiR2Slow_Times)
	("GeSlow", hGeSlow_Times)
	//("blank", hBlank_Times)

	("SiL2Fast", hSiL2Fast_Times)
	("SiR2Fast", hSiR2Fast_Times)
	("GeFast", hGeFast_Times)
	//("blank", hBlank_Times)


	("SiL1_1Fast", hSiL1_1Fast_Times)
	("SiL1_2Fast", hSiL1_2Fast_Times)
	("SiL1_3Fast", hSiL1_3Fast_Times)
	("SiL1_4Fast", hSiL1_4Fast_Times)
	("SiR1_1Fast", hSiR1_1Fast_Times)
	("SiR1_2Fast", hSiR1_2Fast_Times)
	("SiR1_3Fast", hSiR1_3Fast_Times)
	("SiR1_4Fast", hSiR1_4Fast_Times)

	("SiL1_1Slow", hSiL1_1Slow_Times)
	("SiL1_2Slow", hSiL1_2Slow_Times)
	("SiL1_3Slow", hSiL1_3Slow_Times)
	("SiL1_4Slow", hSiL1_4Slow_Times)
	("SiR1_1Slow", hSiR1_1Slow_Times)
	("SiR1_2Slow", hSiR1_2Slow_Times)
	("SiR1_3Slow", hSiR1_3Slow_Times)
	("SiR1_4Slow", hSiR1_4Slow_Times);
	

#endif /* end of include guard: DETECTORMAP_H_MPIL1BS0 */

