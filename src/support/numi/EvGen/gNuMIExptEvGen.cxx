//________________________________________________________________________________________
/*!

\program gNuMIevgen

\brief   A GENIE event generation driver `customized' for the NuMI beam-line experiments.
         This program was adapted from gT2Kevgen used at T2K.

         This driver can use either the NuMI neutrino flux ntuples (generated by gNuMI), 
         or plain flux histograms for all neutrino species you are considering.
         You can specify either a ROOT-based detailed detector geometry description or a
         simple target mix. See below for further details.

         Users should note that the generic GENIE event generation driver (gevgen) may 
         still be a more appropriate tool to use for the simpler event generation cases 
         required for many 4-vector level / systematic studies.
         Please see the GENIE documentation (http://www.genie-mc.org) and contact me 
         <costas.andreopoulos \at stfc.ac.uk> if in doubt.

         *** Syntax :

           gNuMIevgen [-h] 
                     [-r run#] 
                      -f flux 
                      -g geometry 
                     [-t top_volume_name_at_geom] 
                     [-m max_path_lengths_xml_file]
                     [-L length_units_at_geom] 
                     [-D density_units_at_geom]
                     [-n n_of_events] 
                     [-e exposure_in_POTs]
		     [-o output_event_file_prefix]

         *** Options :

           [] Denotes an optional argument

           -h Prints out the gNuMIevgen syntax and exits

           -r Specifies the MC run number [default: 1000]

           -g Input 'geometry'.
              This option can be used to specify any of:
              1 > A ROOT file containing a ROOT/GEANT geometry description
                  [Examples] 
		  - To use the master volume from the ROOT geometry stored 
                    in the argoneut-geom.root file, type:
                    '-g /some/path/nova-geom.root'
              2 > A mix of target materials, each with its corresponding weight,
                  typed as a comma-separated list of nuclear pdg codes (in the
                  std PDG2006 convention: 10LZZZAAAI) with the weight fractions
                  in brackets, eg code1[fraction1],code2[fraction2],...
                  If that option is used (no detailed input geometry description) 
                  then the interaction vertices are distributed in the detector
                  by the detector MC.
                  [Examples] 
                  - To use a target mix of 95% O16 and 5% H type:
                    '-g 1000080160[0.95],1000010010[0.05]'
		  - To use a target which is 100% C12, type:
                    '-g 1000060120'

           -t Input 'top volume' for event generation - 
              can be used to force event generation in given sub-detector
              [default: the 'master volume' of the input geometry]

	   -m An XML file (generated by gmxpl) with the max (density weighted) 
              path-lengths for each target material in the input ROOT geometry.              
              If no file is input, then the geometry will be scanned at MC job 
              initialization to determine those max path lengths. 
              Supplying this file can speed-up the MC job initialization. 

           -L Input geometry length units, eg "m", "cm", "mm", ...
              [default: mm]

           -D Input geometry density units, eg "g_cm3", "clhep_def_density_unit",... 
              [default: g_cm3]

           -f Input 'neutrino flux'.
              This option can be used to specify any of:
              1 > A gNuMI beam simulation output file and the detector location
                  The general sytax is:
                      -f /full/path/flux_file.root,detector
                  [Notes] 
                  - For more information on the flux ntuples, see the gNuMI doc
                  - See GNuMIFlux.xml for all supported detector locations
                  - The original hbook ntuples need to be converted to a ROOT 
                    format using the h2root ROOT utility.   
                  - The gNuMI flux ntuples are read via GENIE's GNuMIFlux driver. 
                    This customized GENIE event generation driver passes-through the 
                    complete gNuMI input flux information (eg parent decay kinematics 
                    / position etc) for each neutrino event it generates (an additional 
                    'flux' branch is added at the output event tree).
                  [Examples] 
                  - To use the gNuMI flux ntuple flux.root at MINOS near detector location, 
                    type:
                     '-f /path/flux.root,MINOS-NearDet'
              2 > A set of histograms stored in a ROOT file.
                  The general syntax is:
                      -f /path/histogram_file.root,neutrino_code[histo_name],...
                  [Notes] 
                  - The neutrino codes are the PDG ones:
                    nu_e     ->  12, nu_mu     ->  14, nu_tau     ->  16,
                    nu_e_bar -> -12, nu_mu_bar -> -14, nu_tau_bar -> -16
                  - The 'neutrino_code[histogram_name]' part of the option can be 
                    repeated multiple times (separated by commas), once for each 
                    flux neutrino species you want to consider, eg
                    '-f somefile.root,12[nuehst],-12[nuebarhst],14[numuhst]'
                  - When using flux from histograms then there is no point in using
                    a 'detailed detector geometry description' as your flux input
                    contains no directional information for those flux neutrinos.
                    The neutrino direction is conventionally set to be +z {x=0,y=0}.
                    So, when using this option you must be using a simple 'target mix'
                    See the -g option for possible geometry settings.
                    If you want to use the detailed detector geometry description
                    then you should be feeding this driver with the jnubeam flux 
                    simulation outputs.
                  - When using flux from histograms there is no branch with neutrino
                    parent information added in the output event tree as your flux 
                    input contains no such information. 
                  - Note that the relative normalization of the flux histograms is
                    taken into account and is reflected in the relative frequency 
                    of flux neutrinos thrown by the flux driver
                  [Examples] 
                  - To use the histogram 'h100' (representing the nu_mu flux) and
                    the histogram 'h300' (representing the nu_e flux) and the 
                    histogram 'h301' (representing the nu_e_bar flux( from the
                    flux.root file in /path/ 
                    type:
                      '-f /path/flux.root,14[h100],12[h300],-12[h301]
                  
           -e Specifies how many POTs to generate.

           -n Specifies how many events to generate.

             [Note on exposure / statistics] 
              Both -e and -n options can be used to set the exposure.
              - If the input flux is a gNuMI ntuple then any of these options can
                be used (one at a time). 
              - If the input flux is described with histograms then only the -n
                option is available.

           -o Sets the prefix of the output event file. 
              The output filename is built as: 
              [prefix].[run_number].[event_tree_format].[file_format]
              The default output filename is: 
              gntp.[run_number].ghep.root
              This cmd line arguments lets you override 'gntp'

         *** Examples:
        
         (1) shell% gNuMIevgen 
                       -r 1001 
                       -f /data/mc_inputs/flux/flux_00001.root,MINOS-NearDet
                       -g /data/mc_inputs/geom/minos.root 
                       -L mm -D g_cm3
                       -e 5E+17

             Generate events (run number 1001) using the gNuMI flux ntuple in
             /data/mc_inputs/flux/v1/flux_00001.root 
             The job will load the MINOS near detector detector geometry description 
             from /data/mc_inputs/geom/minos.root and interpret it using 'mm' as the
             length unit and 'g/cm^3' as the density unit.
             The job will stop as it accumulates a sample corresponding to 5E+17 POT.

         (2) shell% gNuMIevgen 
                       -r 1001 
                       -f /data/t2k/flux/hst/flux.root,12[h100],-12[h101],14[h200]
                       -g 1000080160[0.95],1000010010[0.05]
                       -n 50000

         You can further control the GENIE behaviour by setting its standard 
         environmental variables. Please read the GENIE user manual for more information.

\author  Costas Andreopoulos <costas.andreopoulos \at stfc.ac.uk>
         STFC, Rutherford Appleton Laboratory

\created August 20, 2008

\cpright Copyright (c) 2003-2009, GENIE Neutrino MC Generator Collaboration
         For the full text of the license visit http://copyright.genie-mc.org
         or see $GENIE/LICENSE
*/
//_________________________________________________________________________________________

