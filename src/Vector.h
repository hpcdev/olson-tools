// -*- c++ -*-
// $Id: Vector.h,v 1.1.1.1 2005/01/08 04:57:25 olsonse Exp $
/*
 * Copyright 2004 Spencer Olson
 *
 * $Log: Vector.h,v $
 * Revision 1.1.1.1  2005/01/08 04:57:25  olsonse
 * Initial CVS import (finally).
 *
 *
 */

/** \file
 * Generic Vector and SquareMatrix implementations that work together nicely.
 * I would hardly recommend using this stuff for time-sensitive operations
 * because of the inefficiency of using temporaries all over the place.  For
 * time-insensitive operations, these classes are really helpful as the code
 * is easier to write, looks cleaner, and much easier to read.
 * 
 * One <em>could</em> use parts of these classes in time-sensitive operations
 * if one is careful.  I don't want to go into all the caveats for
 * time-sensitive operations, but be assured they can be worked around.
 *
 * Copyright 2004 Spencer Olson
 */


#ifndef VECTOR_H
#define VECTOR_H

#include <string.h>
#include <ostream>
#include <math.h>
#include <stdarg.h>
#include <stdint.h>
#include "m_eps.h"
#include "power.h"

/** A simple define to help make it easier and cleaner to initialize
 * 3-Vectors (of a certain type).
 * Because this uses va_arg stuff: single byte types are only possible with
 * specific implementations of stdarg.h (such as Intel's).  GCC will issue a
 * warning and then barf if the code is actually executed.
 * The safest way to handle single byte types (bools for example) is to
 * promote them to something larger and then cast them back.  V3tcast will do
 * this for you.  For booleans, see V3b(...).
 *
 * For single byte types, V3t_ can be used, and if that doesn't work (some
 * compilers complain), then use V3tcast.
 */
#define V3t(type,a,b,c)       __CONCAT(vect3,type)(VInit,((type)(a)),((type)(b)),((type)(c)))

#define V3tcast(type,cast_type,a,b,c)   __CONCAT(vect3,type)((cast_type)0,VInit,((cast_type)(a)),((cast_type)(b)),((cast_type)(c)))

/** Another simple define to help make it easier and cleaner to init 3-vectors
 * (of a given type).  This macro can usually (compiler dependent) only
 * helpful if the arguments are primitive constants.
 */
#define V3t_(type,a,b,c)       (*(Vector<type,3>*)((const type[3]){((type)(a)),((type)(b)),((type)(c))}))

/** A simple define to help make it easier and cleaner to initialize
 * 3-Vectors (of doubles). */
#define V3(a,b,c)       V3t(double,a,b,c)

/** A simple define to help make it easier and cleaner to initialize
 * 3-Vectors (of bools). */
#define V3b(a,b,c)       V3tcast(bool,uint32_t,a,b,c)


/** Simple define to help casting static N-element arrays to
 * N-Vectors.
 */
#define VNCAST(t,n,a)          (*((Vector<t,n>*)(a)))

/** Another simple define to help casting static 3-element arrays to
 * 3-Vectors.
 */
#define V3C(a)          VNCAST(double,3,a)

typedef struct { int dontbugme; } VInit_t;
#define VInit   ((VInit_t *)NULL)


/** Vector class of arbitrary type.  The idea here is to provide a clean
 * interface to vector calculations that are not too slow.  The size is
 * compile time and the compiler may opt to unroll loops and perform other
 * optimizations.   
 */
template <class T, unsigned int L>
class Vector {
  public:
    /** The internal storage array of the vector elements. */
    T val[L];

    /** Default constructor does not do anything. */
    inline Vector () {}

    /** Copy constructor--from a Vector. */
    inline Vector (const Vector & that) {*this = that;}

    /** Copy constructor--from an array. */
    inline Vector (const T that[L]) {*this = that;}

    /** Assignment constructor.
     * Assigns all elements of the vector to the given value that.
     */
    inline Vector (const T that) {*this = that;}

    /** Copy list of values into Vector.
     * @param dummy
     *     a bogus pointer (casting NULL appropriately is fine).
     */
    inline Vector (const VInit_t * dummy, ...) {
        va_list ap;
        va_start(ap,dummy);
        for (unsigned int i = 0; i < L; i++) this->val[i] = va_arg(ap,T);
        va_end(ap);
    }

