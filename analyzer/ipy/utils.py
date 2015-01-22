import math
import numpy

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
    # Below are (value, error) pairs
    eff_a = (0.06736, 7.919e-03)
    eff_b = (-0.8383, 1.831e-02)
    en_m  = (0.1219,  1.752e-06)
    en_b  = (-0.5529, 8.079e-03)

    def __init__(self, eff_a=eff_a, eff_b=eff_b, en_m=en_m, en_b=en_b):
        self.eff_a = eff_a
        self.eff_b = eff_b
        self.en_m  = en_m
        self.en_b  = en_b

    def Eff(self, e):
        return self.eff_a[0] * e ** self.eff_b[0]

    def En(self, adc):
        return self.en_m[0]*adc + self.en_b[0]

    def ErrEff(self, e):
        return (e ** self.eff_b[0] * self.eff_a[1] ** 2. +
                self.Eff(e) * numpy.log(e) * self.eff_b[1] ** 2.) ** 0.5

    def ErrEn(self, adc):
        return ((adc*self.en_m[1]) ** 2. + self.en_b ** 2.) ** 0.5


class Gauss:
    rt2pi = (2.*math.pi)**0.5
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
        return self.amp*math.exp(-0.5*((x-self.mean)/self.sig)**2.)

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
    return ( (x2[0]*math.log(x1[1])-x1[0]*math.log(x2[1]))/(x2[0]-x1[0]),
             math.log(x2[1]/x1[1])/(x2[0]-x1[0]) )
