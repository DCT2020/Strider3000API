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
#ifndef FORCE_SEAT_MI_POSITIONING_H
#define FORCE_SEAT_MI_POSITIONING_H

#include "ForceSeatMI_Common.h"

#pragma pack(push, 1)

/*
 * This structure defines position of top frame (table) in logical units (percents).
 * It does not use Inverse Kinematics module so rotation and movements are not always linear.
 */
typedef struct FSMI_TopTablePositionLogical
{
          FSMI_UINT8  structSize; // put here sizeof(FSMI_TopTablePositionLogical)
          FSMI_UINT32 mask;       // set here bits to tell motion software which of below fields are provided

/*BIT:1*/ FSMI_UINT8  state;      // state flag (bit fields, it is used to PAUSE or UNPAUSE the system)

/*BIT:2*/ FSMI_INT16  roll;       // -32k max left, +32k max right
/*BIT:2*/ FSMI_INT16  pitch;      // -32k max rear, +32k max front
/*BIT:2*/ FSMI_INT16  yaw;        // -32k max left, +32k max right
/*BIT:2*/ FSMI_INT16  heave;      // -32k max bottom, +32k max top
/*BIT:2*/ FSMI_INT16  sway;       // -32k max left, +32k max right
/*BIT:2*/ FSMI_INT16  surge;      // -32k max rear, +32k max front

/*BIT:3*/ FSMI_UINT16 maxSpeed; // 0 to 64k, actual speed is not always equal to max speed due to ramps
/*BIT:4*/ FSMI_UINT8  triggers; // State of buttons, gun triggers, etc. It is passed directly to the motion platform
/*BIT:5*/ FSMI_FLOAT  aux[8];   // Custom use field that can be used in script in ForceSeatPM

} FSMI_TopTablePositionLogical;

/*
 * This structure defines position of top frame (table) in physical units (rad, mm).
 * It uses Inverse Kinematics module and it might not be supported by all motion platforms.
 * By default BestMatch strategy is used.
 */
typedef struct FSMI_TopTablePositionPhysical
{
          FSMI_UINT8  structSize; // put here sizeof(FSMI_TopTablePositionPhysical)
          FSMI_UINT32 mask;       // set here bits to tell motion software which of below fields are provided

/*BIT:1*/ FSMI_UINT8  state;      // state flag (bit fields, it is used to PAUSE or UNPAUSE the system)

/*BIT:2*/ FSMI_FLOAT  roll;       // in radians, roll  < 0 = left,  roll > 0  = right
/*BIT:2*/ FSMI_FLOAT  pitch;      // in radians, pitch < 0 = front, pitch > 0 = rear
/*BIT:2*/ FSMI_FLOAT  yaw;        // in radians, yaw   < 0 = right, yaw > 0   = left
/*BIT:2*/ FSMI_FLOAT  heave;      // in mm, heave < 0 - down, heave > 0 - top
/*BIT:2*/ FSMI_FLOAT  sway;       // in mm, sway  < 0 - left, sway  > 0 - right
/*BIT:2*/ FSMI_FLOAT  surge;      // in mm, surge < 0 - rear, surge > 0 - front

/*BIT:3*/ FSMI_UINT16 maxSpeed; // 0 to 64k, actual speed is not always equal to max speed due to ramps
/*BIT:4*/ FSMI_UINT8  triggers; // State of buttons, gun triggers, etc. It is passed directly to the motion platform
/*BIT:5*/ FSMI_FLOAT  aux[8];   // Custom use field that can be used in script in ForceSeatPM

} FSMI_TopTablePositionPhysical;

/*
 * This structure defines position of top frame (table) in physical units (rad, mm) by specifing transformation matrix.
 * It uses Inverse Kinematics module and it is dedicated for 6DoF motion platforms.
 * If matrix transformation is specified, the Inverse Kinematics module always uses FullMatch strategy.
 */
typedef struct FSMI_TopTableMatrixPhysical
{
          FSMI_UINT8  structSize; // put here sizeof(FSMI_TopTableMatrixPhysical)
          FSMI_UINT32 mask;       // set here bits to tell motion software which of below fields are provided

/*BIT:1*/ FSMI_UINT8  state;      // state flag (bit fields, it is used to PAUSE or UNPAUSE the system)

          /* OFFSET (in mm):
           *  x axis = left-right movement, sway,  x < 0 - left, x > 0 - right
           *  y axis = rear-front movement, surge, y < 0 - rear, y > 0 - front
           *  z axis = down-top movement,   heave, z < 0 - down, z > 0 - top
           *
           * ROTATION (in radians):
           * x axis, pitch = x < 0 = front, x > 0 = rear
           * y axis, roll = y < 0  = left,  y > 0 = right
           * z axis, yaw  = z < 0  = right, z > 0 = left
           * /
/*BIT:2*/ FSMI_FLOAT  transformation[4][4]; // 3D transformation matrix


/*BIT:3*/ FSMI_UINT16 maxSpeed; // 0 to 64k, actual speed is not always equal to max speed due to ramps
/*BIT:4*/ FSMI_UINT8  triggers; // State of buttons, gun triggers, etc. It is passed directly to the motion platform
/*BIT:5*/ FSMI_FLOAT  aux[8];   // Custom use field that can be used in script in ForceSeatPM

} FSMI_TopTableMatrixPhysical;

#pragma pack(pop)

// Helpers for mask bits
#define FSMI_POS_BIT_STATE       (1 << 1)
#define FSMI_POS_BIT_POSITION    (1 << 2)
#define FSMI_POS_BIT_MATRIX      (1 << 2)
#define FSMI_POS_BIT_MAX_SPEED   (1 << 3)
#define FSMI_POS_BIT_TRIGGERS    (1 << 4)
#define FSMI_POS_BIT_AUX         (1 << 5)

#endif
