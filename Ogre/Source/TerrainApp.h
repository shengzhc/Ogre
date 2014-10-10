//
//  TerrainApp.h
//  Ogre
//
//  Created by Shengzhe Chen on 10/7/14.
//
//

#ifndef __Ogre__TerrainApp__
#define __Ogre__TerrainApp__

#include <stdio.h>
#include "OgreDemoApp.h"
#include <Terrain/OgreTerrainGroup.h>
#include <Terrain/OgreTerrain.h>

class TerrainApp : public DemoApp
{
private:
    Ogre::TerrainGlobalOptions *m_pTerrainGlobals;
    Ogre::TerrainGroup *m_pTerrainGroup;
    OgreBites::Label *m_pInfoLabel;
    bool m_pTerrainsImported;
    
    void defineTerrain(long x, long y);
    void initBlendMaps(Ogre::Terrain *terrain);
    void configureTerrainDefaults(Ogre::Light *light);
    
protected:
    virtual void setupDemoScene();
    virtual void createScene(void);
    
public:
    virtual bool frameRenderingQueued(const Ogre::FrameEvent& evt);
};

#endif /* defined(__Ogre__TerrainApp__) */
