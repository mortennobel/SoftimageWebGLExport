/// License: Creative Commons Attribution 3.0 Unported (http://creativecommons.org/licenses/by/3.0/)
/// 2011 Morten Nobel-Joergensen / Vaida Laganeckiene
/// https://github.com/mortennobel/SoftimageWebGLExport

var statusElement;

function webGLStart(webGlCanvas, statusId, sceneName){
    var canvas = document.getElementById(webGlCanvas);
    if (!canvas){
        alert("Cannot find webgl-canvas "+webGlCanvas);
        return;
    }
    if (!sceneName){
        sceneName = "scene.json";
    }
    var renderer = new WebGLRenderer();
    renderer.initGL(canvas);
    statusElement = document.getElementById(statusId);

    var loadingQueue = new Array();

    loadScene(sceneName,renderer);
    return renderer;

    function loadScene(sceneName,renderer){
        pushJobToLoadingQueue("Load scene "+sceneName);
        var request = new XMLHttpRequest();
        request.open("GET", sceneName);
        request.onreadystatechange = function () {
            if (request.readyState == 4) {
                try{
                    var scene = JSON.parse(request.responseText);
                    handleLoadedScene(renderer, scene);
                    popJobFromLoadingQueue("Load scene "+sceneName);
                } catch (e) {
                    alert("Error loading scene data");
                    if (console){
                        console.log(e);
                    }
                }
            }
        }
        request.send();
    }
        

    function loadShader(shaderName,renderer){
        pushJobToLoadingQueue("load shader "+shaderName);
        var shaderDoneListener = new Object();
        var shader = renderer.createShader(shaderName);
        shaderDoneListener.shadersLoaded = 0;
        shaderDoneListener.onShaderLoaded = function(){
            if (shaderDoneListener.shadersLoaded<2){
                return;
            }
            shader.initShaders(shaderDoneListener.fragmentShaderSrc, shaderDoneListener.vertexShaderSrc);
            popJobFromLoadingQueue("load shader "+shaderName);
        }

        var requestFrag = new XMLHttpRequest();
        requestFrag.open("GET", shaderName+".frag");
        requestFrag.onreadystatechange = function () {
            if (requestFrag.readyState == 4) {
                shaderDoneListener.fragmentShaderSrc = requestFrag.responseText;
                shaderDoneListener.shadersLoaded++;
                shaderDoneListener.onShaderLoaded();
            }
        }
        requestFrag.send();

        var requestVert = new XMLHttpRequest();
        requestVert.open("GET", shaderName+".vert");
        requestVert.onreadystatechange = function () {
            if (requestVert.readyState == 4) {
                shaderDoneListener.vertexShaderSrc = requestVert.responseText;
                shaderDoneListener.shadersLoaded++;
                shaderDoneListener.onShaderLoaded();
            }
        }
        requestVert.send();
        return shader;
    }

    function loadTextures(renderer,textures){
        return textures.map(function(textureData) {
            var textureObj = renderer.createTexture(textureData.name);
            pushJobToLoadingQueue("Load texture "+textureObj.name);
            var image = new Image();
            image.onload = function () {
                textureObj.setImage(image);
                popJobFromLoadingQueue("Load texture "+textureObj.name);
            }
            image.src = textureData.url;
            return textureObj;
        });
    }

    function loadMaterials(renderer,materials,textures){
        return materials.map(function(materialData){
            var material = renderer.createMaterial(materialData.name, materialData.shader);
            material.init(materialData.parameters);
            textures.forEach(function(texture){
                if (texture.name == materialData.parameters.diffuseTexture){
                    material.diffuseTexture = texture;
                }
            });
            return material;
        });
    }

    function loadSceneObject(renderer,sceneObjects,materials){
        var sceneObjects = sceneObjects.map(function(sceneObjectData){
            var transform = sceneObjectData.transform;
            var sceneObject = renderer.createSceneObject(sceneObjectData.name);
            sceneObject.parentName = sceneObjectData.parent;
            sceneObject.transform = new WebGLTransform(transform.localTranslation,
                    transform.localRotation,transform.localScaling);
            sceneObjectData.components.forEach(function(component){
			    if (component.type == "material"){
                    for (var j=0;j<materials.length;j++){
                        if (materials[j].name == component.name){
                            sceneObject.material = materials[j];
                            break;
                        }
                    }
                }
                else if (component.type == "mesh"){
                    loadMesh(component.url,sceneObject, renderer);
                }
                else if (component.type == "camera"){
                    renderer.initCamera(component,sceneObject);
                }
                else if (component.type == "light"){
                    if (component.lightType == "directional"){ // currently only directional light supported
                        renderer.createLight(component,sceneObject);
                    }
                }
                else {
                    alert("Component type "+component.type+" Unknown");
                }
            });
			return sceneObject;
        });

        sceneObjects.forEach(function(sceneObject){
            // find parents and children
            if (sceneObject.parentName){
                // find parent
                for (var j=0;j<sceneObjects.length;j++){
                    var potentialParent = sceneObjects[j];
                    if (potentialParent.name == sceneObject.parentName){
                        sceneObject.transform.setParent(potentialParent.transform);
                        return;
                    }
                }
                alert("Error - cannot find parent "+sceneObject.parentName+" for object "+sceneObject.name);
            }
        });
    }

    function handleLoadedScene(renderer,sceneData){
        var textures = loadTextures(renderer,sceneData.textures);
        // handle shaders
        sceneData.shaders.forEach(function(shader){
            loadShader(shader,renderer);
        });

        var materials = loadMaterials(renderer, sceneData.materials,textures);

        loadSceneObject(renderer,sceneData.sceneObjects,materials);
    }

    function loadMesh(meshUrl,sceneObject,renderer) {
        pushJobToLoadingQueue("load mesh "+meshUrl);
        var request = new XMLHttpRequest();
        request.open("GET", meshUrl);
        request.onreadystatechange = function () {
            if (request.readyState == 4) {
                try {
                    renderer.handleLoadedMesh(JSON.parse(request.responseText),sceneObject);
                    popJobFromLoadingQueue("Load mesh "+meshUrl);
                } catch (e){
                    alert("Error loading mesh from "+meshUrl);
                    if (console){
                        console.log(e);
                    }
                }
            }
        }
        request.send();
    }

    function pushJobToLoadingQueue(str){
        loadingQueue.push(str);
        if (statusElement && loadingQueue.length==1){
            statusElement.innerHTML = '<img src="ajax-loader.gif"/>&nbsp;'+str;
        }
    }

    function popJobFromLoadingQueue(str){
        var removeIndex = loadingQueue.indexOf(str);
        if (removeIndex==-1){
            removeIndex = 0;
        }

        loadingQueue.splice(0,1);
        if (statusElement){
            if (loadingQueue.length>0){
                statusElement.innerHTML = '<img src="ajax-loader.gif"/>&nbsp;'+loadingQueue[loadingQueue.length-1];
            } else {
                statusElement.innerHTML = '';
                renderer.sceneDraw();
            }
        }
    }
}