#include "bdr_RayTracingAccelerationStructure.h"
#include "bdr_Buffer.h"

bdr::RayTracingAccelerationStructure::~RayTracingAccelerationStructure()
	{
	if( this->AccelerationStructure != nullptr )
		{
		RayTracingExtension::vkDestroyAccelerationStructureKHR( this->Module->GetModule()->GetDevice(), this->AccelerationStructure, nullptr );
		}
	}
