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
#ifndef FORCE_SEAT_MI_TELEMETRY_H
#define FORCE_SEAT_MI_TELEMETRY_H

#include "ForceSeatMI_Common.h"

#pragma pack(push, 1)

/*
 * This interface can used to get motion data from application or game.
 *
 * When the ForceSeatMI telemetry is implemented in application, any motion software is able to gather simulation data and
 * control motion platform (2DOF, 3DOF or 6DOF). This structure should be filled by the application and sent to motion processing 
 * system. It is require to fill "structSize" and "mask", other fields are OPTIONAL. It means that the application does not
 * have to support or provide all parameters mentioned below, but it is good to provide as much as possible.
 *
 * FIELDS LEGEND:
 * ==============
 * All values are in local vehicle coordinates.
 *
 *   YAW   - rotation along vertical axis,
 *          > 0 when vehicle front is rotating right,
 *          < 0 when vehicle front is rotating left
 *   PITCH - rotation along lateral axis,
 *          > 0 when vehicle front is rotating up
 *          < 0 when vehicle front is rotating down
 *   ROLL  - rotation along longitudinal axis,
 *          > 0 when vehicle highest point is rotating right,
 *          < 0 when vehicle highest point is rotating left
 *   SWAY  - transition along lateral axis,
 *          > 0 when vehicle is moving right,
 *          < 0 when vehicle is moving left
 *   HEAVE - transition along vertical axis,
 *          > 0 when vehicle is moving up,
 *          < 0 when vehicle is moving down
 *   SURGE - transition along longitudinal axis,
 *          > 0 when vehicle is moving forward,
 *          < 0 when vehicle is moving backward
 *
 * Please check below link for details description of yaw, pitch and roll:
 * http://en.wikipedia.org/wiki/Ship_motions
 */
typedef struct FSMI_Telemetry
{
// Version 2.61
           FSMI_UINT8  structSize; // put here sizeof(FSMI_Telemetry)
           FSMI_UINT32 mask;       // set here bits to tell motion software which of below fields are provided

/*BIT:1*/  FSMI_UINT8  state;   // only single bit is used in current version
				//  (state & 0x01) == 0 -> no pause
				//  (state & 0x01) == 1 -> paused

/*BIT:2*/  FSMI_UINT32 rpm;     // engine rpm                                 
/*BIT:3*/  FSMI_FLOAT  speed;   // vehicle speed in m/s, can be < 0 for reverse

/*BIT:4*/  FSMI_FLOAT yaw;   // yaw in radians
/*BIT:4*/  FSMI_FLOAT pitch; // vehicle pitch in radians
/*BIT:4*/  FSMI_FLOAT roll;  // vehicle roll in radians

/*BIT:5*/  FSMI_FLOAT yawAcceleration;   // radians/s^2
/*BIT:5*/  FSMI_FLOAT pitchAcceleration; // radians/s^2
/*BIT:5*/  FSMI_FLOAT rollAcceleration;  // radians/s^2

/*BIT:6*/  FSMI_FLOAT yawSpeed;   // radians/s
/*BIT:6*/  FSMI_FLOAT pitchSpeed; // radians/s
/*BIT:6*/  FSMI_FLOAT rollSpeed;  // radians/s

/*BIT:7*/  FSMI_FLOAT swayAcceleration;  // m/s^2
/*BIT:7*/  FSMI_FLOAT heaveAcceleration; // m/s^2
/*BIT:7*/  FSMI_FLOAT surgeAcceleration; // m/s^2

/*BIT:8*/  FSMI_FLOAT swaySpeed;  // m/s
/*BIT:8*/  FSMI_FLOAT heaveSpeed; // m/s
/*BIT:8*/  FSMI_FLOAT surgeSpeed; // m/s

/*BIT:9*/  FSMI_UINT8 gasPedalPosition;    // 0 to 100 (in percent)
/*BIT:9*/  FSMI_UINT8 brakePedalPosition;  // 0 to 100 (in percent)
/*BIT:9*/  FSMI_UINT8 clutchPedalPosition; // 0 to 100 (in percent)

/*BIT:10*/ FSMI_INT8  gearNumber; // -1 for reverse, 0 for neutral, 1, 2, 3, ...

/*BIT:11*/ FSMI_UINT8 leftSideGroundType;    // grass, dirt, gravel, please check FSMI_GroundType
/*BIT:11*/ FSMI_UINT8 rightSideGroundType;

/*BIT:12*/ FSMI_FLOAT collisionForce; // in Newtons (N), zero when there is no collision
/*BIT:12*/ FSMI_FLOAT collisionYaw;   // yaw, pitch and roll for start point of collision force vector, end point is always in vehicle center
/*BIT:12*/ FSMI_FLOAT collisionPitch;
/*BIT:12*/ FSMI_FLOAT collisionRoll;

/*BIT:13*/ FSMI_FLOAT globalPositionX; // global position, Y is vertical axes
/*BIT:13*/ FSMI_FLOAT globalPositionY;
/*BIT:13*/ FSMI_FLOAT globalPositionZ;

/*BIT:14*/ FSMI_UINT32 timeMs;   // simulation time in e.g. m/s, can be relative (e.g. 0 means means application has just started)
/*BIT:15*/ FSMI_UINT8  triggers; // state of buttons, gun triggers, etc. It is passed directly to the motion platform

/*BIT:16*/ FSMI_UINT32 maxRpm;     // engine max rpm used to simulate rev limiter
/*BIT:17*/ FSMI_UINT32 flags;      // combination of FSMI_Flags
/*BIT:18*/ FSMI_FLOAT  aux[8];     // Custom use field that can be used in script in ForceSeatPM

} FSMI_Telemetry;

