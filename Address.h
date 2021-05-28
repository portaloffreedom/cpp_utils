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
#pragma once

#include <string>
#include <vector>
#include <netdb.h>

class Address {
public:
    enum IPVersion { IPv4, IPv6, EITHER };

private:
    std::string _hostname;
    IPVersion _ip_version;
    addrinfo *_addrinfo;

public:
    explicit Address(std::string hostname, IPVersion ipVersion = EITHER);

    ~Address();

    [[nodiscard]] std::vector<const sockaddr*> get_ips() const;
    [[nodiscard]] std::vector<std::string> get_ips_str() const;
};
