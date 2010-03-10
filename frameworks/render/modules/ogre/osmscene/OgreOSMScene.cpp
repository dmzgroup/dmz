/**********************************************************************
 *<
	oFusion Scene Loader CE (see License.txt)

	FILE: OgreOSMScene.cpp

	DESCRIPTION: Contains the OE_XMLScene reader (oe_scene)	

	IMPLEMENTED BY: Andres Carrera

	HISTORY: 

		Original implementation by Lasse Tassing (Channex)
		2005 ITE ApS, Lasse Tassing

 *>	Copyright (c) 2006, All Rights Reserved.
 **********************************************************************/

#include "OgreOSMScene.h"

using namespace Ogre;

enum {
	SCENE_SKYPLANE = 1,
	SCENE_SKYBOX,
	SCENE_SKYDOME,
};

#define ENABLE_LOGMANAGER LogManager::getSingleton().setLogDetail(LL_NORMAL);
#define DISABLE_LOGMANAGER LogManager::getSingleton().setLogDetail(static_cast<LoggingLevel>(0));


OSMScene::OSMScene(SceneManager* pSceneMgr, RenderWindow* win)
{
	mSceneMgr = pSceneMgr;

	if(win)	
		mWindow = win;		
	else
		mWindow = Ogre::Root::getSingleton().getAutoCreatedWindow();
	
	mCallbacks = 0;
}

OSMScene::~OSMScene(void)
{	
	
}

// Init overloads - use either of them
bool OSMScene::initialise(const char* pszXMLFile, OSMSceneCallbacks* pCallbacks)
{
	// Hook up callback interface
	mCallbacks = pCallbacks;

	LogManager::getSingleton().logMessage("********************************");	
	LogManager::getSingleton().logMessage("**     oScene Loader Lib      **");
	LogManager::getSingleton().logMessage("********************************");
	
	String msg("oSceneLoader: Loading '");
	msg += pszXMLFile;
	msg += "' file";
	LogManager::getSingleton().logMessage(msg);	

	// Create new XML document
	mXMLDoc = TiXmlDocumentPtr(new TiXmlDocument());

	DataStreamPtr pStream = ResourceGroupManager::getSingleton().openResource(pszXMLFile);
	if(!pStream->size())
	{
		mXMLDoc.setNull();

		OGRE_EXCEPT(Exception::ERR_ITEM_NOT_FOUND,
					"oSceneLoader: Empty scene file", 
					"OSMScene::initialise");
		
	}			
				
	size_t iSize = pStream->size();
	char *pBuf = new char[iSize+1];
	memset(pBuf, 0, iSize+1);
	pStream->read(pBuf, iSize);
	pStream.setNull();
	mXMLDoc->Parse(pBuf);
	delete[] pBuf;
	
	// check for errors
	if(mXMLDoc->Error())
	{
		mXMLDoc.setNull();

		String errDesc = "oSceneLoader: Failed to load scene file, ";
		msg += mXMLDoc->ErrorDesc();

		OGRE_EXCEPT(Exception::ERR_INVALIDPARAMS,
					errDesc.c_str(), 
					"OSMScene::initialise");		
	}
	
	TiXmlElement* locations = mXMLDoc->RootElement()->FirstChildElement("locations");

	if(locations)
	{

		FileInfoListPtr fileInfo = ResourceGroupManager::getSingleton().findResourceFileInfo(ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, pszXMLFile);

		String strPath = (*fileInfo)[0].archive->getName();

		strPath = StringUtil::standardisePath(strPath);		

		for (TiXmlElement* pLocationElem = locations->FirstChildElement();
				pLocationElem != 0; pLocationElem = pLocationElem->NextSiblingElement())
		{
			// Ogre could cast an exception, in which case we just try to 
			// continue reading the other location paths
			try 
			{	
				const char *pszName = pLocationElem->Attribute("path");
				String strDir = strPath + pszName;

				ResourceGroupManager::getSingleton().addResourceLocation(strDir, "FileSystem");

				
			}
			catch(...) {
			}
		}
	}	

	

	return true;
}

