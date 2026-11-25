#ifndef TRACK_H
#define TRACK_H
#include <iostream>
#include <string>
#include <vector>
#include <array>
#include <algorithm>
#include <cmath>
#include "TMath.h"
#include "TString.h"
#include "Garfield/TrackHeed.hh"
#include "Garfield/MediumMagboltz.hh"
#include "Garfield/ComponentAnalyticField.hh"
#include "Garfield/Sensor.hh"
#include "TTree.h"
#include "TFile.h"
#include "Geom.hh"

using namespace std;
// using namespace Garfield;

class Track
{
public:
    Track(Geom *geom);
    ~Track(){};

    void SetIncidentParticle(string particleName);
    void SetMomentum(double momentum);
    void SetTheta(double theta);
    void SetDirection(double dx,double dz,double dy);
    void SetBetaGamma(double bg);
    
    //void SetParticleDirection(array<double, 3> direction);
 

    double GetMomentum() const { return m_momentum; }
    double GetBetaGamma() const {return m_bg; }
    string GetParticleName() { return fParticleName; }
    array<double,3> GetDirection() { return fDirection;}

    



private:
    Geom *m_geom;
    string fParticleName;
    vector<string> fVParticleName{"muon", "mu-", "mu+",
                                  "electron", "positron", "e-", "e+",
                                  "pion", "pi", "pi-", "pi+",
                                  "kaon", "K", "K-", "K+",
                                  "proton", "p", "anti-proton", "antiproton", "p-bar",
                                  "deuteron", "d"};

    double m_momentum = 1;
    double m_bg = 1;

    const double m_pi = TMath::Pi();
    double m_theta = 0;
    array<double, 3> fDirection{0.,0.,1.};

};

#endif