//////////////////////////////////////////////////////////////////////////
// zoom_real.C —— 读lanlanhe输出，绘制带局部放大的电荷事例图
//
// 用法（root -l 内，Lansxlogon.h 和本宏同目录启动）：
//   .L zoom_real.C
//   zoom_real("build/RESULTS/1.00GeVkaon_0.10padlength_0.10padwidth_0.50costheta_30DiFu.root")
//   // 指定径迹/放大区/几何：
//   zoom_real("build/RESULTS/....root", 0,  0., 20., 4.,   0., 38., 0.6,  47.5, 0.10, 2000)
//                          文件名                      trackID cx  cy half  zx  zy  zhalf R  pad zmax
//////////////////////////////////////////////////////////////////////////

#include "Lansxlogon.h"     // Lansx风格（记得给它加 #ifndef guard）
#include "TFile.h"
#include "TTree.h"
#include "TH2D.h"
#include "TCanvas.h"
#include "TPad.h"
#include "TBox.h"
#include "TLine.h"
#include "TLatex.h"
#include "TStyle.h"

void zoom_real(const char* fname,
               int    trackID = -1,     // -1=全部径迹叠加；0=只画第0条
               double cx = 0.,          // 主图区域中心 x (cm)
               double cy = 20.,         // 主图区域中心 y (cm)
               double half = 4.,        // 主图区域半宽 (cm)
               double zx = -999,        // 放大框中心（默认=主图中心）
               double zy = -999,
               double zhalf = -1,       // 放大框半宽（默认=half/6）
               double outerR = 47.5,    // 必须和数据一致！逆映射用
               double pad = 0.10,       // 必须和数据一致！
               double zmax = -1)        // 色标上限（-1=自动取数据最大值）
{
    myrootlogon();                // Lansx风格
    gStyle->SetOptStat(0);        // 关掉myStyle打开的统计框

    // ---- 粉→紫渐变色板（必须在myrootlogon之后） ----
    const int nS=5, nC=99;
    double st[nS]={0.00,0.30,0.60,0.85,1.00};
    double r [nS]={1.00,0.98,0.86,0.52,0.25};
    double g [nS]={0.96,0.75,0.31,0.08,0.05};
    double b [nS]={0.96,0.80,0.55,0.45,0.45};
    TColor::CreateGradientColorTable(nS,st,r,g,b,nC);
    gStyle->SetNumberContours(nC);

    // ---- ① 读ROOT文件 ----
    TFile *f = TFile::Open(fname);
    if(!f || f->IsZombie()){ printf("打不开 %s\n",fname); return; }
    TTree *t = (TTree*)f->Get("AvaPadInfo");
    if(!t){ printf("没有AvaPadInfo树\n"); return; }
    int trkid=0,row=0,col=0; double hitnum=0.;
    t->SetBranchAddress("AvaTrackID",   &trkid);
    t->SetBranchAddress("Ava_Row",      &row);
    t->SetBranchAddress("Ava_Col",      &col);
    t->SetBranchAddress("AvaHitNum_Ava",&hitnum);

    // ---- ② 建放大区域的直方图（固定240x240细格→参考图的"颗粒点"效果） ----
    int nGrid = int(2*outerR/pad);
    double x0=cx-half, x1=cx+half, y0=cy-half, y1=cy+half;
    if(zx<-999) zx=cx;               // 放大框没指定就取区域中心
    if(zy<-999) zy=cy;
    if(zhalf<0) zhalf=half/6.;
    double bx0=zx-zhalf, bx1=zx+zhalf, by0=zy-zhalf, by1=zy+zhalf;

    auto h = new TH2D("h",";x (cm);y (cm)",240,x0,x1,240,y0,y1);
    long kept=0; double hm=0;
    for(long i=0;i<t->GetEntries();++i){
        t->GetEntry(i);
        if(trackID>=0 && trkid!=trackID) continue;
        double x = (col - nGrid/2. + 0.5)*pad;     // 行列号→物理坐标（逆映射）
        double y = (row - nGrid/2. + 0.5)*pad;
        if(x<x0||x>x1||y<y0||y>y1) continue;       // 区域筛选
        h->Fill(x,y,hitnum); ++kept;
        if(hitnum>hm) hm=hitnum;
    }
    if(kept==0){ printf("区域内没有命中：调大half或换cx,cy\n"); return; }
    h->SetMinimum(0);
    h->SetMaximum(zmax>0 ? zmax : h->GetMaximum());

    // ---- ③ 主pad ----
    auto c = new TCanvas("c","",980,820);
    c->cd();
    gPad->SetLeftMargin(0.10);  gPad->SetRightMargin(0.20);
    gPad->SetBottomMargin(0.12); gPad->SetTopMargin(0.05);
    h->Draw("colz");
    h->GetZaxis()->SetTitle("Charge");
    h->GetZaxis()->SetTitleOffset(1.4);
    h->GetXaxis()->SetTitle("x (cm)");
    h->GetYaxis()->SetTitle("y (cm)");

    // ---- ④ 放大框 ----
    auto box = new TBox(bx0,by0,bx1,by1);
    box->SetFillStyle(0); box->SetLineColor(kGray+2); box->Draw();

    // ---- ⑤ inset子pad ----
    auto pin = new TPad("pin","",0.50,0.58,0.78,0.90);
    pin->SetFillColor(10); pin->SetBorderSize(1);
    pin->Draw(); pin->cd();
    gPad->SetLeftMargin(0.16); gPad->SetBottomMargin(0.20);
    gPad->SetTopMargin(0.02);  gPad->SetRightMargin(0.02);
    auto hz = (TH2D*)h->Clone("hz");
    hz->GetXaxis()->SetRangeUser(bx0,bx1);
    hz->GetYaxis()->SetRangeUser(by0,by1);
    hz->GetXaxis()->SetLabelSize(0);       // 只留刻度线不要数字
    hz->GetYaxis()->SetLabelSize(0);
    hz->Draw("col");

    // ---- ⑥ 连接线：框两角→inset下两角 ----
    c->cd();
    double lm=0.10, rm=0.20, bm=0.12, tm=0.05;
    auto u2n = [&](double x,double y,double &nx,double &ny){
        nx = lm + (x-x0)/(x1-x0)*(1-lm-rm);
        ny = bm + (y-y0)/(y1-y0)*(1-bm-tm);
    };
    double a1,b1,a2,b2;
    u2n(bx0,by1,a1,b1);  u2n(bx1,by1,a2,b2);
    auto l1 = new TLine(a1,b1, 0.50,0.58); l1->SetLineColor(kGray+2); l1->Draw();
    auto l2 = new TLine(a2,b2, 0.78,0.58); l2->SetLineColor(kGray+2); l2->Draw();

    // ---- ⑦ 刷新 + 存图 ----
    c->Update();
    c->SaveAs("zoom_real3.png");
    printf("区域内命中pad数 %ld | 最大单pad电荷 %.0f | 色标上限 %.1f\n",
           kept, hm, h->GetMaximum());
}