typedef enum FSMI_GroundType
{
	FSMI_UknownGround      = 0,
	FSMI_TarmacGround      = 1,
	FSMI_GrassGround       = 2,
	FSMI_DirtGround        = 3,
	FSMI_GravelGround      = 4,
	FSMI_RumbleStripGround = 5
} FSMI_GroundType;

typedef enum FSMI_Flags
{
	FSMI_ShiftLight      = (1 << 0),
	FSMI_AbsIsWorking    = (1 << 1),
	FSMI_EspIsWorking    = (1 << 2),
	FSMI_FrontWheelDrive = (1 << 3),
	FSMI_RearWheelDrive  = (1 << 4)
} FSMI_Flags;

#pragma pack(pop)

// Helpers for mask bits
#define FSMI_TEL_BIT_STATE                         (1 << 1)
#define FSMI_TEL_BIT_RPM                           (1 << 2)
#define FSMI_TEL_BIT_SPEED                         (1 << 3)

#define FSMI_TEL_BIT_YAW_PITCH_ROLL                (1 << 4)
#define FSMI_TEL_BIT_YAW_PITCH_ROLL_ACCELERATION   (1 << 5)
#define FSMI_TEL_BIT_YAW_PITCH_ROLL_SPEED          (1 << 6)
#define FSMI_TEL_BIT_SWAY_HEAVE_SURGE_ACCELERATION (1 << 7)
#define FSMI_TEL_BIT_SWAY_HEAVE_SURGE_SPEED        (1 << 8)

#define FSMI_TEL_BIT_PEDALS_POSITION               (1 << 9)
#define FSMI_TEL_BIT_GEAR_NUMBER                   (1 << 10)
#define FSMI_TEL_BIT_GROUND_TYPE                   (1 << 11)
#define FSMI_TEL_BIT_COLLISION                     (1 << 12)

#define FSMI_TEL_BIT_GLOBAL_POSITION               (1 << 13)
#define FSMI_TEL_BIT_TIME                          (1 << 14)
#define FSMI_TEL_BIT_TRIGGERS                      (1 << 15)

#define FSMI_TEL_BIT_MAX_RPM                       (1 << 16)
#define FSMI_TEL_BIT_FLAGS                         (1 << 17)
#define FSMI_TEL_BIT_AUX                           (1 << 18)

#endif
