#ifndef PSSP_GPU_SYSTEM_H
#define PSSP_GPU_SYSTEM_H

/*
* PSCF - Polymer Self-Consistent Field Theory
*
* Copyright 2016, The Regents of the University of Minnesota
* Distributed under the terms of the GNU General Public License.
*/

#include <util/param/ParamComposite.h>     // base class
#include <pssp_gpu/solvers/Mixture.h>          // member
#include <pssp_gpu/basis/Basis.h>              // member
#include <pssp_gpu/iterator/FtsIterator.h>
#include <pssp_gpu/inter/ChiInteraction.h>
#include <pscf/mesh/Mesh.h>                // member
#include <pssp_gpu/crystal/UnitCell.h>         // member
#include <pscf/homogeneous/Mixture.h>      // member
#include <util/misc/FileMaster.h>          // member
#include <util/containers/DArray.h>        // member template
#include <util/containers/Array.h>         // function parameter
#include <pssp_gpu/field/RDField.h>             // typedef
#include <pssp_gpu/wavelist/WaveList.h>
#include <util/random/Random.h>
#include <curand.h>


namespace Pscf {
namespace Pssp_gpu
{
   template <int D> class FtsIterator;
   class Sweep;
   class SweepFactory;

   using namespace Util;

   /**
   * Main class in SCFT simulation of one system.
   *
   * \ingroup Pscf_Pssp_gpu_Module
   */
   template <int D>
   class System : public ParamComposite
   {

   public:

      /// Base class for WField and CField
      typedef RDField<D> Field;

      /// Monomer chemical potential field type.
      typedef typename Propagator<D>::WField WField;

      /// Monomer concentration / volume fraction field type.
      typedef typename Propagator<D>::CField CField;

      /**
      * Constructor.
      */
      System();

      /**
      * Destructor.
      */
      ~System();

      /// \name Lifetime (Actions)
      //@{

      /**
      * Process command line options.
      */
      void setOptions(int argc, char **argv);

      /**
      * Read input parameters (with opening and closing lines).
      *
      * \param in input parameter stream
      */
      virtual void readParam(std::istream& in);

      /**
      * Read input parameters from default param file.
      */
      void readParam();

      /**
      * Read body of input parameters block (without opening and closing lines).
      *
      * \param in input parameter stream
      */
      virtual void readParameters(std::istream& in);

      /**
      * Read command script.
      * 
      * \param in command script file.
      */
      void readCommands(std::istream& in);

      /**
      * Read commands from default command file.
      */
      void readCommands();

      /**
      * Compute free energy density and pressure for current fields.
      *
      * This function should be called after a successful call of
      * iterator().solve(). Resulting values are returned by the 
      * freeEnergy() and pressure() accessor functions.
      */
      void computeFreeEnergy();

      /**
      * Output thermodynamic properties to a file. 
      *
      * This function outputs Helmholtz free energy per monomer,
      * pressure (in units of kT per monomer volume), and the
      * volume fraction and chemical potential of each species.
      *
      * \param out output stream 
      */
      void outputThermo(std::ostream& out);

      //@}
      /// \name Fields
      //@{

      /**
      * Get array of all chemical potential fields in star basis.
      *
      * The array capacity is equal to the number of monomer types.
      */
      DArray<RDField <D> >& wFields();


      /**
      * Get chemical potential field for a specific monomer type.
      *
      * \param monomerId integer monomer type index
      */
      RDField<D>& wField(int monomerId);

      /**
      * Get array of all chemical potential fields in cartesian space.
      *
      * The array capacity is equal to the number of monomer types.
      */
      DArray<WField>& wFieldGrids();

      WField& wFieldGrid(int monomerId);

      /**
      * Get array of all chemical potential fields on k-space
      *
      * The array capacity is equal to the number of monomer types.
      */
      DArray<RDFieldDft<D> >& wFieldDfts();

      RDFieldDft<D>& wFieldDft(int monomerId);

      /**
      * Get array of all chemical potential fields in star basis.
      *
      * The array capacity is equal to the number of monomer types.
      */
      DArray<RDField <D> >& cFields();

      /**
      * Get chemical potential field for a specific monomer type.
      *
      * \param monomerId integer monomer type index
      */
      RDField<D>& cField(int monomerId);

      /**
      * Get array of all chemical potential fields in cartesian space.
      *
      * The array capacity is equal to the number of monomer types.
      */
      DArray<CField>& cFieldGrids();

      CField& cFieldGrid(int monomerId);

      /**
      * Get array of all chemical potential fields in k-space.
      *
      * The array capacity is equal to the number of monomer types.
      */
      DArray<RDFieldDft<D> >& cFieldDfts();

      RDFieldDft<D>& cFieldDft(int monomerId);

      /**
      * Read chemical potential fields from file.
      *
      * \param in input stream (i.e., input file)
      */
      //void readFields(std::istream& in, DArray< RDField<D> >& fields);

