/*
 * Copyright (C) 2018 Jake Shilling <shilling.jake@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/*
 * File:   acpi.h
 * Author: Jake Shilling <shilling.jake@gmail.com>
 *
 * Created on July 1, 2018, 2:58 PM
 */

#pragma once

#include <sys/cdefs.h>

__BEGIN_DECLS

typedef struct rsdp_v1 rsdp_v1_t;
typedef struct rsdp_v2 rsdp_v2_t;

void acpi_init(void);

__END_DECLS
