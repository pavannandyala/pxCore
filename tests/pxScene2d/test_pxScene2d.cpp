/*

pxCore Copyright 2005-2018 John Robinson

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.

*/

#include <sstream>

#define private public
#define protected public

#include "pxScene2d.h"
#include "rtString.h"
#include <string.h>
#include <unistd.h>
#include "pxTimer.h"

#include "test_includes.h" // Needs to be included last

using namespace std;

extern void populateWaylandAppsConfig();
extern void populateAllAppsConfig();
extern void populateAllAppDetails(rtString& appDetails);
extern map<string, string> gWaylandAppsMap;
extern map<string, string> gWaylandRegistryAppsMap;
extern map<string, string> gPxsceneWaylandAppsMap;
extern rtScript script;

class pxScene2dTest : public testing::Test
{
  public:
    virtual void SetUp()
    {
    }

    virtual void TearDown()
    {
      
    }

    void getArchiveTest()
    {
      rtObjectRef sceneRef =  new pxScene2d();
      pxScene2d* scene = (pxScene2d*) sceneRef.getPtr();

      scene->AddRef();
      rtObjectRef archive;
      EXPECT_TRUE(RT_OK == scene->loadArchive("supportfiles/helloworld.js", archive));
      EXPECT_TRUE(archive.getPtr() == scene->getArchive().getPtr());
    }

    void viewContainerTest()
    {
      pxSceneContainer* container = new pxSceneContainer(NULL);
      container->setUrl("supportfiles/helloworld.js");
      pxScriptView* view = (pxScriptView*) container->mScriptView.getPtr();
      EXPECT_TRUE (NULL != view);
      rtValue scene;
      rtValue args;
      args.setString("scene");
      EXPECT_TRUE(RT_OK == pxScriptView::getScene(1, &args, &scene, (void*)view));
      pxScene2d* opscene = (pxScene2d*)(scene.toObject().getPtr());
      EXPECT_TRUE (NULL != opscene);
      EXPECT_TRUE (opscene->viewContainer() == container);
    }

    void initFromUrlFromParentTest()
    {
      rtObjectRef sceneRef =  new pxScene2d();
      pxScene2d* parentscene = (pxScene2d*) sceneRef.getPtr();
      parentscene->AddRef();
      rtObjectRef archive;
      EXPECT_TRUE(RT_OK == parentscene->loadArchive("supportfiles/test_arc_resources.jar", archive));
      pxSceneContainer* container = new pxSceneContainer(parentscene);
      container->setUrl("mainfile.js");
      pxScriptView* view = (pxScriptView*) container->mScriptView.getPtr();
      EXPECT_TRUE (NULL != view);
      rtValue scene;
      rtValue args;
      args.setString("scene");
      EXPECT_TRUE(RT_OK == pxScriptView::getScene(1, &args, &scene, (void*)view));
      pxScene2d* opscene = (pxScene2d*)(scene.toObject().getPtr());
      opscene->AddRef();
      EXPECT_TRUE (NULL != opscene);
      EXPECT_TRUE (opscene->mArchive.getPtr() == parentscene->mArchive.getPtr());
    }

    void initFromUrlFromLocalTest()
    {
      rtObjectRef sceneRef =  new pxScene2d();
      pxScene2d* parentscene = (pxScene2d*) sceneRef.getPtr();
      parentscene->AddRef();
      rtObjectRef archive;
      EXPECT_TRUE(RT_OK == parentscene->loadArchive("supportfiles/helloworld.js", archive));
      pxSceneContainer* container = new pxSceneContainer(parentscene);
      container->setUrl("supportfiles/simple.js");
      pxScriptView* view = (pxScriptView*) container->mScriptView.getPtr();
      EXPECT_TRUE (NULL != view);
      rtValue scene;
      rtValue args;
      args.setString("scene");
      EXPECT_TRUE(RT_OK == pxScriptView::getScene(1, &args, &scene, (void*)view));
      pxScene2d* opscene = (pxScene2d*)(scene.toObject().getPtr());
      opscene->AddRef();
      EXPECT_TRUE (NULL != opscene);
      EXPECT_TRUE (opscene->mArchive.getPtr() != parentscene->mArchive.getPtr());
    }
    void populateWaylandAppsConfigTest()
    { 
      populateWaylandAppsConfig();
      EXPECT_TRUE ( 0 == gWaylandRegistryAppsMap.size());
      setenv("WAYLAND_APPS_CONFIG", "../../examples/pxScene2d/src/waylandregistry.conf", 1);
      populateWaylandAppsConfig();
      EXPECT_TRUE ( 5 == gWaylandRegistryAppsMap.size());
      setenv("WAYLAND_APPS_CONFIG", "../../tests/pxScene2d/supportfiles/jsonParseError.json", 1);
      populateWaylandAppsConfig();
    }
    void populateAllAppsConfigTest()
    {
      populateAllAppsConfig();
      setenv("PXSCENE_APPS_CONFIG", "../../tests/pxScene2d/supportfiles/pxsceneappregistry.conf", 1);
      populateAllAppsConfig();
      setenv("PXSCENE_APPS_CONFIG", "../../tests/pxScene2d/supportfiles/jsonParseError.json", 1);
      populateAllAppsConfig();
    }
    void populateAllAppDetailsTest()
    {
      rtString appDetails;
      populateAllAppDetails(appDetails);
      
      setenv("PXSCENE_APPS_CONFIG", "../../tests/pxScene2d/supportfiles/fileNotPresent.json", 1);
      populateAllAppDetails(appDetails);
      
      setenv("PXSCENE_APPS_CONFIG", "../../tests/pxScene2d/supportfiles/jsonParseError.json", 1);
      populateAllAppDetails(appDetails);
      
      setenv("PXSCENE_APPS_CONFIG", "../../tests/pxScene2d/supportfiles/pxsceneappregistry.conf", 1);
      populateAllAppsConfig();
      
      setenv("WAYLAND_APPS_CONFIG", "../../examples/pxScene2d/src/waylandregistry.conf", 1);
      populateWaylandAppsConfig();
      populateAllAppDetails(appDetails);
 
    }
    
