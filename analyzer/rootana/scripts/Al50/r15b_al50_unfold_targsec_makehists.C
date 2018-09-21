// Make the histograms for looking at TMs as a function of proton
// starting position in target. These histograms can take a while to make.

{
  TFile* f[2] = {
    new TFile("data/pselal50.root"),
    new TFile("data/tmal50_protuniformintarg.root")
  };
  TTree* tr[2][2] = { // [RL,MC][L,R]
    {
      (TTree*)f[0]->Get("PID_LP"),
      (TTree*)f[0]->Get("PID_RP")
    }, {
      (TTree*)f[1]->Get("SiL_TransTree"),
      (TTree*)f[1]->Get("SiR_TransTree")
    }
  };
  TFile* ofile = new TFile("data/unfold_targsec.root", "RECREATE");
  TCut latposcuts[5] = {
    "                x0 < -2.61",   // Left side of target
    "-2.61   < x0 && x0 < -0.87",
    "-0.87   < x0 && x0 < 0.87",
    "0.87    < x0 && x0 < 2.61",
    "2.61    < x0",                 // Right side of target
  };
  TCut depposcuts[5] = {
    "                z0 < -0.0015", // Front of target
    "-0.0015 < z0 && z0 < -0.0005",
    "-0.0005 < z0 && z0 < 0.0005",
    "0.0005  < z0 && z0 < 0.0015",
    "0.0015  < z0"                  // Back of target
  };

  tr[0][0]->Draw("e>>h1ldata(30,0,15e3)", "t > 400 && abs(dt) < 200 && e3 == 0.");
  tr[0][1]->Draw("e>>h1rdata(30,0,15e3)", "t > 400 && abs(dt) < 200 && e3 == 0.");
  tr[1][0]->Draw("e0:emeas>>h2llatpos_1(30,0,15e3,30,0,15e3)", latposcuts[0] && "emeas>0");
  tr[1][0]->Draw("e0:emeas>>h2llatpos_2(30,0,15e3,30,0,15e3)", latposcuts[1] && "emeas>0");
  tr[1][0]->Draw("e0:emeas>>h2llatpos_3(30,0,15e3,30,0,15e3)", latposcuts[2] && "emeas>0");
  tr[1][0]->Draw("e0:emeas>>h2llatpos_4(30,0,15e3,30,0,15e3)", latposcuts[3] && "emeas>0");
  tr[1][0]->Draw("e0:emeas>>h2llatpos_5(30,0,15e3,30,0,15e3)", latposcuts[4] && "emeas>0");
  tr[1][1]->Draw("e0:emeas>>h2rlatpos_1(30,0,15e3,30,0,15e3)", latposcuts[0] && "emeas>0");
  tr[1][1]->Draw("e0:emeas>>h2rlatpos_2(30,0,15e3,30,0,15e3)", latposcuts[1] && "emeas>0");
  tr[1][1]->Draw("e0:emeas>>h2rlatpos_3(30,0,15e3,30,0,15e3)", latposcuts[2] && "emeas>0");
  tr[1][1]->Draw("e0:emeas>>h2rlatpos_4(30,0,15e3,30,0,15e3)", latposcuts[3] && "emeas>0");
  tr[1][1]->Draw("e0:emeas>>h2rlatpos_5(30,0,15e3,30,0,15e3)", latposcuts[4] && "emeas>0");
  tr[1][0]->Draw("e0:emeas>>h2ldeppos_1(30,0,15e3,30,0,15e3)", depposcuts[0] && "emeas>0");
  tr[1][0]->Draw("e0:emeas>>h2ldeppos_2(30,0,15e3,30,0,15e3)", depposcuts[1] && "emeas>0");
  tr[1][0]->Draw("e0:emeas>>h2ldeppos_3(30,0,15e3,30,0,15e3)", depposcuts[2] && "emeas>0");
  tr[1][0]->Draw("e0:emeas>>h2ldeppos_4(30,0,15e3,30,0,15e3)", depposcuts[3] && "emeas>0");
  tr[1][0]->Draw("e0:emeas>>h2ldeppos_5(30,0,15e3,30,0,15e3)", depposcuts[4] && "emeas>0");
  tr[1][1]->Draw("e0:emeas>>h2rdeppos_1(30,0,15e3,30,0,15e3)", depposcuts[0] && "emeas>0");
  tr[1][1]->Draw("e0:emeas>>h2rdeppos_2(30,0,15e3,30,0,15e3)", depposcuts[1] && "emeas>0");
  tr[1][1]->Draw("e0:emeas>>h2rdeppos_3(30,0,15e3,30,0,15e3)", depposcuts[2] && "emeas>0");
  tr[1][1]->Draw("e0:emeas>>h2rdeppos_4(30,0,15e3,30,0,15e3)", depposcuts[3] && "emeas>0");
  tr[1][1]->Draw("e0:emeas>>h2rdeppos_5(30,0,15e3,30,0,15e3)", depposcuts[4] && "emeas>0");
  tr[1][0]->Draw("e0>>h1llatpos_1(30,0,15e3)",                latposcuts[0]);
  tr[1][0]->Draw("e0>>h1llatpos_2(30,0,15e3)",                latposcuts[1]);
  tr[1][0]->Draw("e0>>h1llatpos_3(30,0,15e3)",                latposcuts[2]);
  tr[1][0]->Draw("e0>>h1llatpos_4(30,0,15e3)",                latposcuts[3]);
  tr[1][0]->Draw("e0>>h1llatpos_5(30,0,15e3)",                latposcuts[4]);
  tr[1][1]->Draw("e0>>h1rlatpos_1(30,0,15e3)",                latposcuts[0]);
  tr[1][1]->Draw("e0>>h1rlatpos_2(30,0,15e3)",                latposcuts[1]);
  tr[1][1]->Draw("e0>>h1rlatpos_3(30,0,15e3)",                latposcuts[2]);
  tr[1][1]->Draw("e0>>h1rlatpos_4(30,0,15e3)",                latposcuts[3]);
  tr[1][1]->Draw("e0>>h1rlatpos_5(30,0,15e3)",                latposcuts[4]);
  tr[1][0]->Draw("e0>>h1ldeppos_1(30,0,15e3)",                depposcuts[0]);
  tr[1][0]->Draw("e0>>h1ldeppos_2(30,0,15e3)",                depposcuts[1]);
  tr[1][0]->Draw("e0>>h1ldeppos_3(30,0,15e3)",                depposcuts[2]);
  tr[1][0]->Draw("e0>>h1ldeppos_4(30,0,15e3)",                depposcuts[3]);
  tr[1][0]->Draw("e0>>h1ldeppos_5(30,0,15e3)",                depposcuts[4]);
  tr[1][1]->Draw("e0>>h1rdeppos_1(30,0,15e3)",                depposcuts[0]);
  tr[1][1]->Draw("e0>>h1rdeppos_2(30,0,15e3)",                depposcuts[1]);
  tr[1][1]->Draw("e0>>h1rdeppos_3(30,0,15e3)",                depposcuts[2]);
  tr[1][1]->Draw("e0>>h1rdeppos_4(30,0,15e3)",                depposcuts[3]);
  tr[1][1]->Draw("e0>>h1rdeppos_5(30,0,15e3)",                depposcuts[4]);

  ofile->Write();
  ofile->Close();
}
