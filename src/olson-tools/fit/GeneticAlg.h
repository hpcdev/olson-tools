// -*- c++ -*-
// $Id: GeneticAlg.h,v 1.2 2005/06/09 08:50:42 olsonse Exp $
/*
 * Copyright 1998-2005 Spencer Eugene Olson --- All Rights Reserved
 *
 * $Log: GeneticAlg.h,v $
 * Revision 1.2  2005/06/09 08:50:42  olsonse
 * Style changes mostly.  Bug fixes in use of Histogram to encourage diversity.
 *
 * Revision 1.1  2005/06/07 20:38:11  olsonse
 * Fixed the old genetic algorithm files.  They compile.  Hopefully they work.
 *
 * Revision 1.1.1.1  2005/01/08 04:27:25  olsonse
 * Initial import
 *
 * Revision 1.8  2000/07/20 00:10:58  olsons
 * Changed debug scheme and default debug level to 0.
 * Added setParam function to be able to call from an interface.
 *
 * Revision 1.7  2000/06/15 19:31:00  olsons
 * Made debug_level act more like levels rather than codes.
 *
 * Revision 1.6  2000/05/06 20:37:18  olsons
 * Reorganized libfit: made it self-containing and use new definition of alleles.
 *
 * Revision 1.5  1999/10/18 18:56:34  olsons
 * Added documentation in comments to be parsed by doc++.
 *
 * Revision 1.4  1999/08/18 13:49:39  olsons
 * Fixed extra 'static class::freetoheap' declaration in memory.
 * Added an escape function to generation and geneticalg so
 * that the algorithm can be interrupted.
 *
 * Revision 1.3  1999/07/28 00:32:08  olsons
 * fixed and re-enabled class::freetoheap so that we can free the memory to the heap again.
 * found a bug and squashed it in simfit (it was in checking for minvals[] and maxvals[] -- my addition to simfit)
 *
 * Revision 1.2  1999/07/03 05:27:04  olsons
 * Genetic Algorithm is now a seperate entity from XUVMulti and is written so it
 * can be used for any purpose.
 * libgeneticalg.so was also added.
 *
 * Revision 1.1.1.1  1999/06/04 13:47:54  olsons
 * Imported as XUVMulti into cvs
 *
 *
 */

/** \file
 * Genetic algorithm container class.
 *
 * Copyright 1998-2005 Spencer Eugene Olson --- All Rights Reserved
 *
 */

/** \example fit/ga/testGA.C
 * Example use of the genetic algorithm for finding global optima.
 */


#ifndef olson_tools_fit_GeneticAlg_h
#define olson_tools_fit_GeneticAlg_h

#include <olson-tools/fit/Generation.h>
#include <olson-tools/fit/Gene.h>

#include <limits>
#include <string>

namespace olson_tools{
  namespace fit {

    /** Genetic algorithm driver.
     * @param _options
     *    @see make_options
     */
    template < typename _options >
    class GeneticAlg {
      /* TYPEDEFS */
    public:
      typedef _options optionsT;

      /// Defining the exception class for GeneticAlg (not in use currently).
      class stopGeneticAlg { };

      /* MEMBER STORAGE */
    private:
      ///The arguments
      optionsT options;

      ///
      Generation<optionsT> parents;

      ///The actual generation that we have just iterated.
      int generation_iter;

      ///Initialized to gene and will contain the gene fit by the GeneticAlg.
      Gene *fitgene;

      /* MEMBER FUNCTIONS */
    public:
      ///
      GeneticAlg(Gene &gene, const optionsT & options = optionsT() );
      ///
      ~GeneticAlg() {}
      ///
      merit_t fit( std::ostream * output = NULL,
                   float tolerance = 0,
                   merit_t maxmerit = -std::numeric_limits<merit_t>::infinity() );

      /* FRIEND FUNCTIONS */
      ///
      template < typename T >
      friend std::ostream & operator<<( std::ostream &, const GeneticAlg<T> & );
    };

    ///The GeneticAlg print function.
    template < typename T >
    std::ostream & operator<<( std::ostream &, const GeneticAlg<T> &);

  }/*namespace olson_tools::fit */
}/*namespace olson_tools */

#include <olson-tools/fit/GeneticAlg.cpp>

#endif //  olson_tools_fit_GeneticAlg_h

