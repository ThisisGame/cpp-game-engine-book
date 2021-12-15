# 导出骨骼权重，需要在Blender合并Mesh，然后选择Mesh，然后执行脚本。
import bpy
import bmesh
import os
import struct
import json
from mathutils import Matrix
from math import radians

# 引擎顶点结构(坐标、颜色、纹理坐标)
class EngineVertex:
    x=y=z=r=g=b=a=u=v=0

    def __init__(self,x,y,z,r,g,b,a,u,v):
        self.x=x
        self.y=y
        self.z=z
        self.r=r
        self.g=g
        self.b=b
        self.a=a
        self.u=u
        self.v=v
    
    def __eq__(self,other):
        equal= self.x==other.x and self.y==other.y and self.z==other.z and self.r==other.r and \
            self.g==other.g and self.b==other.b and self.a==other.a and self.u==other.u and self.v==other.v
        return equal

    def __str__(self) -> str:
        return "x:{0},y:{1},z:{2},r:{3},g:{4},b:{5},a:{6},u:{7},v:{8}".format(self.x,self.y,self.z,self.r,self.g,self.b,self.a,self.u,self.v)

    def __repr__(self) -> str:#显示属性 用来打印。
        return str(self)

    def write(self,f):
        f.write(struct.pack('f',self.x))
        f.write(struct.pack('f',self.y))
        f.write(struct.pack('f',self.z))
        f.write(struct.pack('f',self.r))
        f.write(struct.pack('f',self.g))
        f.write(struct.pack('f',self.b))
        f.write(struct.pack('f',self.a))
        f.write(struct.pack('f',self.u))
        f.write(struct.pack('f',self.v))

# 显示信息框
def ShowMessageBox(message = "", title = "Message Box", icon = 'INFO'):
    def draw(self, context):
        self.layout.label(text=message)
    bpy.context.window_manager.popup_menu(draw, title = title, icon = icon)

# 当前选中的Mesh
object=bpy.context.object
mesh = object.data
uv_layer = mesh.uv_layers.active.data
    
print("---- COLLECT MESH -----")
# 收集顶点
mEngineVertexes=[]
for v in mesh.vertices:
    engine_vertex=EngineVertex(v.co.x,v.co.z,v.co.y,1,1,1,1,0,0)#构造引擎顶点，注意 y z调换
    mEngineVertexes.append(engine_vertex)

# 收集UV
mEngineVertexIndexes=[]
for poly in mesh.polygons:#遍历多边形
    # print("Polygon index: %d, length: %d" % (poly.index, poly.loop_total))#检查是三角形还是四边形
    if poly.loop_total==4:
        ShowMessageBox("Need Triangle","Polygon Error",  'ERROR')
        break
    # 遍历一个多边形的3个顶点
    for loop_index in range(poly.loop_start, poly.loop_start + poly.loop_total):
        vertex_index=mesh.loops[loop_index].vertex_index #顶点在Blender中的索引
        mEngineVertexIndexes.append(vertex_index)#把index放入顶点索引表

        v=mesh.vertices[vertex_index] #拿顶点索引获取顶点
        uv=uv_layer[loop_index].uv #获取当前顶点的UV坐标
        engine_vertex=mEngineVertexes[vertex_index]#补全顶点UV数据
        engine_vertex.u=uv.x
        engine_vertex.v=uv.y

print(*mEngineVertexes,sep='\n')
print(mEngineVertexIndexes)

print("---------- COLLECT BONE ------------")
# 收集骨骼名字 [bone_name , bone_name , ...]
boneNames=[]
armature_obj = bpy.data.objects["Armature"]
for i in range(0, len(armature_obj.data.bones)):
    bone = armature_obj.data.bones[i]
    boneNames.append(bone.name)
print(boneNames)

print("---------- COLLECT WEIGHT ------------")
# 收集骨骼权重
armature_obj = bpy.data.objects["Armature"]
obj_verts = mesh.vertices#当前选中的物体的所有顶点

obj_group_names = [g.name for g in object.vertex_groups]# 获取当前Mesh 关联的所有的顶点组名称

print(obj_group_names)

for bone in armature_obj.pose.bones:
    print(bone.name)
    if bone.name not in obj_group_names:
        continue

    # 获取骨骼的顶点组序号
    gidx = object.vertex_groups[bone.name].index
    # 获取骨骼的顶点组关联的顶点
    bone_verts = [v for v in obj_verts if gidx in [g.group for g in v.groups]]
    # 遍历顶点
    for v in bone_verts:
        for g in v.groups:
            if g.group == gidx: 
                w = g.weight
                print('Vertex',v.index,'has a weight of',w,'for bone',bone.name)




# if os.path.exists("export")==False:
#     os.mkdir("export")
    
# dir_path="export/"+object.name_full

# if os.path.exists(dir_path)==False:
#     os.mkdir(dir_path)
    
# with open(dir_path+"/"+object.name_full+'.mesh', 'wb') as f:
#     #写入文件头
#     f.write("mesh".encode())
#     f.write(struct.pack('H',len(kVertexRemoveDumplicateVector)))
#     f.write(struct.pack('H',len(kVertexIndexVector)))
    
#     #写入顶点数据
#     for engine_vertex in kVertexRemoveDumplicateVector:
#         engine_vertex.write(f)

#     #写入索引数据
#     kVertexIndexVectorInverse=kVertexIndexVector[::-1]
#     for engine_vertex_index in kVertexIndexVectorInverse:
#         f.write(struct.pack('H',engine_vertex_index))
    
#     f.close()
    
# print("----SUCCESS-----------------------------------")