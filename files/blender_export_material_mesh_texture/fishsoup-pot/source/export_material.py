import bpy
import json
import os
from shutil import copyfile

print("----COLLECT-----------------------------------")

object = bpy.context.object
print("Object:",object.name_full)

for materialSlot in object.material_slots:
    material=materialSlot.material
    print("Material:",material.name_full)
    
    print(" ","Params:")
    print(" ","blend_method:",material.blend_method)
    print(" ","diffuse_color:",material.diffuse_color[0],material.diffuse_color[1],material.diffuse_color[2],material.diffuse_color[3])
    print(" ","metallic:",material.metallic)
    print(" ","pass_index:",material.pass_index)
    print(" ","line_color:",material.line_color[0],material.line_color[1],material.line_color[2],material.line_color[3])
    print(" ","line_priority:",material.line_priority)
    print(" ","roughness:",material.roughness)
    print(" ","shadow_method:",material.shadow_method)
    print(" ","specular_color:",material.specular_color)
    print(" ","specular_intensity:",material.specular_intensity)
    print(" ","use_backface_culling:",material.use_backface_culling)
    print(" ","use_nodes:",material.use_nodes)
    print(" ","use_screen_refraction:",material.use_screen_refraction)
    print(" ","use_sss_translucency:",material.use_sss_translucency)
    
    

    print("Nodes:")
    for node in material.node_tree.nodes:
        print(" ",node,node.type)
        if node.type=="TEX_IMAGE":
            print(" "," ",node.image.filepath,node.image.file_format)
            
print("----EXPORT-----------------------------------")

if os.path.exists("export")==False:
    os.mkdir("export")
    
dir_path="export/"+object.name_full

if os.path.exists(dir_path)==False:
    os.mkdir(dir_path)
    


for materialSlot in object.material_slots:
    material=materialSlot.material
    print("Material:",material.name_full)
    
    export_material={
        "blend_method":material.blend_method,
        "diffuse_color":"{},{},{},{}".format(material.diffuse_color[0],material.diffuse_color[1],material.diffuse_color[2],material.diffuse_color[3]),
        "metallic":material.metallic,
        "roughness":material.roughness,
        "specular_color":"{},{},{}".format(material.specular_color.r,material.specular_color.g,material.specular_color.b),
        "specular_intensity":material.specular_intensity,
        "image_paths":[]
    }
    
    for node in material.node_tree.nodes:
        if node.type=="TEX_IMAGE":
            imgpath=node.image.filepath
            imgpath=imgpath.replace("//","").replace("\\","/")
            
            
            copyfile(imgpath,dir_path+"/"+os.path.split(imgpath)[1])
            export_material["image_paths"].append(os.path.split(imgpath)[1])
            
    print(json.dumps(export_material,indent=2))
    
    with open(dir_path+"/"+material.name_full+'.json', 'w') as f:
        json.dump(export_material, f,indent=2)
        
print("----SUCCESS-----------------------------------")