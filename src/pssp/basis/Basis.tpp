#ifndef PSSP_BASIS_TPP
#define PSSP_BASIS_TPP

/*
* PSCF - Polymer Self-Consistent Field Theory
*
* Copyright 2016, The Regents of the University of Minnesota
* Distributed under the terms of the GNU General Public License.
*/

#include "Basis.h"
#include <pscf/crystal/shiftToMinimum.h>
#include <vector>


namespace Pscf {
namespace Pssp
{

   template <int D>
   Basis<D>::Basis()
    : nWave_(0), 
      nStar_(0), 
      unitCellPtr_(0), 
      meshPtr_(0)
   {}

   template <int D>
   void Basis<D>::makeBasis(const Mesh<D>& mesh, 
                            const UnitCell<D>& unitCell,
                            std::string groupName)
   {
      // Save pointers to mesh and unit cell
      meshPtr_ = &mesh;
      unitCellPtr_ = &unitCell;

      // Allocate arrays
      nWave_ = mesh.size();
      waves_.allocate(nWave_);
      waveId_.allocate(nWave_); 

      // Make sorted array of waves
      makeWaves(mesh, unitCell);

      // Identify stars of waves that are related by symmetry
      makeStars(mesh, unitCell);

   }

   template <int D>
   void Basis<D>::makeWaves(const Mesh<D>& mesh, const UnitCell<D>& unitCell)
   {
      IntVec<D> meshDimensions = mesh.dimensions();

      // Generate dft mesh of waves, store in local std::vector twaves_
      std::vector<NWave> twaves;
      twaves.reserve(nWave_);
      {
         NWave w;
         IntVec<D> v;
         MeshIterator<D> itr(mesh.dimensions());
         for (itr.begin(); !itr.atEnd(); ++itr) {
            w.indicesDft = itr.position();
            v = shiftToMinimum(w.indicesDft, meshDimensions, *unitCellPtr_);
            w.indicesBz = v;
            w.sqNorm = unitCellPtr_->ksq(v);
            twaves.push_back(w);
   
            #if 0
            for (int i = 0; i < 2; ++i) {
                std::cout << " " << v[i];
            }
            std::cout << "  ";
            for (int i = 0; i < 2; ++i) {
                std::cout << " " << w.indicesBz[i];
            }
            std::cout << "    " << w.sqNorm << std::endl;
            #endif
   
         }
      }

      // Sort temporary container twaves_ by wavevector norm
      {
         // Define function object to be used for sorting
         struct NormComparator{
            bool operator () (NWave a, NWave b)
            {  return (a.sqNorm < b.sqNorm); }
         };

         NormComparator comp;
         std::sort(twaves.begin(), twaves.end(), comp);
      }

      // Copy temporary array twaves_ into member variable waves_
      for (int i = 0; i < nWave_; ++i) {
         waves_[i].sqNorm = twaves[i].sqNorm;
         waves_[i].indicesDft = twaves[i].indicesDft;
         waves_[i].indicesBz = twaves[i].indicesBz;
         #if 0
         for (int j = 0; j < 2; ++j) {
             std::cout << " " << waves_[i].indicesBz[j];
         }
         std::cout << "    " << waves_[i].sqNorm << std::endl;
         #endif
      }

   }

   template <int D>
   bool Basis<D>::NWaveComp::operator() (Basis::NWave a, Basis::NWave b) const
   {
       if (a.indicesDft == b.indicesDft) {
          return false;
       } else 
       if (a.indicesBz > b.indicesBz) {
          return true;
       } else
       if (a.indicesBz < b.indicesBz) {
          return false;
       } else {
          return (a.indicesDft < b.indicesDft);
       }
   }

