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

var homeUrl = "https://www.sparkui.org/examples/text/sample.md";

px.configImport({"browser:" : /*px.getPackageBaseFilePath() + */ "browser/"});

var uiReadyResolve = null;
var uiReadyReject = null;
module.exports.uiReady = new Promise(function(resolve, reject) 
{
  uiReadyResolve = resolve;
  uiReadyReject = reject;
});

px.import({ scene:   'px:scene.1.js',
            keys:    'px:tools.keys.js',
            ListBox: 'browser:listbox.js',
            EditBox: 'browser:editbox.js',
            mime:    'mime.js'
}).then( function importsAreReady(imports)
{
  var scene = imports.scene;
  var keys  = imports.keys;
  var root  = imports.scene.root;

  var resolveSceneUrl = imports.mime.resolveSceneUrl;

  var url       = "";
  var helpShown = false;

  const LINEAR      = scene.animation.TWEEN_LINEAR;
  const FASTFORWARD = scene.animation.OPTION_FASTFORWARD;
  const LOOP        = scene.animation.OPTION_LOOP;
  const FOREVER     = scene.animation.COUNT_FOREVER;

  var urlFocusColor     = 0x303030ff; // "#303030"
  var urlSucceededColor = 0x0c8508ff; // "#0c8508"
  var urlFailedColor    = 0xde0700ff; // "#de0700"

  var myStretch = scene.stretch.STRETCH;

  var fontRes       = scene.create({ t: "fontResource",  url: "FreeSans.ttf" });
  var backButtonRes = scene.create({ t: "imageResource", url: 'browser/images/arrow-circle-left-solid.svg',  w: 32, h: 32 });
  var foreButtonRes = scene.create({ t: "imageResource", url: 'browser/images/arrow-circle-right-solid.svg', w: 32, h: 32 });
  var menuButtonRes = scene.create({ t: "imageResource", url: 'browser/images/bars-solid.svg',               w: 24, h: 24 });

  var bg            = scene.create({ t:"image",  parent: root, url:"browser/images/status_bg.svg", stretchX: myStretch, stretchY: myStretch });
  var browser       = scene.create({ t:"object", parent: bg} );
  var content       = scene.create({ t:"scene",  parent: bg,      x:10, y:60, clip:true });

  var highlightBG   = scene.create({ t:"rect",   parent: browser, x:10, y:60, fillColor: "#0000", lineColor: "#080", lineWidth: 8, a: 0.0 });
  var contentBG     = scene.create({ t:"rect",   parent: browser, x:10, y:60, fillColor: "#fff", a: 0.05 });
  var inputBox      = new imports.EditBox( {     parent: browser, url: "browser/images/input2.png", x: 70+10, y: 10, w: 800-70-32, h: 35, pts: 24 });
  var listBox       = new imports.ListBox( {     parent: content, x: 950,   y: 0, w: 200, h: 100, visible:false, numItems:3 });
  var spinner       = scene.create({ t:"image",  parent: browser, url: "browser/images/spinningball2.png",  y:-80, cx: 50, cy: 50, sx: 0.3, sy: 0.3,a:0.0 });
  var backButton    = scene.create({ t:'image',  parent: browser, x:10,     y:12, w:32, h:32, resource: backButtonRes, a:0.2})
  var foreButton    = scene.create({ t:'image',  parent: browser, x:10+34,  y:12, w:32, h:32, resource: foreButtonRes, a:0.2})
  var menu          = scene.create({ t:'image',  parent: browser, x:800-28, y:14, w:24, h:24, resource: menuButtonRes, a:0.2})

  var helpBox  = null;
  var backUrls = []
  var currUrl  = ''
  var foreUrls = []

  var pageInsetL = 20;
  var pageInsetT = 70;

  var showFullscreen = false;

  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  //
  //  CAPABILITIES:  Test for 'inline' SVG support
  //
  var hasSvg = true;

  if( scene.capabilities              == undefined ||
      scene.capabilities.graphics     == undefined ||
      scene.capabilities.graphics.svg == undefined ||
      scene.capabilities.graphics.svg != 2)
  {
    // If *inline* SVG is not supported...
    hasSvg = false;
  }

  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  //
  //  CAPABILITIES:  Test for 'events' DRAG'n'DROP enhanced support
  //
  var hasDragDrop2 = true;

  if( scene.capabilities                    == undefined ||
      scene.capabilities.events             == undefined ||
      scene.capabilities.events.drag_n_drop == undefined ||
      scene.capabilities.events.drag_n_drop != 2)
  {
    // If DRAG'n'DROP enhanced support NOT supported...
    hasDragDrop2 = false;
  }
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


  scene.addServiceProvider(function(serviceName, serviceCtx){
    if (serviceName == ".navigate")
      // TODO JRJR have to set url in a timer to avoid reentrancy
      // should move deferring to setUrl method...
      return {setUrl:function(u){setTimeout(function(){
        /*content.url = u;*/ /*inputBox.text = u;*/ reload(u)},1);}}  // return a javascript object that represents the service
    else
      return "allow"; // allow request to bubble to parent
  });

  var currentGen = 0
  function reload(u, keepHistory)
  {
    currentGen++
    inputBox.textColor = urlFocusColor
    var originalUrl = u?u.trim():''
    u = resolveSceneUrl(originalUrl);
    console.log("RELOADING .... [ " + u + " ]");

    // Prime the Spinner !
    //inputBox.doLater( function() { spinner.a = 1.0; }, 500 ); // 500 ms
    spinner.a = 1.0

    if(false)
    {
      // DEBUG DEBUG DEBUG DEBUG DEBUG DEBUG DEBUG DEBUG DEBUG
      setTimeout(function delayLoadURL() { // Simulate latency in URL loading

          content.url = u;
          inputBox.cancelLater( function() { spinner.a = 0;} );
      }, 3000);
      // DEBUG DEBUG DEBUG DEBUG DEBUG DEBUG DEBUG DEBUG DEBUG
    }
    else
    {
      content.url = u;
    }


    // JRJR BUGBUG
    // Promise doesn't seem to fire if url is empty
    if (u == '')
    {
      spinner.a = 0
    }

    inputBox.text = originalUrl;

    if (keepHistory)
    {
      currUrl = u;
    }

    if (true)
    {
      content.ready.then(
        // Use closure to capture the current navigation gen
        // so we can compare at completion
        function(gen) {
          return function(o) {
            listBox.addItem(inputBox.text);
            contentBG.draw = true;
            content.focus = true;

            inputBox.textColor = urlSucceededColor;

            inputBox.hideCursor();
            inputBox.clearSelection()
            //inputBox.cancelLater( function() { spinner.a = 0;} );
            spinner.a = 0

            // Only truncate forward history if we're completing
            // the current matching navigation to resolve
            // a race condition
            if (gen == currentGen) {
              if (!keepHistory) {
                if (u != '' && currUrl != '' && currUrl != u) {
                  backUrls.push(currUrl)
                  foreUrls = []
                }
                currUrl = u
              }
            }

            backButton.a = backUrls.length ? 0.65 : 0.2
            foreButton.a = foreUrls.length ? 0.65 : 0.2
          }
        }(currentGen),
        function()
        {
          inputBox.focus = true
          inputBox.selectAll()
          inputBox.textColor = urlFailedColor;

          //inputBox.cancelLater( function() { spinner.a = 0;} );
          spinner.a = 0
        }
      );
    }
  }//reload()

  function goBack() {
    if (backUrls.length) {
      foreUrls.push(currUrl)
      reload(backUrls.pop(), true)
    }
  }

  function goForward() {
    if (foreUrls.length) {
        backUrls.push(currUrl)
        reload(foreUrls.pop(), true)
    }
  }

  backButton.on('onMouseUp', function(e){
    goBack()
  })

  foreButton.on('onMouseUp', function(e){
    goForward()
  })

//##################################################################################################################################

  content.on("onPreMouseUp", function(e)
  {
    //inputBox.focus = false;
    content.focus = true;
  });

  // layout
  function updateSize(w,h)
  {
    console.log("\n\n BROWSER:  Resizing... WxH: " + w + " x " + h + " \n\n");

    bg.w = w;
    bg.h = h;

    // show/hide browser chrome
    browser.a    = showFullscreen ?     0 : 1;

    // Anchor
    content.x   = showFullscreen ?  0 : 10;
    content.y   = showFullscreen ?  0 : 60;

    // Apply insets
    content.w   = showFullscreen ?  w : w - pageInsetL;
    content.h   = showFullscreen ?  h : h - pageInsetT;

    contentBG.w = content.w;
    contentBG.h = content.h;

    highlightBG.w = content.w + 8;
    highlightBG.h = content.h + 8;

    highlightBG.x = content.x - 4;
    highlightBG.y = content.y - 4;

    inputBox.w  = w - pageInsetL - 70 - 32;

    menu.x = w-36

    helpBox.x   = inputBox.x;
    helpBox.y   = inputBox.y + pageInsetL;

    spinner.x   = inputBox.x + inputBox.w - 70;
    spinner.y   = inputBox.y - inputBox.h +  2;
  }

  scene.root.on("onPreKeyDown", function(e)
  {
    if(keys.is_CTRL_ALT_SHIFT(e.flags) || keys.is_CTRL_ALT(e.flags))
    {
      if (e.keyCode == keys.L )
      {
        inputBox.focus = true;
        inputBox.selectAll();
        e.stopPropagation();
      }
    }

    if( keys.is_CTRL_ALT( e.flags ) ) // CTRL-ALT keys !!
    {
      switch(code)
      {
        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        case keys.LEFT:   //  CTRL-ALT-LEFT
        {
          console.log("goback")
          goBack()
          e.stopPropagation()
        }
        break;
        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        case keys.RIGHT:   //  CTRL-ALT-RIGHT
        {
          goForward()
          e.stopPropagation()
        }
        break;
        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        case keys.R:   //  CTRL-ALT-R
        {
          console.log("Browser.js Reloading");
          reload(currUrl, true);
          e.stopPropagation();
          console.log("Browser.js reload done");
        }
        break;
        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        case keys.R:   //  CTRL-ALT-H
        {
          console.log("browser.js Loading home");
          reload(homeUrl);
          e.stopPropagation();
        }
        break;
        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
      }//SWITCH
    }//ENDIF  CTRL-ALT
  });

  function showHelp(delay_ms)
  {
    helpBox.animateTo({ a: 1.0  }, 0.75, LINEAR, FASTFORWARD, 1).then
    (
      function(o) {
        helpShown = true;
        hideHelp(delay_ms);  // auto hide
      }
    );
  }

  function hideHelp(delay_ms)
  {
      setTimeout(function()
      {
          helpBox.animateTo({ a: 0 }, 0.75, LINEAR, FASTFORWARD, 1).then
          (
            function(o) { helpShown = false; }
          )
      }, delay_ms);
  }

  scene.root.on("onKeyDown", function(e)
  {
    var code = e.keyCode;
    // console.log("onKeyDown browser.js  >> code: " + code + " key:" + keys.name(code) + " flags: " + flags);

    if( keys.is_CTRL_ALT( e.flags ) ) // CTRL-ALT keys !!
    {
      switch(code)
      {
        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        case keys.A:  //  CTRL-ALT-A
        {
          console.log("about.js Loading about");
          reload("about.js");
          e.stopPropagation();
        }
        break;

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        case keys.F:  //  CTRL-ALT-F
        {
          showFullscreen = !showFullscreen;
          updateSize(scene.w, scene.h)
          e.stopPropagation()
        }
        break;

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        case keys.H:  //  CTRL-ALT-H
        {
          // console.log("browser.js Loading home");
          // reload(homeUrl);
          // e.stopPropagation();
        }
        break;

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        case keys.K:  //  CTRL-ALT-K
        {
          helpShown ? hideHelp(0) : showHelp(4500); // Hide / Show
          e.stopPropagation();
        }
        break;

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        case keys.R:   //  CTRL-ALT-R
        {
          // console.log("Browser.js Reloading");
          // reload(currUrl, true);
          // e.stopPropagation();
          // console.log("Browser.js reload done");
        }
        break;
      }//SWITCH
    }
    else
    {
      switch(code)
      {
        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        case keys.PAGEDOWN:
        {
          listBox.visible = !listBox.visible;
          listBox.focus   = !listBox.focus;
          //e.stopPropagation();
        }
        break;

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        case keys.ENTER:
        if(inputBox.focus == true)
        {
          url = inputBox.text;
          inputBox.moveToEnd();

          reload(url);
          //e.stopPropagation();
        }
        else
        if(listBox.visible == true)
        {
          var listBoxItem = listBox.selectedItem();
          if (listBoxItem == "UNAVAILABLE")
          {
            url = inputBox.text;
          }
          else
          {
            url = listBoxItem;
          }
          reload(url);
          //e.stopPropagation();
        }
        break;
        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
      }//SWITCH
    }//ENDIF   CTRL-ALT
  });

  if(hasDragDrop2)
  {
    content.on("onMouseExit", function (e)
    {
      highlightBG.animateTo({ a: 0 }, 0.5, LINEAR, FASTFORWARD, 1);
    });

    content.on("onDragEnter", function (e)
    {
      highlightBG.animateTo({ a: 1.0  }, 0.5, LINEAR, FASTFORWARD, 1);
    });

    content.on("onDragLeave", function (e)
    {
      highlightBG.animateTo({ a: 0 }, 0.25, LINEAR, FASTFORWARD, 1);
    });

    content.on("onDragMove", function (e)
    {
      if(highlightBG.a <=0)
      {
        highlightBG.animateTo({ a: 1 }, 0.25, LINEAR, FASTFORWARD, 1); /// Hmmm
      }
    });

    // Load URL dropped in 'content' area
    content.on("onDragDrop", function (e)
    {
      highlightBG.animateTo({ a: 0 }, 0.5, LINEAR, FASTFORWARD, 1);

      if(e.type == scene.dragType.URL)
      {
        var proto = (e.dropped.charAt(0) == '/') ? "file://" : "";
        var url   =  "" + proto + e.dropped;

        console.log(">>> Loading Dropped URL ... [ " + e.dropped + " ]");
        reload(url);

        e.stopPropagation();
      }
    });
  }

  scene.on("onResize", function(e) { updateSize(e.w,e.h); });

  Promise.all([listBox, inputBox, bg, spinner, content, fontRes])
      .catch( function (err)
      {
          console.log(">>> Loading Assets ... err = " + err);
          
          uiReadyReject();
      })
      .then( function (success, failure)
      {
        inputBox.focus = true;
        spinner.animateTo({r:360},1.0, LINEAR, LOOP, FOREVER);

        helpBox = scene.create({t:"textBox", parent: bg, textColor: 0x202020ff,
                                      x: 20, y: 100,  w: 350, h: 520, a: 0.0,
                                      font: fontRes, pixelSize: 14, wordWrap: true,
                                      interactive: false,  // <<< Essential !
                                      text: " BROWSER: \n\n"+
                                            "  CTRL-ALT-K        ...  Show Keys \n" +
                                            "\n"+
                                            "  CTRL-ALT-A        ...  Show About.js \n" +
                                            "  CTRL-ALT-R        ...  Reload URL \n" +
                                            "  CTRL-ALT-F        ...  Toggle 'Fullscreen' \n" +
                                            "  CTRL-ALT-H        ...  Load 'Browser.js' \n" +
                                            "\n"+
                                            "  CTRL-ALT-SHIFT-L  ...  Load Another URL \n\n" +
                                            " SHELL:   \n\n"+
                                            "  CTRL-ALT-D        ...  Toggle Dirty Rectangles \n" +
                                            "  CTRL-ALT-O        ...  Toggle Outlines \n" +
                                            "  CTRL-ALT-S        ...  Screenshot > screenshot.png \n" +
                                            "  CTRL-ALT-Y        ...  Toggle FPS \n" +
                                            "\n"+
                                            "  CTRL-ALT-SHIFT-D  ...  Log Debug Metrics  \n" +
                                            "  CTRL-ALT-SHIFT-H  ...  Reload HOME \n" +
                                            "  CTRL-ALT-SHIFT-R  ...  Reload BROWSER \n",
                                      alignHorizontal: scene.alignHorizontal.LEFT,
                                      alignVertical:   scene.alignVertical.CENTER})

        updateSize(scene.w, scene.h);
        reload(homeUrl)
        
        uiReadyResolve();
      });


}).catch( function importFailed(err){
  console.error("Import failed for browser.js: " + err);
});

