{
	char host_str[100] = "localhost";
	int port = 9090;
	TSocket *s = new TSocket(host_str,port);
	if (!s->IsValid())
	{
		printf("Unable to establish connection to %s at port %d\n", host_str, port);
		exit(1);
	}

	char ver[100];
	s->Recv(ver,100);
	printf("Online analyzer version: %s\n", ver); 

	char req[256];
	TMessage *msg;

	printf("Object listing ...\n");
	sprintf(req,"LIST");
	s->Send(req); // send request
	s->Recv(msg); // receive a TMessage from the server
	TObjArray *names = (TObjArray*)msg->ReadObject(msg->GetClass());
	for (int i = 0; i < names.GetEntries(); ++i)
	{
		TObjString *histname = names.At(i);
		//printf("object %d: %s\n", i, histname->GetString().Data());
		// Get the histogram
		sprintf(req,"GET %s", histname->GetString().Data());
		s->Send(req);
		s->Recv(msg);
		TH1 *hist = (TH1*) msg->ReadObject(msg->GetClass());
		hist->Print();
		hist->Draw();
	}
}
