#include "graph.h"

#include "node.h"

namespace dependency_graph { namespace io {

void adl_serializer<Graph>::to_json(json& j, const ::dependency_graph::Graph& g) {
	std::map<std::string, unsigned> uniqueIds;
	std::map<const ::dependency_graph::Node*, std::string> nodeIds;

	j["nodes"] = "{}"_json;
	for(auto& n : g.nodes()) {
		// figure out a unique name - type with a number appended
		std::string name = n.metadata().type();
		auto slash = name.rfind('/');
		if(slash != std::string::npos)
			name = name.substr(slash+1);
		name += "_" + std::to_string(uniqueIds[name]++);

		// and use this to save the node
		j["nodes"][name] = n;

		// remember the assigned ID for connection saving
		nodeIds[&n] = name;
	}

	j["connections"] = "[]"_json;
	for(auto& c : g.connections()) {
		j["connections"].push_back("{}"_json);

		auto& connection = j["connections"].back();
		connection["out_node"] = nodeIds[&c.first.node()];
		connection["out_port"] = c.first.index();
		connection["in_node"] = nodeIds[&c.second.node()];
		connection["in_port"] = c.second.index();
	}
}

void adl_serializer<Graph>::from_json(const json& j, ::dependency_graph::Graph& g) {
	g.nodes().clear();

	std::map<std::string, size_t> nodeIds;

	for(json::const_iterator ni = j["nodes"].begin(); ni != j["nodes"].end(); ++ni) {
		const json& n = ni.value();

		std::unique_ptr<BaseData> blindData;
		if(!n["blind_data"].is_null()) {
			blindData = BaseData::create(n["blind_data"]["type"].get<std::string>());
			assert(blindData != nullptr);
			blindData->fromJson(n["blind_data"]["value"]);
		}

		Node& node = g.nodes().add(Metadata::instance(n["type"].get<std::string>()), n["name"].get<std::string>(), std::move(blindData));
		adl_serializer<Node>::from_json(n, node);

		nodeIds[ni.key()] = g.nodes().size()-1;
	}

	for(auto& c : j["connections"]) {
		Node& n1 = g.nodes()[nodeIds[c["out_node"].get<std::string>()]];
		Node& n2 = g.nodes()[nodeIds[c["in_node"].get<std::string>()]];

		n1.port(c["out_port"].get<size_t>()).connect(
			n2.port(c["in_port"].get<size_t>()));
	}
}

} }
