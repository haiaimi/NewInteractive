# WarSimulation
触摸屏交互，战场模拟

 * 本项目目的是三维战场模拟，初始阶段就先实现触摸屏交互，还有很多后续的东西需要做
 
 ### 1、五指呼出右侧菜单
  * 五指放大就是呼出菜单，五指收缩就是收起菜单
  
  ![image](https://github.com/haiaimi/PictureRepository/blob/master/PictureRepository/WarSimulate/%E4%BA%94%E6%8C%87%E5%91%BC%E5%87%BA%E8%8F%9C%E5%8D%95.gif)
  
  ### 2、长按预览
  * 长按生成按钮就可以预览要生成的目标，可以进行一些旋转操作
  
  ![image](https://github.com/haiaimi/PictureRepository/blob/master/PictureRepository/WarSimulate/%E9%95%BF%E6%8C%89%E9%A2%84%E8%A7%88.gif)
  
  ### 3、放入抽屉
  * 把目标物体放入到抽屉，抽屉会按序排放
  
  ![image](https://github.com/haiaimi/PictureRepository/blob/master/PictureRepository/WarSimulate/%E6%8B%96%E6%8B%BD%E8%87%B3%E6%8A%BD%E5%B1%89.gif)
  
  ### 4、抽屉的呼入呼出
  * 直接进行拖拽就可以进行呼入呼出
  
  ![image](https://github.com/haiaimi/PictureRepository/blob/master/PictureRepository/WarSimulate/%E6%8A%BD%E5%B1%89%E5%91%BC%E5%87%BA%E6%94%B6%E8%B5%B7.gif)
  
  ### 5、单个物体删除
  * 长按目标物体就可以删除
  
  ![image](https://github.com/haiaimi/PictureRepository/blob/master/PictureRepository/WarSimulate/%E5%8D%95%E4%B8%AA%E5%88%A0%E9%99%A4.gif)
  
  ### 6、批量删除
  * 多选框呈现是使用Slate实现，多选功能实现其实就是物体与多面体相交测试，多面体就是把平面中的长方形转换到三维世界中的平截头体
  * 可以框选多个物体然后长按删除
  
  ![image](https://github.com/haiaimi/PictureRepository/blob/master/PictureRepository/WarSimulate/%E6%89%B9%E9%87%8F%E5%88%A0%E9%99%A4.gif)
  
  ### 7、地形拖拽
  * 可以拖拽地形（当然实际是视角摄像机的位置变换）
  ![image](https://github.com/haiaimi/PictureRepository/blob/master/PictureRepository/WarSimulate/%E5%9C%B0%E5%BD%A2%E6%8B%96%E6%8B%BD.gif)
  
  ### 8、地形缩放
  * 同样是摄像机的空间位置变换
  ![image](https://github.com/haiaimi/PictureRepository/blob/master/PictureRepository/WarSimulate/%E5%9C%B0%E5%BD%A2%E7%BC%A9%E6%94%BE.gif)
  
  ### 9、添加选择控制目标的按钮
  * 可以通过弹出菜单选择控制的目标，不然Controller无法指定Pawn
  ![image]()
  
  ### 10、切换到控制画面
  * 由俯视观察模式切换到控制飞机的画面（这只是粗糙的显示一下，还没有设置相应的机场）
  ![image]()
  
  ### 11、飞机在飞机状态
  * 飞行的动作以及显控状态变化，这是使用的UI材质，但然也可以使用正常的模型材质，线控画面以平面模型显示（还没有加飞机的尾焰）
  ![image]()
  
  ### 12、选择僚机
  * 同样可以通过弹出菜单选择自己的僚机，右下角的小地图就是用来显示僚机飞机画面
  ![image]()
  
  ### 13、跟飞状态
  * 僚机会跟随玩家控制的飞，包括一些左右倾斜上仰等动作
  ![image]()
  
  ### 14、多架僚机
  * 当然可以款选多架飞机作为自己的僚机
  ![image]()
