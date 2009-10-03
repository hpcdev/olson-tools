#include <olson-tools/random/random.h>
#include <olson-tools/Distribution.h>
#include <olson-tools/KeyedBin.h>

#include <physical/physical.h>

#include <iostream>
#include <fstream>
#include <cmath>
#include <cstdlib>

using namespace physical::constants;

/** A flat distribution for use.
*/
struct FlatDistribution {
  /** Return 0.5.  */
  inline double distrib (const double & x) const {
    return 0.5;
  }
};

int main() {
  olson_tools::KeyedBin<double,double,3,201> bin(-0.5,0.5);

  int iter = 0;
  std::cout << "Enter the nubmer of samples:  "
            << std::flush;
  std::cin >> iter;
  if (iter == 0) return EXIT_FAILURE;
  std::cout << iter << " samples requested." << std::endl;

  FlatDistribution flat;

  olson_tools::Distribution distro(flat, -0.5, 0.5, 1000);
  for (int i = 0; i < iter; i++) {
    using olson_tools::Vector;
    using olson_tools::V3;
    double x = distro();
    Vector<double,3> v =
      V3( std::log(std::abs(x)), std::exp(x), std::pow(2.0,x) );
    bin.bin(distro(),v);
  }

  std::ofstream outf("bin.dat");
  bin.print(outf,"");
  outf.close();
  return 0;
}