// Declare all resources used in the scene
void OSMScene::declareResources(void)
{
	if(!mXMLDoc.isNull())
	{

		TiXmlElement* rootElem = mXMLDoc->RootElement();

		try 
		{
			// Get mesh filename from entities
			TiXmlElement *pMeshNode = rootElem->FirstChildElement("entities");
			if(pMeshNode)
			{
				// Iterate all meshes, creating them. 
				for (TiXmlElement* pMeshElem = pMeshNode->FirstChildElement();
				pMeshElem != 0; pMeshElem = pMeshElem->NextSiblingElement())
				{	
					// Declare mesh resource
					const char *pszFileName = pMeshElem->Attribute("filename");	
					ResourceGroupManager::getSingleton().declareResource(pszFileName, "Mesh");
				}
			}
		} catch(...)
		{
		}

	}
}

// Create scene, optionally attaching it to a parent node
bool OSMScene::createScene(Ogre::SceneNode* pParent)
{
	if(!mXMLDoc.isNull())
	{
		String msg("oSceneLoader: Creating scene on '");
		msg += pParent ? pParent->getName() : "Root";
		msg += "' node";
		LogManager::getSingleton().logMessage(msg);

		bool bHandled = false;

		TiXmlElement* rootElem = mXMLDoc->RootElement();		

		if(mSceneMgr == NULL)
		{
			if(rootElem->FirstChildElement("sceneManager"))
				pParent = createSceneManager(rootElem, bHandled);
			else
				mSceneMgr = Root::getSingleton().createSceneManager(ST_GENERIC);
		}
		
		if(pParent==NULL)
			pParent=mSceneMgr->getRootSceneNode()->createChildSceneNode();

		assert(pParent);

		TiXmlElement* list;

		try {
			// Entities
			list = rootElem->FirstChildElement("entities");
			if(list)
				createEntities(list, pParent);
		} catch(...)
		{
			LogManager::getSingleton().logMessage("Error while loading entities");
			
			OGRE_EXCEPT(Exception::ERR_RT_ASSERTION_FAILED,
				"oSceneLoader: Error while loading entities",
				"OSMScene::createScene");
		}

		try {
			// lights
			list = rootElem->FirstChildElement("lights");
			if(list)
				createLights(list, pParent);
		} catch(...)
		{
			LogManager::getSingleton().logMessage("Error while loading lights");

			OGRE_EXCEPT(Exception::ERR_RT_ASSERTION_FAILED,
				"oSceneLoader: Error while loading lights",
				"OSMScene::createScene");
		}

		if(!bHandled) {
			try {
				// cameras
				list = rootElem->FirstChildElement("cameras");
				if(list)
					createCameras(list, pParent);
			} catch(...)
			{
				LogManager::getSingleton().logMessage("Error while loading cameras");

				OGRE_EXCEPT(Exception::ERR_RT_ASSERTION_FAILED,
				"oSceneLoader: Error while loading cameras",
				"OSMScene::createScene");
			}
		}

		/*
		try {
			// helpers
			list = rootElem->FirstChildElement("helpers");
			if(list)
				createHelpers(list, pParent);
		} catch(...)
		{
			LogManager::getSingleton().logMessage("Error while loading helpers");

			OGRE_EXCEPT(Exception::ERR_RT_ASSERTION_FAILED,
				"oSceneLoader: Error while loading helpers",
				"OSMScene::createScene");
		}

		try {
			// shapes
			list = rootElem->FirstChildElement("shapes");
			if(list)
				createShapes(list);
		} catch(...)
		{
			LogManager::getSingleton().logMessage("Error while loading shapes");

			OGRE_EXCEPT(Exception::ERR_RT_ASSERTION_FAILED,
				"oSceneLoader: Error while loading shapes",
				"OSMScene::createScene");
		}		

		try {
			// External skeletal animation files
			list = rootElem->FirstChildElement("skeletal_animations");
			if(list)
			{
				SkeletonHandles skelHandles;
				loadAnimations(list, skelHandles);				
			}
		}catch(...)
		{
			LogManager::getSingleton().logMessage("Error while loading external animations");

			OGRE_EXCEPT(Exception::ERR_RT_ASSERTION_FAILED,
				"oSceneLoader: Error while loading external animations",
				"OSMScene::createScene");
		}
		*/

		// Set scene properties
		setSceneProperties(rootElem);

		// Static Geometry
		list = rootElem->FirstChildElement("staticGeometry");
		if(list)
			createStaticGeometry(list);

		LogManager::getSingleton().logMessage("********************************");
		LogManager::getSingleton().logMessage("** oSceneLoader: Scene loaded **");
		LogManager::getSingleton().logMessage("********************************");		

		return true;
	}

	return false;

	
}

