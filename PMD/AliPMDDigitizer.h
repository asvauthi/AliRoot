#ifndef ALIPMDDIGITIZER_H
#define ALIPMDDIGITIZER_H
//-----------------------------------------------------//
//                                                     //
//  Header File : PMDDigitization.h, Version 00        //
//                                                     //
//  Date   : September 20 2002                         //
//                                                     //
//-----------------------------------------------------//

class TClonesArray;
class TFile;
class TMath;
class TObjArray;
class TParticle;
class TTree;
class TNtuple;

class AliLoader;
class AliRunLoader;
class AliRun;
class AliDetector;
class AliPMDhit;
class AliHit;
class AliHeader;

class AliPMDcell;
class AliPMDsdigit;
class AliPMDdigit;
class AliPMDClustering;

class AliPMDDigitizer
{
 public:

  AliPMDDigitizer();
  virtual ~AliPMDDigitizer();

  void OpengAliceFile(char *file, Option_t *option);

  void Hits2SDigits(Int_t ievt);
  void Hits2Digits(Int_t ievt);
  void SDigits2Digits(Int_t ievt);
  void TrackAssignment2Cell();
  void MeV2ADC(Float_t mev, Float_t & adc);
  void AddSDigit(Int_t trnumber, Int_t det, Int_t smnumber, 
		 Int_t cellnumber, Float_t adc);
  void AddDigit(Int_t trnumber, Int_t det, Int_t smnumber, 
		Int_t cellnumber, Float_t adc);
  void  SetZPosition(Float_t zpos);
  Float_t GetZPosition() const;
  void ResetCell();
  void ResetSDigit();
  void ResetDigit();
  void ResetCellADC();
  void UnLoad(Option_t * /* option */);

 protected:
  AliRunLoader *fRunLoader;  // Pointer to Run Loader
  AliRun       *fAlice;      // Pointer to a Run
  AliPMDhit    *fPMDHit;     // Pointer to specific detector hits
  AliDetector  *fPMD;        // Get pointers to Alice detectors 
                             // and Hits containers 
  AliLoader    *fPMDLoader;  // Pointer to specific detector loader

  TClonesArray *fHits;       // Pointer to hits array
  TObjArray    *fPArray;     // Pointer to particle array
  TParticle    *fParticle;   // Pointer to a given particle

  TTree        *fTreeH;      // Hits tree
  TTree        *fTreeS;      // Summable Digits tree
  TTree        *fTreeD;      // Digits tree

  TClonesArray *fSDigits;    // List of summable digits
  TClonesArray *fDigits;     // List of digits

  TObjArray    *fCell;       // List of pmd cells
  AliPMDcell   *fPMDcell;    // Pointer to a PMD cell

  Int_t   fNsdigit;          // Summable digits counter
  Int_t   fNdigit;           // Digits counter
  Int_t   fDetNo;            // Detector Number (0:PRE, 1:CPV)
  Float_t fZPos;             // z-position of the detector

  static const Int_t fgkTotUM = 24; // Total Unit modules in one detector
  static const Int_t fgkRow   = 48; // Total number of rows in one unitmodule
  static const Int_t fgkCol   = 96; // Total number of cols in one unitmodule
  Float_t fCPV[fgkTotUM][fgkRow][fgkCol]; // CPV Array containing total edep
  Float_t fPRE[fgkTotUM][fgkRow][fgkCol]; // PRE Array containing total edep
  Int_t   fPRECounter[fgkTotUM][fgkRow][fgkCol]; // Number of times each cell
                                                 // is fired in PMD
  Int_t   fPRETrackNo[fgkTotUM][fgkRow][fgkCol]; // PRE Array containing track number
  Int_t   fCPVTrackNo[fgkTotUM][fgkRow][fgkCol]; // CPV Array containing track number


  ClassDef(AliPMDDigitizer,2)    // To digitize PMD Hits
};
#endif

