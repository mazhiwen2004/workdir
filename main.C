#include <iostream>
#include <cmath>
#include <string>
#include "TApplication.h"
#include "TRint.h"
#include "Geom.hh"
#include "Track.hh"
#include "Ionization.hh"
#include "Process.hh"

int main(int argc,char **argv)
{
    if(argc<3){
        cout<<"Usage: ./lanlanhe mom costheta\n";
        return 1;
    }
    double mom=atof(argv[1]);
    cout<<"Momentum===========> "<<mom<<endl;

    // int tracknum=atoi(argv[2]);
    // cout<<"tracknumber===========> "<<tracknum<<endl;

    double costheta=atof(argv[2]);
    cout<<"cos theta===========> "<<costheta<<endl;

    // double padlength=atof(argv[2]);
    // cout<<"padlength===========> "<<padlength<<endl;
    
    TApplication app("app", &argc, argv);
    //input track number && magnetic
    Geom geom(5);
    geom.SetPadLength(0.1);
    geom.SetPadWidth(0.1);
    geom.SetHalfLength(160);
    //geom.SetInnerR(0);
    geom.SetOuterR(47.5);
    
    cout<< "TrackNumber=====>"<<geom.GetTrackNum() <<endl;
    //cout<< "Magnetic========>"<<geom.GetMagneticField()<<" T"<<endl;
    cout<< "PadLength=========>"<<geom.GetPadLength()<<" cm"<<endl;
    cout<< "PadWidth=========>"<<geom.GetPadWidth()<<" cm"<<endl;
    //cout<< "InnerR==========>"<<geom.GetInnerR()<<" cm"<<endl;
    cout<< "OuterR==========>"<<geom.GetOuterR()<<" cm"<<endl;
    cout<< "HalfLength======>"<<geom.GetHalfLength()<<" cm"<<endl;


    Track trk(&geom);
    string str="kaon";
    trk.SetIncidentParticle(str);

    trk.SetMomentum(mom);
    

    string filename=Form("./RESULTS/IonInfo%s_%.0fGeV_TDR_%.2ftheta.root",str.c_str(),mom,costheta);
    //string filename=Form("./test01.root");
    
    Cluster clus(filename);
    double yDirection = sqrt(1-costheta*costheta)/costheta;
    cout << "yDirection============== " <<yDirection <<endl;
    Ionization ionsimu(&trk,&geom,&clus);
    ionsimu.SetStartPoint(0,0,0);
    ionsimu.SetIncidentDirection(0,yDirection,1);
    ionsimu.RunIonization(geom.GetHalfLength(),geom.GetOuterR());

    double arrFitRes[4]={0.,0.,0.,0.};
    FindHit(filename,geom.GetTrackNum(),geom.GetHalfLength(),geom.GetOuterR(),geom.GetPadLength(),geom.GetPadWidth(),mom,trk.GetParticleName(),arrFitRes,costheta);
    //FindHit(filename,10,2,0.05,20,"pion",arrFitRes);
    ofstream fout("./Ret.txt",ios::app);
    //fout<<mom<<"\t"<<arrFitRes[0]<<"\t"<<arrFitRes[1]<<"\t"<<arrFitRes[2]<<"\t"<<arrFitRes[3]<<endl;
    fout<<mom<<"\t"<<arrFitRes[0]<<"\t"<<endl;
    fout.close();

    cout << "code finished" <<endl;
    //app.Run(kTRUE);
    return 0;
}