#include <cassert>
#include <cstdlib>
#include <string>
#include <sstream>
#include <vector>
#include <map>

#include <TSystem.h>
#include <TTree.h>
#include <TFile.h>
#include <TH1D.h>
#include <TMath.h>
#include <TGeoVolume.h>
#include <TGeoShape.h>

#include "Conventions/Units.h"
#include "EVGCore/EventRecord.h"
#include "EVGDrivers/GFluxI.h"
#include "EVGDrivers/GMCJDriver.h"
#include "EVGDrivers/GMCJMonitor.h"
#include "Messenger/Messenger.h"
#include "Ntuple/NtpWriter.h"
#include "PDG/PDGLibrary.h"
#include "PDG/PDGCodes.h"
#include "Ntuple/NtpMCFormat.h"
#include "Utils/XSecSplineList.h"
#include "Utils/StringUtils.h"
#include "Utils/UnitUtils.h"
#include "Utils/CmdLineArgParserUtils.h"
#include "Utils/CmdLineArgParserException.h"

#ifdef __GENIE_FLUX_DRIVERS_ENABLED__
#include "FluxDrivers/GNuMIFlux.h"
#include "FluxDrivers/GCylindTH1Flux.h"
#endif

#ifdef __GENIE_GEOM_DRIVERS_ENABLED__
#include "Geo/ROOTGeomAnalyzer.h"
#include "Geo/PointGeomAnalyzer.h"
#endif

using std::string;
using std::vector;
using std::map;
using std::ostringstream;

using namespace genie;

void GetCommandLineArgs (int argc, char ** argv);
void PrintSyntax        (void);

