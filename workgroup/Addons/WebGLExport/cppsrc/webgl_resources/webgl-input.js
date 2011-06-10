/// License: Creative Commons Attribution 3.0 Unported (http://creativecommons.org/licenses/by/3.0/)
/// 2011 Morten Nobel-Joergensen / Vaida Laganeckiene
/// https://github.com/mortennobel/SoftimageWebGLExport

function WebGLInput(webglRenderer){
    window.oncontextmenu = function(){return false};
    var canvas = webglRenderer.getCanvas();
    var leftMouseTimer = undefined;
    var rightMouseTimer = undefined;
    var middleMouseTimer = undefined;
    var mousePositionX = 0;
    var mousePositionY = 0;
    var mousePositionXNew = 0;
    var mousePositionYNew = 0;
    this.rotationSpeed = 0.4;
	this.movementSpeed = 0.3;

    var doMove = function(dir){
        var camera = webglRenderer.getCamera();
        var movementDirection = [0.0,0.0,-this.movementSpeed*dir];
        var cameraTransform = camera.sceneObject.transform;

        var rotation = cameraTransform.getGlobalRotationMatrix();

        var movement = mat4.multiplyVec3(rotation,movementDirection);
        cameraTransform.localTranslation = vec3.add(movement,cameraTransform.localTranslation);

        cameraTransform.markUpdated();
        webglRenderer.sceneDraw();
    }

    var move = function(){
        doMove(1);
    }

    var moveBack = function(){
        doMove(-1);
    }
    var drag = function(){
        var deltaMouseX = mousePositionXNew-mousePositionX;
        var deltaMouseY = mousePositionYNew-mousePositionY;

        var camera = webglRenderer.getCamera();
        var cameraTransform = camera.sceneObject.transform;

        if (deltaMouseX != 0 || deltaMouseY != 0){
            var rotation = cameraTransform.localRotation;
            rotation[1] += deltaMouseX*this.rotationSpeed;
            rotation[0] += deltaMouseY*this.rotationSpeed;
            if (rotation[0]>89.9){
                rotation[0]=89.9;
            } else if (rotation[0]<-89.9){
                rotation[0]=-89.9;
            }
            cameraTransform.markUpdated();
        }

        mousePositionX = mousePositionXNew;
        mousePositionY = mousePositionYNew;
        webglRenderer.sceneDraw();
    }

    var mouseMove = function(event){
        mousePositionXNew = event.pageX;
        mousePositionYNew = event.pageY;

    }

    var mouseDown = function(event){
        if (event.button==0){
            leftMouseTimer = setInterval ( move, 0.5 );
        }
        if (event.button==1){
            middleMouseTimer = setInterval ( moveBack, 0.5 );
        }
        if (event.button==2){
            mousePositionX = event.pageX;
            mousePositionY = event.pageY;

            mousePositionXNew = mousePositionX;
            mousePositionYNew = mousePositionY;

            rightMouseTimer = setInterval ( drag, 0.5 );
            canvas.addEventListener('mousemove',mouseMove,0);
        }

    }
    var mouseUp = function(event){
        if (event.button==0){
            if (leftMouseTimer){
                clearInterval(leftMouseTimer);
                leftMouseTimer = undefined;
            }
        }
        if (event.button==1){
            if (middleMouseTimer){
                clearInterval(middleMouseTimer);
                middleMouseTimer = undefined;
            }
        }
        if (event.button==2){
            if (rightMouseTimer){
                clearInterval(rightMouseTimer);
                rightMouseTimer = undefined;
                canvas.removeEventListener('mousemove',mouseMove,0);
            }
        }
    }

    canvas.addEventListener('mousedown',mouseDown,0);
    canvas.addEventListener('mouseup',mouseUp,0);
}