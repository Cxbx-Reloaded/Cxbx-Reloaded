// ******************************************************************
// *
// *  This file is part of the Cxbx project.
// *
// *  Cxbx and Cxbe are free software; you can redistribute them
// *  and/or modify them under the terms of the GNU General Public
// *  License as published by the Free Software Foundation; either
// *  version 2 of the license, or (at your option) any later version.
// *
// *  This program is distributed in the hope that it will be useful,
// *  but WITHOUT ANY WARRANTY; without even the implied warranty of
// *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// *  GNU General Public License for more details.
// *
// *  You should have recieved a copy of the GNU General Public License
// *  along with this program; see the file COPYING.
// *  If not, write to the Free Software Foundation, Inc.,
// *  59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
// *
// *  (c) 2019      RadWolfie
// *
// *  All rights reserved
// *
// ******************************************************************

#include "CxbxUtil.h" // for StripQuotes
#include "cliConverter.hpp"
#include "cliConfig.hpp"

typedef std::unordered_map<std::string, std::string> unordered_map_strings;

constexpr char str_quote[] = "\"";
constexpr char str_quote_space[] = "\" ";
constexpr char str_space_quote[] = " \"";
constexpr char str_space[] = " ";
constexpr char str_slash_forward[] = "/";

std::string cliMapPairsToString(std::unordered_map<std::string, std::string>& map_pairs_out)
{
    std::string to_string;

    unordered_map_strings::iterator i = map_pairs_out.begin();

    // If map pairs are empty, return empty string.
    // Or "exec" is not in first iterator by requirement, then return empty string.
    if (i == map_pairs_out.end() || i->first.compare(cli_config::exec)) {
        return to_string;
    }

    to_string += str_quote + i->second + str_quote_space;

    i++;

    for (i; i != map_pairs_out.end();) {
        // If argument 1 was input, ignore it since key is reserved from user first input.
        if (!i->first.compare(cli_config::arg1)) {
            i++;
            continue;
        }
        // If argument has space inside, return as empty.
        size_t found = i->first.find(str_space);
        if (found != std::string::npos) {
            return std::string();
        }
        // If argument has quote inside, return as empty.
        found = i->first.find(str_quote);
        if (found != std::string::npos) {
            return std::string();
        }
        to_string += str_slash_forward + i->first;

        if (i->second.length() != 0) {
            found = i->second.find(str_space);
            // If found space inside, then escape with quote.
            if (found != std::string::npos) {
                // If argument has quote inside, return as empty.
                found = i->second.find(str_quote);
                if (found != std::string::npos) {
                    return std::string();
                }

                to_string += str_space_quote + i->second + str_quote;
            }
            else {
                to_string += str_space + i->second;
            }
        }

        // If there are more, then add space
        i++;
        if (i != map_pairs_out.end()) {
            to_string += str_space;
        }
        else {
            break;
        }
    }

    return to_string;
}

std::unordered_map<std::string, std::string> cliToMapPairs(char** argv, int argc)
{
    unordered_map_strings map_pairs;

    // Always set first since first argument is the path to executable file.
    map_pairs[cli_config::exec] = argv[0];

    for (int i = 1; i < argc; i++) {
        // Check for forward slash to trigger pair bind.
        std::string first = StripQuotes(argv[i]);
        if (first.at(0) == str_slash_forward[0]) {
            first = first.substr(1);
            // Do not allow overwrite argv[0].
            if (!first.compare(cli_config::exec)) {
                continue;
            }
            // And do not allow overwrite argv[1].
            if (!first.compare(cli_config::arg1)) {
                continue;
            }

            // If the count is at maximum, then simply do empty pair.
            if (i + 1 == argc) {
                map_pairs[first] = "";
                continue;
            }
            // Check for forward slash to bind pair.
            else if (i + 1 < argc) {
                std::string second = StripQuotes(argv[i + 1]);
                // If next arg has a slash, then do a empty pair.
                if (second.at(0) == str_slash_forward[0]) {
                    map_pairs[first] = "";
                }
                // Otherwise, do the input pair.
                else {
                    map_pairs[first] = second;
                    i++;
                }
                continue;
            }
        }
        // Check if 1st argument exist then allow forward it.
        else if (i == 1) {
            map_pairs[cli_config::arg1] = first;
            continue;
        }
        // Otherwise, let's mark as invalid input
        return unordered_map_strings();
    }
    return map_pairs;
}
