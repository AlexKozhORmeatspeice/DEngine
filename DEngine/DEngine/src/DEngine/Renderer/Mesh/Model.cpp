#include "dpch.h"
#include "Model.h"

namespace DEngine
{
	Model::Model(const std::vector<MeshRenderData>& renderData, const std::string& name)
		: m_RenderDatas(renderData), m_Name(name)
	{
	}
}