// Get list of cameras in this scene
OSMScene::CameraList& OSMScene::getCameraList(void)
{
	return mCameras;
}

// Get list of lights in this scene
OSMScene::LightList& OSMScene::getLightList(void)
{
	return mLights;
}

// Get list of lights in this scene
OSMScene::EntityList& OSMScene::getEntityList(void)
{
	return mEntities;
}

Ogre::SceneNode* OSMScene::createNode(TiXmlElement* pElem, Ogre::SceneNode* pSceneRoot)
{
	SceneNode *pNode=0;

	// Try to find the parent node
	const char *pszName	= pElem->Attribute("name");
	if(pszName == NULL) return NULL;

	// Check if this node has a parent
	const char *pszParent = pElem->Attribute("parent");						
	if(pszParent == NULL)
	{	
		// Check if the scene node has already been created by a child
		DISABLE_LOGMANAGER

		try 
		{ 
			pNode = mSceneMgr->getSceneNode(pszName);
		} catch(...)
		{
			pNode = pSceneRoot->createChildSceneNode(pszName);
		}
		
		ENABLE_LOGMANAGER

	} else
	{
		SceneNode *pParent=0;
		DISABLE_LOGMANAGER

		try 
		{
			// Try to find parent scenenode
			pParent=mSceneMgr->getSceneNode(pszParent);			
		} catch(...)
		{	
			// We try to create the parent node as child of root node.
			// Later when the parent (hopefully) is created, we can adjust it,
			// if it is child of another node.
			pParent=pSceneRoot->createChildSceneNode(pszParent);
		}

		
		try 
		{ 
			// Check if the scene node has already been created by a child
			// In this case we would have to change the parent.
			pNode = mSceneMgr->getSceneNode(pszName);

			// Get old parent (probably scene root)
			SceneNode *pOldParent=pNode->getParentSceneNode();

			// Remove this node
			pOldParent->removeChild(pNode);

			// Insert us as child on the "real" parent
			pParent->addChild(pNode);
		} catch(...)
		{
			pNode = pParent->createChildSceneNode(pszName);
		}

		ENABLE_LOGMANAGER
	}

	// Position
	TiXmlElement* posElem = pElem->FirstChildElement("position");
    if(posElem)
    {
		Vector3 pos;			
		pos.x = StringConverter::parseReal(posElem->Attribute("x"));
		pos.y = StringConverter::parseReal(posElem->Attribute("y"));
		pos.z = StringConverter::parseReal(posElem->Attribute("z"));
		pNode->setPosition(pos);		
    }

	// Rotation
	TiXmlElement* rotElem = pElem->FirstChildElement("rotation");
    if(rotElem)
    {				
		pNode->setOrientation(
			StringConverter::parseReal(rotElem->Attribute("w")),
			StringConverter::parseReal(rotElem->Attribute("x")),
			StringConverter::parseReal(rotElem->Attribute("y")),
			StringConverter::parseReal(rotElem->Attribute("z")));

    }
	
	// Scale
	TiXmlElement* scaleElem = pElem->FirstChildElement("scale");
    if(scaleElem)
    {
		Vector3 scale;
		scale.x = StringConverter::parseReal(scaleElem->Attribute("x"));
		scale.y = StringConverter::parseReal(scaleElem->Attribute("y"));
		scale.z = StringConverter::parseReal(scaleElem->Attribute("z"));
		pNode->setScale(scale);
    }

	// Notify
	if(mCallbacks)
		mCallbacks->OnNodeCreate(pNode, pElem);

	// Animation
	TiXmlElement* animList = pElem->FirstChildElement("animations");
    if(animList)
	{
		// 
		for (TiXmlElement* animElem = animList->FirstChildElement();
            animElem != 0; animElem = animElem->NextSiblingElement())
		{		
			// Get name of animation
			const char *pszName = animElem->Attribute("name");

			Animation *pAnim = 0;
			DISABLE_LOGMANAGER
			try 
			{
				pAnim=mSceneMgr->getAnimation(pszName);
			} catch(...)
			{
			}
			ENABLE_LOGMANAGER

			// If this animation has not been created yet, we create it
			if(pAnim == 0)
			{			
				float fLength = StringConverter::parseReal(animElem->Attribute("length"));
				pAnim = mSceneMgr->createAnimation(pszName, fLength);
				pAnim->setInterpolationMode(Animation::IM_LINEAR);				
			}		

			// Create animation track for this node
			NodeAnimationTrack *pTrack = pAnim->createNodeTrack(pAnim->getNumNodeTracks()+1, pNode);

			// Iterate all keyframes for this node
			for (TiXmlElement* pKeyframeElem = animElem->FirstChildElement();
					pKeyframeElem != 0; pKeyframeElem = pKeyframeElem->NextSiblingElement())
			{
				float fTime=StringConverter::parseReal(pKeyframeElem->Attribute("time"));
				TransformKeyFrame *pKeyFrame = pTrack->createNodeKeyFrame(fTime);

				// Position
				TiXmlElement* posElem = pKeyframeElem->FirstChildElement("position");
				if(posElem)
				{
					Vector3 trans;			
					trans.x = StringConverter::parseReal(posElem->Attribute("x"));
					trans.y = StringConverter::parseReal(posElem->Attribute("y"));
					trans.z = StringConverter::parseReal(posElem->Attribute("z"));				
					pKeyFrame->setTranslate(trans);
				}

				// Rotation
				TiXmlElement* rotElem = pKeyframeElem->FirstChildElement("rotation");
				if(rotElem)
				{		
					Quaternion qRot;
					qRot.x = StringConverter::parseReal(rotElem->Attribute("x"));
					qRot.y = StringConverter::parseReal(rotElem->Attribute("y"));
					qRot.z = StringConverter::parseReal(rotElem->Attribute("z"));		
					qRot.w = StringConverter::parseReal(rotElem->Attribute("w"));		
					pKeyFrame->setRotation(qRot);
				}
				
				// Scale
				TiXmlElement* scaleElem = pKeyframeElem->FirstChildElement("scale");
				if(scaleElem)
				{
					Vector3 scale;
					scale.x = StringConverter::parseReal(scaleElem->Attribute("x"));
					scale.y = StringConverter::parseReal(scaleElem->Attribute("y"));
					scale.z = StringConverter::parseReal(scaleElem->Attribute("z"));				
					pKeyFrame->setScale(scale);
				}
			}		
		}
	}

	return pNode;
}

