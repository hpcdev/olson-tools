// $Id: GeneticAlg.h,v 1.1 2005/06/07 20:38:11 olsonse Exp $
/*
 * $Log: GeneticAlg.h,v $
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


#ifndef GENETICALG_H
#define GENETICALG_H

#include <string>
#include "Generation.h"
#include "Gene.h"

///The args for GeneticAlg class.
class GeneticAlgArgs {
  public:
    ///
    GeneticAlgArgs():population(400), tolerance(1e-3), max_merit(1e100), replace(0.9),
                     local_fit_max_individuals_prctage(0),
                     local_fit_tolerance(1e-3),
                     local_fit_max_iteration(100), crossprob(0.85),
                     mutprob(0.35), maxgeneration(200),
                     createind(create_Individual), meritfnc(NULL),
                     exterior_pointer( NULL )
                     {}
    /// Genetic Algorithm arguments copy constructor.
    GeneticAlgArgs(const GeneticAlgArgs & that) { *this = that; }
    ~GeneticAlgArgs() {}

    void setParam (const std::string & name, double & var);
    /**The actual variable arguments for the Genetic Algorithm.
    */
    //@{
    int      population;
    float    tolerance;
    merit_t  max_merit;
    float    replace;
    float    local_fit_max_individuals_prctage;
    float    local_fit_tolerance;
    int      local_fit_max_iteration;
    float    crossprob;
    float    mutprob;
    int      maxgeneration;
    CREATE_IND_FUNC   createind;
    void *   meritfnc;
    void *   exterior_pointer;
    //@}

};//class GeneticAlgArgs

///
class GeneticAlg {
  public:
    ///
    GeneticAlg(Gene &gene, GeneticAlgArgs & ga_args );
    ///
    ~GeneticAlg() {}
    ///
    merit_t fit( std::ostream * output = NULL, float tolerance = 0, merit_t maxmerit = -1e60 );
    ///
    friend std::ostream & operator<<(std::ostream &,const GeneticAlg &);
  private:
    ///The arguments
    GeneticAlgArgs args;

    ///
    Generation parents;

    ///The actual generation that we have just iterated.
    int generation_iter;

    ///Initialized to gene and will contain the gene fit by the GeneticAlg.
    Gene *fitgene;

  public:
    /// Defining the exception class for GeneticAlg (not in use currently).
    class stopGeneticAlg { };
};

///The GeneticAlg print function.
std::ostream & operator<<(std::ostream &, const GeneticAlg &);

#endif //  GENETICALG_H
