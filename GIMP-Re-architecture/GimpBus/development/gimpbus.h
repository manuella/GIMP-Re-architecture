#ifndef __GIMPBUS_H__
#define __GIMPBUS_H__

/**
 * gimpbus.h
 *   Some basic information for using GimpBus.
 *
 *  Copyright (c) Samuel A. Rebelsky.
 *
 *  This file is part of GimpBus.
 *
 *  GimpBus is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as 
 *  published by the Free Software Foundation, either version 3 of the 
 *  License, or (at your option) any later version.
 *
 *  GimpBus is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public 
 *  License along with GimpBus .  If not, see 
 *    <http://www.gnu.org/licenses/>.
 */

/**
 * The prefixes used for GimpBus services, paths, and interfaces.
 */
#define GIMPBUS_SERVICE		"org.glimmer.gimpbus"
#define GIMPBUS_PATH		"/org/glimmer/gimpbus"
#define GIMPBUS_INTERFACE	"org.glimmer.gimpbus"

/**
 * GimpBus Administration
 */
#define GIMPBUS_ADMIN_SERVICE 	GIMPBUS_SERVICE	  ".admin"
#define GIMPBUS_ADMIN_PATH 	GIMPBUS_PATH 	  "/admin"
#define GIMPBUS_ADMIN_INTERFACE	GIMPBUS_INTERFACE ".admin"	

/**
 * Information on the pdbinfo service.
 */
#define GIMPBUS_PDBINFO_SERVICE 	GIMPBUS_SERVICE	  ".pdbinfo"
#define GIMPBUS_PDBINFO_PATH 		GIMPBUS_PATH 	  "/pdbinfo"
#define GIMPBUS_PDBINFO_INTERFACE	GIMPBUS_INTERFACE ".pdbinfo"	

/**
 * Information on the pdb service.
 */
#define GIMPBUS_PDB_SERVICE 		GIMPBUS_SERVICE	  ".pdb"
#define GIMPBUS_PDB_PATH 		GIMPBUS_PATH 	  "/pdb"
#define GIMPBUS_PDB_INTERFACE		GIMPBUS_INTERFACE ".pdb"	

/**
 * Information on the libgimp service.
 */
#define GIMPBUS_LIBGIMP_SERVICE 	GIMPBUS_SERVICE	  ".libgimp"
#define GIMPBUS_LIBGIMP_PATH 		GIMPBUS_PATH 	  "/libgimp"
#define GIMPBUS_LIBGIMP_INTERFACE	GIMPBUS_INTERFACE ".libgimp"	

#endif // __GIMPBUS_H__
