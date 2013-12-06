#include "TVacuumData.h"

#include <cmath>
#include <sstream>
#include <cstdlib>

#include "TF1.h"

using std::vector;
using std::string;

TVacuumData::TVacuumData()
{
	Reset();
}

TVacuumData::TVacuumData(
		std::vector<int> time, std::vector<int> status, 
		std::vector<float> pressure)
{
	Reset();
	fStatus = status;
	fPressure = pressure;
	fTime = time;
}

void TVacuumData::Reset(Option_t* o)
{
	fStatus.clear();
	fPressure.clear();
	fTime.clear();
}

