#ifndef TEMPLATEFASTFITPARAMETERS_H 
#define TEMPLATEFASTFITPARAMETERS_H 

#include "definitions.h"
#include <TObject.h>

struct TemplateFastFitParameters:public TObject{
	double determinant;
	double n;
	double sum_y;
	double sum_y_T;
	double sum_sq_T;
	double sum_T;
	TemplateFastFitParameters(double a, double b, double c, double d, double e,double f):
		determinant(a),n(b),sum_y(c),sum_y_T(d),sum_sq_T(e),sum_T(f) { }
	TemplateFastFitParameters():
		determinant(definitions::DefaultValue),
                n(definitions::DefaultValue),
                sum_y(definitions::DefaultValue),
                sum_y_T(definitions::DefaultValue),
                sum_sq_T(definitions::DefaultValue),
                sum_T(definitions::DefaultValue) { }

	ClassDef(TemplateFastFitParameters,1);
};

#endif // TEMPLATEFASTFITPARAMETERS_H 
