/**********************************************************************
 *<
	oFusion Scene Loader CE (see License.txt)

	FILE: ogreAnimationCompiler.h

	DESCRIPTION: OE_XMLScene reader (oe_scene) include header	

	IMPLEMENTED BY: Andres Carrera

	HISTORY: 

		Original implementation by Lasse Tassing (Channex)
		2005 ITE ApS, Lasse Tassing

 *>	Copyright (c) 2006, All Rights Reserved.
 **********************************************************************/


#ifndef __OGRE_XML_SCENE__
#define __OGRE_XML_SCENE__


#include <Ogre/Ogre.h>
#include <Ogre/OgreSharedPtr.h>
#include "../tinyxml/tinyxml.h"

#include "IOSMSceneCallbacks.h"
#include "OSMAnimSerializer.h"


class TiXmlDocumentPtr : public Ogre::SharedPtr<TiXmlDocument> {
public:
	TiXmlDocumentPtr() : Ogre::SharedPtr<TiXmlDocument>() {}	
	explicit TiXmlDocumentPtr(TiXmlDocument* rep) : Ogre::SharedPtr<TiXmlDocument>(rep) {}	
};


class OSMScene
{
public:

	typedef std::vector<Ogre::Camera*>	CameraList;
	typedef std::vector<Ogre::Light*>	LightList;
	typedef std::vector<Ogre::Entity*>	EntityList;

	OSMScene(Ogre::SceneManager* pSceneMgr = 0, Ogre::RenderWindow* win = 0);
	~OSMScene(void);

	// Initialise
	bool	initialise(const char *pszXMLFile, OSMSceneCallbacks* pCallbacks = NULL);

	// Declare all resources used in the scene
	void	declareResources(void);

	// Create scene, optionally attaching it to a parent node
	bool	createScene(Ogre::SceneNode *pParent = NULL);

	// Get list of cameras in this scene
	CameraList&	getCameraList(void);

	// Get list of lights in this scene
	LightList&	getLightList(void);

	// Get list of lights in this scene
	EntityList&	getEntityList(void);

	// Get created scene manager
	Ogre::SceneManager* getSceneManager(void) { return mSceneMgr; }

private:
	// Create node from information
	Ogre::SceneNode* createNode(TiXmlElement* pElem, Ogre::SceneNode* pSceneRoot);

	// Create SceneManager
	Ogre::SceneNode* createSceneManager(TiXmlElement* sceneProp, bool& bHandled);

	// Set scene properties
	void setSceneProperties(TiXmlElement* sceneProp);

	// Creation helpers		
	void	createEntities(TiXmlElement* pEntityNode, Ogre::SceneNode* pSceneRoot);
	void	createLights(TiXmlElement* pLightNode, Ogre::SceneNode* pSceneRoot);
	void	createCameras(TiXmlElement* pCameraNode, Ogre::SceneNode* pSceneRoot);
	void	createStaticGeometry(TiXmlElement* pStaticGeom);
	void	createHelpers(TiXmlElement* pLightNode, Ogre::SceneNode* pSceneRoot);
	void	createShapes(TiXmlElement* pLightNode);
	
	
	typedef std::set<unsigned int> SkeletonHandles;
	void	loadAnimations(TiXmlElement* animationsNode, SkeletonHandles& handles);	

	// Created objects
	CameraList	mCameras;
	LightList	mLights;
	EntityList	mEntities;

	// Callback interface
	OSMSceneCallbacks* mCallbacks;

	// Scene manager
	Ogre::SceneManager* mSceneMgr;
	Ogre::RenderWindow* mWindow;

	// Scene XML document
	TiXmlDocumentPtr mXMLDoc;
};

#endif
