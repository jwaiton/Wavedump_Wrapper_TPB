/*----------
  PURPOSE
  TStyle for WATCHMAN
  Matthew Needham 10 07 19

  ORIGIN (also see comments below ////////////)
  Developed from LHCB style settings for use as a macro.
  
  MODIFIED 12 07 19
  gary.smith@ed.ac.uk
  
  MODIFICATIONS
  Developed for general usage 
  (including in compiled code)
  as Getter function which
  returns TStyle object
  
  Lines removed from the end of the file 
  gROOT->SetStyle("wmStyle");
  gROOT->ForceStyle();
  now have to be implemented locally
  
  USAGE
  #include "TStyle.h"
  #include "wmStyle.C"
  
  //..... code
  
  TStyle * wmStyle = GetwmStyle();
  
  gROOT->SetStyle("wmStyle");
  gROOT->ForceStyle();
  
  //....  code
  
*/



////////////////////////////////////////////////////////////////////
// PURPOSE:
//
// This macro defines a reasonable style for (black-and-white) 
// "publication quality" ROOT plots. The default settings contain 
// many features that are either not desirable for printing on white 
// paper or impair the general readibility of plots.
//
// USAGE:
//
// Simply include the line
//   gROOT->ProcessLine(".x $LHCBSTYLE/root/lhcbstyle.C");
// at the beginning of your root macro.
//
// SOME COMMENTS:
//
// Statistics and fit boxes:
//
// "Decorative" items around the histogram are kept to a minimum.
// In particular there is no box with statistics or fit information.
// You can easily change this either by editing your private copy
// of this style file or by calls to "gStyle" in your macro.
// For example, 
//   gStyle->SetOptFit(1011);
// will add some fit information.
//
// Font:
// 
// The font is chosen to be 62, i.e.helvetica-bold-r-normal with
// precision 2. Font is of course a matter of taste, but most people
// will probably agree that Helvetica bold gives close to optimal
// readibility in presentations. It appears to be the ROOT default, 
// and since there are still some features in ROOT that simply won't 
// respond to any font requests, it is the wise choice to avoid 
// ugly font mixtures on the same plot... The precision of the font (2)
// is chosen in order to have a rotatable and scalable font. Be sure
// to use true-type fonts! I.e.
// Unix.*.Root.UseTTFonts: true  in your .rootrc file. 
//
// "Landscape histograms":
//
// The style here is designed for more or less quadratic plots.
// For very long histograms, adjustements are needed. For instance, 
// for a canvas with 1x5 histograms:
//  TCanvas* c1 = new TCanvas("c1", "L0 muons", 600, 800);
//  c1->Divide(1,5);
// adaptions like the following will be needed:
//  gStyle->SetTickLength(0.05,"x");
//  gStyle->SetTickLength(0.01,"y");
//  gStyle->SetLabelSize(0.15,"x");
//  gStyle->SetLabelSize(0.1,"y");
//  gStyle->SetStatW(0.15);
//  gStyle->SetStatH(0.5);
//
////////////////////////////////////////////////////////////////////
//    Thomas.Schietinger@cern.ch" << endl;
////////////////////////////////////////////////////////////////////


#include "TStyle.h"
#include "TPaveText.h"
#include "TLatex.h"