    /** Copy list of values into Vector.
     * @param another_dummy
     *     a bogus value of a given type (casting 0 works fine).  This
     *     constructor assumes that all values passed in are of this type.
     *     The values will then be cast back to the type of the Vector.
     * @param dummy
     *     a bogus pointer (casting NULL appropriately is fine).
     * @see V3tcast macro to use the more easily.
     */
    template <class TP>
    inline Vector (const TP another_dummy, const VInit_t * dummy, ...) {
        va_list ap;
        va_start(ap,dummy);
        for (unsigned int i = 0; i < L; i++) this->val[i] = (T)va_arg(ap,TP);
        va_end(ap);
    }

    /** The length of the val array. */
    inline unsigned int length() const { return L;}

    /** Zero the Vector. */
    inline void zero () {
        memset (&this->val[0], 0, sizeof(T)*L);
    }

    /** Apply fabs(double) to all elements and save.  
     * fabs(double) is called regardless of the actual type of the vector. */
    inline void save_fabs() {
        for (unsigned int i = 0; i < L; i++) this->val[i] = fabs(this->val[i]);
    }

    /** Apply sqrt(double) to all elements and save. */
    inline void save_sqrt() {
        for (unsigned int i = 0; i < L; i++) this->val[i] = sqrt(this->val[i]);
    }

    /** Index operator--non-const version. */
    inline T & operator[](const int & i) { return val[i]; }

    /** Index operator--const version. */
    inline const T & operator[](const int & i) const { return val[i]; }

    /** Assignment operator--from Vector of different type. 
     * The assignment employs an explicit cast from T2 to T. */
    template <class T2>
    inline const Vector & operator=(const Vector<T2,L>& that) {
        for (unsigned int i = 0; i < L; i++) this->val[i] = (T)that.val[i];
        return *this;
    }

    /** Assignment operator--from Vector of same type. */
    inline const Vector & operator=(const Vector & that) {
        memcpy (&this->val[0], &that.val[0], sizeof(T)*L);
        return *this;
    }

    /** Assignment operator--from array of same type. */
    inline const Vector & operator=(const T that[L]) {
        memcpy (&this->val[0], &that[0], sizeof(T)*L);
        return *this;
    }

    /** Assignment operator--from scalar value.  All elements will be assigned
     * to this scalar.  */
    inline const Vector & operator=(const T & that) {
        for (unsigned int i = 0; i < L; i++) this->val[i] = that;
        return *this;
    }

    /** Inner product of two Vectors. */
    inline T operator*(const Vector & that) const {
        T retval(0);
        for (unsigned int i = 0; i < L; i++) retval += this->val[i]*that.val[i];
        return retval;
    }

    /** Vector X Scalar  multiplication. */  
    inline Vector operator*(const T & that) const {
        Vector ret;
        for (unsigned int i = 0; i < L; i++) ret.val[i] = this->val[i] * that;
        return ret;
    }

    /** Scalar X Vector multiplication (friend method). */  
    template <class TT, unsigned int LL>
    friend inline Vector operator*(const T & that, const Vector & v);

    /** Vector X Scalar immediate multiplication. */  
    inline const Vector & operator*=(const T & that) {
        for (unsigned int i = 0; i < L; i++) this->val[i] *= that;
        return *this;
    }

    /** Vector X Scalar division. */  
    inline Vector operator/(const T & that) const {
        Vector ret;
        for (unsigned int i = 0; i < L; i++) ret.val[i] = this->val[i] / that;
        return ret;
    }

    /** Vector X Scalar immediate division. */  
    inline const Vector & operator/=(const T & that) {
        for (unsigned int i = 0; i < L; i++) this->val[i] /= that;
        return *this;
    }

    /** Vector - Vector subtraction. */  
    inline Vector operator-(const Vector& that) const {
        Vector retval;
        for (unsigned int i = 0; i < L; i++) retval.val[i] = this->val[i]-that.val[i];
        return retval;
    }

    /** Vector - Scalar subtraction. */  
    inline Vector operator-(const T& that) const {
        Vector retval;
        for (unsigned int i = 0; i < L; i++) retval.val[i] = this->val[i]-that;
        return retval;
    }

    /** Vector - Scalar immediate subtraction. */  
    inline const Vector & operator-=(const T& that) {
        for (unsigned int i = 0; i < L; i++) this->val[i] -= that;
        return *this;
    }

    /** Vector - Vector immediate subtraction. */  
    inline const Vector & operator-=(const Vector& that) {
        for (unsigned int i = 0; i < L; i++) this->val[i] -= that.val[i];
        return *this;
    }

