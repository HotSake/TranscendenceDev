//	CApproachOrder.cpp
//
//	CApproachOrder class implementation
//	Copyright (c) 2014 by Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"

const Metric APPROACH_THRESHOLD =		(50.0 * LIGHT_SECOND);
const Metric APPROACH_THRESHOLD2 =		(APPROACH_THRESHOLD * APPROACH_THRESHOLD);

const Metric PATROL_SENSOR_RANGE =		(30.0 * LIGHT_SECOND);
const Metric NAV_PATH_THRESHOLD =		(3.0 * PATROL_SENSOR_RANGE);
const Metric NAV_PATH_THRESHOLD2 =		(NAV_PATH_THRESHOLD * NAV_PATH_THRESHOLD);

const Metric NAV_PATH_RECALC_THRESHOLD = (50.0 * LIGHT_SECOND);
const Metric NAV_PATH_RECALC_THRESHOLD2 = (NAV_PATH_RECALC_THRESHOLD * NAV_PATH_RECALC_THRESHOLD);

void CApproachOrder::OnBehavior (CShip *pShip, CAIBehaviorCtx &Ctx)

//	OnBehavior
//
//	Behavior

	{
	DEBUG_TRY

	switch (m_iState)
		{
		case EState::OnCourseViaNavPath:
			{
			Metric rDest2;

			bool bAtDest;
			Ctx.ImplementFollowNavPath(pShip, &bAtDest);
			if (bAtDest)
				{
				Ctx.ClearNavPath();
				m_iState = EState::Approaching;
				}
			else if ((rDest2 = (m_Objs[objDest]->GetPos() - pShip->GetPos()).Length2()) < m_rMinDist2)
				{
				Ctx.ClearNavPath();
				pShip->CancelCurrentOrder();
				}

			//	If our destination can move and it has moved away from the nav path
			//	destination, then we need to recalc.

			else if (m_Objs[objDest]->CanThrust() 
					&& (m_Objs[objDest]->GetPos() - Ctx.GetNavPath()->GetPathEnd()).Length2() > NAV_PATH_RECALC_THRESHOLD2)
				{
				//	If we're still far enough away that we need a new nav path, then
				//	recalc.

				if (rDest2 > NAV_PATH_THRESHOLD2
						&& Ctx.CalcNavPath(pShip, m_Objs[objDest]))
					;

				//	Otherwise, done with nav path.

				else
					{
					Ctx.ClearNavPath();
					m_iState = EState::Approaching;
					}
				}

			break;
			}

		case EState::Approaching:
			{
			//	Maneuver

			CVector vTarget = m_Objs[objDest]->GetPos() - pShip->GetPos();
			Metric rTargetDist2 = vTarget.Dot(vTarget);

			//	If we're close enough, we're done

			if (rTargetDist2 < m_rMinDist2)
				pShip->CancelCurrentOrder();

			//	Otherwise, if we're close, we use formation flying

			else if (rTargetDist2 < APPROACH_THRESHOLD2)
				Ctx.ImplementFormationManeuver(pShip, m_Objs[objDest]->GetPos(), m_Objs[objDest]->GetVel(), pShip->GetRotation());

			//	Otherwise, just close on target

			else
				Ctx.ImplementCloseOnTarget(pShip, m_Objs[objDest], vTarget, rTargetDist2);

			break;
			}

		default:
			throw CException(ERR_FAIL);
		}

	DEBUG_CATCH
	}

void CApproachOrder::OnBehaviorStart (CShip &Ship, CAIBehaviorCtx &Ctx, const COrderDesc &OrderDesc)

//	OnBehaviorStart
//
//	Start behavior

	{
	DEBUG_TRY

	if (!OrderDesc.GetTarget())
		throw CException(ERR_FAIL);

	//	Make sure we're undocked because we're going flying

	Ctx.Undock(&Ship);

	//	Set our basic data

	m_Objs[objDest] = OrderDesc.GetTarget();
	m_rMinDist2 = LIGHT_SECOND * Max(1, (int)OrderDesc.GetDataInteger());
	m_rMinDist2 *= m_rMinDist2;

	//	See if we should take a nav path

	if (Ship.GetDistance2(OrderDesc.GetTarget()) > NAV_PATH_THRESHOLD2
			&& Ctx.CalcNavPath(&Ship, OrderDesc.GetTarget()))
		m_iState = EState::OnCourseViaNavPath;

	//	Otherwise, go there

	else
		m_iState = EState::Approaching;

	DEBUG_CATCH
	}

void CApproachOrder::OnReadFromStream (SLoadCtx &Ctx, const COrderDesc &OrderDesc)

//	OnReadFromStream
//
//	Read

	{
	DWORD dwLoad;

	Ctx.pStream->Read((char *)&dwLoad, sizeof(DWORD));
	m_iState = (EState)dwLoad;

	Ctx.pStream->Read((char *)&m_rMinDist2, sizeof(Metric));
	}

void CApproachOrder::OnWriteToStream (IWriteStream *pStream) const

//	OnWriteToStream
//
//	Write

	{
	DWORD dwSave;

	dwSave = (DWORD)m_iState;
	pStream->Write((char *)&dwSave, sizeof(DWORD));

	pStream->Write((char *)&m_rMinDist2, sizeof(Metric));
	}