// Create SceneManager
Ogre::SceneNode* OSMScene::createSceneManager(TiXmlElement* sceneProp, bool& bHandled)
{
	assert(sceneProp);

	// Scene manager
	TiXmlElement* sceneMgrElem = sceneProp->FirstChildElement("sceneManager");
	int type = Ogre::StringConverter::parseInt(sceneMgrElem->Attribute("type"));
	Ogre::SceneType sceneType = static_cast<Ogre::SceneType>(1 << (type - 1));
	mSceneMgr = Ogre::Root::getSingleton().createSceneManager(sceneType);
	assert(mSceneMgr);	

	SceneNode* pSceneRoot = mSceneMgr->getRootSceneNode()->createChildSceneNode();
	assert(pSceneRoot);

	// Scene shadows
	TiXmlElement* shadowsElem = sceneProp->FirstChildElement("shadowTechnique");
	if(shadowsElem)
	{
		int type = StringConverter::parseInt(shadowsElem->Attribute("type"));
		Ogre::ShadowTechnique shadowType = static_cast<Ogre::ShadowTechnique>(type);

		mSceneMgr->setShadowTechnique(shadowType);

		int tex_size = StringConverter::parseInt(shadowsElem->Attribute("tex_size"));
		int tex_count = StringConverter::parseInt(shadowsElem->Attribute("tex_count"));

		mSceneMgr->setShadowTextureSettings(tex_size, tex_count);

		// Shadow Color
		TiXmlElement* colorElem = shadowsElem->FirstChildElement("color");
		if(colorElem)
		{	
			ColourValue color;
			color.r = StringConverter::parseReal(colorElem->Attribute("r"));
			color.g = StringConverter::parseReal(colorElem->Attribute("g"));
			color.b = StringConverter::parseReal(colorElem->Attribute("b"));
			
			mSceneMgr->setShadowColour(color);
		}
	}

	// Scene fog
	TiXmlElement* fogElem = sceneProp->FirstChildElement("fogMode");
	if(fogElem)
	{
		int type = StringConverter::parseInt(fogElem->Attribute("type"));
		FogMode mode = static_cast<FogMode>(type);
		
		float density = StringConverter::parseReal(fogElem->Attribute("density"));
		float linearStart = StringConverter::parseReal(fogElem->Attribute("linearStart"));
		float linearEnd = StringConverter::parseReal(fogElem->Attribute("linearEnd"));

		Ogre::ColourValue color;

		// Fog Color
		TiXmlElement* colorElem = fogElem->FirstChildElement("color");
		if(colorElem)
		{
			color.r = StringConverter::parseReal(colorElem->Attribute("r"));
			color.g = StringConverter::parseReal(colorElem->Attribute("g"));
			color.b = StringConverter::parseReal(colorElem->Attribute("b"));			
		}

		mSceneMgr->setFog(mode, color, density, linearStart, linearEnd);
	}
	
	// Notify
	if(mCallbacks)
		mCallbacks->OnSceneManagerCreate(mSceneMgr, sceneMgrElem);

	// World Geometry
	const char* worldGeometry = sceneMgrElem->Attribute("worldGeometry");
	if(worldGeometry != NULL)
	{
		// Some scene managers need cameras created before the world geometry
		try {
			// cameras
			TiXmlElement* list = sceneProp->FirstChildElement("cameras");
			if(list)
				createCameras(list, pSceneRoot);
			
			if(sceneType == ST_EXTERIOR_CLOSE)
			{
				
				if(mCameras.empty()) 
				{

					OGRE_EXCEPT(Exception::ERR_RT_ASSERTION_FAILED,
					"oSceneLoader: Scene does not contain cameras, \
					TerrainSceneManager needs a camera to initialize",
					"OSMScene::createSceneManager");
				}
				
				Camera* primaryCamera = mCameras[0];

				mWindow->getViewport(0)->setCamera(primaryCamera);

				mSceneMgr->setOption("PrimaryCamera", primaryCamera);
			}

			// cameras created, the main scene loader method should not re-parse the list
			bHandled = true;
		} catch(...)
		{
		}

		mSceneMgr->setWorldGeometry(worldGeometry);
	}
	
	return pSceneRoot;
}

