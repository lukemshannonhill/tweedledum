/*-------------------------------------------------------------------------------------------------
| This file is distributed under the MIT License.
| See accompanying file /LICENSE for details.
| Author(s): Bruno Schmitt
*------------------------------------------------------------------------------------------------*/
#pragma once

#include "../gates/gate_kinds.hpp"
#include "../views/depth_view.hpp"

#include <algorithm>
#include <cstdint>
#include <fmt/format.h>
#include <fstream>
#include <iostream>
#include <vector>

namespace tweedledum {

/*! \brief Writes network in DOT format into output stream
 *
 * An overloaded variant exists that writes the network into a file.
 *
 * **Required gate functions:**
 * - `is`
 * - `is_control`
 * - `foreach_target`
 *
 * **Required network functions:**
 * - `get_node`
 * - `clear_marks`
 * - `mark`
 * - `foreach_child`
 * - `foreach_gate`
 * - `foreach_input`
 * - `foreach_output`
 *
 * \param network Network
 * \param os Output stream
 */
template<typename Network>
void write_dot(Network const& dag_network, std::ostream& os)
{
	// std::cout << "Writing dot file...\n";
	// std::cout << "  + Computing node levels\n";
	auto ntk_lvl = depth_view(dag_network);

	// std::cout << "  + Write dot header\n";
	os << "# Quantum DAG structure generated by tweedledum package\n";
	os << "digraph QuantumNet {\n";
	os << "\trankdir = \"RL\";\n";
	os << "\tsize = \"7.5,10\";\n";
	os << "\tcenter = true;\n";
	os << "\tedge [dir = back];\n\n";

	// std::cout << "  + Create invisible level label nodes\n";
	os << "\t{\n";
	os << "\t\tnode [shape = plaintext];\n";
	os << "\t\tedge [style = invis];\n";
	for (auto level = ntk_lvl.depth() + 1; level-- > 0;) {
		os << fmt::format("\t\tLevel{} [label = \"\"];\n", level);
	}
	os << "\t\t";
	for (auto level = ntk_lvl.depth() + 1; level-- > 0;) {
		os << fmt::format("Level{}{}", level, level != 0 ? " -> " : ";");
	}
	os << "\n\t}\n";

	// std::cout << "  + Create output nodes\n";
	os << "\t{\n";
	os << "\t\trank = same;\n";
	os << fmt::format("\t\tLevel{};\n", ntk_lvl.depth());
	dag_network.foreach_output([&os](auto const& node, auto index) {
		node.gate.foreach_target([&os, &index](auto qid) {
			os << fmt::format("\t\tNode{} [label = \"{}\", shape = cds, color = coral, "
			                  "fillcolor = coral];\n",
			                  index, qid);
		});
	});
	os << "\t}\n";

	// std::cout << "  + Create gate nodes\n";
	for (auto level = ntk_lvl.depth(); level-- > 1;) {
		os << "\t{\n";
		os << "\t\trank = same;\n";
		os << fmt::format("\t\tLevel{};\n", level);
		ntk_lvl.foreach_gate([&ntk_lvl, &os, level](auto const& node, auto index) {
			if (ntk_lvl.level(node) != level) {
				return;
			}
			os << fmt::format("\t\tNode{} [label = \"{}\", shape = {}];\n", index, index,
			                  node.gate.is(gate_kinds_t::cx) ? "doublecircle" :
			                                                   "ellipse");
		});
		os << "\t}\n";
	}

	// std::cout << "  + Create input nodes\n";
	os << "\t{\n";
	os << "\t\trank = same;\n";
	os << "\t\tLevel0;\n";
	ntk_lvl.foreach_input([&os](auto const& node, auto index) {
		node.gate.foreach_target([&os, &index](auto qid) {
			os << fmt::format("\t\tNode{} [label = \"{}\", shape = cds, color = coral, "
			                  "fillcolor = coral];\n",
			                  index, qid);
		});
	});
	os << "\t}\n\n";

	// std::cout << "  + Create edges\n";
	ntk_lvl.foreach_gate([&ntk_lvl, &os](auto const& node, auto index) {
		ntk_lvl.foreach_child(node, [&](auto child, auto qubit_id) {
			os << fmt::format("\tNode{} -> Node{} [style = {}];\n", index, child.index,
			                  node.gate.is_control(qubit_id) ? "dotted" : "solid");
		});
	});
	ntk_lvl.foreach_output([&ntk_lvl, &os](auto const& node, auto index) {
		ntk_lvl.foreach_child(node, [&](auto child) {
			os << fmt::format("\tNode{} -> Node{} [style = solid];\n", index,
			                  child.index);
		});
	});
	os << "}\n";
	// std::cout << "[done]\n";
}

/*! \brief Writes network in DOT format into a file
 *
 * **Required gate functions:**
 * - `is`
 * - `is_control`
 * - `foreach_target`
 *
 * **Required network functions:**
 * - `get_node`
 * - `clear_marks`
 * - `mark`
 * - `foreach_child`
 * - `foreach_gate`
 * - `foreach_input`
 * - `foreach_output`
 *
 * \param network Network
 * \param filename Filename
 */
template<typename Network>
void write_dot(Network const& network, std::string const& filename)
{
	std::ofstream os(filename.c_str(), std::ofstream::out);
	write_dot(network, os);
}

} // namespace tweedledum