    void process(float nosecs)
    {
      double  secs = pxSeconds();
      while ((pxSeconds() - secs) < nosecs)
      {
        if (NULL != mView)
        {
          mView->onUpdate(pxSeconds());
          script.pump();
        }
      }
    }
 
    void pxObjectTest()
    {
      mUrl = "test_OSCILLATE.js";
      mView = new pxScriptView(mUrl,"");
      process(5.0);
      rtObjectRef   scene = mView->mScene;
      pxScene2d* sceneptr = (pxScene2d*)scene.getPtr();
      ASSERT_NE(sceneptr, nullptr); 
      
      mRoot = sceneptr->getRoot();
      vector<rtRef<pxObject> > childrenVector = mRoot->mChildren ;
      EXPECT_TRUE ( RT_OK == childrenVector[2]->moveToBack()); 
      EXPECT_TRUE ( RT_OK == childrenVector[2]->moveToFront()); 
      
      EXPECT_TRUE ( RT_OK == childrenVector[3]->moveForward());
      EXPECT_TRUE ( RT_OK == childrenVector[3]->moveBackward());
      childrenVector[2]->createSnapshotOfChildren();
      childrenVector[3]->releaseData(false);
      childrenVector[6]->reloadData(true);
      EXPECT_TRUE ( RT_OK == childrenVector[2]->remove());
      EXPECT_TRUE ( RT_OK == childrenVector[3]->removeAll());
      mRoot->reloadData(false);
      mRoot->reloadData(true);
      EXPECT_TRUE ( RT_OK == mRoot->setPainting(true));  
      EXPECT_TRUE ( RT_OK == mRoot->setPainting(false));

      rtObjectRef props = new rtMapObject();
      props.set("t","image");
      rtObjectRef animateObjTest;
      EXPECT_TRUE ( RT_OK == mRoot->animateToObj(props, 20, 0, 1, 1, animateObjTest));


      EXPECT_TRUE(false == sceneptr->onMouseDown(3, 2, 0));
      EXPECT_TRUE(false == sceneptr->onMouseUp(3, 2, 0));
      EXPECT_TRUE(false == sceneptr->onMouseMove(3, 2));
      EXPECT_TRUE(false == sceneptr->onKeyDown(3, 0));
      EXPECT_TRUE(false == sceneptr->onKeyUp(3, 0));
      EXPECT_TRUE(false == sceneptr->onChar(65));
      
    }
   
