/*
 * AutoDeadringer.cpp
 *
 *  Created on: Apr 12, 2018
 *      Author: bencat07
 */
#include "common.hpp"
namespace hacks::shared::deadringer
{
static CatVar
    enabled(CV_SWITCH, "deadringer_auto", "0", "Auto deadringer",
            "automatically pull out DR on low health or projectile nearby");
static CatVar trigger_health(CV_SWITCH, "deadringer_health", "30", "Health",
                             "Trigger on this much health or less");

bool IsProjectile(CachedEntity *ent)
{
    return (ent->m_iClassID() == CL_CLASS(CTFProjectile_Rocket) ||
            ent->m_iClassID() == CL_CLASS(CTFProjectile_Flare) ||
            ent->m_iClassID() == CL_CLASS(CTFProjectile_EnergyBall) ||
            ent->m_iClassID() == CL_CLASS(CTFProjectile_HealingBolt) ||
            ent->m_iClassID() == CL_CLASS(CTFProjectile_Arrow) ||
            ent->m_iClassID() == CL_CLASS(CTFProjectile_SentryRocket) ||
            ent->m_iClassID() == CL_CLASS(CTFProjectile_Cleaver) ||
            ent->m_iClassID() == CL_CLASS(CTFGrenadePipebombProjectile) ||
            ent->m_iClassID() == CL_CLASS(CTFProjectile_EnergyRing));
}
int NearbyEntities()
{
    int ret = 0;
    if (CE_BAD(LOCAL_E))
        return ret;
    for (int i = 0; i < HIGHEST_ENTITY; i++)
    {
        CachedEntity *ent = ENTITY(i);
        if (CE_BAD(ent))
            continue;
        if (ent == LOCAL_E)
            continue;
        if (!ent->m_bAlivePlayer())
            continue;
        if (ent->m_flDistance() <= 300.0f)
            ret++;
    }
    return ret;
}
void CreateMove()
{
    if (!enabled)
        return;
    if (CE_BAD(LOCAL_E))
        return;
    if (CE_BYTE(LOCAL_E, netvar.m_bFeignDeathReady))
        return;
    if (HasCondition<TFCond_Cloaked>(LOCAL_E) || HasCondition<TFCond_CloakFlicker>(LOCAL_E))
    	return;
    if (CE_INT(LOCAL_E, netvar.iHealth) < (int) trigger_health &&
        NearbyEntities() > 1)
        g_pUserCmd->buttons |= IN_ATTACK2;
    for (int i = 0; i < HIGHEST_ENTITY; i++)
    {
        CachedEntity *ent = ENTITY(i);
        if (CE_BAD(ent))
            continue;
        if (!IsProjectile(ent) && !ent->m_bGrenadeProjectile())
            continue;
        if (ent->m_bCritProjectile() && ent->m_flDistance() <= 1000.0f)
            g_pUserCmd->buttons |= IN_ATTACK2;
        if (ent->m_flDistance() < 300.0f)
            g_pUserCmd->buttons |= IN_ATTACK2;
    }
}
}