// Default options (override them using the command line arguments):
//
string          kDefOptGeomLUnits   = "mm";    // default geometry length units
string          kDefOptGeomDUnits   = "g_cm3"; // default geometry density units
NtpMCFormat_t   kDefOptNtpFormat    = kNFGHEP; // default event tree format
string          kDefOptEvFilePrefix = "gntp";

// User-specified options:
//
Long_t          gOptRunNu;                     // run number
bool            gOptUsingRootGeom = false;     // using root geom or target mix?
bool            gOptUsingHistFlux = false;     // using jnubeam flux ntuples or flux from histograms?
map<int,double> gOptTgtMix;                    // target mix  (tgt pdg -> wght frac) / if not using detailed root geom
map<int,TH1D*>  gOptFluxHst;                   // flux histos (nu pdg  -> spectrum)  / if not using beam sim ntuples
string          gOptRootGeom;                  // input ROOT file with realistic detector geometry
string          gOptRootGeomTopVol = "";       // input geometry top event generation volume 
double          gOptGeomLUnits = 0;            // input geometry length units 
double          gOptGeomDUnits = 0;            // input geometry density units 
string          gOptExtMaxPlXml;               // max path lengths XML file for input geometry 
string          gOptFluxFile;                  // ROOT file with jnubeam flux ntuple
string          gOptDetectorLocation;          // detector location (see GNuMIFlux.xml for supported locations))
int             gOptNev;                       // number of events to generate
double          gOptPOT;                       // exposure (in POT)
string          gOptEvFilePrefix;              // event file prefix

