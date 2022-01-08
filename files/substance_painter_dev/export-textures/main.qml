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
			InterfaceButton.clicked.connect( exportTextures );
		}
	}
	
	function exportTextures()
	{
		try
		{ 
			// 没打开项目那就算了
			if( !alg.project.isOpen() )
			{
				return;
			}

			// 检索当前选择的纹理集和子堆栈
			var MaterialPath = alg.texturesets.getActiveTextureSet()
			var UseMaterialLayering = MaterialPath.length > 1
			var TextureSetName = MaterialPath[0]
			var StackName = ""

			if( UseMaterialLayering )
			{
				StackName = MaterialPath[1]
			}

			// Retrieve the Texture Set information
			var Documents = alg.mapexport.documentStructure()
			var Resolution = alg.mapexport.textureSetResolution( TextureSetName )
			var Channels = null

			for( var Index in Documents.materials )
			{
				var Material = Documents.materials[Index]

				if( TextureSetName == Material.name )
				{
					for( var SubIndex in Material.stacks )
					{
						if( StackName == Material.stacks[SubIndex].name )
						{
							Channels = Material.stacks[SubIndex].channels
							break
						}
					}
				}
			}

			// Create the export settings
			var Settings = {
				"padding":"Infinite",
				"dithering":"disbaled", // Hem, yes...
				"resolution": Resolution,
				"bitDepth": 16,
				"keepAlpha": false
			}

			// 导出后存放的目录
			var BasePath = alg.fileIO.urlToLocalFile( alg.project.url() )
			BasePath = BasePath.substring( 0, BasePath.lastIndexOf("/") );

			// Export the each channel
			for( var Index in Channels )
			{
				// Create the stack path, which defines the channel to export
				var Path = Array.from( MaterialPath )
				Path.push( Channels[Index] )

				// Build the filename for the texture to export
				var Filename = BasePath + "/" + TextureSetName

				if( UseMaterialLayering )
				{
					Filename += "_" + StackName
				}

				Filename += "_" + Channels[Index] + ".png"

				// Perform the export
				alg.mapexport.save( Path, Filename, Settings )
				alg.log.info( "Exported: " + Filename )
			}
		}
		catch( error )
		{
			// Print errors in the log window
			alg.log.exception( error )
		}
	}
}