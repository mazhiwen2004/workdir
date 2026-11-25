#ifndef IONIZATION_H
#define IONIZATION_H

#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <math.h>
#include "TString.h"
#include "Garfield/TrackHeed.hh"
#include "Garfield/MediumMagboltz.hh"
#include "Garfield/ComponentAnalyticField.hh"
#include "Garfield/Sensor.hh"
#include "TTree.h"
#include "TFile.h"
#include "Geom.hh"
#include "Track.hh"
#include "Cluster.hh"

class Ionization
{

public:
  
    Ionization(Track* trk, Geom* geo,Cluster* clus);
    ~Ionization(){};

    
    
    void SetStartPoint(double x0,double z0,double y0);
    void SetStartTime(double t0);
    void SetIncidentDirection(double dx,double dz,double dy);


    double GetStartPoint() const{return m_x0, m_z0, m_y0;}
    double GetIncidentDirection() const{return m_dx,m_dz,m_dy;}
    double GetStartTime() const{return m_t0;}


    void RunIonization(double halflength,double outerR);
    

private:
    Track *fTrk;
    Geom *fgeo;
    Cluster* fClus;


    Garfield::TrackHeed *fGarTrack;
    //Garfield::MediumMagboltz *fGas;
    Garfield::Sensor *fSensor;
    TFile *file;
    
    double m_t0 = 0;
    double m_x0 = 0, m_y0 = 0, m_z0 = 0;
    double m_dx = 0, m_dy = 0, m_dz = 0;
    
};



#endif