// Set Scene Properties
void OSMScene::setSceneProperties(TiXmlElement* sceneProp)
{
	// Ambient light Color
	TiXmlElement* colorElem = sceneProp->FirstChildElement("lightColor");
	if(colorElem)
	{	
		ColourValue color;
		color.r = StringConverter::parseReal(colorElem->Attribute("r"));
		color.g = StringConverter::parseReal(colorElem->Attribute("g"));
		color.b = StringConverter::parseReal(colorElem->Attribute("b"));

		mSceneMgr->setAmbientLight(color);
	}

	// Background Color
	colorElem = sceneProp->FirstChildElement("bkgcolor");
	if(colorElem && mWindow)
	{	
		int numViewports = mWindow->getNumViewports();
		if(numViewports)
		{
			Ogre::ColourValue color;
			color.r = StringConverter::parseReal(colorElem->Attribute("r"));
			color.g = StringConverter::parseReal(colorElem->Attribute("g"));
			color.b = StringConverter::parseReal(colorElem->Attribute("b"));			
		
			for(int i=0; i<numViewports; ++i)
				mWindow->getViewport(i)->setBackgroundColour(color);
		}
	}

	// Scene sky
	TiXmlElement* skyElem = sceneProp->FirstChildElement("skyTechnique");
	if(skyElem)
	{
		int type = StringConverter::parseInt(skyElem->Attribute("type"));
		String materialName = skyElem->Attribute("material");
		
		if(materialName != " ") {
			bool drawFirst = StringConverter::parseBool(skyElem->Attribute("drawFirst"));
			float tiling = StringConverter::parseReal(skyElem->Attribute("tiling"));
			float scale = StringConverter::parseReal(skyElem->Attribute("scale"));
			float dist = StringConverter::parseReal(skyElem->Attribute("dist"));
			float bow = StringConverter::parseReal(skyElem->Attribute("bow"));
			int xSegments = StringConverter::parseInt(skyElem->Attribute("xSegments"));
			int ySegments = StringConverter::parseInt(skyElem->Attribute("ySegments"));
			Ogre::Quaternion quat(Quaternion::IDENTITY);
			Ogre::Plane plane;
			plane.d = dist;
			plane.normal = -(Vector3::UNIT_Y);

			switch(type) {

			case SCENE_SKYPLANE:

				mSceneMgr->setSkyPlane(true, plane, materialName, scale, 
									   tiling, drawFirst, bow, xSegments, ySegments);

				mSceneMgr->setSkyBox(false, "");
				mSceneMgr->setSkyDome(false, "");

				break;

			case SCENE_SKYBOX:

				mSceneMgr->setSkyBox(true, materialName, dist, drawFirst, quat);
				mSceneMgr->setSkyPlane(false, plane, "");
				mSceneMgr->setSkyDome(false, "");

				break;

			case SCENE_SKYDOME:

				mSceneMgr->setSkyDome(true, materialName, bow, tiling, dist,
									  drawFirst, quat, xSegments, ySegments);

				mSceneMgr->setSkyPlane(false, plane, "");
				mSceneMgr->setSkyBox(false, "");

				break;
				
			}
		}
	}
}