      void readRFields(std::istream& in, DArray< RDField<D> >& fields);

      void readKFields(std::istream& in, DArray< RDFieldDft<D> >& fields);
      /**
      * Write concentration or chemical potential fields to file.
      *
      * \param out output stream (i.e., output file)
      * \param fields array of fields for different species
      */
      //void writeFields(std::ostream& out, DArray< RDField<D> > const & fields);

      void writeRFields(std::ostream& out, DArray< RDField<D> > const& fields);

      void writeKFields(std::ostream& out, DArray< RDFieldDft<D> > const& fields);

      //@}
      /// \name Accessors (get objects by reference)
      //@{

      /**
      * Get Mixture by reference.
      */
      Mixture<D>& mixture();

      /**
      * Get spatial discretization mesh by reference.
      */
      Mesh<D>& mesh();

      /**
      * Get crystal unitCell (i.e., lattice type and parameters) by reference.
      */
      UnitCell<D>& unitCell();

      /**
      * Get interaction (i.e., excess free energy model) by reference.
      */
      ChiInteraction& interaction();
	  
      /**
      * Get the Iterator by reference.
      */
      //temporarily changed to allow testing on member functions
      FtsIterator<D>& iterator();

      /**
      * Get basis object by reference.
      */
      Basis<D>& basis();

      WaveList<D>& wavelist();

      FFT<D>& fft();
      /**
      * Get homogeneous mixture (for reference calculations).
      */
      Homogeneous::Mixture& homogeneous();

      /**
      * Get FileMaster by reference.
      */
      FileMaster& fileMaster();

      /** 
      * Get group name.
      */  
      std::string groupName();

      /**
      * Get precomputed Helmoltz free energy per monomer / kT.
      *
      * The value retrieved by this function is computed by the
      * computeFreeEnergy() function.
      */
      double fHelmholtz() const;

      /**
      * Get precomputed pressure x monomer volume kT.
      *
      * The value retrieved by this function is computed by the
      * computeFreeEnergy() function.
      */
      double pressure() const;

	  RDField<D>& pressureField();
	  RDField<D>& compositionField();
      //@}

   private:

      /**
      * Mixture object (solves MDE for all species).
      */
      Mixture<D> mixture_;

      /**
      * Spatial discretization mesh.
      */
      Mesh<D> mesh_;

      /**
      * group name.
      */
      std::string groupName_;

      /**
      * Crystallographic unit cell (type and dimensions).
      */
      UnitCell<D> unitCell_;

      /**
      * Filemaster (holds paths to associated I/O files).
      */
      FileMaster fileMaster_;

      /**
      * Homogeneous mixture, for reference.
      */
      Homogeneous::Mixture homogeneous_;

      /**
      * Pointer to Interaction (excess free energy model).
      */
      ChiInteraction* interactionPtr_;
	  
      /**
      * Pointer to an iterator.
      */
      FtsIterator<D>* iteratorPtr_;

      /**
      * Pointer to a Basis object
      */
      Basis<D>* basisPtr_;
      
      WaveList<D>* wavelistPtr_;

      /**
      * FFT object to be used by iterator
      */
      FFT<D> fft_;

      /**
      * Pointer to an Sweep object
      */
      Sweep* sweepPtr_;

      /**
      * Pointer to SweepFactory object
      */
      SweepFactory* sweepFactoryPtr_;

      /**
      * Array of chemical potential fields for monomer types.
      *
      * Indexed by monomer typeId, size = nMonomer.
      */
      DArray<RDField<D> > wFields_;

      /**
      * Array of chemical potential fields for monomer types.
      *
      * Indexed by monomer typeId, size = nMonomer.
      */
      DArray<WField> wFieldGrids_;

      /**
      * work space for chemical potential fields
      *
      */
      DArray<RDFieldDft<D> > wFieldDfts_;

      /**
      * Array of concentration fields for monomer types.
      *
      * Indexed by monomer typeId, size = nMonomer.
      */
      DArray< RDField<D> > cFields_;

      DArray<CField> cFieldGrids_;

      DArray<RDFieldDft<D> > cFieldDfts_;

      /**
      * Work array (size = # of grid points).
      */
      DArray<double> f_;

      /**
      * Work array (size = # of monomer types).
      */
      DArray<double> c_;

      /**
      * Helmholtz free energy per monomer / kT.
      */
      double fHelmholtz_;

      /**
      * Pressure times monomer volume / kT.
      */
      double pressure_;

      /**
      * Has the mixture been initialized?
      */
      bool hasMixture_;

      /**
      * Has the Mesh been initialized?
      */
      bool hasMesh_;

      /**
      * Has the UnitCell been initialized?
      */
      bool hasUnitCell_;

      /**
      * Have initial chemical potential fields been read from file?
      */
      bool hasFields_;

      /**
      * Does this system have a Sweep object?
      */
      bool hasSweep_;

      /**
      * Allocate memory for fields (private)
      */
      void allocateFields();

