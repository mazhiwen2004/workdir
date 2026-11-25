//////////////////////////////////////////////////////////////////////////
// event_display.C —— NvDEx风格事例显示图
//   模式A（真实数据）: root -l 'event_display.C' 然后
//       draw_event("RESULTS/xxx_30DiFu.root")
//   模式B（合成0νββ事例示意）:
//       demo_0nbb()
//////////////////////////////////////////////////////////////////////////
#include "TFile.h"
#include "TTree.h"
#include "TH2D.h"
#include "TCanvas.h"
#include "TEllipse.h"
#include "TMarker.h"
#include "TLine.h"
#include "Lansxlogon.h"
#include "TLatex.h"
#include "TStyle.h"
#include "TRandom.h"
#include <vector>

void set_evt_style()
{
    gStyle->SetOptStat(0);
    // ROOT>=6.26 可用 kViridis；旧版本用 kInvertedDarkBodyRadiator(inferno风格)
    gStyle->SetPalette(kInvertedDarkBodyRadiator);
}

//-------- 通用收尾：圆形边界+保存 --------
void finish(TCanvas* c, TH2D* h, double outerR, const char* png)
{
    h->GetZaxis()->SetTitle("charge [e^{-}]");
    auto circ = new TEllipse(0,0,outerR,outerR);
    circ->SetFillStyle(0); circ->SetLineColor(kBlack); circ->SetLineWidth(2);
    circ->Draw("same");
    c->SaveAs(png);
}

