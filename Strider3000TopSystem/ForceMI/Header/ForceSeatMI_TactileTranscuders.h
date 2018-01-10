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
#ifndef FORCE_SEAT_MI_TACTILE_TRANSCUDERS_H
#define FORCE_SEAT_MI_TACTILE_TRANSCUDERS_H

#include "ForceSeatMI_Common.h"

#pragma pack(push, 1)

/*
 * Definition of available streams for tactile feedback system
 */
typedef enum FSMI_TactileFeedbackStream
{
	FSMI_StreamCenter,
	FSMI_StreamEngine,
	FSMI_StreamFL,
	FSMI_StreamFR,
	FSMI_StreamRL,
	FSMI_StreamRR,
	FSMI_StreamCH,

	FSMI_StreamsCount,
} FSMI_TactileTranscudersStream;

/*
 * This structure defines effects for tactile transcuders. Signals of given frequency and amplitude are generates on associated audio outputs.
 */
typedef struct FSMI_TactileFeedbackEffects
{
	FSMI_UINT8  structSize; // put here sizeof(FSMI_TactileFeedbackEffects)

	FSMI_UINT32 frequency[FSMI_StreamsCount]; // frequency in Hz
	FSMI_FLOAT  amplitude[FSMI_StreamsCount]; // signal amplitude from 0 to 1
} FSMI_TactileFeedbackEffects;

#pragma pack(pop)

#endif
