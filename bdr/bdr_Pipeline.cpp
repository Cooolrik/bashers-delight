#include "bdr_Pipeline.h"

bdr::Pipeline::~Pipeline()
	{
	if( this->PipelineHandle )
		{
		vkDestroyPipeline( this->Module->GetDevice(), this->PipelineHandle, nullptr );
		}
	if( this->PipelineLayout )
		{
		vkDestroyPipelineLayout( this->Module->GetDevice(), this->PipelineLayout, nullptr );
		}
	}
