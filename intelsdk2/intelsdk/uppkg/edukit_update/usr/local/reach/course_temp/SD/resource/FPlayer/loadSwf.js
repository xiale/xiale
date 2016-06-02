var flashvars = {
	playModeType:0, // 0表示点播，1表示直播
	videoLevel:"sd", // 默认的播放码流类型
	recordType:0, // 此参数暂时没用
	sdVideoUrl: "../../../SD/resource/", // 标清url
	hdVideoUrl: "../../../HD/resource/", // 高清url
	sdStreamerUrl:"", // 标清stream
	hdStreamerUrl:"", // 高清stream
	sdFileUrl: "", // 标清file
	hdFileUrl: "", // 高清file
	isShowControlMenu:"true", // 是否显示控制菜单
	isShowPoint:"true", // 是否显示打点信息
	pointInfoUrl:"index.xml", // 索引配置文件
	videoInfoUrl:"info.xml", // 媒体资源配置文件
	startPlayTime:0, // 开始播放时间
	isShowHSBtn:"true", // 是否显示高标清切换按钮
	connectWaitTime:20, // 无流等待时间
	onEnd:'onEnd', // 视频播放完毕执行方法
	onStart:'loadPage', // 视频加载完毕执行方法
	passKey:"passKey", // 备用参数，rtmp请求时会追加此参数
	isDebug:"false" // 是否开启debug调试信息的输出
};

var swfUrlStr = "FPlayer/FPlayer.swf";
function checkLocalOrNet()
{
	var str = window.location.href;
	var fileNum = str.indexOf("file");
	if( fileNum == 0)
	{
		str = deleteURL(3,str);
		flashvars.sdVideoUrl = str + "/SD/resource/";
		flashvars.hdVideoUrl = str + "/HD/resource/";
		flashvars.sdFileUrl= "";
		flashvars.hdFileUrl= "";
		flashvars.sdStreamerUrl= "";
		flashvars.hdStreamerUrl= "";
		swfUrlStr = "FPlayer/FPlayer_localhost.swf"
		flashvars.playModeType = 2;
	}
	loadSwf();
}
function deleteURL(num,url)
{
	var pos = 0;
	for( var i =  0; i < num; i++ )
	{
		pos = url.lastIndexOf("/");
		url = url.substr(0,pos);
	}
	return url;
}
function loadSwf()
{
	var swfVersionStr = "10.2.0";
	// To use express install, set to playerProductInstall.swf, otherwise the empty string. 
	var xiSwfUrlStr = "playerProductInstall.swf";
	
	var params = {};
	params.quality = "high";
	params.bgcolor = "#000000";
	params.allowscriptaccess = "sameDomain";
	params.allowfullscreen = "true";
	var attributes = {};
	attributes.id = "MultiStreamPlayer";
	attributes.name = "MultiStreamPlayer";
	attributes.align = "middle";
	swfobject.embedSWF(
	    swfUrlStr+"?type="+Math.random(), "flashContent", 
	    "100%", "100%", 
	    swfVersionStr, xiSwfUrlStr, 
	    flashvars, params, attributes);
	                 //-use-network=false  
	// JavaScript enabled so display the flashContent div in case it is not replaced with a swf object.
	swfobject.createCSS("#flashContent", "display:block;text-align:left;");
}
function openEdit()
{
	setUrlAndPlayType(JsActionType.OPEDITMODE,{});
}
