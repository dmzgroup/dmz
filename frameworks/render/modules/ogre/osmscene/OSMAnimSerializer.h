/**********************************************************************
 *<
	oFusion Scene Loader CE (see License.txt)

	FILE: OSMAnimSerializer.h

	DESCRIPTION: External Animation serializer include header

	IMPLEMENTED BY: Andres Carrera

	HISTORY: 

		Original implementation by Lasse Tassing (Channex)
		2005 ITE ApS, Lasse Tassing

 *>	Copyright (c) 2006, All Rights Reserved.
 **********************************************************************/

#ifndef _OSMANIMSERIALIZER_H_
#define _OSMANIMSERIALIZER_H_

class OSMAnimSerializer {
public:

	OSMAnimSerializer();
	
	void addAnimation(Ogre::SkeletonPtr skel, const Ogre::String& filename);
};

#endif // _OSMANIMSERIALIZER_H_