//____________________________________________________________________________
int main(int argc, char ** argv)
{
  // Parse command line arguments
  GetCommandLineArgs(argc,argv);
  
  // Autoload splines (from the XML file pointed at the $GSPLOAD env. var.,
  // if the env. var. has been set)
  //XSecSplineList * xspl = XSecSplineList::Instance();
  //xspl->AutoLoad();

  // *************************************************************************
  // * Create / configure the geometry driver 
  // *************************************************************************
  GeomAnalyzerI * geom_driver = 0;
  double zmin=0, zmax=0;

  if(gOptUsingRootGeom) {
    //
    // *** Using a realistic root-based detector geometry description 
    //

    // creating & configuring a root geometry driver
    geometry::ROOTGeomAnalyzer * rgeom = 
            new geometry::ROOTGeomAnalyzer(gOptRootGeom);
    rgeom -> SetLengthUnits  (gOptGeomLUnits);
    rgeom -> SetDensityUnits (gOptGeomDUnits);
    rgeom -> SetTopVolName   (gOptRootGeomTopVol);
    // getting the bounding box dimensions along z so as to set the
    // appropriate upstream generation surface for the NuMI flux driver
    TGeoVolume * topvol = rgeom->GetGeometry()->GetTopVolume();
    if(!topvol) {
      LOG("gNuMIevgen", pFATAL) << "Null top ROOT geometry volume!";
      exit(1);
    }
    TGeoShape * bounding_box = topvol->GetShape();
    bounding_box->GetAxisRange(3, zmin, zmax);
    zmin *= rgeom->LengthUnits();
    zmax *= rgeom->LengthUnits();
    // casting to the GENIE geometry driver interface
    geom_driver = dynamic_cast<GeomAnalyzerI *> (rgeom);
  } 
  else {
    //
    // *** Using a 'point' geometry with the specified target mix 
    // *** ( = a list of targets with their corresponding weight fraction)
    //
 
    // creating & configuring a point geometry driver
    geometry::PointGeomAnalyzer * pgeom = 
  	      new geometry::PointGeomAnalyzer(gOptTgtMix);
    // casting to the GENIE geometry driver interface
    geom_driver = dynamic_cast<GeomAnalyzerI *> (pgeom);
  } 

  // *************************************************************************
  // * Create / configure the flux driver 
  // *************************************************************************
  GFluxI * flux_driver = 0;

  flux::GNuMIFlux *      numi_flux_driver = 0;
  flux::GCylindTH1Flux * hst_flux_driver  = 0;

  if(!gOptUsingHistFlux) {
    //
    // *** Using the detailed NuMI neutrino flux desription by feeding-in 
    // *** the gNuMI flux simulation ntuples
    //

    // creating & configuring a NuMI neutrino flux driver
    numi_flux_driver = new flux::GNuMIFlux;
    numi_flux_driver->LoadBeamSimData(gOptFluxFile, gOptDetectorLocation);
  //numi_flux_driver->SetFilePOT(gOptFluxNorm);
    numi_flux_driver->SetUpstreamZ(zmin);
    numi_flux_driver->SetNumOfCycles(0);
    // casting to the GENIE flux driver interface
    flux_driver = dynamic_cast<GFluxI *> (numi_flux_driver);
  } 
  else {
    //
    // *** Using fluxes from histograms (for all specified neutrino species)
    //

    // creating & configuring a generic GCylindTH1Flux flux driver
    TVector3 bdir (0,0,1); // dir along +z
    TVector3 bspot(0,0,0);
    hst_flux_driver = new flux::GCylindTH1Flux;
    hst_flux_driver->SetNuDirection      (bdir);
    hst_flux_driver->SetBeamSpot         (bspot);
    hst_flux_driver->SetTransverseRadius (-1);
    map<int,TH1D*>::iterator it = gOptFluxHst.begin();
    for( ; it != gOptFluxHst.end(); ++it) {
        int    pdg_code = it->first;
        TH1D * spectrum = it->second;
        hst_flux_driver->AddEnergySpectrum(pdg_code, spectrum);
    }
    // casting to the GENIE flux driver interface
    flux_driver = dynamic_cast<GFluxI *> (hst_flux_driver);
  }

  // *************************************************************************
  // * Create/configure the event generation driver 
  // *************************************************************************
  GMCJDriver * mcj_driver = new GMCJDriver;
  mcj_driver->UseFluxDriver(flux_driver);    
  mcj_driver->UseGeomAnalyzer(geom_driver);        
  mcj_driver->UseMaxPathLengths(gOptExtMaxPlXml);  
  mcj_driver->Configure();                         
  mcj_driver->UseSplines();        
  mcj_driver->ForceSingleProbScale();

  // *************************************************************************
  // * Prepare for writing the output event tree & status file
  // *************************************************************************

  // Initialize an Ntuple Writer to save GHEP records into a TTree
  NtpWriter ntpw(kDefOptNtpFormat, gOptRunNu);
  ntpw.Initialize(gOptEvFilePrefix);

  // Add a custom-branch at the standard GENIE event tree so that
  // info on the flux neutrino parent particle can be passed-through
  flux::GNuMIFluxPassThroughInfo * flux_info = 0;
  if(!gOptUsingHistFlux) {
    TBranch * flux = ntpw.EventTree()->Branch("flux",
       "genie::flux::GNuMIFluxPassThroughInfo", &flux_info, 32000, 1);
    assert(flux);
    flux->SetAutoDelete(kFALSE);
  }

  // Create a MC job monitor for a periodically updated status file
  GMCJMonitor mcjmonitor(gOptRunNu);

  // *************************************************************************
  // * Event generation loop
  // *************************************************************************

  int ievent = 0;
  while (1) 
  {
     LOG("gNuMIevgen", pNOTICE) 
          << " *** Generating event............ " << ievent;

     // In case the required statistics was expressed as 'number of events'
     // then quit if that number has been generated
     if(ievent == gOptNev) break;

     // In case the required statistics was expressed as 'number of POT' 
     // then exit the event loop if the requested POT has been generated.
     if(gOptPOT>0) {
        double fpot = numi_flux_driver->POT_curr();     // current POT in flux file 
        double psc  = mcj_driver->GlobProbScale();      // interaction prob. scale 
        double pot  = fpot / psc;                       // POT for generated sample
        if(pot >= gOptPOT) break;
     }

     // Generate a single event using neutrinos coming from the specified flux
     // and hitting the specified geometry or target mix
     EventRecord * event = mcj_driver->GenerateEvent();

     // Check whether a null event was returned due to the flux driver reaching
     // the end of the input flux ntuple - exit the event generation loop
     if(!event && numi_flux_driver->End()) {
	LOG("gNuMIevgen", pNOTICE) 
          << "** The NuMI flux driver read all the input flux ntuple entries";
	break;
     }
     if(!event) {
  	 LOG("gNuMIevgen", pERROR) 
             << "Got a null generated neutino event! Retrying ...";
         continue;
     }
     LOG("gNuMIevgen", pINFO)  
         << "Generated event: " << *event;

     // A valid event was generated: extract flux info (parent decay/prod
     // position/kinematics) for that simulated event so that it can be 
     // passed-through.
     // Can only do so if I am generating events using the jnubeam flux
     // ntuples, not simple histograms
     if(!gOptUsingHistFlux) {
        flux_info = new flux::GNuMIFluxPassThroughInfo(
            numi_flux_driver->PassThroughInfo());
        LOG("gNuMIevgen", pINFO) 
          << "Pass-through flux info associated with generated event: " 
          << *flux_info;
     }

     // Add event at the output ntuple, refresh the mc job monitor & clean-up
     ntpw.AddEventRecord(ievent, event);
     mcjmonitor.Update(ievent,event);
     delete event;
     if(flux_info) delete flux_info;
     ievent++;
  } //1

  LOG("gNuMIevgen", pNOTICE) 
    << "The GENIE MC job is done generaing events - Cleaning up & exiting...";

  // *************************************************************************
  // * Print job statistics & 
  // * calculate normalization factor for the generated sample
  // *************************************************************************
  if(!gOptUsingHistFlux && gOptUsingRootGeom) 
  {
    // POT normalization will only be calculated if event generation was based
    // on beam simulation  outputs (not just histograms) & a detailed detector
    // geometry description.
    double fpot = numi_flux_driver->POT_curr();     // current POT in flux file 
    double psc  = mcj_driver->GlobProbScale();      // interaction prob. scale 
    double pot  = fpot / psc;                       // POT for generated sample
    // Get nunber of flux neutrinos read-in by flux friver, number of flux
    // neutrinos actually thrown to the event generation driver and number
    // of neutrino interactions actually generated
    long int nflx_evg = mcj_driver       -> NFluxNeutrinos(); 
    long int nflx     = numi_flux_driver -> NFluxNeutrinos();
    long int nev      = ievent;

    LOG("gNuMIevgen", pNOTICE) 
        << "\n >> Interaction probability scaling factor:  " << psc
        << "\n >> N of flux v read-in by flux driver:      " << nflx
        << "\n >> N of flux v thrown to event gen driver:  " << nflx_evg
        << "\n >> N of generated v interactions:           " << nev
        << "\n ** Normalization for generated sample:      " << pot << " POT * detector";

    ntpw.EventTree()->SetWeight(pot); // store POT
  }

  // *************************************************************************
  // * Save & clean-up
  // *************************************************************************

  // Save the generated event tree & close the output file
  ntpw.Save();

  // Clean-up
  delete geom_driver;
  delete flux_driver;
  delete mcj_driver;
  map<int,TH1D*>::iterator it = gOptFluxHst.begin();
  for( ; it != gOptFluxHst.end(); ++it) {
    TH1D * spectrum = it->second;
    if(spectrum) delete spectrum;
  }
  gOptFluxHst.clear();

  LOG("gNuMIevgen", pNOTICE) << "Done!";

  return 0;
}
//____________________________________________________________________________
void GetCommandLineArgs(int argc, char ** argv)
{
  //
  // >>> get the command line arguments
  //

  // help?
  bool help = genie::utils::clap::CmdLineArgAsBool(argc,argv,'h');
  if(help) {
      PrintSyntax();
      exit(0);
  }

  LOG("gNuMIevgen", pNOTICE) << "Parsing command line arguments";

  // run number:
  try {
    LOG("gNuMIevgen", pDEBUG) << "Reading MC run number";
    gOptRunNu = genie::utils::clap::CmdLineArgAsInt(argc,argv,'r');
  } catch(exceptions::CmdLineArgParserException e) {
    if(!e.ArgumentFound()) {
      LOG("gNuMIevgen", pDEBUG) << "Unspecified run number - Using default";
      gOptRunNu = 0;
    }
  } //-r

  //
  // *** geometry
  //

  string geom = "";
  string lunits, dunits;
  try {
    LOG("gNuMIevgen", pDEBUG) << "Getting input geometry";
    geom = genie::utils::clap::CmdLineArgAsString(argc,argv,'g');

    // is it a ROOT file that contains a ROOT geometry?
    bool accessible_geom_file = 
            ! (gSystem->AccessPathName(geom.c_str()));
    if (accessible_geom_file) {
      gOptRootGeom      = geom;
      gOptUsingRootGeom = true;
    }                 
  } catch(exceptions::CmdLineArgParserException e) {
    if(!e.ArgumentFound()) {
      LOG("gNuMIevgen", pFATAL) 
        << "No geometry option specified - Exiting";
      PrintSyntax();
      exit(1);
    }
  } //-g

  if(gOptUsingRootGeom) {
     // using a ROOT geometry - get requested geometry units

     // legth units:
     try {
        LOG("gNuMIevgen", pDEBUG) 
           << "Checking for input geometry length units";
        lunits = genie::utils::clap::CmdLineArgAsString(argc,argv,'L');
     } catch(exceptions::CmdLineArgParserException e) {
        if(!e.ArgumentFound()) {
            LOG("gNuMIevgen", pDEBUG) << "Using default geometry length units";
            lunits = kDefOptGeomLUnits;
        }
     } // -L
     // density units:
     try {
        LOG("gNuMIevgen", pDEBUG) 
           << "Checking for input geometry density units";
        dunits = genie::utils::clap::CmdLineArgAsString(argc,argv,'D');
     } catch(exceptions::CmdLineArgParserException e) {
        if(!e.ArgumentFound()) {
            LOG("gNuMIevgen", pDEBUG) << "Using default geometry density units";
            dunits = kDefOptGeomDUnits;
        }
     } // -D 
     gOptGeomLUnits = genie::utils::units::UnitFromString(lunits);
     gOptGeomDUnits = genie::utils::units::UnitFromString(dunits);

     // check whether an event generation volume name has been 
     // specified -- default is the 'top volume'
     try {
        LOG("gNuMIevgen", pDEBUG) << "Checking for input volume name";
        gOptRootGeomTopVol = 
            genie::utils::clap::CmdLineArgAsString(argc,argv,'t');
     } catch(exceptions::CmdLineArgParserException e) {
        if(!e.ArgumentFound()) {
            LOG("gNuMIevgen", pDEBUG) << "Using the <master volume>";
        }
     } // -t 

     // check whether an XML file with the maximum (density weighted)
     // path lengths for each detector material is specified -
     // otherwise will compute the max path lengths at job init
     try {
        LOG("gNuMIevgen", pDEBUG) 
              << "Checking for maximum path lengths XML file";
        gOptExtMaxPlXml = 
            genie::utils::clap::CmdLineArgAsString(argc,argv,'m');
     } catch(exceptions::CmdLineArgParserException e) {
        if(!e.ArgumentFound()) {
            LOG("gNuMIevgen", pDEBUG) 
              << "Will compute the maximum path lengths at job init";
            gOptExtMaxPlXml = "";
        }
     } // -m
  } // using root geom?

  else {
    // User has specified a target mix.
    // Decode the list of target pdf codes & their corresponding weight fraction
    // (specified as 'pdg_code_1[fraction_1],pdg_code_2[fraction_2],...')
    // See documentation on top section of this file.
    //
    gOptTgtMix.clear();
    vector<string> tgtmix = utils::str::Split(geom,",");
    if(tgtmix.size()==1) {
         int    pdg = atoi(tgtmix[0].c_str());
         double wgt = 1.0;
         gOptTgtMix.insert(map<int, double>::value_type(pdg, wgt));    
    } else {
      vector<string>::const_iterator tgtmix_iter = tgtmix.begin();
      for( ; tgtmix_iter != tgtmix.end(); ++tgtmix_iter) {
   	 string tgt_with_wgt = *tgtmix_iter;
	 string::size_type open_bracket  = tgt_with_wgt.find("[");
	 string::size_type close_bracket = tgt_with_wgt.find("]");
         if (open_bracket ==string::npos || 
             close_bracket==string::npos) 
         {
             LOG("gNuMIevgen", pFATAL) 
                << "You made an error in specifying the target mix"; 
             PrintSyntax();
             exit(1);
         }
	 string::size_type ibeg = 0;
	 string::size_type iend = open_bracket;
	 string::size_type jbeg = open_bracket+1;
	 string::size_type jend = close_bracket;
         int    pdg = atoi(tgt_with_wgt.substr(ibeg,iend-ibeg).c_str());
         double wgt = atof(tgt_with_wgt.substr(jbeg,jend-jbeg).c_str());
         LOG("gNuMIevgen", pDEBUG) 
            << "Adding to target mix: pdg = " << pdg << ", wgt = " << wgt;
         gOptTgtMix.insert(map<int, double>::value_type(pdg, wgt));

      }// tgtmix_iter
    } // >1 materials in mix
  } // using tgt mix?

  //
  // *** flux 
  // 
  try {
    LOG("gNuMIevgen", pDEBUG) << "Getting input flux";
    string flux = genie::utils::clap::CmdLineArgAsString(argc,argv,'f');
    gOptUsingHistFlux = (flux.find("[") != string::npos);

    if(!gOptUsingHistFlux) {
        // Using jnubeam flux ntuples
        // Extract jnubeam flux (root) file name & detector location
        //
        vector<string> fluxv = utils::str::Split(flux,",");
        if(fluxv.size()<2) {
           LOG("gNuMIevgen", pFATAL) 
             << "You need to specify both a flux ntuple ROOT file " 
             << " _AND_ a detector location";
           PrintSyntax();
           exit(1);
        }
        gOptFluxFile         = fluxv[0];
        gOptDetectorLocation = fluxv[1];
    } else {
        // Using flux from histograms
        // Extract the root file name & the list of histogram names & neutrino 
        // species (specified as 'filename,histo1[species1],histo2[species2],...') 
        // See documentation on top section of this file.
        //
        vector<string> fluxv = utils::str::Split(flux,",");      
        if(fluxv.size()<2) {
           LOG("gNuMIevgen", pFATAL) 
             << "You need to specify both a flux ntuple ROOT file " 
             << " _AND_ a detector location";
           PrintSyntax();
           exit(1);
        }
        gOptFluxFile = fluxv[0];
        bool accessible_flux_file = !(gSystem->AccessPathName(gOptFluxFile.c_str()));
        if (!accessible_flux_file) {
            LOG("gNuMIevgen", pFATAL) 
              << "Can not access flux file: " << gOptFluxFile;
            PrintSyntax();
            exit(1);
        }
        // Extract energy spectra for all specified neutrino species
        TFile flux_file(gOptFluxFile.c_str(), "read");
        for(unsigned int inu=1; inu<fluxv.size(); inu++) {
            string nutype_and_histo = fluxv[inu];
   	    string::size_type open_bracket  = nutype_and_histo.find("[");
	    string::size_type close_bracket = nutype_and_histo.find("]");
            if (open_bracket ==string::npos || 
                close_bracket==string::npos) 
            {
                LOG("gNuMIevgen", pFATAL) 
                   << "You made an error in specifying the flux histograms"; 
                PrintSyntax();
                exit(1);
            }
   	    string::size_type ibeg = 0;
	    string::size_type iend = open_bracket;
	    string::size_type jbeg = open_bracket+1;
	    string::size_type jend = close_bracket;
            string nutype = nutype_and_histo.substr(ibeg,iend-ibeg);
            string histo  = nutype_and_histo.substr(jbeg,jend-jbeg);
            // access specified histogram from the input root file
            TH1D * ihst = (TH1D*) flux_file.Get(histo.c_str()); 
            if(!ihst) {
                LOG("gNuMIevgen", pFATAL) 
                  << "Can not find histogram: " << histo 
                  << " in flux file: " << gOptFluxFile;
                PrintSyntax();
                exit(1);
            }
            // create a local copy of the input histogram
            TH1D * spectrum = new TH1D(
                 histo.c_str(), histo.c_str(), ihst->GetNbinsX(),  
                 ihst->GetXaxis()->GetXmin(), ihst->GetXaxis()->GetXmax());
            spectrum->SetDirectory(0);
            for(int ibin = 1; ibin <= ihst->GetNbinsX(); ibin++) {
               spectrum->SetBinContent(ibin, ihst->GetBinContent(ibin));
            }
            // convert neutrino name -> pdg code
            int pdg = atoi(nutype.c_str());
            if(!pdg::IsNeutrino(pdg) && !pdg::IsAntiNeutrino(pdg)) {
                LOG("gNuMIevgen", pFATAL) 
                    << "Unknown neutrino type: " << nutype; 
                PrintSyntax();
                exit(1);
            }
            // store flux neutrino code / energy spectrum
            LOG("gNuMIevgen", pDEBUG) 
              << "Adding energy spectrum for flux neutrino: pdg = " << pdg;
            gOptFluxHst.insert(map<int, TH1D*>::value_type(pdg, spectrum));
        }//inu
        if(gOptFluxHst.size()<1) {
           LOG("gNuMIevgen", pFATAL) 
               << "You have not specified any flux histogram!";
           PrintSyntax();
           exit(1);
        }
        flux_file.Close();
    } // flux from histograms or from jnubeam ntuples?

  } catch(exceptions::CmdLineArgParserException e) {
    if(!e.ArgumentFound()) {
      LOG("gNuMIevgen", pFATAL) << "No flux info was specified - Exiting";
      PrintSyntax();
      exit(1);
    }
  }

  // number of events to generate
  try {
    LOG("gNuMIevgen", pDEBUG) 
        << "Reading limit on number of events to generate";
    gOptNev = genie::utils::clap::CmdLineArgAsInt(argc,argv,'n');
  } catch(exceptions::CmdLineArgParserException e) {
    if(!e.ArgumentFound()) {
      LOG("gNuMIevgen", pDEBUG)
        << "Will keep on generating events till the flux driver stops";
      gOptNev = -1;
    }
  } //-n

  // statistics to generate in terms of POT
  try {
    LOG("gNuMIevgen", pDEBUG)  << "Reading requested exposure in POT";
    gOptPOT = genie::utils::clap::CmdLineArgAsDouble(argc,argv,'e');
  } catch(exceptions::CmdLineArgParserException e) {
    if(!e.ArgumentFound()) {
      LOG("gNuMIevgen", pDEBUG) << "No POT exposure was requested";
      gOptPOT = -1;
    }
  } //-e

  // event file prefix
  try {
    LOG("gNuMIevgen", pDEBUG) << "Reading the event filename prefix";
    gOptEvFilePrefix = genie::utils::clap::CmdLineArgAsString(argc,argv,'o');
  } catch(exceptions::CmdLineArgParserException e) {
    if(!e.ArgumentFound()) {
      LOG("gNuMIevgen", pDEBUG)
        << "Will set the default event filename prefix";
      gOptEvFilePrefix = kDefOptEvFilePrefix;
    }
  } //-o

  //
  // >>> perform 'sanity' checks on command line arguments
  //

  // Tthe 'exposure' may be set either as:
  // - a number of POTs 
  // - a number of generated events
  // Only one of those options can be set.
  if(!gOptUsingHistFlux) {
    int nset=0;
    if(gOptPOT > 0) nset++;
    if(gOptNev > 0) nset++;
    if(nset==0) {
       LOG("gNuMIevgen", pFATAL) 
        << "** To use a gNuMI flux ntuple you need to specify an exposure, "
        << "either via the -e or -n options";
       PrintSyntax();
       exit(1);
    } 
    if(nset>1) {
       LOG("gNuMIevgen", pFATAL) 
         << "You can not specify more than one of the -e or -n options";
       PrintSyntax();
       exit(1);
    } 
  }
  // If we use a flux histograms (not gNuMI flux ntuples) then -currently- the
  // only way to control exposure is via a number of events
  if(gOptUsingHistFlux) {
     if(gOptNev < 0) {
       LOG("gNuMIevgen", pFATAL) 
         << "If you're using flux from histograms you need to specify the -n option";
       PrintSyntax();
       exit(1);
     }
  }
  // If we don't use a detailed ROOT detector geometry (but just a target mix) then 
  // don't accept POT as a way to control job statistics (not enough info is passed
  // in the target mix to compute POT & the calculation can be easily done offline)
  if(!gOptUsingRootGeom) {
    if(gOptPOT > 0) {
       LOG("gNuMIevgen", pFATAL) 
         << "You may not use the -e option without a detector geometry description";
       exit(1);
    }
  }

  //
  // >>> print the command line options
  //
  PDGLibrary * pdglib = PDGLibrary::Instance();

  ostringstream gminfo;
  if (gOptUsingRootGeom) {
    gminfo << "Using ROOT geometry - file = " << gOptRootGeom 
           << ", top volume = " 
           << ((gOptRootGeomTopVol.size()==0) ? "<master volume>" : gOptRootGeomTopVol)
           << ", max{PL} file = " 
           << ((gOptExtMaxPlXml.size()==0) ? "<none>" : gOptExtMaxPlXml)
           << ", length  units  = " << lunits
           << ", density units  = " << dunits;
  } else {
    gminfo << "Using target mix: ";
    map<int,double>::const_iterator iter;
    for(iter = gOptTgtMix.begin(); iter != gOptTgtMix.end(); ++iter) {
          int    pdg_code = iter->first;
          double wgt      = iter->second;
          TParticlePDG * p = pdglib->Find(pdg_code);
          if(p) {
            string name = p->GetName();
            gminfo << "(" << name << ") -> " << 100*wgt << "% / ";
          }//p?
    }
  }

  ostringstream fluxinfo;
  if(gOptUsingHistFlux) {
    fluxinfo << "Using histograms: ";
    map<int,TH1D*>::const_iterator iter;
    for(iter = gOptFluxHst.begin(); iter != gOptFluxHst.end(); ++iter) {
          int    pdg_code = iter->first;
          TH1D * spectrum = iter->second;
          TParticlePDG * p = pdglib->Find(pdg_code);
          if(p) {
            string name = p->GetName();
            fluxinfo << "(" << name << ") -> " << spectrum->GetName() << " / ";
          }//p?
    }
  } else {
    fluxinfo << "Using jnubeam simulation - "
             << "file = "        << gOptFluxFile
             << ", location = "  << gOptDetectorLocation;
  }

  ostringstream exposure;
  if(gOptPOT > 0) 
      exposure << "Number of POTs = " << gOptPOT;
  if(gOptNev > 0) 
      exposure << "Number of events = " << gOptNev;

  LOG("gNuMIevgen", pNOTICE) 
     << "\n MC Job (" << gOptRunNu << ") Settings: "
     << "\n - Flux     @ " << fluxinfo.str()
     << "\n - Geometry @ " << gminfo.str()
     << "\n - Exposure @ " << exposure.str();
}
//____________________________________________________________________________
void PrintSyntax(void)
{
  LOG("gNuMIevgen", pFATAL) 
   << "\n **Syntax**"
   << "\n gNuMIevgen [-h] [-r run#]"
   << "\n            -f flux -g geometry"
   << "\n            [-p pot_normalization_of_flux_file]"
   << "\n            [-t top_volume_name_at_geom] [-m max_path_lengths_xml_file]"
   << "\n            [-L length_units_at_geom] [-D density_units_at_geom]"
   << "\n            [-n n_of_events] [-c flux_cycles] [-e, -E exposure_in_POTs]"
   << "\n            [-o output_event_file_prefix]"
   << "\n"
   << " Please also read the detailed documentation at "
   << "$GENIE//src/support/numi/EvGen/gNuMIExptEvGen.cxx"
   << "\n";
}
//____________________________________________________________________________
