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
 * File:   tss.h
 * Author: Jake Shilling <shilling.jake@gmail.com>
 *
 * Created on June 20, 2018, 12:39 PM
 */

#pragma once

#include <sys/cdefs.h>
#include <stddef.h>

__BEGIN_DECLS
        
typedef struct _tss_t tss_t;
extern const size_t tss_size;
extern tss_t tss;

__END_DECLS
