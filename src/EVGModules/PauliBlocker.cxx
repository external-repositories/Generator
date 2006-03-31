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

#include "EVGCore/EVGThreadException.h"
#include "EVGModules/PauliBlocker.h"
#include "GHEP/GHepRecord.h"
#include "GHEP/GHepParticle.h"
#include "GHEP/GHepFlags.h"
#include "Interaction/Interaction.h"
#include "Interaction/IUtils.h"
#include "Messenger/Messenger.h"
#include "Nuclear/FermiMomentumTablePool.h"
#include "Nuclear/FermiMomentumTable.h"

using namespace genie;

//___________________________________________________________________________
PauliBlocker::PauliBlocker() :
EventRecordVisitorI("genie::PauliBlocker")
{

}
//___________________________________________________________________________
PauliBlocker::PauliBlocker(string config) :
EventRecordVisitorI("genie::PauliBlocker",  config)
{

}
//___________________________________________________________________________
PauliBlocker::~PauliBlocker()
{

}
//___________________________________________________________________________
void PauliBlocker::ProcessEventRecord(GHepRecord * event_rec) const
{
  //-- Get the Interaction & InitialState objects
  Interaction * interaction = event_rec->GetInteraction();
  const InitialState & init_state = interaction->GetInitialState();

  //-- Pauli Blocking is only relevant for nucleon bound in a nucleus

  if( init_state.GetTarget().IsNucleus() ) {

    int tgt_pdgc = init_state.GetTarget().PDGCode();
    int nuc_pdgc = utils::interaction::RecoilNucleonPdgCode(interaction);

    if(nuc_pdgc != 0) {
       // Find the recoil nucleon in the EventRecord

       GHepStatus_t ist = kIStStableFinalState;
       GHepParticle * nuc = event_rec->FindParticle(nuc_pdgc, ist, 0);

       if(nuc) {
         // get the Fermi momentum
         const double kf = fKFTable->FindClosestKF(tgt_pdgc, nuc_pdgc);
         LOG("PauliBlock", pINFO) << "KF = " << kf;

         double p = nuc->P4()->P(); // |p| for the recoil nucleon
         LOG("PauliBlock", pINFO) << "Recoil nucleon |P| = " << p;

         if(p < kf) {
              LOG("PauliBlock", pINFO)
                   << "\n The generated event is Pauli-blocked: "
                          << " |p| = " << p << " < Fermi-Momentum = " << kf;

              event_rec->EventFlags()->SetBitNumber(kPauliBlock, true);
              genie::exceptions::EVGThreadException exception;
              exception.SetReason("Pauli-blocked event");
              exception.SwitchOnFastForward();
              throw exception;
         }
       }//nuc!=0
    }//nuc_pdgc!=0
  }//not a free nucleon
}
//___________________________________________________________________________
void PauliBlocker::Configure(const Registry & config)
{
  Algorithm::Configure(config);
  this->LoadKFTable();
}
//___________________________________________________________________________
void PauliBlocker::Configure(string param_set)
{
  Algorithm::Configure(param_set);
  this->LoadKFTable();
}
//___________________________________________________________________________
void PauliBlocker::LoadKFTable(void)
{
  fKFTable = 0;

  // get the Fermi momentum table
  FermiMomentumTablePool * kftp = FermiMomentumTablePool::Instance();
  fKFTable = kftp->GetTable("Default");

  assert(fKFTable);
}
//___________________________________________________________________________

