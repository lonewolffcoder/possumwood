#include <possumwood_sdk/node_implementation.h>

#include <opencv2/opencv.hpp>

#include <actions/traits.h>

#include "lightfield_pattern.h"
#include "lightfield_samples.h"
#include "tools.h"

namespace {

dependency_graph::InAttr<possumwood::opencv::LightfieldPattern> a_pattern;
dependency_graph::InAttr<float> a_uvOffset, a_uvThreshold;
dependency_graph::OutAttr<possumwood::opencv::LightfieldSamples> a_samples;

dependency_graph::State compute(dependency_graph::Values& data) {
	const possumwood::opencv::LightfieldPattern& pattern = data.get(a_pattern);

	data.set(a_samples, possumwood::opencv::LightfieldSamples(pattern, data.get(a_uvOffset), data.get(a_uvThreshold)));

	return dependency_graph::State();
}

void init(possumwood::Metadata& meta) {
	meta.addAttribute(a_pattern, "pattern", possumwood::opencv::LightfieldPattern(), possumwood::AttrFlags::kVertical);
	meta.addAttribute(a_uvOffset, "uv_offset", 0.0f);
	meta.addAttribute(a_uvThreshold, "uv_threshold", 1.0f);
	meta.addAttribute(a_samples, "samples", possumwood::opencv::LightfieldSamples(), possumwood::AttrFlags::kVertical);

	meta.addInfluence(a_pattern, a_samples);
	meta.addInfluence(a_uvOffset, a_samples);
	meta.addInfluence(a_uvThreshold, a_samples);

	meta.setCompute(compute);
}


possumwood::NodeImplementation s_impl("opencv/lightfields/samples_refocus", init);

}