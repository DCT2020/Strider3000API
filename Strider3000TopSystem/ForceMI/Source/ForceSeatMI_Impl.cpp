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

#include "ForceSeatMI_Impl.h"
#include "ForceSeatMI_API.h"

ForceSeatMI_Impl::ForceSeatMI_Impl()
	: m_api(nullptr)
{
	m_api = new ForceSeatMI_API();
}

ForceSeatMI_Impl::~ForceSeatMI_Impl()
{
	// API has to be removed as last one
	delete m_api;
	m_api = nullptr;
}

IForceSeatMI_API& ForceSeatMI_Impl::GetAPI()
{
	return *m_api;
}

IForceSeatMI_PlaneTelemetry& ForceSeatMI_Impl::GetPlaneTelemetry()
{
	return *m_planeTelemetry;
}

//IMPLEMENT_MODULE(ForceSeatMI_Impl, ForceSeatMI)
