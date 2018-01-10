/*
* Copyright (C) 2012-2017 Motion Systems
*
* This file is part of ForceSeat motion system.
*
* www.motionsystems.eu
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE.
*/

#pragma once

#include "IForceSeatMI_PlaneTelemetry.h"
#include "IForceSeatMI_API.h"

#include "TransformVectorized.h"
#include "GameFramework/Pawn.h"

class ForceSeatMI_PlaneTelemetry : public IForceSeatMI_PlaneTelemetry
{
public:
	explicit ForceSeatMI_PlaneTelemetry(IForceSeatMI_API& api);
	virtual ~ForceSeatMI_PlaneTelemetry();
	
	virtual void Begin();
	virtual void End();
	virtual void Tick(const APawn& pawn, float delta, bool paused);

private:
	FTransform m_prevTransform;
	FVector m_prevLocation;
	float m_prevSurgeSpeed;
	float m_prevSwaySpeed;
	float m_prevHeaveSpeed;
	bool m_firstCall;

	IForceSeatMI_API&  m_api;
	FSMI_Telemetry     m_telemetry;
};
