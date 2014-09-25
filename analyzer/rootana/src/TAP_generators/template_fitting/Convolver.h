#ifndef TEMPLATE_FITTING_CONVOLVER_H
#define TEMPLATE_FITTING_CONVOLVER_H

#include "debug_tools.h"
#include <TH1.h>
#include <iostream>
#include <utility>

namespace Algorithm{
   class TH1_c_iterator;
   class TH1_wrapper;
   template < typename WeightIterator>
   class Convolver;
   template < typename InputIterator>
   class Pedestal_iterator;
}

class Algorithm::TH1_c_iterator:public std::iterator<std::random_access_iterator_tag, double>{
   public:
     TH1_c_iterator():fHist(NULL),fCurrentBin(0),fScale(1.){}
     TH1_c_iterator(const TH1* hist, int init=1,double scale=1.):fHist(hist),fCurrentBin(init),fScale(scale){}
     TH1_c_iterator(const TH1_c_iterator& rhs):fHist(rhs.fHist),fCurrentBin(rhs.fCurrentBin),fScale(rhs.fScale){}
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
       return fHist?fHist->GetBinContent(fCurrentBin)*fScale:0;
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
     double fScale;
};

struct Algorithm::TH1_wrapper{
    TH1_wrapper(const TH1*  hist, double scale=1.): fHisto(hist), fScale(scale){};
    TH1_c_iterator begin(int skip=0)const{return TH1_c_iterator(fHisto,1+skip,fScale);}
    TH1_c_iterator end(int skip=0)const{return TH1_c_iterator(fHisto,fHisto->GetNbinsX()-skip,fScale);}

    private:
    const TH1* fHisto;
    double fScale;
};

template <typename InputIterator >
struct Algorithm::Pedestal_iterator:public InputIterator{
typedef std::pair<TH1_c_iterator, TH1_c_iterator> HistIter;
typedef std::vector<HistIter> HistList;
     Pedestal_iterator(const InputIterator& it, double ped):InputIterator(it),fPedestal(ped){}
     Pedestal_iterator(const InputIterator& it):InputIterator(it),fPedestal(0){}
     double operator*()const{
        ;
        double val=InputIterator::operator*() - fPedestal;
        for(HistList::const_iterator i_hist=fHistograms.begin(); i_hist!=fHistograms.end(); ++i_hist){
           val-=*(i_hist->first);
        }
       return val;
     }
     void operator++(){
        InputIterator::operator++();
        for(HistList::iterator i_hist=fHistograms.begin(); i_hist!=fHistograms.end(); ){
           ++(i_hist->first);
           if( i_hist->first == i_hist->second ){
             fHistograms.erase(i_hist); 
	     continue;
	   }
           ++i_hist;
	}
     }
     void AddHistogram(TH1* hist,double scale){
       TH1_wrapper wrap(hist, scale);
       fHistograms.push_back(std::make_pair(wrap.begin(), wrap.end()));
     }

     private:
     double fPedestal;
     HistList fHistograms;
};

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
      OutputIterator Process(const InputIterator& start, const InputIterator& stop, OutputIterator result){
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

#endif// TEMPLATE_FITTING_CONVOLVER_H
