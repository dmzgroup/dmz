/*
*
* This file is part of QMapControl,
* an open-source cross-platform map widget
*
* Copyright (C) 2007 - 2008 Kai Winter
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU Lesser General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU Lesser General Public License for more details.
*
* You should have received a copy of the GNU Lesser General Public License
* along with QMapControl. If not, see <http://www.gnu.org/licenses/>.
*
* Contact e-mail: kaiwinter@gmx.de
* Program URL   : http://qmapcontrol.sourceforge.net/
*
*/

#include "osmmapadapter.h"
namespace qmapcontrol
{
	OSMMapAdapter::OSMMapAdapter()
// 	: TileMapAdapter("192.168.8.1", "/img/img_cache.php/%1/%2/%3.png", 256, 0, 17)
//	: TileMapAdapter("tile.openstreetmap.org", "/%1/%2/%3.png", 256, 0, 17)
//http://tiles.cloudmade.com/APIKEY/2/256/!z!/!x!/!y!.png
      : TileMapAdapter("tile.cloudmade.com", "/b999cacc40c7586fbdd6407362411e4d/2/256/%1/%2/%3.png", 256, 0, 17)
//      : TileMapAdapter("tile.cloudmade.com", "/b999cacc40c7586fbdd6407362411e4d/2/64/%1/%2/%3.png", 64, 0, 17)
//	: TileMapAdapter("mt2.google.com", "/mt?n=404&x=%2&y=%3&zoom=%1", 256, 17, 0)
//	: TileMapAdapter("172.20.90.188", "/cgi-bin/tilecache.cgi/1.0.0/osm/%1/%2/%3.png", 256, 0, 17)
	{
	}

	OSMMapAdapter::~OSMMapAdapter()
	{
	}
}