    /** Vector + Vector subtraction. */  
    inline Vector operator+(const Vector& that) const {
        Vector retval;
        for (unsigned int i = 0; i < L; i++) retval.val[i] = this->val[i]+that.val[i];
        return retval;
    }

    /** Vector + Scalar subtraction. */  
    inline Vector operator+(const T& that) const {
        Vector retval;
        for (unsigned int i = 0; i < L; i++) retval.val[i] = this->val[i]+that;
        return retval;
    }

    /** Vector + Vector immediate subtraction. */  
    inline const Vector & operator+=(const Vector& that) {
        for (unsigned int i = 0; i < L; i++) this->val[i] += that.val[i];
        return *this;
    }

    /** Vector + Scalar immediate subtraction. */  
    inline const Vector & operator+=(const T& that) {
        for (unsigned int i = 0; i < L; i++) this->val[i] += that;
        return *this;
    }

    /** component by component multiplication after type of that is converted
     * type of this.
     * @return reference to this (type Vector<T,L>).
     */
    template <class T2>
    inline const Vector & compMult(const Vector<T2,L>& that) {
        for (unsigned int i = 0; i < L; i++) this->val[i] *= (T)that.val[i];
        return *this;
    }

    /** component by component division after type of that is converted
     * type of this.
     * @return reference to this (type Vector<T,L>).
     */
    template <class T2>
    inline const Vector & compDiv(const Vector<T2,L>& that) {
        for (unsigned int i = 0; i < L; i++) this->val[i] /= (T)that.val[i];
        return *this;
    }

    /** Compute the magnitude of this vector. */
    inline T abs () const {
        return sqrt ((*this) * (*this));
    }


    /* ************** COMPARISON OPERATORS *************** */

    /** Cumulative comparison (gt) between Vector and a scalar.
     * Comparison between Vectors of different types are allowed to use default
     * promotion of types.
     */
    template <class T2>
    inline bool operator>(const T2& that) const {
        bool retval = true;
        for (unsigned int i = 0; i < L; i++)
            retval = retval && (this->val[i] > that);

        return retval;
    }

    /** Cumulative comparison (gteq) between Vector and a scalar.
     * Comparison between Vectors of different types are allowed to use default
     * promotion of types.
     */
    template <class T2>
    inline bool operator>=(const T2& that) const {
        bool retval = true;
        for (unsigned int i = 0; i < L; i++)
            retval = retval && (this->val[i] >= that);

        return retval;
    }

    /** Cumulative comparison (gt) between Vector and a Vector.
     * Comparison between Vectors of different types are allowed to use default
     * promotion of types.
     * @return cumulative expression of component-wise comparison.
     */
    template <class T2>
    inline bool operator>(const Vector<T2,L>& that) const {
        bool retval = true;
        for (unsigned int i = 0; i < L; i++)
            retval = retval && (this->val[i] > that.val[i]);

        return retval;
    }

    /** Cumulative comparison (gteq) between Vector and a Vector.
     * Comparison between Vectors of different types are allowed to use default
     * promotion of types.
     * @return cumulative expression of component-wise comparison.
     */
    template <class T2>
    inline bool operator>=(const Vector<T2,L>& that) {
        bool retval = true;
        for (unsigned int i = 0; i < L; i++)
            retval = retval && (this->val[i] >= that.val[i]);

        return retval;
    }

    /** Cumulative comparison (lt) between Vector and a scalar.
     * Comparison between Vectors of different types are allowed to use default
     * promotion of types.
     * @return cumulative expression of component-wise comparison.
     */
    template <class T2>
    inline bool operator<(const T2& that) {
        bool retval = true;
        for (unsigned int i = 0; i < L; i++)
            retval = retval && (this->val[i] < that);

        return retval;
    }

    /** Cumulative comparison (leq) between Vector and a scalar.
     * Comparison between Vectors of different types are allowed to use default
     * promotion of types.
     * @return cumulative expression of component-wise comparison.
     */
    template <class T2>
    inline bool operator<=(const T2& that) {
        bool retval = true;
        for (unsigned int i = 0; i < L; i++)
            retval = retval && (this->val[i] <= that);

        return retval;
    }

    /** Cumulative comparison (lt) between Vector and a Vector.
     * Comparison between Vectors of different types are allowed to use default
     * promotion of types.
     * @return cumulative expression of component-wise comparison.
    */
    template <class T2>
    inline bool operator<(const Vector<T2,L>& that) {
        bool retval = true;
        for (unsigned int i = 0; i < L; i++)
            retval = retval && (this->val[i] < that.val[i]);

        return retval;
    }

