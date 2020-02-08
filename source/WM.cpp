#include "wm/WM.h"

namespace wm
{

CU_SINGLETON_DEFINITION(WM);

extern void regist_rttr();

WM::WM()
{
	regist_rttr();
}

}