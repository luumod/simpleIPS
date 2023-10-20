## 这是对于QtCreator的运行支持

可能会有少许Bug，具体配置请查看 pro_simpleIPS.pro 文件。

1. 删除 `.pro` 文件以下的内容，并且添加你自己的opencv环境。
```
win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../Tools/openCV/openCV/build/x64/vc16/lib/ -lopencv_world470
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../Tools/openCV/openCV/build/x64/vc16/lib/ -lopencv_world470d

INCLUDEPATH += $$PWD/../../../Tools/openCV/openCV/build/include
DEPENDPATH += $$PWD/../../../Tools/openCV/openCV/build/include
```
2. opencv添加方法: 首先添加你的opencv `bin` 环境变量，然后右击项目，选择 `添加库`
   ![image](https://github.com/luumod/simpleIPS/assets/93420580/4f088d7c-d72b-4894-9b6b-261c18fb1953)
`外部库`，然后配置 `库文件`和 `包含文件`的路径
![image](https://github.com/luumod/simpleIPS/assets/93420580/ff086c36-4be7-40f3-9c3f-6e51602bddc7)
取消勾选 linux macos，并且选择 `后缀自动添加d`

3. 关键步骤：
需要把 `DESTDIR = F:/code/simplePS_work/pro_simpleIPS` 改为你自己的QtCreator的项目主文件目录
例如，就是改成这个位置：
![image](https://github.com/luumod/simpleIPS/assets/93420580/7ce976cf-73ed-4f54-b6b9-87b2c211c7f8)
因为exe文件需要**直接读取** resource文件夹下的资源，如图标等等，如果不修改则无法加载图标资源