    /** Cumulative comparison (leq) between Vector and a Vector.
     * Comparison between Vectors of different types are allowed to use default
     * promotion of types.
     * @return cumulative expression of component-wise comparison.
    */
    template <class T2>
    inline bool operator<=(const Vector<T2,L>& that) {
        bool retval = true;
        for (unsigned int i = 0; i < L; i++)
            retval = retval && (this->val[i] <= that.val[i]);

        return retval;
    }

    /** The product of all components of the Vector.
    */
    inline T prod() const {
        T retval = (T)1;
        for (unsigned int i = 0; i < L; i++) {
            retval *= this->val[i];
        }
        return retval;
    }

    /** Return a type casted Vector from the original. */
    template <class T2>
    inline Vector<T2,L> to_type() const {
        Vector<T2,L> retval;
        retval = *this;
        return retval;
    }


    /** Add a fraction of another Vector to this Vector. */
    template <class T2>
    inline Vector<T,L> addFraction(const double & f, const Vector<T2,L> & that) {
        for (unsigned int i = 0; i < L; i++) this->val[i] += (T)(f*that.val[i]);
        return *this;
    }
};

/** Cumulative '==' comparison of Vector types.
 * Comparison between Vectors of different types are allowed to use default
 * promotion of types.
 */
template <class T1, class T2, unsigned int L>
inline bool operator==(const Vector<T1,L>& v1, const Vector<T2,L>& v2) {
    bool retval = true;
    for (unsigned int i = 0; i < L; i++)
        retval = retval && (v1.val[i] == v2.val[i]);

    return retval;
}

/** Cumulative '!=' comparison of Vector types.
 * Comparison between Vectors of different types are allowed to use default
 * promotion of types.
 */
template <class T1, class T2, unsigned int L>
inline bool operator!=(const Vector<T1,L>& v1, const Vector<T2,L>& v2) {
    return !(v1 == v2);
}

/** Cumulative '==' comparison of Vector types.
 * Specialization:  Comparison between Vectors of doubles.  Equivalence is
 * defined by less than 4*M_EPS.
 */
template <unsigned int L>
inline bool operator==(const Vector<double,L>& v1, const Vector<double,L>& v2) {
    bool retval = true;
    for (unsigned int i = 0; i < L; i++)
        retval = retval && ( fabs(v1.val[i] - v2.val[i]) <= fabs(4*M_EPS*v1.val[i]) ) ;

    return retval;
}

/** Scalar X Vector multiplication operator. */
template <class T, unsigned int L>
inline Vector<T,L> operator*(const T & that, const Vector<T,L> & v) {
    Vector<T,L> ret;
    for (unsigned int i = 0; i < L; i++) ret.val[i] = v.val[i] * that;
    return ret;
}

/** Vector X Vector cross product returned via a temporary Vector. */
template <class T>
inline Vector<T,3> cross (const Vector<T,3> & a, const Vector<T,3> b) {
    Vector<T,3> retval;
    retval.val[0] = a.val[1]*b.val[2] - a.val[2]*b.val[1];
    retval.val[1] = a.val[2]*b.val[0] - a.val[0]*b.val[2];
    retval.val[2] = a.val[0]*b.val[1] - a.val[1]*b.val[0];
    return retval;
}

/** Vector X Vector cross product returned via a given input buffer. */
template <class T>
inline void cross (Vector<T,3> &retval, const Vector<T,3> & a, const Vector<T,3> b) {
    retval.val[0] = a.val[1]*b.val[2] - a.val[2]*b.val[1];
    retval.val[1] = a.val[2]*b.val[0] - a.val[0]*b.val[2];
    retval.val[2] = a.val[0]*b.val[1] - a.val[1]*b.val[0];
}

/** component by component multiplication after type of that is converted
 * type of this.
 * @return reference to this (type Vector<T,L>).
 *
 * @see Vector::compDiv.
 */
template <class T1, class T2, unsigned int L>
inline const Vector<T1,L> compMult(const Vector<T1,L> & v1, const Vector<T2,L>& v2) {
    Vector<T1,L> retval;
    for (unsigned int i = 0; i < L; i++)
        retval.val[i] = v1.val[i] * (T1)v2.val[i];
    return retval;
}

/** component by component division after type of that is converted
 * type of this.
 * @return reference to (type Vector<T,L>).
 *
 * @see Vector::compMult.
 */
