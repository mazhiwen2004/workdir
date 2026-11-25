#ifndef PROCESS_H
#define PROCESS_H

#include <string>
#include <iostream>
#include <cmath>
#include <set>
#include <vector>
#include <fstream>
#include <math.h>
#include <unordered_map>
#include <utility>

#include "TString.h"
#include "TFile.h"
#include "TTree.h"
#include "TH1F.h"
#include "TRandom3.h"
#include "TCanvas.h"
#include "TFitResult.h"
#include "TFitResultPtr.h"
#include "TH2D.h"
#include "TF1.h"

#include "Geom.hh"
#include "Track.hh"

void GetRowColId(int &row_id,int &col_id,double xxe,double yye,double padlength,double padwidth,double outerR);
void FindHit(std::string ROOTfileName,int trknum,double halflength, double outerR,double padlength,double padwidth,double momentum,string particle_name,double *fitRes,double costheta);




#endif

