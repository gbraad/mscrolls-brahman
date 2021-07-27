
#ifdef IFI_BUILD

#include "opt.h"
#include "ificlient.h"
#include "ifiglue.h"

// so that we can get it from glue
IFIClient* ifi;

IFI* IFI::create(IFI::Ctx* p)
{
    ifi = new IFIClient();
    return ifi;
}

int IFIClient::client_main(int argc, char** argv)
{
  // this is a call to ifi_main
    return ::main(argc, argv);
}

#endif
