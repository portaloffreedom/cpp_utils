/*
 * This file is part of the revolve project.
 * Copyright (c) 2021 Matteo De Carlo.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, version 3.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */
#include "Address.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <cstring>
#include <utility>
#include <stdexcept>
#include <sstream>

std::string get_ip_str(const struct sockaddr *sa);

Address::Address(std::string hostname, IPVersion ipVersion)
    : _hostname(std::move(hostname))
    , _ip_version(ipVersion)
{
    addrinfo hints;
    std::memset(&hints, 0, sizeof(hints));
    switch (_ip_version) {
        case IPv4:
            hints.ai_family = AF_INET;
            break;
        case IPv6:
            hints.ai_family = AF_INET6;
            break;
        case EITHER:
            hints.ai_family = AF_UNSPEC;
            break; // ipv4 or ipv6 does not matter
        default:
            throw std::runtime_error("UNRECOGNIZED IP VERSION!: " + std::to_string(_ip_version));
    }
    hints.ai_socktype = 0; // Any socket type
    hints.ai_flags = AI_V4MAPPED | AI_ADDRCONFIG;
    hints.ai_protocol = 0; // Any protocol
    hints.ai_canonname = nullptr;
    hints.ai_addr = nullptr;
    hints.ai_next = nullptr;
    // getaddrinfo(hostname, port(can be in name form), *hints, **result)
    int r = getaddrinfo(this->_hostname.c_str(), nullptr, &hints, &_addrinfo);
    if (r != 0) {
        std::ostringstream error;
        error << "Error in dns lookup of \"" << this->_hostname << "\": " << gai_strerror(r);
        throw std::runtime_error(error.str());
    }
}

Address::~Address() {
    // Remember to free memory, this is C stuff
    freeaddrinfo(_addrinfo);
}

std::vector<const sockaddr*> Address::get_ips() const
{
    std::vector<const sockaddr*> ips;

    // `getaddrinfo()` returns a list of address structures.
    // We try each address until we successfully connect.
    for (addrinfo *list_elem = _addrinfo; list_elem != nullptr; list_elem = list_elem->ai_next) {
        ips.push_back(list_elem->ai_addr);
    }

    return ips;
}

std::vector<std::string> Address::get_ips_str() const {
    std::vector<const struct sockaddr*> ips = get_ips();
    std::vector<std::string> ips_str;
    ips_str.reserve(ips.size());
    for (const struct sockaddr *ip: ips) {
        ips_str.emplace_back(get_ip_str(ip));
    }
    return ips_str;
}


std::string get_ip_str(const struct sockaddr *sa)
{
    char buffer[std::max(INET_ADDRSTRLEN, INET6_ADDRSTRLEN)];
    const char *r;
    switch (sa->sa_family) {
        case AF_INET:
            r = inet_ntop(AF_INET, &(((struct sockaddr_in *) sa)->sin_addr),
                          buffer, INET_ADDRSTRLEN);
            break;

        case AF_INET6:
            r = inet_ntop(AF_INET6, &(((struct sockaddr_in6 *) sa)->sin6_addr),
                          buffer, INET6_ADDRSTRLEN);
            break;

        default:
            throw std::runtime_error("Unrecognized Address family");
    }

    if (r == nullptr) {
        throw std::runtime_error("Error converting address into string! errno=" + std::to_string(errno));
    }

    return std::string(r);
}