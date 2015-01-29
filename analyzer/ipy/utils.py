from math import pi, exp, log

# Found this on the internet
class ListTable(list):
    def _repr_html_(self):
        html = ["<table>"]
        
        br = True
        for row in self:
            html.append("<tr>")
            for col in row:
                if br:
                    html.append("<th>{0}</th>".format(col))
                else:
                    html.append("<td>{0}</td>".format(col))
            br = False
            html.append("</tr>")

        html.append("</table>")
        return ''.join(html)


class Ge:
    eff_0   =  0.06736
    eff_1   = -0.8383
    eff_cor = ( (  1. ,      -9.954e-01  ),
                ( -9.954e-01, 1.         ) )
    eff_cov = ( (  6.271e-05, -1.443e-04 ),
                ( -1.443e-04,  3.353e-04 ) )
    en_0    =  0.1219
    en_1    = -0.5529
    en_cor  = ( (  1.,       -8.649e-01  ),
                ( -8.649e-01, 1.         ) )
    en_cov  = ( (  1.908e-04, -2.79e-08  ),
                ( -2.79e-08,   5.457e-12 ) )

    def __init__(self,
                 eff_0=eff_0, eff_1=eff_1, eff_cor=eff_cor, eff_cov=eff_cov,
                 en_0=en_0,   en_1=en_1,   en_cor=en_cor,   en_cov=en_cov):
        self.eff_0 = eff_0
        self.eff_1 = eff_1
        self.en_0  = en_0
        self.en_1  = en_1

    def Eff(self, e):
        return self.eff_0 * e ** self.eff_1

    def En(self, adc):
        return self.en_0 + self.en_1*adc

    def ErrEff(self, e):
        return e**self.eff_1*self.eff_cov[0][0]+self.eff_0*e**self.eff_1*log(e)*self.eff_cov[1][1]+2*self.eff_0*e**(2.*self.eff_1)*log(e)*self.eff_cov[0][1]

    def ErrEn(self, adc):
        return (adc**2*self.en_cov[0][0]+self.en_cov[1][1]+2.*adc*self.en_cov[0][1])**0.5


class Gauss:
    rt2pi = (2.*pi)**0.5
    def __init__(self, a=None, x=None, s=None, ea=0., ex=0., es=0., fit_result=None, first_param=None):
        if not fit_result:
            self.amp     = a
            self.mean    = x
            self.sig     = s
            self.erramp  = ea
            self.errmean = ex
            self.errsig  = es
        else:
            self.amp     = fit_result.Parameter(first_param)
            self.mean    = fit_result.Parameter(first_param+1)
            self.sig     = fit_result.Parameter(first_param+2)
            self.erramp  = fit_result.ParError(first_param)
            self.errmean = fit_result.ParError(first_param+1)
            self.errsig  = fit_result.ParError(first_param+2)

    def __call__(self, x):
        return self.amp*exp(-0.5*((x-self.mean)/self.sig)**2.)

    def GetCount(self):
        return Gauss.rt2pi*self.amp*self.sig

    def GetCountError(self):
        return ((self.erramp/self.amp)**2.+(self.errsig/self.sig)**2.)**0.5

    def Parameters(self):
        return [self.amp, self.mean, self.sig]


class PolN:
    def __init__(self, c=None, e=[], fit_result=None, first_param=None, order=None):
        if not fit_result:
            self.coef = c
            self.err  = e
        else:
            self.coef = []
            self.err  = []
            for i in range(order+1):
                self.coef.append(fit_result.Parameter(first_param+i))
                self.err.append(fit_result.ParError(first_param+i))

    def __call__(self, x):
        res = 0.
        for i in range(len(self.coef)):
            res += self.coef[i]*x**i
        return res

    def Parameters(self):
        return self.coef

def CountEnteringMuons(h):
    res = 0.
    for i in range(1, h.GetNbinsX()+1):
        res += h.GetBinContent(i)
    return res

def GuessExpoParams(x1, x2):
    return ( (x2[0]*log(x1[1])-x1[0]*log(x2[1]))/(x2[0]-x1[0]),
             log(x2[1]/x1[1])/(x2[0]-x1[0]) )
