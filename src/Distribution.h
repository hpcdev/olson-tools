// -*- c++ -*-
// $Id: Distribution.h,v 1.3 2005/05/12 04:27:29 olsonse Exp $
/*
 * Copyright 2004 Spencer Olson
 *
 * $Log: Distribution.h,v $
 * Revision 1.3  2005/05/12 04:27:29  olsonse
 * Fixed to for Intel 8.1 compilers.
 * Found (using intel compiler) and fixed an array overflow in BField::potential.
 * Didn't find it earlier because the array is on the stack for the function.
 *
 * Added fmacros.h file to simplify mixing fortran code with others.
 * Added alias function names for Fortran interoperability.
 *
 * Revision 1.2  2005/04/19 17:23:21  olsonse
 * Added new RKIntegrator wrapper class to allow for generic integration
 * templates.
 *
 * Also added trapfe library to help with trapping floating point exceptions.
 *
 * Fixed Distribution inverter finally (hopefull).  It no longer truncates the
 * distribution or reads from bogus memory.
 *
 * Added rk2 integrator (modified Euler) to rk.F.
 *
 * Various other fixes.
 *
 * Revision 1.1  2005/01/14 14:09:31  olsonse
 * Fixed documentation on memory.h, msh.h, options.h.
 * Moved new files Distribution.[hC] listutil.h readData.h from dsmc code.
 *
 * Revision 1.1.1.1  2005/01/08 04:57:25  olsonse
 * Initial CVS import (finally).
 *
 *
 */

/** \file
 * Generic distribution interpolator.
 * Copyright 2004 Spencer Olson.
 */


#ifndef DISTRIBUTION_H
#define DISTRIBUTION_H


#include <stdexcept>
#include <math.h>
#if defined (BUILD_TOOLS)
#  include "random/random.h"
#else
#  include "random.h"
#endif
#include "physical.h"

/** A generic class to invert distributions.
 * The user needs to provide a class or struct that contains a 'distrib'
 * function that will return the value of the (un)normalized distribution at a
 * given independent variable.
 *
 * @see Distribution::Distribution constructor.
 * */
class Distribution {
  public:
    /** Distribution constructor.
     * This is templated constructor to allow for various types of classes or
     * structs to provide the distribution function.  A simple and basic
     * example of a struct that might be used to define the distribution is as
     * follows:
     * \verbatim
        struct ThermalDistrib {
            inline double distrib (const double & v) const {
                return v*v * exp(-0.5 * 87.0 * amu * v*v / ( K_B * 500.0 * uK ) );
            }
        };
     * \endverbatim
     *
     * @param distro
     *     Distribution class reference; the distribution class must have a
     *     function of type:  double distrib(const double & x) const.
     * @param min
     *     Minimum of independent variable to include in distribution
     *     inversion.
     * @param max
     *     Maximum of independent variable to include in distribution
     *     inversion.
     * @param nbins
     *     Number of bins to use in distribution inversion.
     */
    template <class T>
    inline Distribution( const T & distro,
                  const double & min, const double & max,
                  const int & nbins = 100 ) : L(nbins) {

        if (L <= 1) {
            throw std::runtime_error("Distribution needs more than one bin.");
        }

        double dx = (max-min) / (L-1.0);

        double * ptmp = new double[L];

        /* integrate the distribution */
        ptmp[0] = distro.distrib(min);
        int i = 1;
        for (double x = min+dx; i < L; x+=dx, i++) {
            ptmp[i] = distro.distrib(x) + ptmp[i-1];
        }

        {   /* normalize the sum. */
            register double qmax = ptmp[L-1];
            for (i = 0; i < L; ptmp[i++] /= qmax);
        }

        /* now we invert the distribution by using the integral. */
        q = new double[L+1];
        double dprob = (ptmp[L-1] - ptmp[0]) / L;

        for (i = 0; i <= L; i++) {
            /* the ith probability: */
            double prob = i*dprob + ptmp[0];

            /* find the probability location above this one. */
            int j = 1;
            while (j < (L-1) && ptmp[j] < prob) j++;

            /* now we use linear interpolation to determine the input value at
             * this probability.  Because j starts at 1 above, this should
             * also take care of extrapolation towards the zero probability
             * too.  The j < (L-1) takes care of extrapolation towards 1
             * (although not the best, since it is extrapolated from the L-2
             * point.  oh well).
             */

            q[i] = (min+(j-1)*dx)
                 + (dx / (ptmp[j] - ptmp[j-1]))
                   * (prob    - ptmp[j-1]);
        }

        /* cleanup */
        delete[] ptmp;
    }

    ~Distribution();

    /** Get a random number from this distribution.
     * This calls lever().
     * @see lever().
     */
    inline double operator() (void) const {
        //return q[(int) rint(MTRNGrand() * (L+0.999999)) ];
        return lever();
    }

    /** Get a random number from this distribution. */
    inline double lever() const {
        double r = MTRNGrand() * L;
        register int ri = int(r);
        return q[ri] + (q[ri+1] - q[ri]) * (r - ri);
    }

  private:
    int L;
    double * q; /* length L + 1 */
};

/** A 3D thermal distribution for use.
*/
class MaxwellianDistrib3D {
  public:
    /** Contructor for 3D Maxwell distribution.
     * @param b
     *    Set \f$ \beta \f$ for the distribution defined by 
     *    \f$ v^{2} {\rm e}^{-\beta v^{2}} \f$.
     */
    inline MaxwellianDistrib3D(const double & b) : beta(b) {}
    double beta;

    /** Evaluate \f$ v^{2} {\rm e}^{-\beta v^{2}} \f$.
     * @param v
     *     Independent variable.
     */
    inline double distrib (const double & v) const {
        return v*v * exp(-v*v * beta );
    }
};

/** A 2D thermal distribution for use.
*/
class MaxwellianDistrib2D {
  public:
    /** Contructor for 2D Maxwell distribution.
     * @param b
     *    Set \f$ \beta \f$ for the distribution defined by 
     *    \f$ v {\rm e}^{-\beta v^{2}} \f$.
     */
    inline MaxwellianDistrib2D(const double & b) : beta(b) {}
    double beta;

    /** Evaluate \f$ v {\rm e}^{-\beta v^{2}} \f$.
     * @param v
     *     Independent variable.
     */
    inline double distrib (const double & v) const {
        return v * exp(-v*v * beta );
    }
};

/** Gaussian distribution for use.
*/
class GaussianDistrib {
  public:
    /** Contructor for gaussian distribution.
     * @param b
     *    Set \f$ \beta \f$ for the distribution defined by 
     *    \f$ {\rm e}^{-\beta v^{2}} \f$.
     */
    inline GaussianDistrib(const double & b) : beta(b) {}
    double beta;

    /** Evaluate \f$ {\rm e}^{-\beta v^{2}} \f$.
     * @param v
     *     Independent variable.
     */
    inline double distrib (const double & v) const {
        return exp(-v*v * beta);
    }
};

#endif // DISTRIBUTION_H
