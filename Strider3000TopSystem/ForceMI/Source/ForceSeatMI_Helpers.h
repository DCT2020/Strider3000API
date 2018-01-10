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

namespace ForceSeatMI_Helpers
{
	template<class T>
	inline T toMeters(T centimeters)
	{
		return centimeters / 100;
	}

	template <class T>
	inline void lowPass(T stored, T newValue, T factor)
	{
		stored += (newValue - stored) * factor;
	}
}