    void pxScene2dClassTest()
    {
      mUrl = "test_OSCILLATE.js";
      mView = new pxScriptView(mUrl,"");
      process(5.0); 
      rtObjectRef   scene = mView->mScene;
      pxScene2d* scenePtr = (pxScene2d*)scene.getPtr();
      ASSERT_NE(scenePtr, nullptr);
      
      mRoot = scenePtr->getRoot();
      rtValue  v; 
      bool flag; 
      EXPECT_TRUE ( RT_OK == scenePtr->collectGarbage());
      setenv("SPARK_ENABLE_COLLECT_GARBAGE", "1", 1);
      EXPECT_TRUE ( RT_OK == scenePtr->collectGarbage());
 
      EXPECT_TRUE ( RT_OK == scenePtr->suspend(v, flag));
      EXPECT_TRUE ( RT_OK == scenePtr->resume(v, flag));

      flag = true;
      bool setFlag = false;
      EXPECT_TRUE ( RT_OK == scenePtr->showOutlines(flag));
      EXPECT_TRUE ( RT_OK == scenePtr->setShowOutlines(setFlag));
      EXPECT_TRUE ( RT_OK == scenePtr->showDirtyRect(flag));
      EXPECT_TRUE ( RT_OK == scenePtr->setShowDirtyRect(setFlag));

      rtFunctionRef cAnimator; 
      EXPECT_TRUE ( RT_OK == scenePtr->customAnimator(cAnimator));

      rtString type = "image/png;base64";
      rtValue pngData;
      EXPECT_TRUE ( RT_OK == scenePtr->screenshot(type, pngData));

      rtString typeImage = "image/image";
      rtValue imageValue;
      EXPECT_TRUE ( RT_OK == scenePtr->screenshot(typeImage, imageValue));

      rtObjectRef imageA = new rtMapObject();
      rtObjectRef cImageA = new rtMapObject();
      imageA.set("t","imageA");
      EXPECT_TRUE ( RT_OK == scenePtr->createImageA(imageA, cImageA));
      EXPECT_TRUE ( RT_OK == scenePtr->createImage9Border(imageA, cImageA));
      EXPECT_TRUE ( RT_OK == scenePtr->createImageAResource(imageA, cImageA));

      rtString paramVal;
      EXPECT_TRUE ( RT_OK == scenePtr->clipboardSet("text", "codeCoverage")); 
      EXPECT_TRUE ( RT_OK == scenePtr->clipboardGet("text", paramVal));
   
      rtObjectRef retObj;
      EXPECT_TRUE ( RT_OK == scenePtr->addServiceProvider(mRoot->mEmit.getPtr()));
      EXPECT_TRUE ( RT_OK == scenePtr->getService("text", retObj));
      EXPECT_TRUE ( RT_OK == scenePtr->removeServiceProvider(mRoot->mEmit.getPtr()));
   }
   
   void pxScene2dHdrTest ()
   {
      mUrl = "test_OSCILLATE.js";
      mView = new pxScriptView(mUrl,"");
      process(5.0);
      rtObjectRef   scene = mView->mScene;
      pxScene2d* sceneptr = (pxScene2d*)scene.getPtr();
      ASSERT_NE(sceneptr, nullptr);

      mRoot = sceneptr->getRoot(); 
      
      rtString getStringVal = "";
      int32_t getIntVal = 0 ;
      bool getBoolVal = false ;
      float getFloatVal = 0.0;
      EXPECT_TRUE ( RT_OK == mRoot->numChildren(getIntVal));

      EXPECT_TRUE ( RT_OK == mRoot->id(getStringVal));
      EXPECT_TRUE ( RT_OK == mRoot->interactive(getBoolVal));
      mRoot->w();
      mRoot->h();
      EXPECT_TRUE ( RT_OK == mRoot->setPX(2.0));
      EXPECT_TRUE ( RT_OK == mRoot->px(getFloatVal));
      EXPECT_TRUE ( RT_OK == mRoot->setPY(2.0));
      EXPECT_TRUE ( RT_OK == mRoot->py(getFloatVal));
      EXPECT_TRUE ( RT_OK == mRoot->cx(getFloatVal));
      EXPECT_TRUE ( RT_OK == mRoot->cy(getFloatVal));
      EXPECT_TRUE ( RT_OK == mRoot->sx(getFloatVal));
      EXPECT_TRUE ( RT_OK == mRoot->sy(getFloatVal));
      EXPECT_TRUE ( RT_OK == mRoot->setRX(2.0));
      EXPECT_TRUE ( RT_OK == mRoot->rx(getFloatVal));
      EXPECT_TRUE ( RT_OK == mRoot->setRY(2.0));
      EXPECT_TRUE ( RT_OK == mRoot->ry(getFloatVal));
      EXPECT_TRUE ( RT_OK == mRoot->setRZ(2.0));
      EXPECT_TRUE ( RT_OK == mRoot->rz(getFloatVal));
      EXPECT_TRUE ( RT_OK == mRoot->painting(getBoolVal));
      EXPECT_TRUE ( RT_OK == mRoot->clip(getBoolVal));
      EXPECT_TRUE ( RT_OK == mRoot->setMask(true));
      EXPECT_TRUE ( RT_OK == mRoot->mask(getBoolVal));
      EXPECT_TRUE ( RT_OK == mRoot->setDrawEnabled(true));
      EXPECT_TRUE ( RT_OK == mRoot->drawEnabled(getBoolVal));
      EXPECT_TRUE ( RT_OK == mRoot->setHitTest(true));
      EXPECT_TRUE ( RT_OK == mRoot->hitTest(getBoolVal));
      EXPECT_TRUE ( RT_OK == mRoot->focus(getBoolVal)); 
 
      rtRef<pxObject> f, t;
      pxMatrix4f m;
      pxObject::getMatrixFromObjectToObject(mRoot, t, m); 
      pxVector4f vf, vt;
      pxObject::transformPointFromObjectToScene(f, vf, vt);
      pxObject::transformPointFromSceneToObject(f, vf, vt);
      pxObject::transformPointFromObjectToObject(f, t, vf, vt);
  }

