//____________________________________________________________________________
/*!

\class   genie::PauliBlocker

\brief   Examines whether the generated event should be Pauli blocked.

         Is a concerete implementation of the EventRecordVisitorI interface.

\author  Costas Andreopoulos <C.V.Andreopoulos@rl.ac.uk>
         CCLRC, Rutherford Appleton Laboratory

\created October 08, 2004

*/
//____________________________________________________________________________

#include <TLorentzVector.h>
#include <TVector3.h>

#include "EVGModules/PauliBlocker.h"
#include "GHEP/GHepRecord.h"
#include "GHEP/GHepParticle.h"
#include "Interaction/Interaction.h"
#include "Interaction/IUtils.h"
#include "Messenger/Messenger.h"
#include "Nuclear/FermiMomentumTablePool.h"
#include "Nuclear/FermiMomentumTable.h"

using namespace genie;

//___________________________________________________________________________
PauliBlocker::PauliBlocker() :
EventRecordVisitorI()
{
  fName = "genie::PauliBlocker";
}
//___________________________________________________________________________
PauliBlocker::PauliBlocker(const char * param_set) :
EventRecordVisitorI(param_set)
{
  fName = "genie::PauliBlocker";

  this->FindConfig();
}
//___________________________________________________________________________
PauliBlocker::~PauliBlocker()
{

}
//___________________________________________________________________________
void PauliBlocker::ProcessEventRecord(GHepRecord * event_rec) const
{
  //-- Initialize
  LOG("Nuclear", pINFO) << "Init PauliBlocking switch to FALSE";
  event_rec->SwitchIsPauliBlocked(true);

  //-- Get the Interaction & InitialState objects
  Interaction * interaction = event_rec->GetInteraction();
  const InitialState & init_state = interaction->GetInitialState();

  //-- Pauli Blocking is only relevant for nucleon bound in a nucleus

  if( init_state.GetTarget().IsNucleus() ) {

    int tgt_pdgc = init_state.GetTarget().PDGCode();
    int nuc_pdgc = interaction_utils::RecoilNucleonPdgCode(interaction);

    if(nuc_pdgc != 0) {
       // Find the recoil nucleon in the EventRecord
       GHepParticle * nuc = event_rec->FindParticle(nuc_pdgc, 1, 0);

       if(nuc) {
         // get the Fermi momentum
         FermiMomentumTablePool * kftp = FermiMomentumTablePool::Instance();
         const FermiMomentumTable * kft  = kftp->GetTable("Default");
         const double kf = kft->FindClosestKF(tgt_pdgc, nuc_pdgc);
         LOG("Nuclear", pINFO) << "KF = " << kf;

         double p = nuc->P4()->P(); // |p| for the recoil nucleon

         if(p < kf) {
              LOG("Nuclear", pINFO)
                   << "\n The generated event is Pauli-blocked: "
                          << " |p| = " << p << " < Fermi-Momentum = " << kf;
              event_rec->SwitchIsPauliBlocked(true);
         }

       }//nuc!=0
    }//nuc_pdgc!=0
  }//not a free nucleon
}
//___________________________________________________________________________
