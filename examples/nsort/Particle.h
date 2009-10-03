#include <olson-tools/Vector.h>


struct Particle {
    typedef std::vector<Particle> list;

    Particle(const olson_tools::Vector<double,3> & x = 0.0,
             const olson_tools::Vector<double,3> & v = 0.0,
             const int & type = 0) : x(x), v(v), type(type) {}
    olson_tools::Vector<double, 3> x;
    olson_tools::Vector<double, 3> v;
    int type;
};


std::ostream & operator<< (std::ostream & out, const Particle & p) {
    return out << "{x: (" << p.x[0] << ", " << p.x[1] << ", " << p.x[2] << "), "
                   "v: (" << p.v[0] << ", " << p.v[1] << ", " << p.v[2] << "), "
                   "t: " << p.type   << '}';
}

void initPVector(Particle::list & pv, const int & n, const int & n_type) {
    static const unsigned int SEED = time(NULL);
    using olson_tools::Vector;
    srand(SEED);
    pv.resize(n);
    for (int i = 0; i < n; i++) {
        Vector<double,3> x = olson_tools::V3(
                            100.0*rand()/((double)RAND_MAX) - 50.0,
                            100.0*rand()/((double)RAND_MAX) - 50.0,
                            100.0*rand()/((double)RAND_MAX) - 50.0
                         ),
                         v = olson_tools::V3(
                            100.0*rand()/((double)RAND_MAX) - 50.0,
                            100.0*rand()/((double)RAND_MAX) - 50.0,
                            100.0*rand()/((double)RAND_MAX) - 50.0
                         );
        int type = (int)rint( ((double)(n_type-1)*rand())/((double)RAND_MAX) );
        pv[i] = Particle(x,v,type);
    }
}

void initPtrVector( Particle::list  & pv,
                    std::vector<Particle*> & ptrv,
                    const int & n, const int & n_type) {
    /* with ptr array */
    initPVector(pv, n, n_type);
    ptrv.resize(n);
    for (unsigned int i = 0; i < pv.size(); ++i)
        ptrv[i] = & pv[i];
}









/* comparators for std::sort and similar routines. */
template <unsigned int dir>
struct position_comp {
    const double pivot;
    position_comp(const double & pivot = 0.0) : pivot(pivot) {}
    bool operator() (const Particle & lhs, const Particle & rhs) {
        return lhs.x[dir] < rhs.x[dir];
    }
};

struct type_comp {
    bool operator() (const Particle & lhs, const Particle & rhs) {
        return lhs.type < rhs.type;
    }
};

