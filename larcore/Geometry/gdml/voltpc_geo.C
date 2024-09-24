typedef struct _drawopt {
  char const* volume;
  int color;
} drawopt;

voltpc_geo(TString volName = "")
{
  gSystem->Load("libGeom");
  gSystem->Load("libGdml");

  TGeoManager::Import("voltpc.gdml");

  drawopt optuboone[] = {{"volWorld", 0},
                         {"volDetEnclosure", kWhite},
                         {"volCryostat", kOrange},
                         {"volTPC", kOrange - 5},
                         {"volTPCBackWall", kRed},
                         {"volTPCVertWall", kCyan - 5},
                         {"volTPCHorizWall", kOrange},
                         {0, 0}};

  // for (int i=0;; ++i) {
  //   if (optuboone[i].volume==0) break;
  //     gGeoManager->FindVolumeFast(optuboone[i].volume)->SetLineColor(optuboone[i].color);
  // }
  TList* mat = gGeoManager->GetListOfMaterials();
  TIter next(mat);
  TObject* obj;
  while (obj = next()) {
    obj->Print();
  }

  gGeoManager->GetTopNode();
  gGeoManager->CheckOverlaps(1e-16);
  gGeoManager->PrintOverlaps();
  gGeoManager->SetMaxVisNodes(70000);

  TGeoVolume* TPC = gGeoManager->FindVolumeFast("volTPC");
  float m_tpc = TPC->Weight();

  gGeoManager->FindVolumeFast("volWorld")->Draw("ogl");

  //gGeoManager->GetTopVolume()->Draw();
  if (!volName.IsNull()) gGeoManager->FindVolumeFast(volName)->Draw("ogl");

  TFile* tf = new TFile("voltpc.root", "RECREATE");

  gGeoManager->Write();

  tf->Close();
}