   template <int D>
   void Basis<D>::makeStars(const Mesh<D>& mesh, 
                            const UnitCell<D>& unitCell)
   {

      std::set<NWave, NWaveComp> list; // vectors of equal norm
      std::set<NWave, NWaveComp> star; // symmetry-related vectors 
      typename std::set<NWave, NWaveComp>::iterator itr;
      NWave wave;

      double Gsq;
      double Gsq_max = 1.0;
      double epsilon = 1.0E-8;
      int listId = 0;
      int listBegin = 0;
      int listEnd = 0;
      int listSize = 0;
      int starId = 0;
      int i, j;
      bool newList;
      for (i = 0; i <= nWave_; ++i) {

          newList = false;
          if (i == nWave_) {
             listEnd = i;
             newList = true;
          } else {
             Gsq = waves_[i].sqNorm;
             if (Gsq > Gsq_max + epsilon) {
                Gsq_max = Gsq;
                listEnd = i;
                newList = true;
             }
          }

          // Process new list
          if (newList && listEnd > 0) {
             listSize = listEnd - listBegin;

             // Copy list into temporary array "list"
             list.clear();
             for (j = listBegin; j < listEnd; ++j) {
                wave.indicesDft = waves_[j].indicesDft;
                wave.indicesBz = waves_[j].indicesBz;
                wave.sqNorm = waves_[j].sqNorm;
                list.insert(wave);
             }

             std::cout << std::endl;
             std::cout << "list id   = " << listId << std::endl;
             std::cout << "list size = " << listSize << std::endl;
             for (itr = list.begin(); itr != list.end(); ++itr) {
                std::cout << (*itr).indicesBz << std::endl;
             }

             //int invertFlag = 1;
             while (list.size() > 0) {

                int rootId = 0;
                getStar(rootId, list, star);

                std::cout << "star id   = " << starId << std::endl;
                for (itr = star.begin(); itr != star.end(); ++itr) {
                   std::cout << (*itr).indicesBz << std::endl;
                }

                ++starId;
             }

             ++listId;
             listBegin = listEnd;
          }
      }
       
   }
 
   template <int D>
   void Basis<D>::getStar(int rootId,
                          std::set<NWave, NWaveComp>& list,
                          std::set<NWave, NWaveComp>& star)
   {
      typename std::set<NWave, NWaveComp>::iterator itr;
      itr = list.begin();
      NWave rootWave = *itr;

      star.clear();
      star.insert(*itr);
      list.erase(itr);
   }

      #if 0
         waves_[itr.rank()].sqNorm = unitCell.ksq(itr.position());
         waves_[itr.rank()].indicesDft = itr.position();

         if ((itr.position(D-1) + 1) > (meshPtr_->dimension(D-1)/2 + 1)) {
            waves_[itr.rank()].implicit = true;
         } else {
            waves_[itr.rank()].implicit = false;
         }

         //`unsorted' waves; waves appear in grid order
         waveId_[itr.rank()] = itr.rank();
      }
      
      //make stars
      //To do: If not I, sort according to increasing ksq values
      //Changed: The size of stars_ is not known apriori. Used a GArray
      //For `I', reserved the right size.
      stars_.reserve(mesh.size());
      int beginWave = 0;
      int endWave = 0;
      int iStar = 0;
      bool cancel;
      int invertFlag = 0;
      std::complex<double> cNorm;

      for (itr.begin(); !itr.atEnd() ; ++itr) {
         stars_[itr.rank()].invertFlag = 3;
      }

