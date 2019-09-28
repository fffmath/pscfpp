#ifndef PSSP_ITERATOR_H
#define PSSP_ITERATOR_H

/*
* PSCF - Polymer Self-Consistent Field Theory
*
* Copyright 2016, The Regents of the University of Minnesota
* Distributed under the terms of the GNU General Public License.
*/

#include <util/param/ParamComposite.h>    // base class
#include <util/global.h>                  

namespace Pscf {
namespace Pssp
{

   template <int D>
   class System;

   using namespace Util;

   /**
   * Base class for iterative solvers for SCF equations.
   *
   * \ingroup Pssp_Iterator_Module
   */
   template <int D>
   class Iterator : public ParamComposite
   {

   public:

      #if 0
      /**
      * Default constructor.
      */
      Iterator();
      #endif

      /**
      * Constructor.
      * 
      * \param system parent System object
      */
      Iterator(System<D>* system);

      /**
      * Destructor.
      */
      ~Iterator();

      /**
      * Iterate to solution.
      *
      * \return error code: 0 for success, 1 for failure.
      */
      virtual int solve() = 0;

   protected:

      /// Pointer to parent System object
      System<D>* systemPtr_;

   };

} // namespace Pssp
} // namespace Pscf
#include "Iterator.tpp"
#endif
