

#include "Geom.hh"


Geom::Geom() {}
Geom::~Geom() {}

Geom::Geom(int trknum) : m_trknum(trknum)
{
    fGas = new Garfield::MediumMagboltz();
    fGas->SetTemperature(293.15);
    fGas->SetPressure(750.);
    fGas->SetComposition("Ar", 95., "isobutane", 2., "CF4", 3.);

    //在Garfield中没有AddPlaneZ这一选项，因此当前的Y坐标为全局中的Z坐标
    //即为束流方向(后面会进行坐标变换)

    fComp = new Garfield::ComponentAnalyticField();
    fComp->AddPlaneY(0, -60000., "pad_plane");
    fComp->AddPlaneY(160, 0., "HV");
    //fComp->SetMagneticField(0, magnetic, 0);

    fSensor = new Garfield::Sensor();
    fSensor->SetArea(-50, 0, -50, 50, 160, 50);
}

void Geom::SetTrackLength(double trklength)
{ 
    m_trklength = trklength;
}

void Geom::SetTrackNum(int trknum)
{
    m_trknum = trknum;
}
/*void Geom::SetMagneticField(int magnetic)
{
    m_magnetic = magnetic;
}*/
void::Geom::SetPadLength(double padlength)
{
    m_padlength = padlength;
}
void::Geom::SetPadWidth(double padwidth)
{
    m_padwidth = padwidth;
}
void Geom::SetPixelSize(double pixelsize)
{
    m_pixelsize = pixelsize;
}

void Geom::SetHalfLength(double halflength)
{
    m_halflength = halflength;
}

/*void Geom::SetInnerR(double innerR)
{
    m_innerR = innerR;
}*/

void Geom::SetOuterR(double outerR)
{
    m_outerR = outerR;
}
//TPC简图显示
TCanvas* Geom::ShowTrack(std::vector<double> px,std::vector<double> pz,std::vector<double> py)
{
    auto myc=new TCanvas("cc","cc",800,600);
    TGraph2D *grTrk=new TGraph2D(px.size(),px.data(),pz.data(),py.data());
    auto hEmpty=new TH3F("hEmpty",";x;z;y",10,-m_outerR,m_outerR,10,0,m_halflength+20,10,-m_outerR,m_outerR);
    hEmpty->SetStats(0);
    hEmpty->Draw("FB BB");
    
    double angle=0;
    lcircle = nullptr;
    double Posz[2]={0.,m_halflength};
    for(int i=0;i<2;++i){
        lcircle=new TPolyLine3D(100);
        for(int j=0;j<=100;++j){
            angle=j*2*TMath::Pi()/100.;
            lcircle->SetPoint(j,m_outerR*TMath::Cos(angle),Posz[i],m_outerR*TMath::Sin(angle));
        }

        lcircle->SetLineColor(kBlue);lcircle->SetLineStyle(kDashDotted);
        lcircle->Draw("same");
    }
    /*for(int i=0;i<2;++i){
        lcircle=new TPolyLine3D(100);
        for(int j=0;j<=100;++j){
            angle=j*2*TMath::Pi()/100.;
            lcircle->SetPoint(j,innerR*TMath::Cos(angle),Posz[i],innerR*TMath::Sin(angle));
        }

        lcircle->SetLineColor(kBlue);lcircle->SetLineStyle(kDashDotted);
        lcircle->Draw("same");
    }*/
    line3d = nullptr;
    for(int i=0;i<=8;++i){
        line3d=new TPolyLine3D(2);
        angle=i*2*TMath::Pi()/8.;
        line3d->SetPoint(0,m_outerR*TMath::Cos(angle),0.,m_outerR*TMath::Sin(angle));
        line3d->SetPoint(1,m_outerR*TMath::Cos(angle),m_halflength,m_outerR*TMath::Sin(angle));
        line3d->SetLineColor(kBlue);line3d->SetLineStyle(kDashed);
        line3d->Draw("same");
    }

    grTrk->SetMarkerStyle(kFullCircle);
    grTrk->SetMarkerSize(0.2);
    grTrk->SetMarkerColor(kOrange-3);
    grTrk->Draw("p same FB BB");
    return myc;
}