      for (itr.begin(); !itr.atEnd() ; ++itr) {
         //To do: If not I, create a list of waves with equal ksq 
         //magnitudes. The I space group implicitly assumes each waves belong
         //in a single star and the sorting is not done.
         
         endWave = beginWave+1;
         double starPhase[1] = {0};
         //To do: unimplemented local scope function
         //cancel = isCancelled(waves_[beginWave].indices);
         cancel = false;

         #if 0
         //check for cancellation consistency in star
         for(int j = beginWave; j < endWave; ++j) {
            if( cancel != isCancelled(waves_[j].indices))
               UTIL_THROW("Inconsistent cancellation in star");
         }
         #endif

         if (!cancel) {
            if (invertFlag == 1) { 
               cNorm = exp(std::complex<double>(0,1) *
                          starPhase[endWave-1-beginWave]);
            } else { 
               cNorm = exp(std::complex<double>(0,1)*starPhase[0]);
            }

            cNorm *= sqrt(double(endWave - beginWave));
         }

         for (int j = beginWave; j < endWave; ++j) {
            waves_[j].starId = iStar;
            if (!cancel) {
               waves_[j].coeff = exp(std::complex<double>(0,1) *
                                    starPhase[j - beginWave])/cNorm;
               //std::cout<<waves_[j].coeff<<std::endl;
            } else {   
               waves_[j].coeff = exp(std::complex<double>(0,0));
            }
         }
         
         //fill up stars_ object
         //unimplemented: sign flag.
         stars_[iStar].size = endWave - beginWave;
         stars_[iStar].beginId = beginWave;
         stars_[iStar].endId = endWave - 1;
         stars_[iStar].cancel = cancel;
         ++nStar_;

         //To do: a method here to decide what is the actual invertFlag
         //A lot easier when waves are sorted in min(ksq). Currently, the second
         //wave of 001 index 1 is paired with say 009.
         IntVec<D> G1 = itr.position();
         IntVec<D> G2;

         bool isClosed = true;
         for (int j = 0; j < D; ++j) {
            G2[j] = -G1[j];
         }
         meshPtr_->shift(G2);
         if (G2 != G1) {
            isClosed = false;
         }
         if (isClosed) {
            invertFlag = 0;
            stars_[iStar].invertFlag = invertFlag;
         } else {
            meshPtr_->shift(G2);
            int partnerId = meshPtr_->rank(G2);
            //std::cout<<stars_[partnerId].invertFlag<<std::endl;
            if (stars_[partnerId].invertFlag == 3) {
               stars_[iStar].invertFlag = 1;
            } else {
               stars_[iStar].invertFlag = -1;
            }
         }

         /*
         if(itr.position(D-1) > (meshPtr_.dimensions(D-1)/2 + 1) )
            stars_[iStar].invertFlag = -1;
         else
            stars_[iStar].invertFlag = 1;
         */

         if (invertFlag == -1) {
            stars_[iStar].waveBz = shiftToMinimum(waves_[endWave-1].indicesDft,
                                      meshPtr_->dimensions(), *unitCellPtr_);
         } else {
            stars_[iStar].waveBz = shiftToMinimum(waves_[beginWave].indicesDft,
                                      meshPtr_->dimensions(), *unitCellPtr_);
         }

         iStar++;
         beginWave = endWave;
      }

      //dksq.allocate(unitCellPtr_->nParams(), nStar_); 
      dksq.allocate(6, nStar_);     
 
      // Initialize all elements to zero
      int i, j, p, q;
      for (i = 0; i < 6; ++i) {
         for (j=0; j < nStar_; ++j){
            dksq(i,j)=0.0;
         }   
      }   
    
      for (i = 0; i < unitCellPtr_->nParams(); ++i) {
         for (j=0; j < nStar_; ++j){
            for (p=0; p < D; ++p){
               for (q=0; q < D; ++q){

                  dksq(i,j) = dksq(i,j) 
                            + (stars_[j].waveBz[p]*stars_[j].waveBz[q]*(unitCellPtr_->dkkBasis(i, p,q)));
  
               }   
            }   
         }   
      }
      #endif 