//////////////////////////////////////////////////////////////////////////
// center_titles —— 三个标题全部居中的通用函数
// 用法：h->Draw("colz") 之后调用 center_titles(h);
// 边距参数必须和画图段一致（当前：0.12 / 0.20 / 0.06 / 0.105）
//////////////////////////////////////////////////////////////////////////
void center_titles(TH2D* h,
                   double xl=0.12, double xr=0.20,
                   double yt=0.06, double yb=0.105)
{
    // ① 关掉所有内置标题，防止重影
    gStyle->SetOptTitle(0);
    h->GetXaxis()->SetTitle("");
    h->GetYaxis()->SetTitle("");
    h->GetZaxis()->SetTitle("");

    // ② 两个锚点：绘图区的水平中心和垂直中心
    double xc = (xl + 1 - xr)/2;        // = 0.46，横向正中
    double yc = (yb + 1 - yt)/2;        // ≈ 0.5175，纵向正中

    auto tx = new TLatex();
    tx->SetNDC();                       // 全部用画布比例坐标(0~1)
    tx->SetTextSize(0.04);
    tx->SetTextAlign(22);               // 锚点=文字几何中心，居中的前提

    // ③ 主标题：绘图区上方，水平居中（放在上边距条带的正中）
    tx->DrawLatex(xc, 1 - yt/2, "event display");

    // ④ X轴标题：绘图区正下方，水平居中
    tx->DrawLatex(xc, yb/2, "x [cm]");

    // ⑤ Y轴标题：左侧竖排，垂直居中
    tx->SetTextAngle(90);
    tx->DrawLatex(xl/2, yc, "y [cm]");

    // ⑥ charge标题：色条条带正中，竖排
    tx->DrawLatex(1 - xr/4, yc, "charge [e^{-}]");
    tx->SetTextAngle(0);
}
//================ 模式A：真实数据 =================
void draw_event(const char* fname, int trackID = 0,
                double outerR = 47.5, double pad = 0.05, double mapbin = 0.1)
{
    set_evt_style();                      // 或 myrootlogon()，按你现状保留
    TFile *f = TFile::Open(fname);
    if(!f || f->IsZombie()){ printf("打不开 %s\n", fname); return; }
    TTree *t = (TTree*)f->Get("AvaPadInfo");
    if(!t){ printf("没有AvaPadInfo树\n"); return; }

    int nGrid = int(2*outerR/pad);
    int trkid=0,row=0,col=0; double hitnum=0.;
    t->SetBranchAddress("AvaTrackID",&trkid);
    t->SetBranchAddress("Ava_Row",&row);
    t->SetBranchAddress("Ava_Col",&col);
    t->SetBranchAddress("AvaHitNum_Ava",&hitnum);

    auto h = new TH2D("evt","event display;x [cm]; y [cm]",
                      nGrid,-outerR,outerR, nGrid,-outerR,outerR);
    for(long i=0;i<t->GetEntries();++i){
        t->GetEntry(i);
        if(trackID>=0 && trkid!=trackID) continue;
        double x = (col - nGrid/2. + 0.5)*pad;
        double y = (row - nGrid/2. + 0.5)*pad;
        h->Fill(x,y,hitnum);
    }

    // ---- 画图段：顺序不能乱 ----
    auto c = new TCanvas("cA","event",1080,880);   // 加宽画布，圆保持正圆
    c->cd();
    gPad->SetLeftMargin(0.12);
    gPad->SetRightMargin(0.20);                    // 色条空间
    gPad->SetTopMargin(0.06);
    gPad->SetBottomMargin(0.105);

    h->Draw("colz");                               // ① 直方图（会清pad）
    h->GetZaxis()->SetTitle("charge [e^{-}]");
    h->GetZaxis()->SetTitleOffset(1.8);

    auto circ = new TEllipse(0,0,outerR,outerR);   // ② 圆必须在直方图之后画
    circ->SetFillStyle(0); circ->SetLineColor(kBlack); circ->SetLineWidth(2);
    circ->Draw("same");                            //    ("same"=画在已有内容之上)
    center_titles(h);   
    c->Update();                                   // ③ 刷新
    c->SaveAs("event_data.png");                   // ④ 存图最后
}
//================ 模式B：合成0νββ两电子事例 =================
void demo_0nbb(double outerR = 47.5, double padsz = 0.84)
{
    set_evt_style();
    int nGrid = int(2*outerR/padsz);
    auto h = new TH2D("nbb","0#nu#beta#beta event (simulated);x [cm]; y [cm]",
                      nGrid,-outerR,outerR, nGrid,-outerR,outerR);

    // ---- 事例真值：顶点、两条电子的能量和方向 ----
    double vx = 0.25*outerR, vy = -0.35*outerR;         // 顶点
    double E1 = 1.80, E2 = 1.196;                        // MeV, 和=2.996(Q值)
    double phi1 = 40.*TMath::Pi()/180.;                  // 电子1方向
    double phi2 = 200.*TMath::Pi()/180. + 0.35;          // 电子2:大致背靠背带开角
    double len1 = 3.5*E1, len2 = 3.5*E2;                 // 简化: ~3.5 cm/MeV
    double sig  = 0.35;                                  // 扩散模糊(示意值)

    // ---- 沿径迹撒电离团簇, dE/dx朝末端增强(末端blob) ----
    double endx[2], endy[2];
    for(int k=0;k<2;++k){
        double phi = k==0?phi1:phi2, L = k==0?len1:len2;
        endx[k] = vx + L*cos(phi);
        endy[k] = vy + L*sin(phi);
        int nClus = int(L*40);                           // 40团簇/cm
        for(int i=0;i<nClus;++i){
            double t = double(i)/nClus;
            double w = 1. + 9.*pow(t,3);                 // 末端沉积↑(类Bragg)
            double cx = vx + L*t*cos(phi);
            double cy = vy + L*t*sin(phi);
            for(int e=0;e<int(3*w);++e){                 // 每团簇电子数∝沉积
                double ex = cx + gRandom->Gaus(0,sig);
                double ey = cy + gRandom->Gaus(0,sig);
                if(ex*ex+ey*ey < (outerR-1)*(outerR-1)) h->Fill(ex,ey);
            }
        }
    }

    auto c = new TCanvas("cB","0nbb demo",900,880);
    h->Draw("colz");

    // 真值径迹线(白虚线)+顶点星标
    for(int k=0;k<2;++k){
        auto ln = new TLine(vx,vy,endx[k],endy[k]);
        ln->SetLineColor(kWhite); ln->SetLineStyle(kDashed); ln->SetLineWidth(1);
        ln->Draw("same");
    }
    auto mv = new TMarker(vx,vy,29);                     // 29=五角星
    mv->SetMarkerColor(kWhite); mv->SetMarkerSize(1.8); mv->Draw();

    // 能量标注
    auto tx = new TLatex();
    tx->SetNDC(); tx->SetTextSize(0.035); tx->SetTextFont(42);
    tx->DrawLatex(0.15,0.92,Form("E_{1}=%.2f MeV   E_{2}=%.3f MeV",E1,E2));
    tx->DrawLatex(0.15,0.88,"E_{tot} = 2.996 MeV  (^{82}Se 0#nu#beta#beta)");

    finish(c,h,outerR,"event_0nbb.png");
}
