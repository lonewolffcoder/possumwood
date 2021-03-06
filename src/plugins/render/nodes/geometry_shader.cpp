#include <memory>

#include <possumwood_sdk/node_implementation.h>
#include <possumwood_sdk/source_editor.h>

#include "datatypes/shader.h"
#include "default_shaders.h"

namespace {

dependency_graph::InAttr<std::string> a_src;
dependency_graph::OutAttr<std::shared_ptr<const possumwood::GeometryShader>> a_shader;

class Editor : public possumwood::SourceEditor {
	public:
		Editor() : SourceEditor(a_src) {
		}

};

dependency_graph::State compute(dependency_graph::Values& data) {
	dependency_graph::State result;

	const std::string& src = data.get(a_src);

	std::unique_ptr<possumwood::GeometryShader> shader(new possumwood::GeometryShader());

	shader->compile(src);
	result = shader->state();

	if(!shader->state().errored())
		data.set(a_shader, std::shared_ptr<const possumwood::GeometryShader>(shader.release()));
	else
		data.set(a_shader, std::shared_ptr<const possumwood::GeometryShader>());

	return result;
}

void init(possumwood::Metadata& meta) {
	meta.addAttribute(a_src, "source", possumwood::defaultGeometryShaderSrc());
	meta.addAttribute(a_shader, "shader");

	meta.addInfluence(a_src, a_shader);

	meta.setCompute(&compute);
	meta.setEditor<Editor>();
}

possumwood::NodeImplementation s_impl("render/geometry_shader", init);

}
