/**********************************************************************
 *<
	oFusion Scene Loader CE (see License.txt)

	FILE: ogreAnimationCompiler.h

	DESCRIPTION: OE_XMLScene callbacks interface	

	IMPLEMENTED BY: Andres Carrera

	HISTORY:

		Original implementation by Lasse Tassing (Channex)
		2005 ITE ApS, Lasse Tassing

 *>	Copyright (c) 2006, All Rights Reserved.
 **********************************************************************/


#ifndef __XML_SCENE_CALLBACKS__
#define __XML_SCENE_CALLBACKS__

#include <deque>

typedef std::deque<Ogre::SceneNode*> NodeList;

class TiXmlElement;
namespace Ogre { class Entity; class Light; class SimpleSpline; class StaticGeometry; }


// Callback interface. Use this if you want to set special properties on the
// objects during creation (and/or read custom attribute values).
class OSMSceneCallbacks
{
public:
	virtual ~OSMSceneCallbacks() {};

	// Called when the scene manager has been created
	virtual void OnSceneManagerCreate(Ogre::SceneManager *pManager, TiXmlElement* pNodeDesc) {};

	// Called when a node has been created
	virtual void OnNodeCreate(Ogre::SceneNode *pNode, TiXmlElement* pNodeDesc) {};

	// Called when an entity has been created
	virtual void OnEntityCreate(Ogre::Entity *pEntity, TiXmlElement* pEntityDesc) {};

	// Called when a camera has been created
	virtual void OnCameraCreate(Ogre::Camera *pCamera, TiXmlElement* pCameraDesc) {};

	// Called when a light has been created
	virtual void OnLightCreate(Ogre::Light *pLight, TiXmlElement* pLightDesc) {};

	// Called when a helper has been created
	virtual void OnHelperCreated(Ogre::SceneNode* pHelper, TiXmlElement* pHelperDesc) {};

	// Called when a shape has been loaded	
	virtual void OnShapeLoaded(const Ogre::SimpleSpline& spline) {
		// The client should copy and store the "spline" contents in order to use it
		// Dont store a pointer to it, the "spline" object is destroyed at function return
	};

	// Called when a static geometry has been created
	virtual bool OnStaticGeometryCreated(Ogre::StaticGeometry* pStatic, const NodeList& nodeList)
	{
		// Return true to keep the added nodes (will be hidden)
		// Return false to delete the added nodes from the scene manager

		return false;
	}
};

#endif
