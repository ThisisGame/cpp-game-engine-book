import QtQuick 2.7
import Painter 1.0

// Root object for the plugin
PainterPlugin
{
	// Disable update and server settings
	// since we don't need them
	tickIntervalMS: -1 // Disabled Tick
	jsonServerPort: -1 // Disabled JSON server

	// 插件加载完毕回调
	Component.onCompleted:
	{
		// 将 toolbar.qml 描述的按钮，添加到工具栏
		var InterfaceButton = alg.ui.addToolBarWidget("toolbar.qml");

		// 给按钮加事件
		if( InterfaceButton )
		{
			InterfaceButton.clicked.connect( sayHello );
		}
	}
	
	function sayHello()
	{
		alg.log.info( "hello world")
	}
}