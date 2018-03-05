/*
 * Copyright (C) 2018 Werner Turing <werner.turing@protonmail.com>
 *
 * This file is part of multi-delogo.
 *
 * multi-delogo is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * multi-delogo is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with multi-delogo.  If not, see <http://www.gnu.org/licenses/>.
 */
#include <memory>
#include <string>
#include <sstream>

#include "FilterList.hpp"
#include "Filters.hpp"
#include "RegularScriptGenerator.hpp"

using namespace fg;


#define BOOST_TEST_MODULE regular ffmpeg script generator
#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>

#include "../TestHelpers.hpp"


BOOST_AUTO_TEST_CASE(should_generate_ffmpeg_script)
{
  FilterList list;
  list.insert(1, new DelogoFilter(10, 11, 12, 13));
  list.insert(501, new DrawboxFilter(20, 21, 22, 23));
  list.insert(1001, new NullFilter());
  list.insert(1301, new DrawboxFilter(30, 31, 32, 33));
  list.insert(2001, new DrawboxFilter(40, 41, 42, 43));
  std::shared_ptr<ScriptGenerator> g = RegularScriptGenerator::create(list);

  std::ostringstream out;
  g->generate_ffmpeg_script(out);

  std::string expected =
    "delogo=enable='between(n,0,499)':x=10:y=11:w=12:h=13,\n"
    "drawbox=enable='between(n,500,999)':x=20:y=21:w=22:h=23:c=black:t=max,\n"
    "drawbox=enable='between(n,1300,1999)':x=30:y=31:w=32:h=33:c=black:t=max,\n"
    "drawbox=enable='gte(n,2000)':x=40:y=41:w=42:h=43:c=black:t=max";
  BOOST_CHECK_EQUAL(out.str(), expected);
}


BOOST_AUTO_TEST_CASE(should_discard_a_null_filter_at_the_end)
{
  FilterList list;
  list.insert(1, new DelogoFilter(10, 11, 12, 13));
  list.insert(1001, new NullFilter());
  std::shared_ptr<ScriptGenerator> g = RegularScriptGenerator::create(list);

  std::ostringstream out;
  g->generate_ffmpeg_script(out);

  std::string expected =
    "delogo=enable='between(n,0,999)':x=10:y=11:w=12:h=13";
  BOOST_CHECK_EQUAL(out.str(), expected);
}


BOOST_AUTO_TEST_CASE(should_work_for_a_one_filter_list)
{
  FilterList list;
  list.insert(50, new DelogoFilter(10, 11, 12, 13));
  std::shared_ptr<ScriptGenerator> g = RegularScriptGenerator::create(list);

  std::ostringstream out;
  g->generate_ffmpeg_script(out);

  std::string expected =
    "delogo=enable='gte(n,49)':x=10:y=11:w=12:h=13";
  BOOST_CHECK_EQUAL(out.str(), expected);
}
