/*-------------------------------------------------------------------------------------------------
| This file is distributed under the MIT License.
| See accompanying file /LICENSE for details.
| Author(s): Bruno Schmitt
*------------------------------------------------------------------------------------------------*/
#include <catch.hpp>
#include <sstream>
#include <tweedledum/gates/gate_kinds.hpp>
#include <tweedledum/gates/mcmt_gate.hpp>
#include <tweedledum/gates/mcst_gate.hpp>
#include <tweedledum/io/write_qpic.hpp>
#include <tweedledum/networks/gdg_network.hpp>
#include <tweedledum/networks/gg_network.hpp>
#include <tweedledum/networks/netlist.hpp>

TEST_CASE("Write simple MCMT into qpic", "[qpic]")
{
	using namespace tweedledum;
	gg_network<mcmt_gate> gg_net;
	gg_net.add_qubit();
	gg_net.add_qubit();
	gg_net.add_qubit();
	auto controls = std::vector<uint32_t>({0u, 1u});
	auto target = std::vector<uint32_t>({2u});
	gg_net.add_gate(gate_kinds_t::mcx, controls, target);
	CHECK(gg_net.size() == 7);
	CHECK(gg_net.num_qubits() == 3);
	CHECK(gg_net.num_gates() == 1);

	std::ostringstream os;
	write_qpic(gg_net, os);
	CHECK(os.str() == "q0 W q0 q0\nq1 W q1 q1\nq2 W q2 q2\n+q2  q0 q1\n");
}
