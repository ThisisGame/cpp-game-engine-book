import bpy
import bmesh
import os
import struct


class EngineVertex:
    x=0
    y=0
    z=0
    r=0
    g=0
    b=0
    a=0
    u=0
    v=0
    
    def __eq__(self,other):
        return self.x==other.x and  self.y ==other.y and  self.z ==other.z and  self.r ==other.r and  self. g==other.g and  self.b ==other.b and  self.a ==other.a and  self.u ==other.u and  self.v ==other.v


def ShowMessageBox(message = "", title = "Message Box", icon = 'INFO'):
    def draw(self, context):
        self.layout.label(text=message)
    bpy.context.window_manager.popup_menu(draw, title = title, icon = icon)

# Get the active mesh
object=bpy.context.object
mesh = object.data
uv_layer = mesh.uv_layers.active.data
    
print("----EXPORT-----------------------------------")


kVertexRemoveDumplicateVector=[]
kVertexIndexVector=[]

for poly in mesh.polygons:
    print("Polygon index: %d, length: %d" % (poly.index, poly.loop_total))
    if poly.loop_total==4:
        ShowMessageBox("Need Triangle","Polygon Error",  'ERROR')
        break
    
    # range is used here to show how the polygons reference loops,
    # for convenience 'poly.loop_indices' can be used instead.
    for loop_index in range(poly.loop_start, poly.loop_start + poly.loop_total):
        vertex_index=mesh.loops[loop_index].vertex_index
        v=mesh.vertices[vertex_index]
        uv=uv_layer[loop_index].uv

        
        engine_vertex=EngineVertex()
        engine_vertex.x=v.co.x
        engine_vertex.y=v.co.z #注意 y z调换
        engine_vertex.z=v.co.y
        engine_vertex.r=1
        engine_vertex.g=1
        engine_vertex.b=1
        engine_vertex.a=1
        engine_vertex.u=uv.x
        engine_vertex.v=uv.y
        
        
        #判断顶点是否存在
        find_engine_vertex_index=-1
        for engine_vertex_index in range(len(kVertexRemoveDumplicateVector)):
            if kVertexRemoveDumplicateVector[engine_vertex_index]==engine_vertex:
                find_engine_vertex_index=engine_vertex_index
                break
        
        if find_engine_vertex_index<0:
            find_engine_vertex_index=len(kVertexRemoveDumplicateVector)
            kVertexRemoveDumplicateVector.append(engine_vertex)
            
        kVertexIndexVector.append(find_engine_vertex_index)
        
print(kVertexRemoveDumplicateVector)
print(kVertexIndexVector)


if os.path.exists("export")==False:
    os.mkdir("export")
    
dir_path="export/"+object.name_full

if os.path.exists(dir_path)==False:
    os.mkdir(dir_path)
    
with open(dir_path+"/"+object.name_full+'.mesh', 'wb') as f:
    #写入文件头
    f.write("mesh".encode())
    f.write(struct.pack('H',len(kVertexRemoveDumplicateVector)))
    f.write(struct.pack('H',len(kVertexIndexVector)))
    
    #写入顶点数据
    for engine_vertex in kVertexRemoveDumplicateVector:
        f.write(struct.pack('f',engine_vertex.x))
        f.write(struct.pack('f',engine_vertex.y))
        f.write(struct.pack('f',engine_vertex.z))
        f.write(struct.pack('f',engine_vertex.r))
        f.write(struct.pack('f',engine_vertex.g))
        f.write(struct.pack('f',engine_vertex.b))
        f.write(struct.pack('f',engine_vertex.a))
        f.write(struct.pack('f',engine_vertex.u))
        f.write(struct.pack('f',engine_vertex.v))

    #写入索引数据
    for engine_vertex_index in kVertexIndexVector:
        f.write(struct.pack('H',engine_vertex_index))
    
    f.close()
    
print("----SUCCESS-----------------------------------")