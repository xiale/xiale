loadXML = function(fileRoute){
    var xmlDoc = null;
	if (window.ActiveXObject) {
        xmlDoc = new ActiveXObject('Msxml2.DOMDocument');
        xmlDoc.async = false;
        xmlDoc.load(fileRoute);
    } else if (document.implementation && document.implementation.createDocument){
        var xmlhttp = new window.XMLHttpRequest();
        xmlhttp.open("GET",fileRoute,false);
        xmlhttp.send(null);
        var xmlDoc = xmlhttp.responseXML.documentElement; 
    } else {
    	xmlDoc = null;
	}
    return xmlDoc;
}

var infoXML = loadXML("info.xml");

function getChannel() {
	var channels = infoXML.getElementsByTagName("channel");
	return channels.length;
}

function getWidth(channel, file) {
	var channel = infoXML.getElementsByTagName("channel")[channel];
	var files = channel.getElementsByTagName("files")[0];
	var width = files.getElementsByTagName("file")[file].getAttribute("width");
	return parseInt(width);	
}

function getHeight(channel, file) {
	var channel = infoXML.getElementsByTagName("channel")[channel];
	var files = channel.getElementsByTagName("files")[0];
	var height = files.getElementsByTagName("file")[file].getAttribute("height");
	return parseInt(height);	
}

function getChannelStartTime(channel) {
	var channel = infoXML.getElementsByTagName("channel")[channel];
	var startTime = channel.getAttribute("starttime");
	return parseFloat(startTime);
}

function getFileStartTime(channel, file) {
	var channel = infoXML.getElementsByTagName("channel")[channel];
	var files = channel.getElementsByTagName("files")[0];
	var startTime = files.getElementsByTagName("file")[file].getAttribute("starttime");
	return parseFloat(startTime);
}

function getSectionTime(channel, file) {
	var channel = infoXML.getElementsByTagName("channel")[channel];
	var files = channel.getElementsByTagName("files")[0];
	var sectionTime = files.getElementsByTagName("file")[file].getAttribute("totaltime");
	return parseFloat(sectionTime);	
}

function getTotalTime(channelNo) {
	var totalTime = 0;
	var channels =  getChannel();
	if (channels >= 1) {
		var channel = infoXML.getElementsByTagName("channel")[channelNo];
		var files = channel.getElementsByTagName("files")[0];
		var file = files.getElementsByTagName("file");
		for (var i = 0; i < file.length; i++) {
			totalTime += getSectionTime(0, i);
		}
	}
	return parseFloat(totalTime);
}

function getVideoNum(channel) {
	var channel = infoXML.getElementsByTagName("channel")[channel];
	var files = channel.getElementsByTagName("files")[0];
	var file = files.getElementsByTagName("file");
	return parseInt(file.length);
}

function getVideoSrc(channel, file) {
	var channel = infoXML.getElementsByTagName("channel")[channel];
	var files = channel.getElementsByTagName("files")[0];
	var file = files.getElementsByTagName("file")[file];
	var videoSrc = file.childNodes[0].nodeValue;
	return videoSrc;	
}

var indexXML = loadXML("index.xml");

function xGetIndexNumber() {
	var indexs = indexXML.getElementsByTagName("index");
	return indexs.length;
}

function xGetIndexTime(index) {
	var time = indexXML.getElementsByTagName("index")[index].getAttribute("time");
	return parseInt(time);
}

function xGetIndexImage(index) {
	var image = indexXML.getElementsByTagName("index")[index].getAttribute("image");
	return image;
}

function xGetIndexContent(index) {
	var index = indexXML.getElementsByTagName("index")[index];
	var content = index.childNodes[0].nodeValue;
	return content;
}