template <class T1, class T2, unsigned int L>
inline const Vector<T1,L> compDiv(const Vector<T1,L> & v1, const Vector<T2,L>& v2) {
    Vector<T1,L> retval;
    for (unsigned int i = 0; i < L; i++)
        retval.val[i] = v1.val[i] / (T1)v2.val[i];
    return retval;
}

/** component by component power after type of that is converted
 * type of this.
 * @return reference to (type Vector<T,L>).
 *
 * @see Vector::compMult.
 */
template <class T1, class T2, unsigned int L>
inline const Vector<T1,L> compPow(const Vector<T1,L> & v1, const Vector<T2,L>& v2) {
    Vector<T1,L> retval;
    for (unsigned int i = 0; i < L; i++)
        retval.val[i] = fast_pow(v1.val[i], (T1)v2.val[i]);
    return retval;
}

/** component by component power after type of that is converted
 * type of this.
 * @return reference to (type Vector<T,L>).
 *
 * @see Vector::compMult.
 */
template <class T,unsigned int L>
inline const Vector<T,L> compPow(const Vector<T,L> & v1, const T & e) {
    Vector<T,L> retval;
    for (unsigned int i = 0; i < L; i++)
        retval.val[i] = fast_pow(v1.val[i], e);
    return retval;
}

/** Compute the maximum value in the Vector.
 */
template <class T, unsigned int L>
inline T max (const Vector<T,L> & v) {
    T retval = v[0];
    for (unsigned int i = 1; i < L; i++) {
        T vi = v[i];
        if (retval < vi) retval = vi;
    }
    return retval;
}

/** Calculate the mean value of this vector.
 * Because of truncation, this function doesn't really mean anything unless
 * the calculation is done with at least floating point precision.  Therefore,
 * this function returns a double (doing everything with double precision).
 * The user should round and cast back appropriately.
 */
template <class T, unsigned int L>
inline double mean (const Vector<T,L> & v) {
    return ((double)sum(v)) / ((double)L);
}

/** Compute the sum of the elements of the Vector. */
template <class T, unsigned int L>
inline T sum (const Vector<T,L> & v) {
    T retval = (T)0;
    for (unsigned int i = 0; i < L; i++) {
        retval += v[i];
    }
    return retval;
}

/** Define SQR explicitly to be in the inner product of a Vector with its
 * self.  We define this specialization because the return value is not the
 * same as the arguments. */
template <class T, unsigned int L>
inline T SQR(const Vector<T,L> & v) {
    return v*v;
}

/** Stream output operator. */
template <class T, unsigned int L>
inline std::ostream & operator<<(std::ostream & output, const Vector<T,L> & v) {
    for (unsigned int i = 0; i < L; i++) output << v.val[i] << '\t';
    return output;
}

/** Stream input operator. */
template <class T, unsigned int L>
inline std::istream & operator>>(std::istream & input, Vector<T,L> & v) {
    for (unsigned int i = 0; i < L; i++) input >> v.val[i];
    return input;
}

/** Square matrix class.  The idea here is to provide a clean interface to
 * matrix calculations that are not too slow.  The size is compile time and
 * the compiler may opt to unroll loops and perform other optimizations.  
 */
template <class T, unsigned int L>
class SquareMatrix {
  public:
    /** The internal storage array of the matrix elements. */
    T val[L][L];

    /** Default constructor does nothing. */
    inline SquareMatrix() {}

    /** SquareMatrix copy constructor--from another SquareMatrix. */
    inline SquareMatrix(const SquareMatrix & that) {
        *this = that;
    }

    /** SquareMatrix copy constructor--from a two dimensional C-array. */
    inline SquareMatrix(const T that[L][L]) {
        *this = that;
    }

    /** The side length of the val matrix. */
    inline unsigned int side_length() const { return L;}

    /** Set all matrix elements to zero. */
    inline void zero () {
        memset (&this->val[0][0], 0, sizeof(T)*L*L);
    }

    /** Index operator--non-const version. */
    inline T & operator()(const int & i, const int & j) { return val[i][j]; }

    /** Index operator--const version. */
    inline const T & operator()(const int & i, const int & j) const { return val[i][j]; }

    /** Assignment operator--from SquareMatrix. */
    inline const SquareMatrix & operator=(const SquareMatrix & that) {
        memcpy (&this->val[0][0], &that.val[0][0], sizeof(T)*L*L);
        return *this;
    }

    /** Assignment operator--from two dimensional C-array. */
    inline const SquareMatrix & operator=(const T that[L][L]) {
        memcpy (&this->val[0][0], &that[0][0], sizeof(T)*L*L);
        return *this;
    }