// Create all entities in scene
void OSMScene::createEntities(TiXmlElement* pEntityNode, Ogre::SceneNode* pSceneRoot)
{
	// Iterate all meshes, creating them. 
	for (TiXmlElement* pMeshElem = pEntityNode->FirstChildElement();
            pMeshElem != 0; pMeshElem = pMeshElem->NextSiblingElement())
	{
		// Ogre could cast an exception, in which case we just try to 
		// continue reading the other meshes
		try 
		{
			const char *pszName = pMeshElem->Attribute("name");
			const char *pszFileName = pMeshElem->Attribute("filename");

			// try to create the mesh
			Entity *pEntity = mSceneMgr->createEntity(pszName, pszFileName);			
			if(pEntity==0) continue;

			// Check if the object should cast shadows
			const char *pszCastShadows=pMeshElem->Attribute("CastShadows");
			if(pszCastShadows && stricmp(pszCastShadows, "no")==0)
				pEntity->setCastShadows(false);	
			else
				pEntity->setCastShadows(true);	

			// Create node with full information
			SceneNode *pObjNode=createNode(pMeshElem, pSceneRoot);

			// Attach the mesh entity to node
			pObjNode->attachObject(pEntity);

			// Notify
			if(mCallbacks)
				mCallbacks->OnEntityCreate(pEntity, pMeshElem);

			// Add to entity list
			mEntities.push_back(pEntity);
		} catch(...)
		{
			continue;
		}			
	}	
}

