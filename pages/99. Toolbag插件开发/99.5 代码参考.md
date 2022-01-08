## 99.5 代码参考

导入mset
```python
import mset
```

获取场景文件路径
```python
print("Scene Path:\n" + mset.getScenePath() + "\n")
```

遍历场景物体
```python
scene = mset.getAllObjects()
for obj in scene:
  print(obj.name + "\n")
```

创建材质并设置Normal Map
```python
mat = mset.Material("CustomMat")
mat.getSubroutine("surface").setField("Normal Map", mset.Texture("data/gui/title/loginwindow.png"))
```
获取所有材质
```python
materials = mset.getAllMaterials()
```

材质名字
```python
mat.name == "CustomMat"
```

----
创建UI

```python
import mset

#创建 window
mywindow = mset.UIWindow("My Window")

#button 点击回调
def doSomething():
   print("You pressed a button!")
   mset.shutdownPlugin() # 关闭插件

#创建 button ，设置回调
mybutton = mset.UIButton("My Button")
mybutton.onClick = doSomething

#将 button 添加到window
mywindow.addElement( mybutton )
```

Label
```python
window.addElement(mset.UILabel("Folder"))
```

滑块Slider
```python
slider = mset.UISliderInt(min=5, max=128, name="Slider")
```

Button设置图片
```python
icon_button = mset.UIButton()
icon_button.setIcon(os.path.abspath(os.path.join(os.curdir, "data/gui/control/animationplay.tga")))
```

颜色拾取
```python
picker = mset.UIColorPicker("Color")
```

输入框
```python
text = mset.UITextField()
```

单选框CheckBox
```python
checkbox = mset.UICheckBox()
```

间隔
```python
mywindow.addSpace(16)
```

换行开始绘制控件
```python
drawer_window.addReturn()
```

滚动列表
```python
scrollbox = mset.UIScrollBox()
scrollbox_window = mset.UIWindow(name="", register=False)
scrollbox.containedControl = scrollbox_window
mywindow.addElement(scrollbox)
```

折叠展开
```python
drawer = mset.UIDrawer(name="Settings")
drawer_window = mset.UIWindow(name="", register=False)
drawer.containedControl = drawer_window
mywindow.addElement(drawer)
```

打开文件选择框
```python
path = mset.showOpenFolderDialog()
```

----

将物体分组
```python
import mset

# 创建多个物体
light = mset.LightObject()
fog = mset.FogObject()
objs = [ light, fog ]

# 将上面的物体分组
group = mset.groupObjects(objs)
group.name = "Custom Group"
```

```python
# 设置物体旋转

```

```python
# 聚焦到物体
def focus_camera(self, target):
  objs = mset.getAllObjects()
  meshCount = 0
  for o in objs:
      if isinstance(o,mset.MeshObject):
          meshCount += 1
          if meshCount > 1:
              return
  mset.frameObject(target)
```
