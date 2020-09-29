
BLENDER="C:\Program Files\Blender Foundation\Blender 2.90\blender.exe"
DIST=../dist

all : \
    $(DIST)/bopit.pnct \
    $(DIST)/bopit.scene \

$(DIST)/bopit.scene : bopit.blend export-scene.py
    $(BLENDER) --background --python export-scene.py -- "bopit.blend:Main" "$(DIST)/bopit.scene"

$(DIST)/bopit.pnct : bopit.blend export-meshes.py
    $(BLENDER) --background --python export-meshes.py -- "bopit.blend:Main" "$(DIST)/bopit.pnct" 
