/// License: Creative Commons Attribution 3.0 Unported (http://creativecommons.org/licenses/by/3.0/)
/// 2011 Morten Nobel-Joergensen / Vaida Laganeckiene
/// https://github.com/mortennobel/SoftimageWebGLExport


var degreeToRadian = 0.0174532925;

function WebGLRenderer(){
    var gl;
    var canvas;
    var shaders = new Array();
    var materials = new Array();
    var sceneObjects = new Array();
    var textures = new Array();
    var lights = new Array();
    var camera;
    var cameras = new Array();
    var activeCameraIndex = 0;
    var modelViewMatrix = mat4.create();
    var modelViewMatrixTmp = mat4.create(); // updated for each object
    var projectionMatrix = mat4.create();

    var lastShader;
    var lastMaterial;

    this.initGL = function(canvas_){
        canvas = canvas_; 
        try {
            try {
                gl = canvas.getContext("webgl");
            } catch (e){
                // ignore
            }
            if (!gl){
                gl = canvas.getContext("experimental-webgl");
            }
            gl.viewportWidth = canvas.width;
            gl.viewportHeight = canvas.height;
            gl.clearColor(0.0, 0.0, 0.0, 1.0);
            gl.enable(gl.DEPTH_TEST);
        } catch (e) {
        }
        if (!gl) {
            throw("Could not initialise WebGL, sorry :-(");
        }
    }

    this.initCamera = function(cameraData, sceneObject){
        var c = new WebGLCamera(gl,cameraData, sceneObject);
        if (cameras.length==0){
            camera = c;
        }
        cameras.push(c);
    }

    this.getNumberOfCameras = function(){
        return cameras.length;
    }

    this.setActiveCamera = function(index){
        camera = cameras[index];
        activeCameraIndex = index;
    }

    this.getActiveCameraIndex = function(){
        return activeCameraIndex;
    }

    this.sceneDraw = function () {
        gl.viewport(0, 0, gl.viewportWidth, gl.viewportHeight);
        gl.clear(gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT);

        camera.setupCamera(modelViewMatrix,projectionMatrix);

        lastShader = undefined;
        lastMaterial = undefined;

        for (var i=0;i<sceneObjects.length;i++){
            var sceneObject = sceneObjects[i];
            this.renderObject(sceneObject);
        }
    }

    this.renderObject = function(sceneObject){
		if (!sceneObject.meshVertexIndexBuffer){
			return;
		}
        var shader = sceneObject.material.shader;
        if (lastShader != shader){
            if (typeof(lastShader) != "undefined"){
                lastShader.deactivate();
            }
            shader.activate(lights);
            lastShader = shader;
        }
        if (lastMaterial != sceneObject.material){
            sceneObject.material.setMaterial();
            lastMaterial = sceneObject.material;
        }

        gl.bindBuffer(gl.ARRAY_BUFFER, sceneObject.meshVertexPositionBuffer);
        gl.vertexAttribPointer(shader.vertexPositionAttribute, sceneObject.meshVertexPositionBuffer.itemSize,
                gl.FLOAT, false, 0, 0);

        if (shader.vertexNormalAttribute != -1){
            gl.bindBuffer(gl.ARRAY_BUFFER, sceneObject.meshVertexNormalBuffer);
            gl.vertexAttribPointer(shader.vertexNormalAttribute, sceneObject.meshVertexNormalBuffer.itemSize,
                    gl.FLOAT, false, 0, 0);
        }

        if (shader.vertexTextureCoord != -1){
            gl.bindBuffer(gl.ARRAY_BUFFER, sceneObject.meshVertexTextureCoordBuffer);
            gl.vertexAttribPointer(shader.vertexTextureCoord, sceneObject.meshVertexTextureCoordBuffer.itemSize,
                    gl.FLOAT, false, 0, 0);
        }
        gl.bindBuffer(gl.ELEMENT_ARRAY_BUFFER, sceneObject.meshVertexIndexBuffer);

        mat4.multiply(modelViewMatrix, sceneObject.transform.getGlobalMatrix(), modelViewMatrixTmp);
        shader.setMatrixUniforms(projectionMatrix,modelViewMatrixTmp);

        gl.drawElements(gl.TRIANGLES, sceneObject.meshVertexIndexBuffer.numItems, gl.UNSIGNED_SHORT, 0);
    }

    this.handleLoadedMesh = function (meshData, sceneObject){
        sceneObject.meshVertexNormalBuffer = gl.createBuffer();
        gl.bindBuffer(gl.ARRAY_BUFFER, sceneObject.meshVertexNormalBuffer);
        gl.bufferData(gl.ARRAY_BUFFER, new Float32Array(meshData.vertexNormals), gl.STATIC_DRAW);
        sceneObject.meshVertexNormalBuffer.itemSize = 3;
        sceneObject.meshVertexNormalBuffer.numItems = meshData.vertexNormals.length / 3;

        sceneObject.meshVertexTextureCoordBuffer = gl.createBuffer();
        gl.bindBuffer(gl.ARRAY_BUFFER, sceneObject.meshVertexTextureCoordBuffer);
        gl.bufferData(gl.ARRAY_BUFFER, new Float32Array(meshData.vertexTextureCoords), gl.STATIC_DRAW);
        sceneObject.meshVertexTextureCoordBuffer.itemSize = 2;
        sceneObject.meshVertexTextureCoordBuffer.numItems = meshData.vertexTextureCoords.length / 2;

        sceneObject.meshVertexPositionBuffer = gl.createBuffer();
        gl.bindBuffer(gl.ARRAY_BUFFER, sceneObject.meshVertexPositionBuffer);
        gl.bufferData(gl.ARRAY_BUFFER, new Float32Array(meshData.vertexPositions), gl.STATIC_DRAW);
        sceneObject.meshVertexPositionBuffer.itemSize = 3;
        sceneObject.meshVertexPositionBuffer.numItems = meshData.vertexPositions.length / 3;

        sceneObject.meshVertexIndexBuffer = gl.createBuffer();
        gl.bindBuffer(gl.ELEMENT_ARRAY_BUFFER, sceneObject.meshVertexIndexBuffer);
        gl.bufferData(gl.ELEMENT_ARRAY_BUFFER, new Uint16Array(meshData.indices), gl.STATIC_DRAW);
        sceneObject.meshVertexIndexBuffer.itemSize = 1;
        sceneObject.meshVertexIndexBuffer.numItems = meshData.indices.length;
    }

    this.createShader = function(name){
        var shader = new WebGLShader(gl,name);
        shaders.push(shader);
        return shader;
    }

    this.createMaterial = function(name, shaderName){
        var shader;
        for (var i=0;i<shaders.length;i++){
            if (shaders[i].name == shaderName){
                shader = shaders[i];
                break;
            }
        }
        if (!shader){
            throw("Cannot find shader '"+shaderName+"' for material '"+name+"'");
        }
        var material = new WebGLMaterial(gl,name, shader);
        materials.push(material);
        return material;
    }

    this.createSceneObject = function(name, transform, material){
        var sceneObject = new WebGLSceneObject(name, transform, material);
        sceneObjects.push(sceneObject);
        return sceneObject;
    }

    this.createTexture = function(name){
        var texture = new WebGLTexture(gl, name);
        textures.push(texture);
        return texture; 
    }

    this.createLight = function(lightData,sceneObject){
        var light = new WebGLLight(lightData,sceneObject);
        lights.push(light);
        return light;
    }

    this.getCanvas = function(){
        return canvas;
    }

    this.getCamera = function(){
        return camera;
    }
}

