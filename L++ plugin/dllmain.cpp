
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

IUnit* BestTarget()
{
	auto heroes = GEntityList->GetAllHeros(0, 1);
	auto local = GEntityList->Player();
	
	float max_score = 0;
	IUnit* best = NULL;

	for (auto &h : heroes)
	{
		if (!h || h->IsDead() || local->IsValidTarget(h, local->AttackRange()))
			continue;

		float score = static_cast<float>(h->GetDeaths() - h->GetKills()) + (h->GetMaxHealth() - h->GetHealth()) - (h->TotalMagicDamage() + h->TotalPhysicalDamage());

		int mana = h->GetMana();
		for (int i = 0; i < 6; i++)
			if (h->GetSpellState(static_cast<eSpellSlot>(i)) == NoMana)
				score += static_cast<float>(mana * (i + 1)); // did someone ran out of mana? hehehe

		if (score > max_score)
		{
			max_score = score;
			best = h;
		}
	}
	
	return best;
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