/* Copyright 2017-2018 PaGMO development team

This file is part of the PaGMO library.

The PaGMO library is free software; you can redistribute it and/or modify
it under the terms of either:

  * the GNU Lesser General Public License as published by the Free
    Software Foundation; either version 3 of the License, or (at your
    option) any later version.

or

  * the GNU General Public License as published by the Free Software
    Foundation; either version 3 of the License, or (at your option) any
    later version.

or both in parallel, as here.

The PaGMO library is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
for more details.

You should have received copies of the GNU General Public License and the
GNU Lesser General Public License along with the PaGMO library.  If not,
see https://www.gnu.org/licenses/. */

#define BOOST_TEST_MODULE ring
#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>

#include <algorithm>
#include <cstddef>
#include <sstream>
#include <stdexcept>

#include <boost/algorithm/string/predicate.hpp>

#include <pagmo/s11n.hpp>
#include <pagmo/topologies/ring.hpp>

using namespace pagmo;

void verify_ring_topology(const ring &r)
{
    const auto s = r.num_vertices();

    if (s < 2u) {
        return;
    }

    const auto w = r.get_weight();

    for (std::size_t i = 0; i < s; ++i) {
        const auto conn = r.get_connections(i);
        if (s > 2u) {
            BOOST_CHECK(conn.first.size() == 2u);
            BOOST_CHECK(conn.second.size() == 2u);
        } else {
            BOOST_CHECK(conn.first.size() == 1u);
            BOOST_CHECK(conn.second.size() == 1u);
        }
        BOOST_CHECK(std::all_of(conn.second.begin(), conn.second.end(), [w](double x) { return x == w; }));

        const auto next = (i + 1u) % s;
        const auto prev = (i == 0u) ? (s - 1u) : (i - 1u);
        BOOST_CHECK(std::find(conn.first.begin(), conn.first.end(), next) != conn.first.end());
        BOOST_CHECK(std::find(conn.first.begin(), conn.first.end(), prev) != conn.first.end());
    }
}

BOOST_AUTO_TEST_CASE(basic_test)
{
    ring r0;
    BOOST_CHECK(r0.get_weight() == 1);
    BOOST_CHECK(r0.num_vertices() == 0u);

    r0.push_back();
    BOOST_CHECK(r0.num_vertices() == 1u);

    r0.push_back();
    BOOST_CHECK(r0.num_vertices() == 2u);
    verify_ring_topology(r0);

    r0.push_back();
    BOOST_CHECK(r0.num_vertices() == 3u);
    verify_ring_topology(r0);

    r0.push_back();
    BOOST_CHECK(r0.num_vertices() == 4u);
    verify_ring_topology(r0);

    r0.push_back();
    BOOST_CHECK(r0.num_vertices() == 5u);
    verify_ring_topology(r0);

    r0.push_back();
    BOOST_CHECK(r0.num_vertices() == 6u);
    verify_ring_topology(r0);

    r0 = ring(.5);
    BOOST_CHECK(r0.get_weight() == .5);
    BOOST_CHECK(r0.num_vertices() == 0u);

    r0.push_back();
    BOOST_CHECK(r0.num_vertices() == 1u);

    r0.push_back();
    BOOST_CHECK(r0.num_vertices() == 2u);
    verify_ring_topology(r0);

    r0.push_back();
    BOOST_CHECK(r0.num_vertices() == 3u);
    verify_ring_topology(r0);

    r0.push_back();
    BOOST_CHECK(r0.num_vertices() == 4u);
    verify_ring_topology(r0);

    r0.push_back();
    BOOST_CHECK(r0.num_vertices() == 5u);
    verify_ring_topology(r0);

    r0.push_back();
    BOOST_CHECK(r0.num_vertices() == 6u);
    verify_ring_topology(r0);

    BOOST_CHECK(r0.get_name() == "Ring");

    // Minimal serialization test.
    {
        std::stringstream ss;
        {
            boost::archive::binary_oarchive oarchive(ss);
            oarchive << r0;
        }
        ring r1;
        {
            boost::archive::binary_iarchive iarchive(ss);
            iarchive >> r1;
        }
        BOOST_CHECK(r1.num_vertices() == 6u);
        BOOST_CHECK(r1.get_weight() == .5);
        verify_ring_topology(r1);
    }

    BOOST_CHECK_EXCEPTION(r0 = ring(-2), std::invalid_argument, [](const std::invalid_argument &ia) {
        return boost::contains(ia.what(), " is not in the [0., 1.] range");
    });
}