TStyle * GetwmStyle(){
  
  TStyle *wmStyle= new TStyle("wmStyle","plots style");
  
  // use helvetica-bold-r-normal, precision 2 (rotatable)
  Int_t wmFont = 132;
  // line thickness
  Double_t wmWidth = 1.30;
  
  // use plain black on white colors
  wmStyle->SetFrameBorderMode(0);
  wmStyle->SetCanvasBorderMode(0);
  wmStyle->SetPadBorderMode(0);
  wmStyle->SetPadColor(0);
  wmStyle->SetCanvasColor(0);
  wmStyle->SetStatColor(0);
  wmStyle->SetPalette(1);
  //wmStyle->SetTitleColor(0);
  //wmStyle->SetFillColor(0);
  
  // set the paper & margin sizes
  wmStyle->SetPaperSize(20,26);
  wmStyle->SetPadTopMargin(0.05);
  //wmStyle->SetPadTopMargin(0.08);
  wmStyle->SetPadRightMargin(0.05); // increase for colz plots!!
  //wmStyle->SetPadRightMargin(0.08); // increase for colz plots!!
  //
  wmStyle->SetPadBottomMargin(0.15);
  wmStyle->SetPadLeftMargin(0.1);

  
  // use large fonts
  wmStyle->SetTextFont(wmFont);
  wmStyle->SetTextSize(0.05);
  wmStyle->SetLabelFont(wmFont,"x");
  wmStyle->SetLabelFont(wmFont,"y");
  wmStyle->SetLabelFont(wmFont,"z");
  wmStyle->SetLabelSize(0.05,"x");
  wmStyle->SetLabelSize(0.05,"y");
  wmStyle->SetLabelSize(0.05,"z");
  wmStyle->SetTitleFont(wmFont,"x");
  wmStyle->SetTitleFont(wmFont,"y");
  wmStyle->SetTitleSize(0.05,"x");
  wmStyle->SetTitleSize(0.05,"y");
  wmStyle->SetTitleSize(0.06,"z");
  
  // use bold lines and markers
  wmStyle->SetLineWidth(wmWidth);
  wmStyle->SetFrameLineWidth(wmWidth);
  wmStyle->SetHistLineWidth(wmWidth);
  wmStyle->SetFuncWidth(wmWidth);
  wmStyle->SetGridWidth(wmWidth);
  wmStyle->SetLineStyleString(2,"[12 12]"); // postscript dashes
  //wmStyle->SetMarkerStyle(15);
  wmStyle->SetMarkerStyle(20);
  // wmStyle->SetMarkerSize(1.5);
  wmStyle->SetMarkerSize(1.);
  
  //wmStyle->SetTitleYOffset(1.);
  wmStyle->SetTitleYOffset(1.);
  wmStyle->SetTitleXOffset(1.);
  // wmStyle->SetTitleYSize(0.02)
  // by default, do not display histogram decorations:
  wmStyle->SetOptStat(0);  
  //wmStyle->SetOptStat(1110);  // show only nent, mean, rms
  wmStyle->SetOptTitle(0);
  wmStyle->SetOptFit(0);
  //wmStyle->SetOptFit(1011); // show probability, parameters and errors
  
  // look of the statistics box:
  wmStyle->SetStatBorderSize(1);
  wmStyle->SetStatFont(wmFont);
  wmStyle->SetStatFontSize(0.05);
  /*
    wmStyle->SetStatX(0.9);
    wmStyle->SetStatY(0.9);
    wmStyle->SetStatW(0.25);
    wmStyle->SetStatH(0.15);
  */
  // put tick marks on top and RHS of plots
  wmStyle->SetPadTickX(1);
  wmStyle->SetPadTickY(1);
  
  // histogram divisions: only 5 in x to avoid label overlaps
  wmStyle->SetNdivisions(505,"x");
  wmStyle->SetNdivisions(510,"y");
  
  TPaveText *wmName = new TPaveText(0.65,0.8,0.9,0.9,"BRNDC");
  // TPaveText *wmName = new TPaveText(0.2,0.3,0.45,0.4,"BRNDC");
  wmName->SetFillColor(0);
  wmName->SetTextAlign(12);
  wmName->SetBorderSize(0);
  // wmName->AddText("Wm");
  wmName->AddText("#splitline{#splitline{Wm}{Preliminary}}{#scale[0.7]{#sqrt{s} = 7 TeV Data}}");
  
  TText *wmLabel = new TText();
  wmLabel->SetTextFont(wmFont);
  wmLabel->SetTextColor(1);
  wmLabel->SetTextSize(0.04);
  wmLabel->SetTextAlign(12);
  
  TLatex *wmLatex = new TLatex();
  wmLatex->SetTextFont(wmFont);
  wmLatex->SetTextColor(1);
  wmLatex->SetTextSize(0.04);
  wmLatex->SetTextAlign(12);
  

  // Gary 5th Sept 19
  wmStyle->SetLegendBorderSize(0);

 return wmStyle;

}

