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

#include "IForceSeatMI.h"

class ForceSeatMI_Impl : public IForceSeatMI
{
public:
	ForceSeatMI_Impl();
	virtual ~ForceSeatMI_Impl();
	
	virtual IForceSeatMI_API& GetAPI();
	virtual IForceSeatMI_PlaneTelemetry& GetPlaneTelemetry();

private:
	IForceSeatMI_API* m_api;
	IForceSeatMI_PlaneTelemetry* m_planeTelemetry;
};
