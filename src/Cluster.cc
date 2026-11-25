#include "Cluster.hh"

Cluster::Cluster(std::string ROOTfileName) : fFile(0), fEleTree(0)
{
    fFileName=ROOTfileName;

    m_vecxe=new std::vector<double>();
    m_vecye=new std::vector<double>();
    m_vecze=new std::vector<double>();
    m_vecte=new std::vector<double>();
    m_vecee=new std::vector<double>();
}

Cluster::~Cluster(){
    delete m_vecxe;
    delete m_vecye;
    delete m_vecze;
    delete m_vecte;
    delete m_vecee;
}

void Cluster::OpenROOTFile()
{
    fFile=new TFile(fFileName.c_str(),"RECREATE");
    fEleTree = new TTree("IonEleInfo","IonEleInfo");
    fEleTree->Branch("trkid",&m_trkid,"trkid/I");
    fEleTree->Branch("clusid",&m_clusid,"clusid/I");
    fEleTree->Branch("ne",&m_ne,"ne/I");
    fEleTree->Branch("tc",&m_tc,"tc/D");
    fEleTree->Branch("ec",&m_ec,"ec/D");
    fEleTree->Branch("vecxe",m_vecxe);
    fEleTree->Branch("vecye",m_vecye);
    fEleTree->Branch("vecze",m_vecze);
    fEleTree->Branch("vecte",m_vecte);
    fEleTree->Branch("vecee",m_vecee);
}

void Cluster::PushVector(double xe,double ye, double ze, double te, double ee)
{
    m_vecxe->emplace_back(xe);
    m_vecye->emplace_back(ye);
    m_vecze->emplace_back(ze);
    m_vecte->emplace_back(te);
    m_vecee->emplace_back(ee);
}
void Cluster::FillEleTree(int trkid,int clusid,int ne, double tc, double ec)
{
    //m_vecxe->clear();m_vecye->clear();m_vecze->clear();


    m_trkid=trkid;
    m_clusid=clusid;
    m_ne=ne;
    m_tc=tc;
    m_ec=ec;
    fEleTree->Fill();

    //clear vector
    m_vecxe->clear();
    m_vecye->clear();
    m_vecze->clear();
    m_vecte->clear();
    m_vecee->clear();

}
void Cluster::SaveTree()
{
    
    if(fEleTree->GetEntries()!=0)
    {
        fEleTree->Write();
        delete fEleTree;
    }
    fFile->Close();
    delete fFile;
}


