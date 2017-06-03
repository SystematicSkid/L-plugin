
#include "PluginSDK.h"
#include <algorithm>

PluginSetup("cdk's L++ sample");

std::vector<IUnit*> GetMinionsNearby(bool friendly, bool enemy, bool neutral)
{
	auto minions = GEntityList->GetAllMinions(friendly, enemy, neutral);

	auto local = GEntityList->Player();
	auto local_pos = local->GetPosition();
	auto local_range = local->AttackRange();

	minions.erase(std::remove_if(minions.begin(), minions.end(), [&](IUnit* m) { return !m || m->IsDead() || (m->GetPosition() - local_pos).Length() < local_range; }));

	return minions;
}

PLUGIN_EVENT(void) OnGameUpdate()
{
	auto local = GEntityList->Player();

	if (!local || local->IsDead() || !GOrbwalking->CanAttack())
		return;

	auto minions = GetMinionsNearby(0, 1, 1);

	for (auto &m : minions)
	{
		if (m->GetHealth() <= local->TotalPhysicalDamage())
		{
			GOrbwalking->SetOverrideTarget(m);
			break;
		}
	}
}

PLUGIN_API void OnLoad(IPluginSDK* PluginSDK)
{
	PluginSDKSetup(PluginSDK);
	GEventManager->AddEventHandler(kEventOnGameUpdate, OnGameUpdate);

}

PLUGIN_API void OnUnload()
{
	GEventManager->RemoveEventHandler(kEventOnGameUpdate, OnGameUpdate);
}