// Create all Lights in scene
void OSMScene::createLights(TiXmlElement* pLightNode, Ogre::SceneNode* pSceneRoot)
{
	// Iterate all Lights, creating them. We do not attach them yet, since
	// we need to make sure all potential parent entities have been created.
	for (TiXmlElement* pLightElem = pLightNode->FirstChildElement();
            pLightElem != 0; pLightElem = pLightElem->NextSiblingElement())
	{
		// Ogre could cast an exception, in which case we just try to 
		// continue reading the other Lights
		try 
		{
			const char *pszName = pLightElem->Attribute("name");

			Light *pLight = mSceneMgr->createLight(pszName);
			if(pLight==0) continue;

			// Figure out which type of light we are using
			const char *pszType = pLightElem->Attribute("type");
			if(stricmp(pszType, "omni")==0)
			{
				pLight->setType(Light::LT_POINT);
			} else if(stricmp(pszType, "spot")==0)
			{	
				pLight->setType(Light::LT_SPOTLIGHT);
				pLight->setSpotlightRange(
						Degree(StringConverter::parseReal(pLightElem->Attribute("hotspot"))),
						Degree(StringConverter::parseReal(pLightElem->Attribute("falloff"))));
				pLight->setDirection(0,0,-1);

			}  else if(stricmp(pszType, "directional")==0)
			{
				pLight->setType(Light::LT_DIRECTIONAL);
			}

			// Check if the light should be on
			const char *pszOn = pLightElem->Attribute("on");
			if(pszOn!=0 && stricmp(pszOn, "true")==0)
				pLight->setVisible(true);
			else
				pLight->setVisible(false);

			// Check if the object should cast shadows
			const char *pszCastShadows = pLightElem->Attribute("CastShadows");
			if(pszCastShadows && stricmp(pszCastShadows, "no")==0)
				pLight->setCastShadows(false);	
			else
				pLight->setCastShadows(true);	

			// Diffuse Color
			TiXmlElement* colorElem = pLightElem->FirstChildElement("color");
			if(colorElem)
			{					
				pLight->setDiffuseColour(
					StringConverter::parseReal(colorElem->Attribute("r")),
					StringConverter::parseReal(colorElem->Attribute("g")),
					StringConverter::parseReal(colorElem->Attribute("b")));
			}

			// Specular Color
			TiXmlElement* specularElem = pLightElem->FirstChildElement("specular");
			if(specularElem)
			{						
				pLight->setSpecularColour(
					StringConverter::parseReal(specularElem->Attribute("r")),
					StringConverter::parseReal(specularElem->Attribute("g")),
					StringConverter::parseReal(specularElem->Attribute("b")));
			}

			// Attenuation
			TiXmlElement* attenElem = pLightElem->FirstChildElement("attenuation");
			if(attenElem) {
				pLight->setAttenuation(
					StringConverter::parseReal(attenElem->Attribute("range")),
					StringConverter::parseReal(attenElem->Attribute("constant")),
					StringConverter::parseReal(attenElem->Attribute("linear")),
					StringConverter::parseReal(attenElem->Attribute("quadratic")));
			}
			
			// Create node with full information
			SceneNode *pLightNode=createNode(pLightElem, pSceneRoot);

			// Attach the Light entity to node
			pLightNode->attachObject(pLight);

			// Target
			TiXmlElement* targetElem=pLightElem->FirstChildElement("target");
			if(targetElem)
			{	
				// Create node with full information
				SceneNode *pTargetNode=createNode(targetElem, pSceneRoot);
				pLightNode->setAutoTracking(true, pTargetNode);
			}		

			// Notify
			if(mCallbacks)
				mCallbacks->OnLightCreate(pLight, pLightElem);

			// Add to light list
			mLights.push_back(pLight);
		} catch(...)
		{
			continue;
		}			
	}	
}

