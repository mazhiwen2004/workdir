#include "Process.hh"

struct Element {
  int value;
  int Id;
  int flag;
};

struct InfoXY
{
    double x;
    double y;
};

struct PairHash {
    template <class T1, class T2>
    std::size_t operator()(const std::pair<T1, T2>& p) const {
        auto h1 = std::hash<T1>{}(p.first);
        auto h2 = std::hash<T2>{}(p.second);
        return h1 ^ h2;
    }
};
struct ClusInfo
{
    double Clus_x;
    double Clus_y;
    double Clus_z;
};

bool compare(const ClusInfo& a, const ClusInfo& b)
{
    return a.Clus_x < b.Clus_x;
}

struct HitPadNumAndDisL
{
    double HitPadNum;
    double Disl;
};

double GetEfficiencyFactor(double x_Distance,string particle_name);
void FindHit(std::string ROOTfileName,int trknum,double halflength, double outerR,double padlength,double padwidth,double momentum,string particle_name,double *fitRes,double costheta)
{
    
    double Mon = momentum/1.e+9;
    int padRowArray = int(outerR*2/padwidth);
    int padColArray = int(outerR*2/padlength);
    cout << "========================>outerR: " << outerR << " padRowArray: " << padRowArray<<" padColArray: " <<padColArray<< endl;
    //auto PadInfoROOTFile = Form("./RESULTS/%.2fGev%s_PadNum_5cm_allInfo_test%d.root",Mon,Par_name.c_str(),filenum);
    auto PadInfoROOTFile = Form("./RESULTS/%.2fGeV%s_%.2fpadlength_%.2fpadwidth_%.2fcostheta_30DiFu.root",momentum,particle_name.c_str(),padlength,padwidth,costheta);
    //auto PadInfoROOTFile = Form("test.root");
    int trkStart=0;
    int clusStart=0;
    TFile *myFileIn =TFile::Open(ROOTfileName.data());
    if(!myFileIn)
        return;
    
    TTree *myTreeIn = (TTree*)myFileIn->Get("IonEleInfo");
    int vi_Entry = myTreeIn->GetEntries();
    
    int vi_trkid=0;
    int vi_clusid=0;
    int vi_ne=0;
    auto vi_vecXe = new vector<double>();
    auto vi_vecYe = new vector<double>();
    auto vi_vecZe = new vector<double>();

    auto vecRow = new vector<int>();
    auto vecCol = new vector<int>();

    myTreeIn->SetBranchAddress("trkid",&vi_trkid);
    myTreeIn->SetBranchAddress("clusid",&vi_clusid);
    myTreeIn->SetBranchAddress("ne",&vi_ne);
    myTreeIn->SetBranchAddress("vecxe",&vi_vecXe);
    myTreeIn->SetBranchAddress("vecye",&vi_vecYe);
    myTreeIn->SetBranchAddress("vecze",&vi_vecZe);

    
    TFile *myFileOut = new TFile(PadInfoROOTFile,"RECREATE");
    myFileOut->cd();
    TTree *TruthPadInfo = new TTree("TruthPadInfo","TruthPadInfo");
    int TruthTrackID;
    double TruthHitNum_Drift;
    int Truth_Row;
    int Truth_Col;
    double Truth_TrackLength=0;
    TruthPadInfo->Branch("TruthTrackID",&TruthTrackID,"TruthTrackID/I");
    TruthPadInfo->Branch("TruthHitNum_Drift",&TruthHitNum_Drift,"TruthHitNum_Drift/D");
    TruthPadInfo->Branch("Truth_Row",&Truth_Row,"Truth_Row/I");
    TruthPadInfo->Branch("Truth_Col",&Truth_Col,"Truth_Col/I");
    TruthPadInfo->Branch("Truth_TrackLength",&Truth_TrackLength,"Truth_TrackLength/D");

    TTree *AvaPadInfo = new TTree("AvaPadInfo","AvaPadInfo");
    int AvaTrackID;
    double AvaHitNum_Ava;
    int Ava_Row;
    int Ava_Col;
    double Ava_DisL;
    AvaPadInfo->Branch("AvaTrackID",&AvaTrackID,"AvaTrackID/I");
    AvaPadInfo->Branch("AvaHitNum_Ava",&AvaHitNum_Ava,"AvaHitNum_Ava/D");
    AvaPadInfo->Branch("Ava_Row",&Ava_Row,"Ava_Row/I");
    AvaPadInfo->Branch("Ava_Col",&Ava_Col,"Ava_Col/I");
    AvaPadInfo->Branch("Ava_DisL",&Ava_DisL,"Ava_DisL/D");

    TTree *AvaPadInfo_Correct = new TTree("AvaPadInfo_Correct","AvaPadInfo_Correct");
    int AvaTrackID_Correct;
    double AvaHitNum_Ava_Correct;
    int Ava_Row_Correct;
    int Ava_Col_Correct;
    double Ava_DisL_Correct; //与Ava_DisL相同，只是又在这个branch里保存一遍；
    AvaPadInfo_Correct->Branch("AvaTrackID_Correct",&AvaTrackID_Correct,"AvaTrackID_Correct/I");
    AvaPadInfo_Correct->Branch("AvaHitNum_Ava_Correct",&AvaHitNum_Ava_Correct,"AvaHitNum_Ava_Correct/D");
    AvaPadInfo_Correct->Branch("Ava_Row_Correct",&Ava_Row_Correct,"Ava_Row_Correct/I");
    AvaPadInfo_Correct->Branch("Ava_Col_Correct",&Ava_Col_Correct,"Ava_Col_Correct/I");
    AvaPadInfo_Correct->Branch(" Ava_DisL_Correct",& Ava_DisL_Correct," Ava_DisL_Correct/D");
   
   
    
    TH2D *hShowTrk=new TH2D("projectionXY",";x [cm]; y [cm]",1000,-outerR,outerR,1000,-outerR,outerR);
    //TH1F *h4 = new TH1F("h4","h4",1000,0,10000);
    TF1 *f1=new TF1("fp","[0]*pow(1+[1],1+[1])*pow(x/[2],[1])*exp(-(1+[1])*x/[2])/TMath::Gamma(1+[1])",0,10000);
    f1->SetParameters(3802,0.487,20);
    // TRandom3 *random = new TRandom3();
    //                 for(int jj = 0; jj < 100000; jj++){
    //                     double randomvalue = f1->GetRandom();
    //                     h4->Fill(randomvalue);
    //                 }
    // h4->Draw();
    auto tempGeo = new Geom();
    vector<double> px,py,pz;

    double xe_AfterDrift_1trk =0;
    double ye_AfterDrift_1trk =0;

    double xe_AfterAva_1trk =0;
    double ye_AfterAva_1trk =0;

    int col=0;
    int row=0;

    // int padNum = padColArray*padRowArray;
    int progressInterval = 50;

    std::vector<ClusInfo> matrixClus;

    auto hbkgcut = new TH1F("hbkgcut","Cut Threshold",100,0,10000);
    auto hpadnum = new TH1F("hpadnum","Hit Pad Numbers",100,0,100);
    
    double x_diffusion =0.03118,y_diffusion=0.03122;
    /*if(magnetic == 0)
    {
        x_diffusion = 0.03118;
        y_diffusion = 0.03122;
    }
    else if (magnetic == 1)
    {
        x_diffusion = 8.66021e-03;
        y_diffusion = 8.71839e-03;
    }
    else if (magnetic == 2)
    {
        x_diffusion = 4.68999e-03;
        y_diffusion = 4.69770e-03;
    }

    else
    {
        x_diffusion = 3.22646e-03;
        y_diffusion = 3.26177e-03;
    }
    
    cout << "magnetic = " << magnetic <<"T" <<" x_diffusion = " <<x_diffusion <<" y_diffusion = " <<y_diffusion <<endl;*/
//================================首先从一条径迹中获取cut阈值====================================


    unordered_map<pair<int,int>,int, PairHash> Map_PadNumAfterDrift_1trk;
    unordered_map<pair<int,int>,int, PairHash> Map_PadNumAfterAva_1trk;
    
    
    for(int i=0;i<vi_Entry;++i)
    {
        myTreeIn->GetEntry(i);
        if(vi_trkid == 0)
        {
            for(int ii=0;ii<vi_vecXe->size();++ii)
            {
                auto distance = (double(halflength) - vi_vecZe->at(ii));
                if(sqrt(pow((*vi_vecXe)[ii], 2) + pow((*vi_vecYe)[ii], 2)) < (outerR-0.05)
                && distance > 0)
                {
                    if(ii==0)
                    {
                        matrixClus.push_back({vi_vecXe->at(ii),vi_vecYe->at(ii),vi_vecZe->at(ii)});
                    }
                    px.emplace_back(vi_vecXe->at(ii));
                    py.emplace_back(vi_vecYe->at(ii));
                    pz.emplace_back(vi_vecZe->at(ii));

                    //====================================Drift=====================================
                    xe_AfterDrift_1trk = gRandom->Gaus((*vi_vecXe)[ii],(x_diffusion*sqrt(distance)));
                    ye_AfterDrift_1trk = gRandom->Gaus((*vi_vecYe)[ii],(y_diffusion*sqrt(distance)));
                    GetRowColId(row,col,xe_AfterDrift_1trk,ye_AfterDrift_1trk,padlength,padwidth,outerR);
                    if(row>=padRowArray || col>=padColArray)
                    {
                        continue;
                    }
                    //HitPadNumAfterDrift_1trk[row][col]++;
                    Map_PadNumAfterDrift_1trk[{row,col}]++;
                    //=====================================Avalanche================================
                    //int ran = gRandom->Gaus(2000,45);
                    int poylaRandom = f1->GetRandom();
                    for(int j=0;j<poylaRandom;++j)
                    {
                        xe_AfterAva_1trk = gRandom->Gaus(xe_AfterDrift_1trk, 0.003);
                        ye_AfterAva_1trk = gRandom->Gaus(ye_AfterDrift_1trk, 0.003);
                        GetRowColId(row,col,xe_AfterAva_1trk,ye_AfterAva_1trk,padlength,padwidth,outerR);
                        // HitPadNumAfterAva_1trk[row][col]++;
                        Map_PadNumAfterAva_1trk[{row,col}]++;
                        hShowTrk->Fill(xe_AfterAva_1trk,ye_AfterAva_1trk);
                    }
                    
                }
            }
        }
    }

    //sort(matrixClus.begin(),matrixClus.end(),compare);
   double totTrkL=0.;
    for(int i=1; i< (int)matrixClus.size();++i)
    {
        double disClus = sqrt((matrixClus[i].Clus_x-matrixClus[i-1].Clus_x)*(matrixClus[i].Clus_x-matrixClus[i-1].Clus_x)
                             +(matrixClus[i].Clus_y-matrixClus[i-1].Clus_y)*(matrixClus[i].Clus_y-matrixClus[i-1].Clus_y)
                             +(matrixClus[i].Clus_z-matrixClus[i-1].Clus_z)*(matrixClus[i].Clus_z-matrixClus[i-1].Clus_z));
        //cout << matrixClus[i].Clus_x << "\t" <<matrixClus[i].Clus_y << "\t"<<matrixClus[i].Clus_z <<endl;
        totTrkL += disClus;
    }
    cout << "=========total trk distance: " <<totTrkL <<endl;
    cout<<"=================Amplification end=================="<<endl;
    matrixClus.clear();
   
 
// //==========================统计bkg pad 的hit number=============================
    // for(int i=0;i<padArray;++i)
    // {
    //     for(int j=0;j<padArray;++j)
    //     {
    //         if(HitPadNumAfterAva_1trk[i][j]!=0)
    //         {
    //             if(HitPadNumAfterDrift_1trk[i][j]==0)
    //             {
    //                 hbkgcut->Fill(HitPadNumAfterAva_1trk[i][j]);
    //             }
    //         }
    //     }
    // }
    // double cutThreshold = hbkgcut->GetMean();
    // auto c11 = new TCanvas("c11", "cc", 800, 800);
    // hbkgcut->Draw();
    // cout << "=====================>Cut Threshold:  " << cutThreshold <<endl;

//=============================计算cut后的pad数/径迹长度=====================================

    
    int trkID=0;
    int trk_start=0;
    int trk_end=0;
    vector<int> trackStart;
    trackStart.push_back(0);

    for(int i=0; i<vi_Entry;++i)
    {
        myTreeIn->GetEntry(i);
        if(vi_trkid!=trkID){
            trackStart.push_back(i);
            trkID=vi_trkid;
        }
    }
    cout << "============> track number: " <<trackStart.size()<<endl;

    double xe_AfterDrift =0;
    double ye_AfterDrift =0;

    double xe_AfterAva =0;
    double ye_AfterAva =0;

    for(int id=0;id<trackStart.size();id++)
    {
        
        unordered_map<pair<int,int>,int, PairHash> Map_PadNumAfterDrift;
        unordered_map<pair<int,int>,HitPadNumAndDisL, PairHash> Map_PadNumAfterAva;
        unordered_map<pair<int,int>,HitPadNumAndDisL, PairHash> Map_PadNumAfterAva_Correct;
        vector<InfoXY> Truth;

        trk_start=trackStart[id];
        if(id==trackStart.size()-1){
            trk_end=vi_Entry-1;
        }else{
            trk_end=trackStart[id+1]-1;
        }

        double EleNum = 0;
        int clusNum = 0;
        for(int i=trk_start;i<=trk_end;i++)
        {
            myTreeIn->GetEntry(i);
            //myTreeIn->Show(i);
            //the Eles in one cluster
            bool flag1=false;
            
            for(int ii=0;ii<vi_vecXe->size();++ii)
            {
                auto distance = fabs(double(halflength) - vi_vecZe->at(ii));
                if(sqrt(pow((*vi_vecXe)[ii], 2) + pow((*vi_vecYe)[ii], 2)) < (outerR-0.05)
                && distance > 0)
                {
//====================================Drift=====================================  
                    flag1=true;
                    xe_AfterDrift = gRandom->Gaus((*vi_vecXe)[ii],(x_diffusion*sqrt(distance)));
                    ye_AfterDrift = gRandom->Gaus((*vi_vecYe)[ii],(y_diffusion*sqrt(distance)));
                    GetRowColId(row,col,xe_AfterDrift,ye_AfterDrift,padlength,padwidth,outerR);
                    //HitPadNumAfterDrift[row][col]++;
                    
                    Map_PadNumAfterDrift[{row,col}]++;
                    
                    EleNum++;
                    //int ran = gRandom->Gaus(2000,45);
                    int PoylaRandom = f1->GetRandom();

                    for(int j=0;j<PoylaRandom;++j)
                    {
                        xe_AfterAva = gRandom->Gaus(xe_AfterDrift, 0.003);
                        ye_AfterAva = gRandom->Gaus(ye_AfterDrift, 0.003);
                        GetRowColId(row,col,xe_AfterAva,ye_AfterAva,padlength,padwidth,outerR);
                        
                        if(row>=padRowArray || col >=padColArray)
                        {
                            continue;
                        }
                        //HitPadNumAfterAva[row][col]++;
                        double efficiencyFactor = GetEfficiencyFactor(distance,particle_name);
                        Map_PadNumAfterAva_Correct[{row,col}].HitPadNum+= 1/efficiencyFactor;
                        Map_PadNumAfterAva_Correct[{row,col}].Disl = distance;
                        //cout <<"distance: "<< distance <<" save: " << Map_PadNumAfterAva_Correct[{row,col}].Disl<< " efficiencyFactor: "<<efficiencyFactor<<endl;
                        //cout << "correct: "<<Map_PadNumAfterAva_Correct[{row,col}].HitPadNum <<endl;
                        Map_PadNumAfterAva[{row,col}].HitPadNum++;
                        Map_PadNumAfterAva[{row,col}].Disl = distance;
                        //cout << "Map_PadNumAfterAva[{row,col}].HitPadNum: " <<Map_PadNumAfterAva[{row,col}].HitPadNum << " DisL: " <<Map_PadNumAfterAva[{row,col}].Disl <<endl;
                        //out << "no correct: " << Map_PadNumAfterAva[{row,col}].HitPadNum <<endl;
                    }

                }
                
            }
            if(flag1)
            {
                clusNum++;
            }
        }
        double FindHitNum=0;
        // for(int i=0;i<padArray;i++){
        //     for(int j=0;j<padArray;j++){
        //         // if(HitPadNumAfterAva[i][j]>1000)
        //         // {
        //         //     FindHitNum++;
        //         // }
        //         if(HitPadNumAfterAva[i][j]>0){
        //             Tree_trackID = vi_trkid;
        //             Tree_Row=i;
        //             Tree_Column=j;
        //             Tree_padHitNum_AfterAva=HitPadNumAfterAva[i][j];
        //             Tree_padHitNum_AfterDrift=HitPadNumAfterDrift[i][j];
        //             Tree_TrackLength = trackLenth;
        //             PadInfo->Fill();
        //         }
        //     }
        // }
        for(const auto& Element : Map_PadNumAfterDrift)
        {
            TruthTrackID = vi_trkid;
            Truth_Row = Element.first.first;
            Truth_Col = Element.first.second;
            TruthHitNum_Drift = Element.second;
            Truth_TrackLength = totTrkL;
            TruthPadInfo->Fill();
        }
        for(const auto& Element : Map_PadNumAfterAva)
        {
            AvaTrackID = vi_trkid;
            Ava_Row = Element.first.first;
            Ava_Col = Element.first.second;
            AvaHitNum_Ava = Element.second.HitPadNum;
            Ava_DisL = Element.second.Disl;
            //cout << "No correct: " <<  AvaHitNum_Ava <<endl;
            AvaPadInfo->Fill();
        }
        for(const auto& Element : Map_PadNumAfterAva_Correct)
        {
            AvaTrackID_Correct = vi_trkid;
            Ava_Row_Correct = Element.first.first;
            Ava_Col_Correct = Element.first.second;
            AvaHitNum_Ava_Correct = Element.second.HitPadNum;
            Ava_DisL_Correct = Element.second.Disl;
            //cout << "correct: " <<  AvaHitNum_Ava_Correct <<endl;
            //AvaPadInfo_Correct->Fill();
        }
        //cout<<"==========> "<<FindHitNum<<"===> "<<trackLenth<<"\t"<<FindHitNum/trackLenth<<endl;
        // padHitNum = FindHitNum / trackLenth;
        // PadInfo->Fill();
        //hpadnum->Fill(FindHitNum/trackLenth);
        if((id+1)%progressInterval == 0)
        {
            double progress = static_cast<double>(id+1)/trackStart.size() *100;
            cout << "===============>progress: " << progress << "%" <<endl;
        }
        //cout << "clusnumber: " << clusNum <<endl;

    }

    auto TrkCanvas=tempGeo->ShowTrack(px,pz,py);
    TrkCanvas->Write();
    hShowTrk->SetOption("colz");
    hShowTrk->Write();
    //h4->Write();
    //cout<<"=====================================?"<<hpadnum->GetEntries()<<endl;
    //auto c22 = new TCanvas("c22", "cc", 800, 800);
    //hpadnum->Draw();
    //hpadnum->Write();
    TruthPadInfo->Write();
    AvaPadInfo->Write();
    //AvaPadInfo_Correct->Write();
    myFileOut->Close();
    
    delete tempGeo;


    
}

void GetRowColId(int &row_id,int &col_id,double xxe,double yye,double padlength,double padwidth,double outerR)
{
    int boxSide = outerR*2; //cm
    int nPadRows = boxSide/padwidth;
    int nPadCols = boxSide/padlength;
    row_id = (int)floor(yye/padwidth)+nPadRows/2;
    col_id = (int)floor(xxe/padlength)+nPadCols/2;
    if(row_id == nPadRows)
    {
        row_id = nPadRows-1;
    }
    if(col_id == nPadCols)
    {
        col_id = nPadCols-1;
    }
}

double GetEfficiencyFactor(double x_Distance,string particle_name)
{
    //pion
    double y =0;
    if(particle_name == "kaon")
    {
        y = 0.7216 + 0.006767*x_Distance - 3.001*1.0E-5*x_Distance*x_Distance + 5.51*1.0E-8*x_Distance*x_Distance*x_Distance;
        
    }else
    {
        y = 0.7061 + 0.007086*x_Distance - 3.169*1.0E-5*x_Distance*x_Distance +5.908*1.0E-8*x_Distance*x_Distance*x_Distance;
       
    }
   
    return y;
}

