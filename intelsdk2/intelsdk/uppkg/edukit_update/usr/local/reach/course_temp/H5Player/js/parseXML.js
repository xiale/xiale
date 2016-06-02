/*!- html5 player v1.0.0.1 szreach.com -*/

var loadXML = function(fileRoute) {
	var xmlDoc = null;
	try {
		if (!!window.ActiveXObject || "ActiveXObject" in window) {
			xmlDoc = new ActiveXObject('Msxml2.DOMDocument');
			xmlDoc.async = false;
			xmlDoc.load(fileRoute);
		} else if (document.implementation && document.implementation.createDocument) {
			var xmlhttp = new window.XMLHttpRequest();
			xmlhttp.open("GET",fileRoute,false);
			xmlhttp.send(null);
			var xmlDoc = xmlhttp.responseXML.documentElement;
		} else {
			xmlDoc = null;
		}
	} catch (e) {
		//console.error(e);
	} finally {
		return xmlDoc;
	}
}

var videoLevel;

var infoXML = loadXML("./HD/resource/info.xml");

if (infoXML != null && getChannel() > 0) {
	videoLevel = "HD";
	var tmpInfoXML = loadXML("./SD/resource/info.xml");
	var tmpChannels = tmpInfoXML.getElementsByTagName("channel");
	if (tmpInfoXML != null && tmpChannels.length > 0) {
		document.getElementById("sd_button").style.display = "block";
		document.getElementById("hd_button").style.display = "none";
	}
} else {
	infoXML = loadXML("./SD/resource/info.xml");
}

function getFilesType(i) {
	var channel = infoXML.getElementsByTagName("channel")[i];
	var files = channel.getElementsByTagName("files")[0];
	return files.getAttribute("type");
}

function getChannel() {
	var channels = infoXML.getElementsByTagName("channel");
	var retChannels = 0;
	for (var i = 0; i < channels.length; i++) {
		var type = getFilesType(i);
		if (type == 'jpg') {
			continue;
		}
		retChannels++;
	}
	return retChannels;
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

function getChannelThumbnail(channel) {
	var thumbnail = "";
	var channel = infoXML.getElementsByTagName("channel")[channel];
	var thumbnail = channel.getAttribute("thumbnail");
	if (videoLevel == "HD") {
		return "./HD/resource/images/" + thumbnail;
	} else {
		return "./SD/resource/images/" + thumbnail;
	}
}

function getChannelStartTime(channel) {
	var startTime = 0;
	var channel = infoXML.getElementsByTagName("channel")[channel];
	var sTime = channel.getAttribute("starttime");
	if (sTime) {
		startTime = sTime;
	}
	return parseFloat(startTime);
}

function getFileStartTime(channel, file) {
	var startTime = 0;
	var channel = infoXML.getElementsByTagName("channel")[channel];
	var files = channel.getElementsByTagName("files")[0];
	var sTime = files.getElementsByTagName("file")[file].getAttribute("starttime");
	if (sTime) {
		startTime = sTime;
	}
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
	if (videoLevel == "HD") {
		return "./HD/resource/" + videoSrc;
	} else {
		return "./SD/resource/" + videoSrc;
	}
}

var indexXML;

if (videoLevel == "HD") {
	indexXML = loadXML("./HD/resource/index.xml");
} else {
	indexXML = loadXML("./SD/resource/index.xml");
}

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