  void pxScriptViewTest()
  {
    
    char buffer[MAX_URL_SIZE + 50];
    memset (buffer, 0, sizeof(buffer));
    pxScriptView * scriptView = new pxScriptView(buffer, "javascript/node/v8");
    EXPECT_TRUE ( false == scriptView->onMouseDown(3, 2, true));
    EXPECT_TRUE ( false == scriptView->onMouseUp(3, 2, true));
    EXPECT_TRUE ( false == scriptView->onMouseEnter());
    EXPECT_TRUE ( false == scriptView->onBlur());
    EXPECT_TRUE ( false == scriptView->onKeyDown(65, true));
    EXPECT_TRUE ( false == scriptView->onKeyUp(65, true));
    EXPECT_TRUE ( false == scriptView->onChar(65));
   
    scriptView->mView = NULL;
   
    
    EXPECT_TRUE ( false == scriptView->onMouseDown(3, 2, true));
    EXPECT_TRUE ( false == scriptView->onMouseUp(3, 2, true));
    EXPECT_TRUE ( false == scriptView->onMouseEnter());
    EXPECT_TRUE ( false == scriptView->onBlur());
    EXPECT_TRUE ( false == scriptView->onKeyDown(65, true));
    EXPECT_TRUE ( false == scriptView->onKeyUp(65, true));
    EXPECT_TRUE ( false == scriptView->onChar(65));
    EXPECT_TRUE ( false == scriptView->onMouseLeave());
    EXPECT_TRUE ( false == scriptView->onMouseMove(2, 4));
    EXPECT_TRUE ( false == scriptView->onFocus());

 }

#ifdef PXSCENE_SUPPORT_STORAGE
  void storageTest()
  {
    rtObjectRef sceneRef = new pxScene2d();
    pxScene2d* scene = (pxScene2d*) sceneRef.getPtr();

    // storage init
    rtObjectRef storageRef;
    EXPECT_EQ ((int)RT_OK, (int)scene->storage(storageRef));
    EXPECT_TRUE (storageRef != NULL);

    // storage works
    const char* fnName;
    rtValue key, val, ret;
    fnName = "setItem";
    key = "testKey";
    val = "testValue";
    EXPECT_EQ ((int)RT_OK, (int)storageRef.send(fnName, key, val));
    fnName = "getItem";
    key = "testKey";
    ret.term();
    EXPECT_EQ ((int)RT_OK, (int)storageRef.sendReturns(fnName, key, ret));
    EXPECT_TRUE (ret == val);
    fnName = "removeItem";
    key = "testKey";
    EXPECT_EQ ((int)RT_OK, (int)storageRef.send(fnName, key));
    fnName = "getItem";
    key = "testKey";
    ret.term();
    EXPECT_EQ ((int)RT_OK, (int)storageRef.sendReturns(fnName, key, ret));
    EXPECT_TRUE (ret == rtValue(""));

    // storage destroy
    scene->dispose();
    sceneRef = NULL;

    // storage does nothing after destroy
    fnName = "setItem";
    key = "testKey";
    val = "testValue";
    EXPECT_EQ ((int)RT_OK, (int)storageRef.send(fnName, key, val));
    fnName = "getItem";
    key = "testKey";
    ret.term();
    EXPECT_EQ ((int)RT_OK, (int)storageRef.sendReturns(fnName, key, ret));
    EXPECT_TRUE (ret == rtValue(""));
  }
#endif

  private:
    pxObject*     mRoot;
    pxScriptView* mView;
    rtString      mUrl;
}; 

TEST_F(pxScene2dTest, pxScene2dTests)
{
    getArchiveTest();
    viewContainerTest();
    initFromUrlFromParentTest();
    initFromUrlFromLocalTest();
    populateWaylandAppsConfigTest();
    populateAllAppsConfigTest();
    populateAllAppDetailsTest();
    pxObjectTest();
    pxScene2dClassTest();
    //pxScene2dHdrTest();
    pxScriptViewTest();
#ifdef PXSCENE_SUPPORT_STORAGE
    storageTest();
#endif
}
