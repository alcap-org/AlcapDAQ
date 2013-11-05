void dummy()
{
  /*****************************************************************/
  // Prepare the canvas
	/*gStyle->SetOptStat("ne"); */
	/*TCanvas *canvas = (TCanvas *) gROOT->GetListOfCanvases()->At(0);*/
	/*canvas->Divide(4,2);*/
  /*****************************************************************/
	printf("Dummy module\n");
	TSocket *s = (TSocket *) gROOT->GetListOfSockets()->At(0);
	s->Print();

	char request[2048];
	TMessage *tmsg;
	sprintf(request, "GET %s", "hDummy1");

	s->Send(request);
	s->Recv(tmsg);
	TH1 *hist = (TH1*)tmsg->ReadObject(tmsg->GetClass());
	hist->Draw();

	delete tmsg;
}
