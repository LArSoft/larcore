typedef struct _drawopt {
  char const* volume;
  int color;
} drawopt;

bo_geo(TString volName = "volWorld")
{

  gSystem->Load("libGeom");
  gSystem->Load("libGdml");

  TGeoManager::Import("longbo.gdml");

  drawopt optbo[] = {{"volWorld", 0},
                     {"volCryostat", kRed},
                     {"volTPCPlane", kCyan},
                     {"volDetEnclosure", kWhite},
                     {0, 0}};

  for (int i = 0;; ++i) {
    if (optbo[i].volume == 0) break;
    gGeoManager->FindVolumeFast(optbo[i].volume)->SetFillColor(optbo[i].color);
  }

  gGeoManager->CheckOverlaps(0.01);
  gGeoManager->PrintOverlaps();
  gGeoManager->SetMaxVisNodes(70000);

  TList* mat = gGeoManager->GetListOfMaterials();
  TIter next(mat);
  TObject* obj;
  while (obj = next()) {
    obj->Print();
  }

  //gGeoManager->GetTopVolume()->Draw();
  gGeoManager->FindVolumeFast(volName)->Draw();

  //   TFile *tf = new TFile("bo.root", "RECREATE");
  //   gGeoManager->Write();
  //   tf->Close();
}
