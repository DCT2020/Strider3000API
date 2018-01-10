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
#include "ForceSeatMI_PlaneTelemetry.h"
#include "ForceSeatMI_Helpers.h"

using namespace ForceSeatMI_Helpers;

#define FSMI_PT_ACC_LOW_PASS_FACTOR             0.5f
#define FSMI_PT_ANGLES_SPEED_LOW_PASS_FACTOR    0.7f

ForceSeatMI_PlaneTelemetry::ForceSeatMI_PlaneTelemetry(IForceSeatMI_API& api)
	: m_prevSurgeSpeed(0)
	, m_prevSwaySpeed(0)
	, m_prevHeaveSpeed(0)
	, m_firstCall(true)
	, m_api(api)
{
	memset(&m_telemetry, 0, sizeof(m_telemetry));
	m_telemetry.structSize = sizeof(m_telemetry);

	m_telemetry.mask =
		FSMI_TEL_BIT_STATE |
		FSMI_TEL_BIT_SPEED |
		FSMI_TEL_BIT_YAW_PITCH_ROLL |
		FSMI_TEL_BIT_YAW_PITCH_ROLL_SPEED |
		FSMI_TEL_BIT_SWAY_HEAVE_SURGE_ACCELERATION |
		FSMI_TEL_BIT_SWAY_HEAVE_SURGE_SPEED;
}

ForceSeatMI_PlaneTelemetry::~ForceSeatMI_PlaneTelemetry()
{
	End();
}

void ForceSeatMI_PlaneTelemetry::Begin()
{
	m_firstCall = true;
	m_api.BeginMotionControl();
}

void ForceSeatMI_PlaneTelemetry::End()
{
	m_api.EndMotionControl();
	m_firstCall = false;
}

void ForceSeatMI_PlaneTelemetry::Tick(const APawn& pawn, float delta, bool paused)
{
	if (delta < 0.0001f)
	{
		return;
	}

	m_telemetry.state = paused ? FSMI_STATE_PAUSE : FSMI_STATE_NO_PAUSE;

	FVector    currentLocation  = pawn.GetActorLocation();
	FTransform currentTransform = pawn.GetActorTransform();;

	m_telemetry.pitch = FMath::DegreesToRadians(currentTransform.GetRotation().Euler().Y);
	m_telemetry.roll  = FMath::DegreesToRadians(currentTransform.GetRotation().Euler().X);
	m_telemetry.yaw   = FMath::DegreesToRadians(currentTransform.GetRotation().Euler().Z);

	if (m_firstCall)
	{
		m_firstCall = false;

		m_telemetry.surgeSpeed = 0;
		m_telemetry.swaySpeed  = 0;
		m_telemetry.heaveSpeed = 0;

		m_telemetry.speed = 0;

		m_telemetry.surgeAcceleration = 0;
		m_telemetry.swayAcceleration  = 0;
		m_telemetry.heaveAcceleration = 0;

		m_telemetry.pitchSpeed = 0;
		m_telemetry.rollSpeed  = 0;
		m_telemetry.yawSpeed   = 0;
	}
	else
	{
		FVector velocity;
		velocity.X = (currentLocation.X - m_prevLocation.X) / delta;
		velocity.Y = (currentLocation.Y - m_prevLocation.Y) / delta;
		velocity.Z = (currentLocation.Z - m_prevLocation.Z) / delta;

		m_telemetry.surgeSpeed  = toMeters(FVector::DotProduct(pawn.GetActorForwardVector(), velocity));
		m_telemetry.swaySpeed   = toMeters(FVector::DotProduct(pawn.GetActorRightVector(),   velocity));
		m_telemetry.heaveSpeed  = toMeters(FVector::DotProduct(pawn.GetActorUpVector(),      velocity));

		m_telemetry.speed = m_telemetry.surgeSpeed * 3.6f /* km/h */;

		lowPass(m_telemetry.surgeAcceleration, (m_telemetry.surgeSpeed - m_prevSurgeSpeed) / delta, FSMI_PT_ACC_LOW_PASS_FACTOR);
		lowPass(m_telemetry.swayAcceleration,  (m_telemetry.swaySpeed  - m_prevSwaySpeed)  / delta, FSMI_PT_ACC_LOW_PASS_FACTOR);
		lowPass(m_telemetry.heaveAcceleration, (m_telemetry.heaveSpeed - m_prevHeaveSpeed) / delta, FSMI_PT_ACC_LOW_PASS_FACTOR);

		FTransform deltaTransform = currentTransform.GetRelativeTransform(m_prevTransform);
		lowPass(m_telemetry.pitchSpeed, deltaTransform.GetRotation().Euler().Y / delta, FSMI_PT_ANGLES_SPEED_LOW_PASS_FACTOR);
		lowPass(m_telemetry.rollSpeed,  deltaTransform.GetRotation().Euler().X / delta, FSMI_PT_ANGLES_SPEED_LOW_PASS_FACTOR);
		lowPass(m_telemetry.yawSpeed,   deltaTransform.GetRotation().Euler().Z / delta, FSMI_PT_ANGLES_SPEED_LOW_PASS_FACTOR);
	}

	m_prevSurgeSpeed = m_telemetry.surgeSpeed;
	m_prevSwaySpeed  = m_telemetry.swaySpeed;
	m_prevHeaveSpeed = m_telemetry.heaveSpeed;
	m_prevTransform  = currentTransform;
	m_prevLocation   = currentLocation;

	m_api.SendTelemetry(&m_telemetry);
}