function WebGLShader(gl, name){
    this.name = name;
    this.shaderProgramId = undefined;
    this.vertexPositionAttribute = undefined;
    this.vertexNormalAttribute = undefined;
    this.vertexTextureCoord = undefined;
    this.diffuseTexture = undefined;
    this.diffuseColor = undefined;
    this.specularColor = undefined;
    this.specularExponent = undefined;
    this.ambientColor = undefined;
    this.lightDirection = undefined;
    this.lightDiffuse = undefined;
    this.lightSpecular = undefined;
    this.lightAmbient = undefined;
    this.projectionMatrixUniform = undefined;
    this.modelViewMatrixUniform = undefined;
    this.normalMatrixUniform = undefined;

    this.initShaders = function(fragmentShaderSrc, vertexShaderSrc) {
        var fragmentShader = compileShader(gl, fragmentShaderSrc, true);
        var vertexShader = compileShader(gl, vertexShaderSrc, false);

        var shaderProgramId = gl.createProgram();
        this.shaderProgramId = shaderProgramId;

        gl.attachShader(shaderProgramId, vertexShader);
        gl.attachShader(shaderProgramId, fragmentShader);
        gl.linkProgram(shaderProgramId);

        if (!gl.getProgramParameter(shaderProgramId, gl.LINK_STATUS)) {
            alert("Could not initialise shaders");
        }

        gl.useProgram(shaderProgramId);
        this.vertexPositionAttribute = gl.getAttribLocation(shaderProgramId, "vertexPosition");
        this.vertexNormalAttribute = gl.getAttribLocation(shaderProgramId, "vertexNormal");
        this.vertexTextureCoord = gl.getAttribLocation(shaderProgramId, "vertexTextureCoord");
        this.diffuseTexture = gl.getUniformLocation(shaderProgramId, "diffuseTexture");
        this.diffuseColor = gl.getUniformLocation(shaderProgramId, "diffuseColor");
        this.specularColor = gl.getUniformLocation(shaderProgramId, "specular");
        this.specularExponent = gl.getUniformLocation(shaderProgramId, "specularExponent");
        this.ambientColor = gl.getUniformLocation(shaderProgramId, "ambient");

        this.lightDirection = gl.getUniformLocation(shaderProgramId, "lightDirection");
        this.lightDiffuse = gl.getUniformLocation(shaderProgramId, "lightDiffuse");
        this.lightSpecular = gl.getUniformLocation(shaderProgramId, "lightSpecular");
        this.lightAmbient = gl.getUniformLocation(shaderProgramId, "lightAmbient");

        this.projectionMatrixUniform = gl.getUniformLocation(shaderProgramId, "projectionMatrix");
        this.modelViewMatrixUniform = gl.getUniformLocation(shaderProgramId, "modelViewMatrix");
        this.normalMatrixUniform = gl.getUniformLocation(shaderProgramId, "normalMatrix");
    }

    this.setMatrixUniforms = function(projectionMatrix,modelViewMatrix) {
        gl.uniformMatrix4fv(this.projectionMatrixUniform, false, projectionMatrix);
        gl.uniformMatrix4fv(this.modelViewMatrixUniform, false, modelViewMatrix);
        if (this.normalMatrixUniform != null){
            var normalMatrix = mat3.create();
			mat4.toInverseMat3(modelViewMatrix, normalMatrix);
			mat3.transpose(normalMatrix);
			gl.uniformMatrix3fv(this.normalMatrixUniform, false, normalMatrix);
        }
    }

    this.activate = function(lights){
        gl.useProgram(this.shaderProgramId);
        gl.enableVertexAttribArray(this.vertexPositionAttribute);
        if (this.vertexNormalAttribute != -1){
            gl.enableVertexAttribArray(this.vertexNormalAttribute);
        }
        if (this.vertexTextureCoord != -1){
            gl.enableVertexAttribArray(this.vertexTextureCoord);
        }
        var light = lights[0]; // currently only supports one light
        // direction light only
        if (this.lightDirection != -1){
            var lightDirection = vec3.create();
            lightDirection[0] = 0;
            lightDirection[1] = 0;
            lightDirection[2] = -1;
            mat4.multiplyVec3(light.sceneObject.transform.getGlobalRotationMatrix(), lightDirection);
            gl.uniform3fv(this.lightDirection, lightDirection);
        }
        if (this.lightDiffuse != -1){
            gl.uniform3fv(this.lightDiffuse, light.diffuse);
        }
        if (this.lightSpecular != -1){
            gl.uniform3fv(this.lightSpecular, light.specular);
        }
        if (this.lightAmbient != -1){
            gl.uniform3fv(this.lightAmbient, light.ambient);
        }
    }

    this.deactivate = function(){
        gl.disableVertexAttribArray(this.vertexPositionAttribute);
        if (this.vertexNormalAttribute != -1){
            gl.disableVertexAttribArray(this.vertexNormalAttribute);
        }
        if (this.vertexTextureCoord != -1){
            gl.disableVertexAttribArray(this.vertexTextureCoord);
        }
    }

    // private function
    var compileShader = function (gl, str, fragmentShader) {
        var shader;
        if (fragmentShader) {
            shader = gl.createShader(gl.FRAGMENT_SHADER);
        } else {
            shader = gl.createShader(gl.VERTEX_SHADER);
        }

        gl.shaderSource(shader, str);
        gl.compileShader(shader);

        if (!gl.getShaderParameter(shader, gl.COMPILE_STATUS)) {
            alert(gl.getShaderInfoLog(shader));
            return null;
        }

        return shader;
    }
}

