#include "kissfft.hh"
#include <iostream>
#include <cstdlib>
#include <typeinfo>

#include <sys/time.h>
static inline
double curtime(void)
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (double)tv.tv_sec + (double)tv.tv_usec*.000001;
}

using namespace std;

template <class T>
void dotest(int nfft)
{
    typedef kissfft<T> FFT;
    typedef std::complex<T> cpx_type;

    cout << typeid(T).name() << ":nfft=" << nfft <<endl;

    FFT fft(nfft,false);

    vector<cpx_type> inbuf(nfft);
    vector<cpx_type> outbuf(nfft);
#if 0
    for (int k=0;k<nfft;++k) 
        inbuf[k]= cpx_type(
               cosl(2*k* M_PIl / nfft ),
               sinl(2*k* M_PIl / nfft ) );
#else
    for (int k=0;k<nfft;++k)
        inbuf[k]= cpx_type( 
                (T)(rand()/(double)RAND_MAX - .5),
                (T)(rand()/(double)RAND_MAX - .5) );
#endif
    fft.transform( &inbuf[0] , &outbuf[0] );

    long double totalpower=0;
    long double difpower=0;
    for (int k0=0;k0<nfft;++k0) {
        complex<long double> acc = 0;
        long double phinc = 2*k0* M_PIl / nfft;
        for (int k1=0;k1<nfft;++k1) {
            complex<long double> x(inbuf[k1].real(),inbuf[k1].imag()); 
            acc += x * exp( complex<long double>(0,-k1*phinc) );
        }
        totalpower += norm(acc);
        complex<long double> x(outbuf[k0].real(),outbuf[k0].imag()); 
        complex<long double> dif = acc - x;
        difpower += norm(dif);
    }
    cout << "RMSE:" << sqrt(difpower/totalpower) << "\t";

    double t0 = curtime();
    int nits=20e6/nfft;
    for (int k=0;k<nits;++k) {
        fft.transform( &inbuf[0] , &outbuf[0] );
    }
    double t1 = curtime();
    cout << "MSPS:" << ( (nits*nfft)*1e-6/ (t1-t0) ) << endl;
}

int main(int argc,char ** argv)
{
    dotest<float>(32);
    dotest<double>(32);
    dotest<long double>(32);

    dotest<float>(1024); dotest<double>(1024); dotest<long double>(1024);
    dotest<float>(1800); dotest<double>(1800); dotest<long double>(1800);
    return 0;
}