   template <int D>
   void Basis<D>::convertFieldComponentsToDft(DArray<double>& components, 
                                              RFieldDft<D>& dft)
   {
      std::complex<double> coeff;
      double z;
      double z1;
      double z2;
      IntVec<D> G2;
      int partnerId;
      //loop through a factor of 2 more indices. 
      //Difficult to optimize without specialization or making new objects
      for (int i = 0; i < meshPtr_->size(); ++i) {
         
         //if last indice of waves is not > n3/2+1
         IntVec<D> waveId = meshPtr_->position(i);
         int starId = wave(waveId).starId;
         if (!wave(waveId).implicit) {
            coeff = wave(waveId).coeff;
            //determining the rank of RFieldDft
            IntVec<D> offsets;
            offsets[D-1] = 1;
            for (int j = D-1; j > 0; --j) {
               if (j == D-1) {
                  offsets[j-1] = offsets[j]*(meshPtr_->dimension(j)/2 + 1);
               } else {
                  offsets[j-1] = offsets[j]*meshPtr_->dimension(j);
               }
            }
            int rank = 0;
            for (int j = 0; j < D; ++j) {
               rank += wave(waveId).indicesDft[j] * offsets[j];
            }
            //std::cout<<rank<<std::endl;

            switch (stars_[starId].invertFlag) {
               case 0 :
                  z = components[starId];
                  coeff = z*coeff;
                  dft[rank][0] = coeff.real();
                  dft[rank][1] = coeff.imag();
                  break;
               case 1 :
                  z1 = components[starId];
                  for(int j = 0; j < D; ++j){
                     G2[j] = -waveId[j];
                  }
                  meshPtr_->shift(G2);
                  partnerId = meshPtr_->rank(G2);
                  z2 = components[partnerId];
                  /*//debug code
                  if(stars_[partnerId].invertFlag != -1) {
                     std::cout<<"This star is "<<starId<<" and my partner is "
                     <<partnerId<<std::endl;
                     std::cout << "I have invertFlag of "
                               << stars_[starId].invertFlag
                               << " and he has "
                               << stars_[partnerId].invertFlag<<std::endl;
                     std::cout << " WaveBz "<<stars_[starId].waveBz 
                               << " and he "
                               << stars_[partnerId].waveBz<<std::endl;
                     std::cout << "Wave dft indices"
                               << waves_[stars_[starId].beginId].indicesDft
                               << "and he is" 
                               << waves_[stars_[partnerId].beginId].indicesDft
                               << std::endl;
                     std::cout << "My previous wave has implicit of "
                               << waves_[stars_[starId].beginId - 1].implicit
                               << std::endl;
                     std::cout<<"And position of "
                     <<waves_[stars_[starId].beginId-1].indicesDft<<std::endl;
                  }
                  //end debug*/
                  UTIL_CHECK(stars_[partnerId].invertFlag == -1);
                  coeff = std::complex<double>(z1,-z2)*coeff/sqrt(2);
                  dft[rank][0] = coeff.real();
                  dft[rank][1] = coeff.imag();
                  break;
               case -1 :
                  z1 = components[starId];
                  for(int j = 0; j < D; ++j){
                     G2[j] = -waveId[j];
                  }
                  meshPtr_->shift(G2);
                  partnerId = meshPtr_->rank(G2);
                  z2 = components[partnerId];
                  UTIL_CHECK(stars_[partnerId].invertFlag == 1);
                  coeff = std::complex<double>(z2, z1) * coeff / sqrt(2);
                  dft[rank][0] = coeff.real();
                  dft[rank][1] = coeff.imag();
                  break;
            }

         } else {
            //do nothing. Dft does not need allocation for this
         }
      }
   }