function WebGLMaterial(gl,name,shader){
    this.shader = shader;
    this.name = name;
    this.diffuseTexture = undefined;
    this.diffuseColor = undefined;
    this.specularColor = undefined;
    this.specularExponent = undefined;
    this.ambientColor = undefined;

    this.init = function(materialParameters){
        if (materialParameters.diffuseColor){
            this.diffuseColor = new Float32Array(materialParameters.diffuseColor);
        }
        if (materialParameters.specularColor){
            this.specularColor = new Float32Array(materialParameters.specularColor);
        }
        if (materialParameters.specularExponent){
            this.specularExponent = materialParameters.specularExponent;
        }
        if (materialParameters.ambientColor){
            this.ambientColor = new Float32Array(materialParameters.ambientColor);
        }
    }

    this.setMaterial = function (){
        if (this.diffuseTexture){
            gl.bindTexture(gl.TEXTURE_2D, this.diffuseTexture.textureId);
            gl.uniform1i(this.shader.diffuseTexture, 0);
        }
        if (this.diffuseColor){
            gl.uniform3fv(this.shader.diffuseColor, this.diffuseColor);
        }
        if (this.specularColor){
            gl.uniform3fv(this.shader.specularColor, this.specularColor);
        }
        if (this.specularExponent){
            gl.uniform1f(this.shader.specularExponent, this.specularExponent);
        }
        if (this.ambientColor){
            gl.uniform3fv(this.shader.ambientColor, this.ambientColor);
        }
    }
}

