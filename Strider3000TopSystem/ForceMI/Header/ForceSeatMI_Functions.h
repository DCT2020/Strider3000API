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
#ifndef FORCE_SEAT_MI_FUNCTIONS_H
#define FORCE_SEAT_MI_FUNCTIONS_H

/*
 * This file defines pure C interface for ForceSeatMI. The easiest way to use it, is to link against
 * ForceSeatMI.lib and then just call below functions.
 *
 * Check examples for details.
 */

#include "ForceSeatMI_Status.h"
#include "ForceSeatMI_Positioning.h"
#include "ForceSeatMI_Telemetry.h"
#include "ForceSeatMI_TactileTranscuders.h"
#include "ForceSeatMI_Defines.h"

#ifdef __cplusplus
extern "C" {
#endif

	struct __FSMI_HandleType { int unused; };
	typedef struct __FSMI_HandleType* FSMI_Handle;

	/*
	 * It creates API object. It has to be called before any other functions are allowed.
	 * Remember to call ForceSeatMI_Delete to delete created object.
	 */
	FORCESEAT_MI_EXPORT
	FSMI_Handle __cdecl ForceSeatMI_Create();

	/*
	 * It deletes object created by ForceSeatMI_Create.
	 */
	FORCESEAT_MI_EXPORT
	void __cdecl ForceSeatMI_Delete(FSMI_Handle api);

	/*
	 * Call this function when the SIM is ready for sending data to the motion platform.
	 */
	FORCESEAT_MI_EXPORT
	FSMI_Bool __cdecl ForceSeatMI_BeginMotionControl(FSMI_Handle api);

	/*
	 * Call this function to when the SIM does not want to send any more data to the motion platform.
	 */
	FORCESEAT_MI_EXPORT
	FSMI_Bool __cdecl ForceSeatMI_EndMotionControl(FSMI_Handle api);

	/*
	 * It gets current status of the motion platform. It can be called at any time.
	 */
	FORCESEAT_MI_EXPORT
	FSMI_Bool __cdecl ForceSeatMI_GetPlatformInfoEx(FSMI_Handle api,
	                                                FSMI_PlatformInfo* platformInfo,
	                                                unsigned int platformInfoSize,
	                                                unsigned int timeout);

	/*
	 * It sends updated telemetry information to ForceSeatPM for further processing. 
	 * Make sure that 'mask' and 'state' fields are set correctly.
	 * Make sure to call ForceSeatMI_BeginMotionControl before this function is called.
	 */
	FORCESEAT_MI_EXPORT
	FSMI_Bool __cdecl ForceSeatMI_SendTelemetry(FSMI_Handle api,
												const FSMI_Telemetry* telemetry);

	/*
	 * Use this function if you want to specify position of the top table (top frame) in logical units (percent of maximum rotation and translation).
	 * Make sure to call ForceSeatMI_BeginMotionControl before this function is called.
	 */
	FORCESEAT_MI_EXPORT
	FSMI_Bool __cdecl ForceSeatMI_SendTopTablePosLog(FSMI_Handle api,
	                                                 const FSMI_TopTablePositionLogical* position);

	/*
	 * Use this function if you want to specify position of the top table (top frame) in physical units (radians and milimeters).
	 * Make sure to call ForceSeatMI_BeginMotionControl before this function is called.
	 */
	FORCESEAT_MI_EXPORT
	FSMI_Bool __cdecl ForceSeatMI_SendTopTablePosPhy(FSMI_Handle api,
	                                                 const FSMI_TopTablePositionPhysical* position);

	/*
	 * Use this function if you want to specify transformation matrix for the top table (top frame). 
	 * It is recommended only for 6DoF in cases when rotation center is not in default point (0, 0, 0).
	 * Make sure to call ForceSeatMI_BeginMotionControl before this function is called.
	 */
	FORCESEAT_MI_EXPORT
	FSMI_Bool __cdecl ForceSeatMI_SendTopTableMatrixPhy(FSMI_Handle api,
	                                                    const FSMI_TopTableMatrixPhysical* matrix);

	/*
	 * Call this function to generate vibrations on connected tactile transcuder.
	 * Make sure to call ForceSeatMI_BeginMotionControl before this function is called.
	 */
	FORCESEAT_MI_EXPORT
	FSMI_Bool __cdecl ForceSeatMI_SendTactileFeedbackEffects(FSMI_Handle api,
	                                                         const FSMI_TactileFeedbackEffects* effects);


#ifdef __cplusplus
}
#endif

#endif
