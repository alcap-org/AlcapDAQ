#ifndef TEMPLATE_FITTING_CONVOLVER_H
#define TEMPLATE_FITTING_CONVOLVER_H

#include "debug_tools.h"
#include <TH1.h>
#include <iostream>

namespace Algorithm{
   class TH1_c_iterator;
   class TH1_wrapper;
   template < typename WeightIterator>
   class Convolver;
}

template < typename WeightIterator>
class Algorithm::Convolver{
   public:
      Convolver( WeightIterator begin, WeightIterator end):fNWeights(end-begin),fBegin(begin), fEnd(end){
      }
      
      template <typename InputIterator >
      double operator()(const InputIterator& start){
        double ret_val=0;
        InputIterator i_in=start;
        for(WeightIterator i_weight=fBegin; i_weight!=fEnd; ++i_weight){
           ret_val+=(*i_weight)*(*i_in);
           ++i_in;
        }
        return ret_val;
      }

      template <typename InputIterator, typename OutputIterator >
      OutputIterator operator()(const InputIterator& start, const InputIterator& stop, OutputIterator result){
        InputIterator end=stop-fNWeights;
        for(InputIterator i_in=start;i_in!=end;++i_in){
           *(result)=operator()(i_in);
           ++result;
        }
        return result;
      }

   private:
      int fNWeights;
      WeightIterator fBegin;
      WeightIterator fEnd;
};

class Algorithm::TH1_c_iterator:public std::iterator<std::random_access_iterator_tag, double>{
   public:
     TH1_c_iterator():fHist(NULL),fCurrentBin(0){}
     TH1_c_iterator(const TH1* hist, int init=1):fHist(hist),fCurrentBin(init){}
     TH1_c_iterator(const TH1_c_iterator& rhs):fHist(rhs.fHist),fCurrentBin(rhs.fCurrentBin){}
     TH1_c_iterator operator++(){
       ++fCurrentBin;
       return *this;
     }
     TH1_c_iterator operator++(int){
       TH1_c_iterator tmp(*this);
       ++fCurrentBin;
       return tmp;
     }
     TH1_c_iterator operator--(){
       --fCurrentBin;
       return *this;
     }
     TH1_c_iterator operator--(int){
       TH1_c_iterator tmp(*this);
       --fCurrentBin;
       return tmp;
     }
     double operator*()const{ 
       return fHist?fHist->GetBinContent(fCurrentBin):0;
     }
     TH1_c_iterator operator+=(int n){
       fCurrentBin+=n;
       return *this;
     }

     TH1_c_iterator operator+(int n)const{ 
       TH1_c_iterator tmp(*this);
       tmp.fCurrentBin+=n;
       return tmp;
     }
     TH1_c_iterator operator-(int n)const{ 
       TH1_c_iterator tmp(*this);
       tmp.fCurrentBin-=n;
       return tmp;
     }
     bool operator!=(const TH1_c_iterator& rhs)const{
       return fCurrentBin!=rhs.fCurrentBin || fHist!=rhs.fHist ;
     }
     bool operator==(const TH1_c_iterator& rhs)const{
       return  fCurrentBin==rhs.fCurrentBin && fHist==rhs.fHist;
     }
     int operator-(const TH1_c_iterator& rhs)const{ 
       return fCurrentBin-rhs.fCurrentBin;
     }

   private:
     const TH1* fHist;
     int fCurrentBin;
};

struct Algorithm::TH1_wrapper{
    TH1_wrapper(const TH1*  hist): fHisto(hist){};
    TH1_c_iterator begin(int skip=0)const{return TH1_c_iterator(fHisto,1+skip);}
    TH1_c_iterator end(int skip=0)const{return TH1_c_iterator(fHisto,fHisto->GetNbinsX()-skip);}

    private:
    const TH1* fHisto;
};

#endif// TEMPLATE_FITTING_CONVOLVER_H
