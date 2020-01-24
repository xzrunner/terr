#include "terr/Terr.h"

namespace terr
{

CU_SINGLETON_DEFINITION(Terr);

extern void regist_rttr();

Terr::Terr()
{
	regist_rttr();
}

}