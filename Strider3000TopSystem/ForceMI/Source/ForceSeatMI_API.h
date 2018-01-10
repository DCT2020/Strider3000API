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

#include "IForceSeatMI_API.h"
#include "ForceSeatMI_Functions.h"

class ForceSeatMI_API : public IForceSeatMI_API
{
public:
	ForceSeatMI_API();
	virtual ~ForceSeatMI_API();
	virtual bool BeginMotionControl        ();
	virtual bool EndMotionControl          ();
	virtual bool GetPlatformInfoEx         (FSMI_PlatformInfo* platformInfo, unsigned int platformInfoStructSize, unsigned int timeout);
	virtual bool SendTelemetry             (const FSMI_Telemetry* telemetry);
	virtual bool SendTopTablePosLog        (const FSMI_TopTablePositionLogical* position);
	virtual bool SendTopTablePosPhy        (const FSMI_TopTablePositionPhysical* position);
	virtual bool SendTopTableMatrixPhy     (const FSMI_TopTableMatrixPhysical* matrix);
	virtual bool SendTactileFeedbackEffects(const FSMI_TactileFeedbackEffects* effects);
private:
	FSMI_Handle m_handle;
};
