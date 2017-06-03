
#include "PluginSDK.h"
#include <algorithm>

PluginSetup("cdk's L++ sample");

std::vector<IUnit*> GetMinionsNearby(bool friendly, bool enemy, bool neutral)
{
	auto minions = GEntityList->GetAllMinions(friendly, enemy, neutral);
	auto local = GEntityList->Player();

	minions.erase(std::remove_if(minions.begin(), minions.end(), [&](IUnit* m) { return !m || m->IsDead() || !local->IsValidTarget(m, local->AttackRange()); }));

	return minions;
}

PLUGIN_EVENT(void) BeforeAttack()
{
	auto local = GEntityList->Player();
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
	GEventManager->AddEventHandler(kEventOrbwalkBeforeAttack, BeforeAttack);

}

PLUGIN_API void OnUnload()
{
	GEventManager->RemoveEventHandler(kEventOrbwalkBeforeAttack, BeforeAttack);
}