function WebGLTexture(gl, name){
    this.textureId = gl.createTexture();
    this.name = name;

    this.setImage = function(fullyLoadedImage){
        gl.pixelStorei(gl.UNPACK_FLIP_Y_WEBGL, true);
        gl.bindTexture(gl.TEXTURE_2D, this.textureId);
        gl.texImage2D(gl.TEXTURE_2D, 0, gl.RGBA, gl.RGBA, gl.UNSIGNED_BYTE, fullyLoadedImage);
        gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MAG_FILTER, gl.LINEAR);
        gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MIN_FILTER, gl.LINEAR_MIPMAP_NEAREST);
        gl.generateMipmap(gl.TEXTURE_2D);

        gl.bindTexture(gl.TEXTURE_2D, null);
    }
}

function WebGLSceneObject(name, transform, material){
    this.name = name;
    this.transform = transform;
    this.material = material;
    this.transform = undefined;
    this.material = undefined;
    this.light = undefined;
    this.camera = undefined;
    this.meshVertexNormalBuffer = undefined;
}

function WebGLTransform(localTranslation,localRotation,localScaling){
    this.localTranslation = localTranslation;
    this.localRotation = localRotation;
    this.localScaling = localScaling;
    this.children = new Array();
    var globalMatrix = undefined;
    var localMatrix = undefined;
    var localMatrixInverse = undefined;
    var globalMatrixInverse = undefined;
    this.parentTransform = undefined;
    
    this.setParent = function(newParent){
        if (newParent == this){
            throw('Cannot assign parent to self');
        }
        this.parentTransform = newParent;
        newParent.children.push(this);
    }

    this.getLocalMatrix = function(){
        if (typeof(localMatrix) == "undefined") {
            localMatrix = mat4.create();
            mat4.identity(localMatrix);
            mat4.rotateX(localMatrix, this.localRotation[0]*degreeToRadian);
            mat4.rotateY(localMatrix, this.localRotation[1]*degreeToRadian);
            mat4.rotateZ(localMatrix, this.localRotation[2]*degreeToRadian);
            mat4.translate(localMatrix, this.localTranslation);
            mat4.scale(localMatrix, this.localScaling);
        }
        return localMatrix;
    }

    this.getLocalMatrixInverse = function(){
        if (typeof(localMatrixInverse) == "undefined") {
            localMatrixInverse = mat4.create();
            mat4.identity(localMatrixInverse);
            if (this.localRotation[0] != 0.0){
                mat4.rotateX(localMatrixInverse, -this.localRotation[0]*degreeToRadian);
            }
            if (this.localRotation[1] != 0.0){
                mat4.rotateY(localMatrixInverse, -this.localRotation[1]*degreeToRadian);
            }
            if (this.localRotation[2] != 0.0 ){
                mat4.rotateZ(localMatrixInverse, -this.localRotation[2]*degreeToRadian);
            }
            mat4.translate(localMatrixInverse, [-this.localTranslation[0],-this.localTranslation[1],-this.localTranslation[2]]);
            // ignores scale, since it should currently not be used
        }
        return localMatrixInverse;
    }

    this.getGlobalMatrixInverse = function(){
        if (typeof(globalMatrixInverse) == "undefined") {
            this.calculateGlobalMatrixInverse();
        }
        return globalMatrixInverse;
    }

    this.getGlobalMatrix = function(){
        if (typeof(globalMatrix) == "undefined") {
            this.calculateGlobalMatrix();
        }
        return globalMatrix;
    }

    this.getGlobalRotationMatrix = function(){
        var globalRotationMatrix = mat4.create();
        mat4.set(this.getGlobalMatrix(), globalRotationMatrix);
        // remove transforms
        globalRotationMatrix[12] = globalRotationMatrix[13] = globalRotationMatrix[14] = 0;
        return globalRotationMatrix;
    }

    this.calculateGlobalMatrix = function(){
        globalMatrix = mat4.create();
        mat4.set(this.getLocalMatrix(), globalMatrix);

        var transformIterator = this.parentTransform;
        while (typeof(transformIterator) != "undefined"){
            mat4.multiply(globalMatrix, transformIterator.getLocalMatrix(),globalMatrix);
            transformIterator  = transformIterator.parentTransform;
        }
    }

    this.calculateGlobalMatrixInverse = function(){
        globalMatrixInverse = mat4.create();
        mat4.set(this.getLocalMatrixInverse(), globalMatrixInverse);
        var transformIterator = this.parentTransform;
        while (typeof(transformIterator) != "undefined"){
            mat4.multiply(globalMatrixInverse,transformIterator.getLocalMatrixInverse(),globalMatrixInverse);
            transformIterator  = transformIterator.parentTransform;
        }
    }

    this.markUpdated = function(){
        globalMatrix = undefined;
        localMatrix = undefined;
        localMatrixInverse = undefined;
        globalMatrixInverse = undefined;

        this.children.forEach(function(child){
            child.markUpdated();
        });
    }
}
function WebGLCamera(gl,cameraData,sceneObject){
    this.sceneObject = sceneObject;
    this.fieldOfView = cameraData.fieldOfView;
    this.near = cameraData.near;
    this.far = cameraData.far;
    this.cameraType = cameraData.cameraType;

    this.left = cameraData.left;
    this.right = cameraData.right;
    this.bottom = cameraData.bottom;
    this.top = cameraData.top;

    this.setupCamera = function (modelViewMatrix,projectionMatrix){
        var camera = this;
        if (camera.cameraType == "perspective"){
            mat4.perspective(camera.fieldOfView, gl.viewportWidth / gl.viewportHeight,
                    camera.near, camera.far, projectionMatrix);
        } else if (camera.cameraType == "ortogonale"){
            mat4.ortho(camera.left, camera.right, camera.bottom, camera.top,
                    camera.near, camera.far, projectionMatrix);
        } else {
            throw('Unknown cameraType . Supported types are \'perspective\' and \'ortogonale\'');
        }

        var globalMatrixInv = this.sceneObject.transform.getGlobalMatrixInverse();
        mat4.set(globalMatrixInv, modelViewMatrix);
    }
}

function WebGLLight(lightData,sceneObject){
    this.sceneObject = sceneObject;
    this.lightType = lightData.lightType;
    this.diffuse = new Float32Array(lightData.diffuse);
    this.specular = new Float32Array(lightData.specular);
    this.ambient = new Float32Array(lightData.ambient);
}
