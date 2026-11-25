#ifndef CLUSTER_H
#define CLUSTER_H

#include <iostream>
#include <string>
#include <stdio.h>
#include <vector>

#include "TFile.h"
#include "TTree.h"
#include "TBranch.h"


class Cluster
{

public:
    Cluster(std::string ROOTfileName);
    ~Cluster();

    void OpenROOTFile();
    void PushVector(double xe,double ye, double ze, double te, double ee);
    void FillEleTree(int trkid,int clusid,int ne, double tc, double ec);
    void SaveTree();



private:
    TFile *fFile;
    TTree *fEleTree;
    std::string fFileName;

    std::vector<double> *m_vecxe=nullptr;
    std::vector<double> *m_vecye=nullptr;
    std::vector<double> *m_vecze=nullptr;
    std::vector<double> *m_vecte=nullptr;
    std::vector<double> *m_vecee=nullptr;
    
    

    int m_trkid=0;
    int m_clusid=0;
    int m_ne=0;
    double m_tc = 0.;
    double m_ec=0.;
    double m_xe=0.,m_ye=0.,m_ze=0.,m_te=0.,m_ee=0.;
    
};



#endif