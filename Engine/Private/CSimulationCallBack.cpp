#include "CSimulationCallBack.h"

void CSimulationCallBack::onContact(const PxContactPairHeader& pairHeader, const PxContactPair* pairs, PxU32 nbPairs)
{
	for(PxU32 i = 0; i < nbPairs; i++)
	{
		const PxContactPair& cp = pairs[i];

		if ((pairHeader.actors[0]->getName() == "Player" && pairHeader.actors[1]->getName() == "Weapon") ||
			(pairHeader.actors[0]->getName() == "Weapon" && pairHeader.actors[1]->getName() == "Player"))
		{
			
			int tmep = 0;
			// 이 충돌을 무시
			// 
			// 
			//cp.flags |= PxContactPairFlag::
		}


		if(cp.events & PxPairFlag::eNOTIFY_TOUCH_FOUND)
		{
			//충돌 시작
			OnCollisionEnter(pairHeader, cp);
		}
		if(cp.events & PxPairFlag::eNOTIFY_TOUCH_PERSISTS)
		{
			//충돌 유지
			OnCollisionStay(pairHeader, cp);
		}

		if(cp.events & PxPairFlag::eNOTIFY_TOUCH_LOST)
		{
			//충돌 끝
			OnCollisionExit(pairHeader, cp);
		}

	}





}

void CSimulationCallBack::OnCollisionEnter(const PxContactPairHeader& pairHeader, const PxContactPair& cp)
{
const char* Test = 	pairHeader.actors[0]->getName();

int test = 0;
}

void CSimulationCallBack::OnCollisionStay(const PxContactPairHeader& pairHeader, const PxContactPair& cp)
{
}

void CSimulationCallBack::OnCollisionExit(const PxContactPairHeader& pairHeader, const PxContactPair& cp)
{
}
