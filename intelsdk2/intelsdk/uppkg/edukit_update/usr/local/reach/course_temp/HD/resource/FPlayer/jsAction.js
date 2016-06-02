var GETPLAYTIME = 3001;

var PLAYPAUSE = 3002;

var GETPLAYSTATUS = 3003;

var SEEK = 3004;

var STOP = 3005;

var RETURNTIME = 3006;

var SETTIMEPOINT = 3007;

var STOPCONNECT = 3008;

var RETURNSTATUS = 3009;

var GETTOTALTIME = 3010;

var RETURNTOTALTIME = 3011;

var CURRENTTIME = 0;

var PLAYSTATUS = '';

var TOTALTIME = 0;

function $id(name) {
	return document.getElementById(name);
}

function videoCallBackFun(type, data) {
	switch (type) {
	case RETURNTIME:
		CURRENTTIME = data;
		break;
	case RETURNSTATUS:
		PLAYSTATUS = data;
		break;
	case RETURNTOTALTIME:
		TOTALTIME = data;
		break;
	}
}

function callVideo(type, data) {
	$id("MultiStreamPlayer").videoCallFun(type, data);
}

window.onload = init;

function init() {
	checkLocalOrNet();
}

function setPagePlayPause() {
	callVideo(PLAYPAUSE, null);
}

function getPlayState() {
	callVideo(GETPLAYSTATUS, null);
	return PLAYSTATUS;
}

function setPageStop() {
	callVideo(STOP, null);
}

function setCurrentPlayTime(time) {
	callVideo(SEEK, time);
}

function getCurrentPlayTime() {
	callVideo(GETPLAYTIME, null);
	return CURRENTTIME;
}

function addTimePoint(time) {
	callVideo(SETTIMEPOINT, time);
}

function stopConnect() {
	callVideo(STOPCONNECT, null);
}

function getTotalTime() {
	callVideo(GETTOTALTIME,null);
	return TOTALTIME;
}