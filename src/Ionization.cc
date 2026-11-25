#include "Ionization.hh"
#include "TRandom3.h"


Ionization::Ionization(Track* trk, Geom* geo,Cluster *clus):fTrk(trk),fgeo(geo),fClus(clus)
{
    fGarTrack=new Garfield::TrackHeed();
}

void Ionization::SetStartPoint(double x0,double z0,double y0)
{
    m_x0 = x0;
    m_z0 = z0;
    m_y0 = y0;
}

void Ionization::SetIncidentDirection(double dx,double dy,double dz)
{
    m_dx = dx;
    m_dy = dy;
    m_dz = dz;
}

void Ionization::SetStartTime(double t0)
{
    m_t0 = t0;
}

void Ionization::RunIonization(double halflength,double outerR)
{
    auto gas = fgeo->GetGas();
    auto comp = fgeo->GetComp();
    comp->SetMedium(gas);
    auto sensor=fgeo->GetDriftSensor();

    sensor->AddComponent(comp);
    fGarTrack->SetSensor(sensor);
    fGarTrack->SetParticle(fTrk->GetParticleName());
    fGarTrack->SetMomentum(fTrk->GetMomentum());
    // fGarTrack->SetBetaGamma(fTrk->GetBetaGamma());
    //fGarTrack->EnableMagneticField();

    double maxrange = 0., rforstraight = 0., stepstraight = 0., stepcurved = 0.;
    fGarTrack->GetSteppingLimits(maxrange, rforstraight, stepstraight, stepcurved);
    stepcurved = 0.01;
    fGarTrack->SetSteppingLimits(maxrange, rforstraight, stepstraight, stepcurved);
    //fGarTrack->EnableDebugging();
    
    double xc(0.),yc(0.),zc(0.);
    double xe(0.),ye(0.),ze(0.),te(0.),ee(0.);
    double dx(0.),dy(0.),dz(0.);

    std::vector<double> vecDirect {0,0,0};

    //Open root file
    fClus->OpenROOTFile();

    for(int iTrk=0; iTrk<(fgeo->GetTrackNum()); iTrk++)
    {
        //cout<<"=============\n";
        int temp=gRandom->Integer(1);

        //fTrk->SetDirection(vecDirect.at(0),vecDirect.at(1),vecDirect.at(2));
        //fGarTrack->NewTrack(m_x0,m_y0,m_z0,m_t0,(fTrk->GetDirection()).at(0),(fTrk->GetDirection()).at(1),(fTrk->GetDirection()).at(2));
        //cout << "x: " <<m_x0 <<" y: " <<m_y0 << " z: " <<m_z0 <<endl;
        //cout << "dx: " <<m_dx <<" dy: " <<m_dz << " dz: " <<m_dy <<endl;
        fGarTrack->NewTrack(m_x0,m_y0,m_z0,m_t0,m_dx,m_dz,m_dy);
        //cout << "dx: " <<m_dx <<" dy: " <<m_dz << " dz: " <<m_dy <<endl;
        double ec = 0., extra = 0.;
        int nc = 0;
        //double ee = 0.;
        double tc = 0;
        int Cnts_clus=0;
        //cout<<"=============\n";
        while (fGarTrack->GetCluster(xc, zc, yc, tc, nc, ec, extra))
        {
            //cout << "tc: " <<tc <<" ec: " <<ec <<endl;
            bool flag=false;
            for (int k = 0; k < nc; k++)
            {
                fGarTrack->GetElectron(k, xe, ze, ye, te, ee, dx, dz, dy);
                auto DisL = halflength - ze;
                //cout << "ze: " << ze << " DisL: " <<DisL <<endl;
                if(sqrt(xe*xe+ye*ye) < (outerR-0.05)
                && DisL > 0)
                {
                    fClus->PushVector(xe, ye, ze, te, ee);
                    flag=true;
                }
                
            }
            if(flag)
            {
                fClus->FillEleTree(iTrk, Cnts_clus, nc,tc,ec);
                Cnts_clus++;
            }
            
        }
    }

    fClus->SaveTree();
}