// Create all Cameras in scene
void OSMScene::createCameras(TiXmlElement* pCameraNode, Ogre::SceneNode* pSceneRoot)
{
	// Iterate all Cameras, creating them. We do not attach them yet, since
	// we need to make sure all potential parent entities have been created.
	for (TiXmlElement* pCameraElem = pCameraNode->FirstChildElement();
            pCameraElem != 0; pCameraElem = pCameraElem->NextSiblingElement())
	{
		// Ogre could cast an exception, in which case we just try to 
		// continue reading the other Cameras
		try 
		{
			const char *pszName = pCameraElem->Attribute("name");

			// Create camera
			Camera *pCamera=mSceneMgr->createCamera(pszName);
			if(pCamera==0) continue;

			// Set Field of View on camera
			pCamera->setFOVy(Radian(StringConverter::parseReal(pCameraElem->Attribute("FOV"))));			
			pCamera->setNearClipDistance(5);			

			// Create node with full information
			SceneNode *pCameraNode=createNode(pCameraElem, pSceneRoot);

			// Attach the Camera entity to node
			pCameraNode->attachObject(pCamera);

			// Target
			TiXmlElement* targetElem=pCameraElem->FirstChildElement("target");
			if(targetElem)
			{	
				// Create node with full information
				SceneNode *pTargetNode=createNode(targetElem, pSceneRoot);
				pCameraNode->setAutoTracking(true, pTargetNode);
			}

			if(!mWindow)
			{

				OGRE_EXCEPT(Exception::ERR_RT_ASSERTION_FAILED,
				"oSceneLoader: RenderWindow not valid, \
				If the RenderWindow was not auto created, \
				you must add it in the OSMScene object contructor",
				"OSMScene::createCameras");

			}

			// If viewport(s) are not present, create a default viewport
			if(mWindow->getNumViewports() == 0)
			{
				Viewport* vp = mWindow->addViewport(pCamera);
				
				// Alter the camera aspect ratio to match the viewport
				pCamera->setAspectRatio(
					Real(vp->getActualWidth()) / Real(vp->getActualHeight()));
			}
				

			// Notify
			if(mCallbacks)
				mCallbacks->OnCameraCreate(pCamera, pCameraElem);

			// Add to camera list
			mCameras.push_back(pCamera);
		} catch(...)
		{
			continue;
		}
	}
	
}

// For CE version, helpers, shapes, static geometry 
// and external skeleton animation support is not provided
void OSMScene::createHelpers(TiXmlElement* pHelperNode, Ogre::SceneNode* pSceneRoot)
{
	
}

void OSMScene::createShapes(TiXmlElement* pShapeNode)
{
	

}

void OSMScene::createStaticGeometry(TiXmlElement* pStaticGeom) 
{	

}

void OSMScene::loadAnimations(TiXmlElement* animationsNode, SkeletonHandles& handles) {

	// Iterate all skeletons, adding animations from ".anim" files	
}

OSMAnimSerializer::OSMAnimSerializer() {

}

void OSMAnimSerializer::addAnimation(Ogre::SkeletonPtr skel, const Ogre::String& filename) {
	

}