   template <int D>
   void Basis<D>::convertFieldDftToComponents(RFieldDft<D>& dft, 
                                              DArray<double>& components)
   {
      //if its not too distrupting maybe use consistent names for logical size?
      int nStars = nStar_;
      UTIL_CHECK(nStars == components.capacity());
      IntVec<D> indiceBz;
      std::complex<double> coeff;
      std::complex<double> temp;
      fftw_complex z;
      for (int i = 0; i < nStars; ++i) {
         indiceBz = stars_[i].waveBz;
         coeff = wave(indiceBz).coeff;

         //check if wave exists in dft
         if (!wave(indiceBz).implicit) {
            meshPtr_->shift(indiceBz);
            //std::cout<<"Yoohoo"<<std::endl;
            //might be good to rethink how elements of RFieldDft is accessed
            IntVec<D> offsets;
            offsets[D-1] = 1;
            for (int j = D-1; j > 0; --j) {
               if(j == D-1) {
                  offsets[j-1] = offsets[j]*(meshPtr_->dimension(j)/2 + 1);
               } else {
                  offsets[j-1] = offsets[j]*meshPtr_->dimension(j);
               }
            }
            int rank = 0;
            for (int j = 0; j < D; j++) {
               rank += indiceBz[j] * offsets[j];
            }
            //std::cout<<offsets<<std::endl;
            //std::cout<<indiceBz<<std::endl;
            //std::cout<<rank<<std::endl;
            z[0] = dft[rank][0];
            z[1] = dft[rank][1];
            //Note: the code implies that we do not need indicesDft from Wave? 
         } else {
         //wave does not exists in dft. have to find explicit pair
            meshPtr_->shift(indiceBz);
            for (int j = 0; j < D; ++j) {
               indiceBz[j] = -indiceBz[j];
            }
            meshPtr_->shift(indiceBz);


            IntVec<D> offsets;
            offsets[D-1] = 1;
            for (int j = D-1; j > 0; --j) {
               if(j == D-1) {
                  offsets[j-1] = offsets[j]*(meshPtr_->dimension(j)/2 + 1);
               } else {
                  offsets[j-1] = offsets[j]*meshPtr_->dimension(j);
               }
            }

            int rank = 0;
            for (int j = 0; j < D; j++) {
               rank += indiceBz[j] * offsets[j];
            }
            //std::cout<<"Yoohoo"<<std::endl;
            //std::cout<<rank<<std::endl;
            z[0] = dft[rank][0];
            z[1] = dft[rank][1];
            z[1] = -z[1];
         }
         
         //std::cout<<i<<std::endl;
         //reintepret cast is not used since coding standards is old
         temp = std::complex<double>(z[0],z[1]);
         //std::cout<<temp<<std::endl;
         //assign value to components
         switch(stars_[i].invertFlag) {
            case 0 :
               components[i] = (temp/coeff).real();
               break;
            case 1 :
               components[i] = (temp/coeff).real() * sqrt(2);
               break;
            case -1 :
               components[i] = (temp/coeff).imag() * sqrt(2);
               break;
         }
      }
   }

   template <int D>
   int Basis<D>::nBasis() const
   {
      int count = 0;
      for (int i = 0; i < stars_.capacity(); ++i) {
         if (!stars_[i].cancel) {
            count++;
         }
      }
      return count;
   }

   template <int D>
   typename Basis<D>::Wave& Basis<D>::wave(IntVec<D> vector)
   {
      if (!meshPtr_->isInMesh(vector)) {
         meshPtr_->shift(vector);
      }
      return waves_[waveId_[meshPtr_->rank(vector)]];
   }

   template <int D>
   void Basis<D>::makedksq(const UnitCell<D>& unitCell)
   {

      unitCellPtr_ = &unitCell;

      // Initialize all elements to zero
      int i, j, p, q;
      for (i = 0; i < unitCellPtr_->nParams(); ++i) {
         for (j=0; j < nStar_; ++j){
            dksq(i,j)=0.0;
         }
      }
	
      for (i = 0; i < unitCellPtr_->nParams(); ++i) {
         for (j=0; j < nStar_; ++j){
            for (p=0; p < D; ++p){
               for (q=0; q < D; ++q){

                  dksq(i,j) = dksq(i,j) + (stars_[j].waveBz[p]*stars_[j].waveBz[q]*(unitCellPtr_->dkkBasis(i, p,q)));
  
               }
            }
         }
      }

   }

}
}

#endif
