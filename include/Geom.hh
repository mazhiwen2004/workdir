#ifndef GEOM_H
#define GEOM_H
#include <iostream>
#include <cmath>
#include <string>
#include <algorithm>

#include "TH2D.h"
#include "TMath.h"
#include "TGraph2D.h"
#include "TH3F.h"
#include "TPolyLine3D.h"
#include "TCanvas.h"

#include "Garfield/MediumMagboltz.hh"
#include "Garfield/ComponentAnalyticField.hh"
#include "Garfield/Sensor.hh"
#include "TH2Poly.h"

//for CEPC
//The inner  radius = 0.3 m
//The outer radius = 1.8 m
//The full length = 4.7 m

using namespace std;
class Geom
{

public:
    Geom();
    Geom(int trknum);
    ~Geom();
    
   

    

    void SetTrackNum(int trknum);
    //void SetMagneticField(int magnetic);
    //仅为初始设置的径迹长度，并不是TPC内的径迹长度
    void SetTrackLength(double trklength);
    void SetPadLength(double padlength);
    void SetPadWidth (double padwidth);
    void SetPixelSize(double pixelsize);
    //void SetInnerR(double innerR);
    void SetOuterR(double outerR);
    void SetHalfLength(double halflength);

    

    
    double GetTrackLength() const{return m_trklength;} 
    //double GetMagneticField() const { return m_magnetic; }
    int GetTrackNum() const{return m_trknum;}
    double GetPadLength() const{return m_padlength;}
    double GetPadWidth() const{return m_padwidth;}
    double GetPixelSize() const{return m_pixelsize;}
    //double GetInnerR() {return m_innerR;}
    double GetOuterR() {return m_outerR;}
    double GetHalfLength() const{return m_halflength;}

    TCanvas* ShowTrack(std::vector<double> px,std::vector<double> py,std::vector<double> pz);
    TH2Poly* PadHist();

    Garfield::MediumMagboltz* GetGas() { return fGas;}
    Garfield::ComponentAnalyticField* GetComp() {return fComp;}
    Garfield::Sensor* GetDriftSensor(){return fSensor;}
    


private:

    Garfield::MediumMagboltz *fGas;
    Garfield::ComponentAnalyticField *fComp;
    Garfield::Sensor *fSensor;

    int m_trknum=1;
    //int m_magnetic=0;
    double m_trklength = 3000; //cm
    double m_padlength = 0.1; //cm
    double m_padwidth = 0.1; //cm
    double m_pixelsize = 0.05; //cm

    double m_halflength = 160; //cm
    //double m_innerR = 60; //cm
    double m_outerR = 47.5; //cm

    TH1F *hEleNum;
    TGraph2D *grTrk;
    TH3F *hEmpty;
    TPolyLine3D *lcircle;
    TPolyLine3D *line3d;
};




#endif