      /**
      * Initialize Homogeneous::Mixture object.
      */
      void initHomogeneous();

	  RDField<D> compositionField_; //rField version
	  RDFieldDft<D> compositionKField_; //kField
	  RDField<D> pressureField_;

	  /**
	  * Free energy of the new configuration due to random change
	  */
	  double fHelmholtzOld_;

	  IntVec<D> kMeshDimensions_;

	  RDField<D> workArray;
	  cufftReal* d_kernelWorkSpace_;
	  cufftReal* kernelWorkSpace_;
	  cufftReal innerProduct(const RDField<D>& a, const RDField<D>& b, int size);
	  cufftReal reductionH(const RDField<D>& a, int size);


   };

   // Inline member functions

   template <int D>
   inline RDField<D>& System<D>::pressureField()
   { return pressureField_;}

   template <int D>
   inline RDField<D>& System<D>::compositionField()
   { return compositionField_;}

   /*
   * Get the associated Mixture object.
   */
   template <int D>
   inline Mixture<D>& System<D>::mixture()
   { return mixture_; }

   /*
   * Get the mesh.
   */
   template <int D>
   inline Mesh<D>& System<D>::mesh()
   { return mesh_; }

   /*
   * Get the UnitCell<D>.
   */
   template <int D>
   inline UnitCell<D>& System<D>::unitCell()
   { return unitCell_; }

   /*
   * Get group name.
   */
   template <int D>
   inline std::string System<D>::groupName()
   { return groupName_; }

   /*
   * Get the FileMaster.
   */
   template <int D>
   inline FileMaster& System<D>::fileMaster()
   {  return fileMaster_; }

   /*
   * Get the Homogeneous::Mixture object.
   */
   template <int D>
   inline 
   Homogeneous::Mixture& System<D>::homogeneous()
   {  return homogeneous_; }

   /*
   * Get the Interaction (excess free energy model).
   */
   template <int D>
   inline ChiInteraction& System<D>::interaction()
   {
      UTIL_ASSERT(interactionPtr_);
      return *interactionPtr_;
   }

   /*
   * Get the Iterator.
   */
   template <int D>
   inline FtsIterator<D>& System<D>::iterator()
   {
      UTIL_ASSERT(iteratorPtr_);
      return *iteratorPtr_;
   }

   /*
   * Get the basis Object
   */
   template <int D>
   inline Basis<D>& System<D>::basis()
   {
      UTIL_ASSERT(basisPtr_);
      return *basisPtr_;
   }

   template <int D>
   inline WaveList<D>& System<D>::wavelist()
   {
      return *wavelistPtr_;
   }

   template <int D>
   inline FFT<D>& System<D>::fft()
   { return fft_; }

   template <int D>
   inline
   DArray<RDField<D> >& System<D>::wFields()
   { return wFields_; }

   template <int D>
   inline
   RDField<D>& System<D>::wField(int id)
   { return wFields_[id]; }

   /*
   * Get an array of all monomer excess chemical potential fields.
   */
   template <int D>
   inline 
   DArray< typename System<D>::WField >& System<D>::wFieldGrids()
   {  return wFieldGrids_; }

   /*
   * Get a single monomer excess chemical potential field.
   */
   template <int D>
   inline 
   typename System<D>::WField& System<D>::wFieldGrid(int id)
   {  return wFieldGrids_[id]; }

   template <int D>
   inline
   DArray<RDFieldDft<D> >& System<D>::wFieldDfts()
   { return wFieldDfts_; }

   template <int D>
   inline
   RDFieldDft<D>& System<D>::wFieldDft(int id)
   { return wFieldDfts_[id]; }

   template <int D>
   inline
   DArray<RDField<D> >& System<D>::cFields()
   { return cFields_; }

   template <int D>
   inline
   RDField<D>& System<D>::cField(int id)
   { return cFields_[id]; }

   template <int D>
   inline
   DArray<RDFieldDft<D> >& System<D>::cFieldDfts()
   { return cFieldDfts_; }

   template <int D>
   inline
   RDFieldDft<D>& System<D>::cFieldDft(int id)
   { return cFieldDfts_[id]; }
   /*
   * Get array of all monomer concentration fields.
   */
   template <int D>
   inline
   DArray< typename System<D>::CField >& System<D>::cFieldGrids()
   {  return cFieldGrids_; }

   /*
   * Get a single monomer concentration field.
   */
   template <int D>
   inline typename System<D>::CField& System<D>::cFieldGrid(int id)
   {  return cFieldGrids_[id]; }

   /*
   * Get precomputed Helmoltz free energy per monomer / kT.
   */
   template <int D>
   inline double System<D>::fHelmholtz() const
   {  return fHelmholtz_; }

   /*
   * Get precomputed pressure (units of kT / monomer volume).
   */
   template <int D>
   inline double System<D>::pressure() const
   {  return pressure_; }

} // namespace Pssp_gpu
} // namespace Pscf
#include "System.tpp"
#endif