    /** Assignment operator--from scalar. */
    inline const SquareMatrix & operator=(const T & that) {
        for (unsigned int i = 0; i < L; i++ ) {
            for (unsigned int j = 0; j < L; j++) {
                this->val[i][j] = that;
            }
        }
        return *this;
    }

    /** Matrix inner product. */
    inline SquareMatrix operator *(const SquareMatrix & that) const {
        SquareMatrix result;

        for (unsigned int i = 0; i < L; i++ ) {
            for (unsigned int j = 0; j < L; j++) {
                result.val[i][j] = 0;
                for (unsigned int k = 0; k < L; k++ ) {
                    result.val[i][j] +=  this->val[i][k] * that.val[k][j];
                }
            }
        }

        return result;
    }

    /** Matrix subtraction. */
    inline SquareMatrix operator -(const SquareMatrix & that) const {
        SquareMatrix result;

        for (unsigned int i = 0; i < L; i++ ) {
            for (unsigned int j = 0; j < L; j++) {
                result.val[i][j] = this->val[i][j] - that.val[i][j];
            }
        }

        return result;
    }

    /** Matrix addition. */
    inline SquareMatrix operator +(const SquareMatrix & that) const {
        SquareMatrix result;

        for (unsigned int i = 0; i < L; i++ ) {
            for (unsigned int j = 0; j < L; j++) {
                result.val[i][j] = this->val[i][j] + that.val[i][j];
            }
        }

        return result;
    }

    /** Matrix X Vector multiplication. */
    inline Vector<T,L> operator *(const Vector<T,L> & that) const {
        Vector<T,L> result;

        for (unsigned int i = 0; i < L; i++ ) {
            result.val[i] = 0;
            for (unsigned int j = 0; j < L; j++) {
                result.val[i] += this->val[i][j] * that.val[j];
            }
        }

        return result;
    }

    /** Matrix + Scalar. */
    inline SquareMatrix operator +(const T & that) const {
        SquareMatrix result;

        for (unsigned int i = 0; i < L; i++ ) {
            for (unsigned int j = 0; j < L; j++) {
                result.val[i][j] = this->val[i][j] + that;
            }
        }

        return result;
    }

    /** Matrix - Scalar. */
    inline SquareMatrix operator -(const T & that) const {
        SquareMatrix result;

        for (unsigned int i = 0; i < L; i++ ) {
            for (unsigned int j = 0; j < L; j++) {
                result.val[i][j] = this->val[i][j] - that;
            }
        }

        return result;
    }

    /** Matrix X Scalar immediate multiplication. */
    inline SquareMatrix & operator *=(const T & that) {
        for (unsigned int i = 0; i < L; i++ ) {
            for (unsigned int j = 0; j < L; j++) {
                this->val[i][j] *= that;
            }
        }

        return *this;
    }

    /** Matrix - Scalar immediate subtraction. */
    inline SquareMatrix & operator -=(const T & that) {
        for (unsigned int i = 0; i < L; i++ ) {
            for (unsigned int j = 0; j < L; j++) {
                this->val[i][j] -= that;
            }
        }

        return *this;
    }

    /** Matrix - Scalar immediate addition. */
    inline SquareMatrix & operator +=(const T & that) {
        for (unsigned int i = 0; i < L; i++ ) {
            for (unsigned int j = 0; j < L; j++) {
                this->val[i][j] += that;
            }
        }

        return *this;
    }

    /** Matrix - Scalar immediate division. */
    inline SquareMatrix & operator /=(const T & that) {
        for (unsigned int i = 0; i < L; i++ ) {
            for (unsigned int j = 0; j < L; j++) {
                this->val[i][j] /= that;
            }
        }

        return *this;
    }

    /** Create (in temporary storage) an identity matrix equal in size to the
     * current SquareMatrix. */
    inline static SquareMatrix identity() {
        SquareMatrix retval;
        memset(&retval, 0, sizeof(retval));
        for (unsigned int i = 0; i < L; i++ ) {
            retval.val[i][i] = 1;
        }
        return retval;
    }
};

/* these are kludgy types to be used with the kludgy V3t macro. */
typedef Vector<double,3> vect3double;
typedef Vector<float,3> vect3float;
typedef Vector<int,3> vect3int;
typedef Vector<uint32_t,3> vect3uint32_t;
typedef Vector<bool,3> vect3bool;

#endif // VECTOR_H
