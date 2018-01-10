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

#include "ModuleManager.h"
#include "IForceSeatMI_API.h"
#include "IForceSeatMI_PlaneTelemetry.h"

class IForceSeatMI : public IModuleInterface
{
public:
	// Get reference to API object. The lifespan of the object is managed by this module.
	virtual IForceSeatMI_API& GetAPI() = 0;
	
	// Get reference to PlaneTelemetry object. The lifespan of the object is managed by this module.
	virtual IForceSeatMI_PlaneTelemetry& GetPlaneTelemetry() = 0;

	static inline IForceSeatMI& Get()
	{
		return FModuleManager::LoadModuleChecked<IForceSeatMI>("ForceSeatMI");
	}

	static inline bool IsAvailable()
	{
		return FModuleManager::Get().IsModuleLoaded("ForceSeatMI");
	}
};
