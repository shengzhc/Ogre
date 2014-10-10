//
//  TerrainApp.cpp
//  Ogre
//
//  Created by Shengzhe Chen on 10/7/14.
//
//

#include "TerrainApp.h"

void getTerrainImage(bool flipX, bool flipY, Ogre::Image &img)
{
    img.load("terrain.png", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
    if (flipX) img.flipAroundY();
    if (flipY) img.flipAroundX();
}

void TerrainApp::setupDemoScene()
{
    createScene();
}

void TerrainApp::createScene()
{
    OgreFramework::getSingletonPtr()->m_pCamera->setPosition(Ogre::Vector3(1683, 50, 2116));
    OgreFramework::getSingletonPtr()->m_pCamera->lookAt(Ogre::Vector3(1963, 50, 1660));

    OgreFramework::getSingletonPtr()->m_pCamera->setNearClipDistance(.1f);
    OgreFramework::getSingletonPtr()->m_pCamera->setFarClipDistance(50000);
    
    if (OgreFramework::getSingletonPtr()->m_pRoot->getRenderSystem()->getCapabilities()->hasCapability(Ogre::RSC_INFINITE_FAR_PLANE)){
        OgreFramework::getSingletonPtr()->m_pCamera->setFarClipDistance(0);
    }
    
    Ogre::Vector3 lightdir(0.55, -.3f, .75f);
    lightdir.normalise();
    
    Ogre::Light *light = OgreFramework::getSingletonPtr()->m_pSceneMgr->createLight("tstLight");
    light->setType(Ogre::Light::LT_DIRECTIONAL);
    light->setDirection(lightdir);
    light->setDiffuseColour(Ogre::ColourValue::White);
    light->setSpecularColour(Ogre::ColourValue(0.4, 0.4, 0.4));
    OgreFramework::getSingletonPtr()->m_pSceneMgr->setAmbientLight(Ogre::ColourValue(.2, .2, .2));

    m_pTerrainGlobals = OGRE_NEW Ogre::TerrainGlobalOptions();
    m_pTerrainGroup = OGRE_NEW Ogre::TerrainGroup(OgreFramework::getSingletonPtr()->m_pSceneMgr, Ogre::Terrain::ALIGN_X_Z, 513, 12000.0f);
    m_pTerrainGroup->setFilenameConvention(Ogre::String("BasicTutorial3Terrain"), Ogre::String("dat"));
    m_pTerrainGroup->setOrigin(Ogre::Vector3::ZERO);
    configureTerrainDefaults(light);
    
    for (long x=0; x<=0; x++) {
        for (long y=0; y<=0; y++) {
            defineTerrain(x, y);
        }
    }
    
    m_pTerrainGroup->loadAllTerrains(true);

    if (m_pTerrainsImported) {
        Ogre::TerrainGroup::TerrainIterator ti = m_pTerrainGroup->getTerrainIterator();
        while (ti.hasMoreElements()) {
            Ogre::Terrain *t = ti.getNext()->instance;
            initBlendMaps(t);
        }
    }
    
    m_pTerrainGroup->freeTemporaryResources();
}

void TerrainApp::defineTerrain(long x, long y)
{
    Ogre::String filename = m_pTerrainGroup->generateFilename(x, y);
    if (Ogre::ResourceGroupManager::getSingleton().resourceExists(m_pTerrainGroup->getResourceGroup(), filename)) {
        m_pTerrainGroup->defineTerrain(x, y);
    } else {
        Ogre::Image img;
        getTerrainImage(x % 2 != 0, y%2 != 0, img);
        m_pTerrainGroup->defineTerrain(x, y, &img);
        m_pTerrainsImported = true;
    }
}

void TerrainApp::initBlendMaps(Ogre::Terrain *terrain)
{
    Ogre::TerrainLayerBlendMap *blendMap0 = terrain->getLayerBlendMap(1);
    Ogre::TerrainLayerBlendMap *blendMap1 = terrain->getLayerBlendMap(2);
    Ogre::Real minHeight0 = 70;
    Ogre::Real fadeDist0 = 40;
    Ogre::Real minHeight1 = 70;
    Ogre::Real fadeDist1 = 15;
    
    float *pBlend0 = blendMap0->getBlendPointer();
    float *pBlend1 = blendMap1->getBlendPointer();
    
    for (Ogre::uint16 y=0; y<terrain->getLayerBlendMapSize(); ++y) {
        for (Ogre::uint16 x=0; x<terrain->getLayerBlendMapSize(); ++x) {
            Ogre::Real tx, ty;
            blendMap0->convertImageToTerrainSpace(x, y, &tx, &ty);
            Ogre::Real height = terrain->getHeightAtTerrainPosition(tx, ty);
            Ogre::Real val = (height - minHeight0)/fadeDist0;
            val = Ogre::Math::Clamp(val, (Ogre::Real)0, (Ogre::Real)1);
            *pBlend0++ = val;
            
            val = (height - minHeight1) / fadeDist1;
            val = Ogre::Math::Clamp(val, (Ogre::Real)0, (Ogre::Real)1);
            *pBlend1++ = val;
        }
    }
    blendMap0->dirty();
    blendMap1->dirty();
    blendMap0->update();
    blendMap1->update();
}

void TerrainApp::configureTerrainDefaults(Ogre::Light *light)
{
    m_pTerrainGlobals->setMaxPixelError(8);
    m_pTerrainGlobals->setCompositeMapDistance(3000);

    m_pTerrainGlobals->setLightMapDirection(light->getDerivedDirection());
    m_pTerrainGlobals->setCompositeMapAmbient(OgreFramework::getSingletonPtr()->m_pSceneMgr->getAmbientLight());
    m_pTerrainGlobals->setCompositeMapDiffuse(light->getDiffuseColour());
    
    Ogre::Terrain::ImportData &defaultimp = m_pTerrainGroup->getDefaultImportSettings();
    defaultimp.terrainSize = 513;
    defaultimp.worldSize = 12000.0f;
    defaultimp.inputScale = 600;
    defaultimp.minBatchSize = 33;
    defaultimp.maxBatchSize = 65;

    defaultimp.layerList.resize(3);
    defaultimp.layerList[0].worldSize = 100;
    defaultimp.layerList[0].textureNames.push_back("dirt_grayrocky_diffusespecular.dds");
    defaultimp.layerList[0].textureNames.push_back("dirt_grayrocky_normalheight.dds");
    defaultimp.layerList[1].worldSize = 30;
    defaultimp.layerList[1].textureNames.push_back("grass_green-01_diffusespecular.dds");
    defaultimp.layerList[1].textureNames.push_back("grass_green-01_normalheight.dds");
    defaultimp.layerList[2].worldSize = 200;
    defaultimp.layerList[2].textureNames.push_back("growth_weirdfungus-03_diffusespecular.dds");
    defaultimp.layerList[2].textureNames.push_back("growth_weirdfungus-03_normalheight.dds");
}

bool TerrainApp::frameRenderingQueued(const Ogre::FrameEvent& evt)
{
    bool ret = Ogre::FrameListener::frameRenderingQueued(evt);
    if (m_pTerrainGroup->isDerivedDataUpdateInProgress()) {
        
    }
}


