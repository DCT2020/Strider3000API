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

#include "ForceSeatMI_Status.h"
#include "ForceSeatMI_Positioning.h"
#include "ForceSeatMI_Telemetry.h"
#include "ForceSeatMI_TactileTranscuders.h"

class IForceSeatMI_API
{
public:
	virtual ~IForceSeatMI_API() { }
	virtual bool BeginMotionControl        () = 0;
	virtual bool EndMotionControl          () = 0;
	virtual bool GetPlatformInfoEx         (FSMI_PlatformInfo* platformInfo, unsigned int platformInfoStructSize, unsigned int timeout) = 0;
	virtual bool SendTelemetry             (const FSMI_Telemetry* telemetry) = 0;
	virtual bool SendTopTablePosLog        (const FSMI_TopTablePositionLogical* position) = 0;
	virtual bool SendTopTablePosPhy        (const FSMI_TopTablePositionPhysical* position) = 0;
	virtual bool SendTopTableMatrixPhy     (const FSMI_TopTableMatrixPhysical* matrix) = 0;
	virtual bool SendTactileFeedbackEffects(const FSMI_TactileFeedbackEffects* effects) = 0;
};
