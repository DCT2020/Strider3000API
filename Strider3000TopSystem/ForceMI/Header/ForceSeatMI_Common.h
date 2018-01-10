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
#ifndef FORCE_SEAT_MI_COMMON_H
#define FORCE_SEAT_MI_COMMON_H

#pragma pack(push, 1)

typedef unsigned char      FSMI_UINT8;
typedef signed   char      FSMI_INT8;
typedef unsigned int       FSMI_UINT32;
typedef signed   int       FSMI_INT32;
typedef unsigned short     FSMI_UINT16;
typedef signed   short     FSMI_INT16;
typedef float              FSMI_FLOAT;
typedef unsigned long long FSMI_UINT64;
typedef char               FSMI_Bool;

enum
{
    FSMI_True        = 1,
    FSMI_False       = 0,
    FSMI_MotorsCount = 6
};

#pragma pack(pop)

// State flags
#define FSMI_STATE_NO_PAUSE    (0 << 0)
#define FSMI_STATE_PAUSE       (1 << 0)

#endif
