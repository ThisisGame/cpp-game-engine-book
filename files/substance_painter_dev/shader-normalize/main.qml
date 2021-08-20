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
			InterfaceButton.clicked.connect( normalizeShader );
		}
	}
	
	function normalizeShader()
	{
		alg.log.info( "normalizeShader")
		
		//输出shader参数组列表(参数是分组的)
		alg.log.info(alg.shaders.groups(0));
		
		//遍历所有shader实例，输出shader名字和id
		alg.shaders.instances().forEach(function(shader) {
			alg.log.info(shader.label + ": " + shader.id);
		});
		
		//找到使用这个shader的材质(测试发现没有被使用？？)
		var materials = alg.shaders.materials(0);
		for (var key in materials) {
			alg.log.info("%1: %2".arg(key).arg(materials[key].value));
		}
		
		//打印shader所有参数
		//var parameters = alg.shaders.parameters(0);
		//alg.log.info(parameters);
		//在面板上找到group，然后打印shader指定group的参数
		alg.log.info(alg.shaders.parameters(0, "Coat"));
		
		//获取参数值
		var parameter=alg.shaders.parameter(0, "sssScale");
		alg.log.info("%1: %2".arg(parameter.description.label).arg(parameter.value));
		
		//设置shader参数
		alg.shaders.setParameters(0, {
			coatEnabled: true,
			ao_intensity: 0.5,
			sssScale: 0.1
		});

	}
}