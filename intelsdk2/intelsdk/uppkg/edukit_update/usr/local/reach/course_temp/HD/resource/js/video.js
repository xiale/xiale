/*- °æ±¾ºÅ£º1.0.0   ¹¹½¨ºÅ£º00000000 -*/

/*---------------------------------- utils --------------------------------- */

var addEvent = (function () {
  if (document.addEventListener) {
    return function (el, type, fn) {
      if (el && el.nodeName || el === window) {
        el.addEventListener(type, fn, false);
      } else if (el && el.length) {
        for (var i = 0; i < el.length; i++) {
          addEvent(el[i], type, fn);
        }
      }
    };
  } else {
    return function (el, type, fn) {
      if (el && el.nodeName || el === window) {
        el.attachEvent('on' + type, function () { return fn.call(el, window.event); });
      } else if (el && el.length) {
        for (var i = 0; i < el.length; i++) {
          addEvent(el[i], type, fn);
        }
      }
    };
  }
})();

/*--------------------------------- debug --------------------------------- */

var alert_win = document.getElementById("alert_win");
	alert_x = document.getElementById("alert_x");
	alert_clear = document.getElementById("alert_clear");

function AlertText(text) {
	var date = new Date();
		month = date.getMonth() + 1;
		dateText = date.getFullYear() + "-" + two(month) + "-" + two(date.getDate()) + " " + two(date.getHours()) + ":" + two(date.getMinutes()) + ":" + two(date.getSeconds());
	var alert = document.getElementById("alert_text");
	text = dateText + "  " + text;
	alert.innerHTML += text + "<br/>" ;
	alert_win.style.display = "block";
}

function ClearText() {
	var alert = document.getElementById("alert_text");
	alert.innerHTML = "";
}

addEvent(alert_x, "click", function() {
	alert_win.style.display = "none";
});

addEvent(alert_clear, "click", function() {
	var alert = document.getElementById("alert_text");
	alert.innerHTML = "";
});

/*--------------------------------- Videos --------------------------------- */

var ready = false;
var videos = document.getElementById("videos");

var video1 = document.getElementById("video1");
var video2 = document.getElementById("video2");
var video3 = document.getElementById("video3");

var curVideo1 = 0;
var curVideo2 = 0;
var curVideo3 = 0;

var numVideo1 = 0;
var numVideo2 = 0;
var numVideo3 = 0;

var curMode = 0;
var videoMutedFlag = false;
var videoStopFlag = false;

var togglePlay = document.getElementById('play');
var togglePause = document.getElementById('pause');

var toggleStop = document.getElementById('stop');
var position = document.getElementById('position');
var scrub = document.getElementById('scrub');


var progessDisplay = document.getElementById('progess_display');
var progessTrack = document.getElementById('progess_track');

var videoMute = 0;

var toggleVolumeMax = document.getElementById('volume_max');
var toggleVolumeMin = document.getElementById('volume_min');


var toggleFullScreen = document.getElementById('full_screen');
var toggleNormalScreen = document.getElementById('normal_screen');

var	togglePreChannel = document.getElementById('pre_channel');
var	toggleNextChannel = document.getElementById('next_channel');

var totalTime = getTotalTime(0);
var channel = getChannel();
var delayScrub = 0;

function getWindowSize() {

	var winWidth, winHeight;

	if (window.innerWidth) {
		winWidth = window.innerWidth;
	} else if ((document.body) && (document.body.clientWidth)) {
		winWidth = document.body.clientWidth;
	} if (window.innerHeight) {
		winHeight = window.innerHeight;
	} else if ((document.body) && (document.body.clientHeight)) {
		winHeight = document.body.clientHeight;
	}

	if (document.documentElement && document.documentElement.clientHeight && document.documentElement.clientWidth)
	{
		winHeight = document.documentElement.clientHeight;
		winWidth = document.documentElement.clientWidth;
	}

	return [winWidth, winHeight];
}


function getScreenSize() {
	var screenWidth = window.screen.width;
	var screenHeight = window.screen.height;
	return [screenWidth, screenHeight];
}

function playVideo(video, channel, curVideo, muted) {
	video.src = getVideoSrc(channel, curVideo);
	video.play();
	video.muted = muted;
}

var video1StartTime = 0.0;
var video2StartTime = 0.0;
var video3StartTime = 0.0;

function displayVideos() {

	var winSize = getWindowSize();
	var winWidth = winSize[0];
	var winHeight = winSize[1];

	if (channel >= 1) {
		video1StartTime = getChannelStartTime(0);
	//	playVideo(video1, 0, curVideo1, false);
		numVideo1 = getVideoNum(0);
	}
	if (channel >= 2) {
		video2StartTime = getChannelStartTime(1);
	//	var offsetTime = video2StartTime*1000 - video1StartTime*1000;
	//	if (offsetTime < 0) {
	//		offsetTime = 0;
	//	}
	//	setTimeout("playVideo(video2, 1, curVideo2, true)", parseInt(offsetTime));
		numVideo2 = getVideoNum(1);
	}
	if (channel >= 3) {
		video3StartTime = getChannelStartTime(2);
	//	var offsetTime = video3StartTime*1000 - video1StartTime*1000;
	//	if (offsetTime < 0) {
	//		offsetTime = 0;
	//	}
	//	setTimeout("playVideo(video3, 2, curVideo3, true)", parseInt(offsetTime));
		numVideo3 = getVideoNum(2);
	}

	if (channel == 1) {
		curMode = 1;
	} else if (channel == 2) {
		curMode = 4;
	} else if (channel == 3) {
		curMode = 6;
	}

	changeModeDisplay();

	this.value = "pause";
	document.getElementById("play").style.display = "none";
	document.getElementById("pause").style.display = "block";
	document.getElementById("scrub").style.width = winWidth - 420 + "px";
	document.getElementById("pre_channel").style.top = (winHeight - 208)/2 + "px";
	document.getElementById("next_channel").style.top = (winHeight - 208)/2 + "px";
}

displayVideos();

function playVideos() {
	if (channel >= 1) {
		playVideo(video1, 0, curVideo1, false);
	}
	if (channel >= 2) {
		var offsetTime = video2StartTime*1000 - video1StartTime*1000;
		if (offsetTime < 0) {
			offsetTime = 0;
		}
		setTimeout("playVideo(video2, 1, curVideo2, true)", parseInt(offsetTime));
	}
	if (channel >= 3) {
		var offsetTime = video3StartTime*1000 - video1StartTime*1000;
		if (offsetTime < 0) {
			offsetTime = 0;
		}
		setTimeout("playVideo(video3, 2, curVideo3, true)", parseInt(offsetTime));
	}
}

playVideos();

window.onresize = function() {
	changeModeDisplay();
	setDots();

	var winSize = getWindowSize();
	var winHeight = winSize[1];
	document.getElementById("pre_channel").style.top = (winHeight - 208)/2 + "px";
	document.getElementById("next_channel").style.top = (winHeight - 208)/2 + "px";

}

/*-------------------------------- Controls -------------------------------- */

function videoPlay() {
	if (channel >= 1) {
		video1.play();
		video2.muted = false;
	}
	if (channel >= 2) {
		video2.play();
		video2.muted = true;
	}
	if (channel >= 3) {
		video3.play();
		video3.muted = true;
	}
}

function videoPause() {
	if (channel >= 1) {
		video1.pause();
	}
	if (channel >= 2) {
		video2.pause();
	}
	if (channel >= 3) {
		video3.pause();
	}
}

function videoStop() {
	if (channel >= 1) {
		video1.src = getVideoSrc(0, 0);
		if (sdORhd && hasChange) {
			video1.src = "../../" + sdORhd + "/resource/" + getVideoSrc(0, 0);
		}
		video1.oncanplaythrough = function() {
			video1.currentTime = 0.1;
			video1.pause();
			curVideo1 = 0;
			scrub.value = 0;
			progessTrack.style.width = "0px";
			position.innerHTML = asTime(0);
		}
	}
	if (channel >= 2) {
		video2.src = getVideoSrc(1, 0);
		if (sdORhd && hasChange) {
			video2.src = "../../" + sdORhd + "/resource/" + getVideoSrc(1, 0);
		}
		video2.oncanplaythrough = function() {
			video2.currentTime = 0.1;
			video2.pause();
		}
	}
	if (channel >= 3) {
		video3.src = getVideoSrc(2, 0);
		if (sdORhd && hasChange) {
			video3.src = "../../" + sdORhd + "/resource/" + getVideoSrc(2, 0);
		}
		video3.oncanplaythrough = function() {
			video3.currentTime = 0.1;
			video3.pause();
		}
	}
	videoStopFlag = true;
}

function videoTroggerPlay(video, muted) {
	video.play();
	video.muted = muted;
}

addEvent(togglePlay, 'click', function () {
	if (ready == false) {
		return;
	}
	if (videoStopFlag) {
		if (channel >= 1) {
			videoTroggerPlay(video1, videoMutedFlag);
		}
		if (channel >= 2) {
			var offsetTime = video2StartTime*1000 - video1StartTime*1000;
			if (offsetTime < 0) {
				offsetTime = 0;
			}
			setTimeout("videoTroggerPlay(video2, true)", parseInt(offsetTime));
		}
		if (channel >= 3) {
			var offsetTime = video3StartTime*1000 - video1StartTime*1000;
			if (offsetTime < 0) {
				offsetTime = 0;
			}
			setTimeout("videoTroggerPlay(video3, true)", parseInt(offsetTime));
		}

	} else if (video1.paused) {
		videoPlay();
	}

	this.value = "pause";
	document.getElementById("play").style.display = "none";
	document.getElementById("pause").style.display = "block";
	videoStopFlag = false;
});

addEvent(togglePause, 'click', function () {
	if (ready == false) {
		return;
	}
	if (!video1.paused) {
		videoPause();
		this.value = "play";
		document.getElementById("play").style.display = "block";
		document.getElementById("pause").style.display = "none";
	}
});

addEvent(toggleStop, 'click', function () {
	if (ready == false) {
		return;
	}
	videoStop();
	this.value = "stop";
	document.getElementById("play").style.display = "block";
	document.getElementById("pause").style.display = "none";
});

var valite = false;
var trackV = document.getElementById('volume_track');
var thumb = document.getElementById('volume_thumb');
var displayV = document.getElementById('volume_display');

addEvent(thumb, 'mousedown', function () {
	valite = true;
});

addEvent(thumb, 'mouseup', function () {
	valite = false;
});

var lastClientY = "150px";

addEvent(thumb, 'mousemove', function () {
	if (valite == false) {
		return ;
	}

	var windowSize = getWindowSize();
	var changeY = event.clientY-(windowSize[1]-70);

	if (changeY >= -90 && changeY <= 0) {
		thumb.style.margin = 90+changeY+"px 0px";
		displayV.style.bottom = 60-changeY+"px";
	}

	if (changeY < -90) {
		changeY = -90;
	}
	if (changeY > 0) {
		changeY = 0;
	}

	lastClientY = displayV.style.bottom;
	video1.volume = (0 - changeY) / 90.0;
	displayV.innerHTML = parseInt(video1.volume*100);
	trackV.style.height = parseInt(video1.volume*100)+"px";
	trackV.style.marginTop = parseInt(100-video1.volume*100)+"px";
});


addEvent(toggleVolumeMax, 'click', function () {
	if (ready == false) {
		return;
	}

	if (channel >= 1) {
		video1.muted = true;
		videoMutedFlag = true;
		thumb.style.margin = "90px 0px";

		lastClientY = "60px"
		video1.volume = 0;
	}
	if (channel >= 2) {
		video2.muted = true;
	}
	if (channel >= 3) {
		video3.muted = true;
	}

	document.getElementById("volume_max").style.display = "none";
	document.getElementById("volume_min").style.display = "block";
});

addEvent(toggleVolumeMin, 'click', function () {
	if (ready == false) {
		return;
	}

	if (channel >= 1) {
		video1.muted = false;
		videoMutedFlag = false;
		thumb.style.margin = "0px 0px";
		lastClientY = "150px"
		video1.volume = 1;
	}
	if (channel >= 2) {
		video2.muted = true;
	}
	if (channel >= 3) {
		video3.muted = true;
	}

	document.getElementById("volume_max").style.display = "block";
	document.getElementById("volume_min").style.display = "none";
});

var volumeTimer = setTimeout("hideVolumeTimer()", 0);

var volumeMax = document.getElementById("volume_max");
var volumeMin = document.getElementById("volume_min");
var volumeBackground = document.getElementById("volume_background");

function displayVolume() {
	var volume = document.getElementById('volume_background');
	clearTimeout(volumeTimer);
	volume.style.visibility = "visible";
	displayV.style.visibility = "visible";
	displayV.style.bottom = lastClientY;
	displayV.innerHTML = parseInt(video1.volume*100);
	trackV.style.height = parseInt(video1.volume*100)+"px";
	trackV.style.marginTop = parseInt(100-video1.volume*100)+"px";
}

function hideVolumeTimer() {
	var volume = document.getElementById('volume_background');
	volume.style.visibility = 'hidden';
	displayV.style.visibility = 'hidden';
}

function hideVolume() {
	volumeTimer = setTimeout("hideVolumeTimer()", 1000);
}

addEvent(volumeMax, 'mouseover', function () {
	displayVolume();
});

addEvent(volumeMax, 'mouseout', function () {
	hideVolume();
});

addEvent(volumeMin, 'mouseover', function () {
	displayVolume();
});

addEvent(volumeMin, 'mouseout', function () {
	hideVolume();
});

addEvent(volumeBackground, 'mouseover', function () {
	displayVolume();
});

addEvent(volumeBackground, 'mouseout', function () {
	hideVolume();
});

addEvent(toggleFullScreen, 'click', function () {
	document.documentElement.webkitRequestFullScreen();
	changeModeDisplay();
});

addEvent(toggleNormalScreen, 'click', function () {
    document.webkitExitFullscreen();
	changeModeDisplay();
});

document.addEventListener("webkitfullscreenchange", function () {
    if (document.webkitIsFullScreen) {
		document.getElementById("full_screen").style.display = "none";
		document.getElementById("normal_screen").style.display = "block";
    } else {
		document.getElementById("full_screen").style.display = "block";
		document.getElementById("normal_screen").style.display = "none";
    }
}, false);

function hideScreen(video) {
	video.style.display = "none";
}

function seeking() {
	var value = 0;
	for (var i = 0; i < curVideo1; i++) {
		value = value + getSectionTime(0, i);
	}
	value = value + this.currentTime;
//	scrub.value = value;
//	position.innerHTML = asTime(value);
}

function seeked() {
	var value = 0;
	for (var i = 0; i < curVideo1; i++) {
		value = value + getSectionTime(0, i);
	}
	value = value + this.currentTime;
//	scrub.value = value;
//	position.innerHTML = asTime(value);
}

addEvent(video1, 'seeking', seeking);
addEvent(video1, 'seeked', seeked);

addEvent(video1, 'timeupdate', function () {
	if (delayScrub == 0) {
		var value = 0;
		for (var i = 0; i < curVideo1; i++) {
			value = value + getSectionTime(0, i);
		}
		value = value + this.currentTime;

		var scrubVal = parseFloat(scrub.value);
		if (scrubVal+10.0 > value && value > scrubVal-3.0) {
			scrub.value = value;
			position.innerHTML = asTime(value);

			var width = 1 + scrub.value * parseInt(scrub.style.width) / scrub.max;
			progessTrack.style.width = parseInt(width) + "px";
		}
	}
});

addEvent(video1, 'ended', function () {
	if (channel < 1) {
		return ;
	}
	numVideo1 = getVideoNum(0) - 1;
	if (curVideo1 < numVideo1 && numVideo1 > 1) {
		curVideo1 = curVideo1 + 1;
		video1.src = getVideoSrc(0, curVideo1);
		if (sdORhd && hasChange) {
			video1.src = "../../" + sdORhd + "/resource/" + getVideoSrc(0, curVideo1);
		}
		video1.oncanplaythrough = function() {
			video1.play();
			video1.muted = videoMutedFlag;
		}
	} else {
		togglePlay.value = "play";
		document.getElementById("play").style.display = "block";
		document.getElementById("pause").style.display = "none";

		video1.src = getVideoSrc(0, 0);
		if (sdORhd && hasChange) {
			video1.src = "../../" + sdORhd + "/resource/" + getVideoSrc(0, 0);
		}
		video1.oncanplaythrough = function() {
			video1.currentTime = 0.1;
			video1.pause();
			curVideo1 = 0;
			scrub.value = 0;
			progessTrack.style.width = "0px";
			position.innerHTML = asTime(0);
		}
		videoStopFlag = true;
	}
});

addEvent(video2, 'ended', function () {
	if (channel < 2) {
		return ;
	}
	numVideo2 = getVideoNum(1) - 1;
	if (curVideo2 < numVideo2 && numVideo2 > 1) {
		curVideo2 = curVideo2 + 1;
		video2.src = getVideoSrc(1, curVideo2);
		if (sdORhd && hasChange) {
			video2.src = "../../" + sdORhd + "/resource/" + getVideoSrc(1, curVideo2);
		}
		video2.oncanplaythrough = function() {
			video2.play();
			video2.muted = true;
		}
	} else {
		video2.src = getVideoSrc(1, 0);
		if (sdORhd && hasChange) {
			video2.src = "../../" + sdORhd + "/resource/" + getVideoSrc(1, 0);
		}
		video2.oncanplaythrough = function() {
			video2.currentTime = 0.1;
			video2.pause();
			curVideo2 = 0;
		}
	}
});

addEvent(video3, 'ended', function () {
	if (channel < 3) {
		return ;
	}
	numVideo3 = getVideoNum(2) - 1;
	if (curVideo3 < numVideo3 && numVideo3 > 1) {
		curVideo3 = curVideo3 + 1;
		video3.src = getVideoSrc(2, curVideo3);
		if (sdORhd && hasChange) {
			video3.src = "../../" + sdORhd + "/resource/" + getVideoSrc(2, curVideo3);
		}
		video3.oncanplaythrough = function() {
			video3.play();
			video3.muted = true;
		}
	} else {
		video3.src = getVideoSrc(2, 0);
		if (sdORhd && hasChange) {
			video3.src = "../../" + sdORhd + "/resource/" + getVideoSrc(2, 0);
		}
		video3.oncanplaythrough = function() {
			video3.currentTime = 0.1;
			video3.pause();
			curVideo3 = 0;
		}
	}
});

addEvent(video1, 'canplay', function () {
	video1.muted = false;
	ready = true;
	var duration = totalTime;
	document.querySelector('#duration').innerHTML = asTime(duration);
	scrub.setAttribute('max', duration);
});


var timeValue1;
var timeValue2;
var timeValue3;
var scrubChange = 0;
var scrubValue = 0;

function jumpScrub(jumpTime, infoPath) {
	scrubChange = 1;
	scrubValue = jumpTime;
	var pauseFlag = video1.paused;

	if (channel >= 1) {
		// video1
		timeValue1 = scrubValue;
		var i = 0;
		for (; i < numVideo1; i++) {
			var videoTimes = getSectionTime(0, i);
			if (videoTimes < timeValue1) {
				timeValue1 = timeValue1 - videoTimes;
			} else {
				break;
			}
		}

		if (i != curVideo1) {
			delayScrub = 1;
			curVideo1 = i;
			video1.src = getVideoSrc(0, i);
			if (infoPath && infoPath.length) {
				video1.src = infoPath + getVideoSrc(0, i);
			}
			if (!pauseFlag) {
				video1.play();
			}
			video1.oncanplaythrough = function() {
				if (!pauseFlag) {
					video1.play();
				}
				video1.muted = videoMutedFlag;
				video1.currentTime = timeValue1;
				if (!pauseFlag) {
					video1.play();
				}
				video1.oncanplaythrough = function() {
					video1.currentTime = timeValue1;
					if (!pauseFlag) {
						video1.play();
					}
					scrub.value = scrubValue;
					position.innerHTML = asTime(scrubValue);

					var width = 1 + scrub.value * parseInt(scrub.style.width) / scrub.max;
					progessTrack.style.width = parseInt(width) + "px";

					delayScrub = 0;
				}
			}
		} else {
			delayScrub = 1;
			if (infoPath && infoPath.length) {
				video1.src = infoPath + getVideoSrc(0, i);
			}
			video1.currentTime = timeValue1;
			if (!pauseFlag) {
				video1.play();
			}
			video1.oncanplaythrough = function() {
				video1.currentTime = timeValue1;
				if (!pauseFlag) {
					video1.play();
				}
				scrub.value = scrubValue;
				position.innerHTML = asTime(scrubValue);

				var width = 1 + scrub.value * parseInt(scrub.style.width) / scrub.max;
				progessTrack.style.width = parseInt(width) + "px";

				delayScrub = 0;
			}
		}
	}

	if (channel >= 2) {
		// video2
		var offsetStartTime = video2StartTime - video1StartTime;
		if (offsetStartTime <= 0) {
			offsetStartTime = 0;
		}

		timeValue2 = parseInt(scrubValue) - parseInt(offsetStartTime);
		if (timeValue2 < 0) {
			timeValue2 = 0;
		}
		var j = 0;
		for (; j < numVideo2; j++) {
			var videoTimes = getSectionTime(1, j);
			if (videoTimes < timeValue2) {
				timeValue2 = timeValue2 - videoTimes;
			} else {
				break;
			}
		}
		if (j != curVideo2) {
			curVideo2 = j;
			video2.src = getVideoSrc(1, j);
			if (infoPath && infoPath.length) {
				video2.src = infoPath + getVideoSrc(1, j);
			}
			if (!pauseFlag) {
				video2.play();
			}
			video2.oncanplaythrough = function() {
				if (!pauseFlag) {
					video2.play();
				}
				video2.muted = true;
				video2.currentTime = timeValue2;
				if (!pauseFlag) {
					video2.play();
				}
			}
		} else {
			if (infoPath && infoPath.length) {
				video2.src = infoPath + getVideoSrc(1, j);
			}
			video2.currentTime = timeValue2;
			if (!pauseFlag) {
				video2.play();
			}
			video2.oncanplaythrough = function() {
				video2.currentTime = timeValue2;
				if (!pauseFlag) {
					video2.play();
				}
			}
		}
	}

	if (channel >= 3) {
		// video3
		var offsetStartTime = video3StartTime - video1StartTime;
		if (offsetStartTime <= 0) {
			offsetStartTime = 0;
		}
		timeValue3 = parseInt(scrubValue) - parseInt(offsetStartTime);
		if (timeValue3 < 0) {
			timeValue3 = 0;
		}
		var k = 0;
		for (; k < numVideo3; k++) {
			var videoTimes = getSectionTime(2, k);
			if (videoTimes < timeValue3) {
				timeValue3 = timeValue3 - videoTimes;
			} else {
				break;
			}
		}
		if (k != curVideo3) {
			curVideo3 = k;
			video3.src = getVideoSrc(2, k);
			if (infoPath && infoPath.length) {
				video3.src = infoPath + getVideoSrc(2, k);
			}
			if (!pauseFlag) {
				video3.play();
			}
			video3.oncanplaythrough = function() {
				if (!pauseFlag) {
					video3.play();
				}
				video3.muted = true;
				video3.currentTime = timeValue3;
				if (!pauseFlag) {
					video3.play();
				}
			}
		} else {
			if (infoPath && infoPath.length) {
				video3.src = infoPath + getVideoSrc(2, k);
			}
			video3.currentTime = timeValue3;
			if (!pauseFlag) {
				video3.play();
			}
			video3.oncanplaythrough = function() {
				video3.currentTime = timeValue3;
				if (!pauseFlag) {
					video3.play();
				}
			}
		}
	}
	scrubChange = 0;
}

function changeScrub() {
	jumpScrub(scrub.value);
}

/*
function changeScrub() {
	scrubChange = 1;
	scrubValue = scrub.value;
	var pauseFlag = video1.paused;

	if (channel >= 1) {
		// video1
		timeValue1 = scrubValue;
		var i = 0;
		for (; i < numVideo1; i++) {
			var videoTimes = getSectionTime(0, i);
			if (videoTimes <= timeValue1) {
				timeValue1 = timeValue1 - videoTimes;
			} else {
				break;
			}
		}

		if (i != curVideo1) {
			delayScrub = 1;
			curVideo1 = i;
			video1.src = getVideoSrc(0, i);
			if (!pauseFlag) {
				video1.play();
			}
			video1.oncanplaythrough = function() {
				if (!pauseFlag) {
					video1.play();
				}
				video1.muted = videoMutedFlag;
				video1.currentTime = timeValue1;
				if (!pauseFlag) {
					video1.play();
				}
				video1.oncanplaythrough = function() {
					video1.currentTime = timeValue1;
					if (!pauseFlag) {
						video1.play();
					}
					scrub.value = scrubValue;
					position.innerHTML = asTime(scrubValue);

					var width = 1 + scrub.value * parseInt(scrub.style.width) / scrub.max;
					progessTrack.style.width = parseInt(width) + "px";

					delayScrub = 0;
				}
			}
		} else {
			delayScrub = 1;
			video1.currentTime = timeValue1;
			if (!pauseFlag) {
				video1.play();
			}
			video1.oncanplaythrough = function() {
				video1.currentTime = timeValue1;
				if (!pauseFlag) {
					video1.play();
				}
				scrub.value = scrubValue;
				position.innerHTML = asTime(scrubValue);

				var width = 1 + scrub.value * parseInt(scrub.style.width) / scrub.max;
				progessTrack.style.width = parseInt(width) + "px";

				delayScrub = 0;
			}
		}
	}

	if (channel >= 2) {
		// video2
		var offsetStartTime = video2StartTime - video1StartTime;
		if (offsetStartTime <= 0) {
			offsetStartTime = 0;
		}

		timeValue2 = parseInt(scrubValue) - parseInt(offsetStartTime);
		if (timeValue2 < 0) {
			timeValue2 = 0;
		}
		var j = 0;
		for (; j < numVideo2; j++) {
			var videoTimes = getSectionTime(1, j);
			if (videoTimes <= timeValue2) {
				timeValue2 = timeValue2 - videoTimes;
			} else {
				break;
			}
		}
		if (j != curVideo2) {
			curVideo2 = j;
			video2.src = getVideoSrc(1, j);
			if (!pauseFlag) {
				video2.play();
			}
			video2.oncanplaythrough = function() {
				if (!pauseFlag) {
					video2.play();
				}
				video2.muted = true;
				video2.currentTime = timeValue2;
				if (!pauseFlag) {
					video2.play();
				}
			}
		} else {
			video2.currentTime = timeValue2;
			if (!pauseFlag) {
				video2.play();
			}
			video2.oncanplaythrough = function() {
				video2.currentTime = timeValue2;
				if (!pauseFlag) {
					video2.play();
				}
			}
		}
	}

	if (channel >= 3) {
		// video3
		var offsetStartTime = video3StartTime - video1StartTime;
		if (offsetStartTime <= 0) {
			offsetStartTime = 0;
		}
		timeValue3 = parseInt(scrubValue) - parseInt(offsetStartTime);
		if (timeValue3 < 0) {
			timeValue3 = 0;
		}
		var k = 0;
		for (; k < numVideo3; k++) {
			var videoTimes = getSectionTime(2, k);
			if (videoTimes <= timeValue3) {
				timeValue3 = timeValue3 - videoTimes;
			} else {
				break;
			}
		}
		if (k != curVideo3) {
			curVideo3 = k;
			video3.src = getVideoSrc(2, k);
			if (!pauseFlag) {
				video3.play();
			}
			video3.oncanplaythrough = function() {
				if (!pauseFlag) {
					video3.play();
				}
				video3.muted = true;
				video3.currentTime = timeValue3;
				if (!pauseFlag) {
					video3.play();
				}
			}
		} else {
			video3.currentTime = timeValue3;
			if (!pauseFlag) {
				video3.play();
			}
			video3.oncanplaythrough = function() {
				video3.currentTime = timeValue3;
				if (!pauseFlag) {
					video3.play();
				}
			}
		}
	}
	scrubChange = 0;
}
*/

addEvent(scrub, 'change', changeScrub);

var sValite = false;

addEvent(scrub, 'mousedown', function () {
	sValite = true;
	progessDisplay.innerHTML = asTime(scrub.value);
	progessDisplay.style.visibility = 'visible';
});

addEvent(scrub, 'mouseup', function () {
	sValite = false;
	progessDisplay.style.visibility = 'hidden';
});

addEvent(scrub, 'mousemove', function () {
	if (sValite == false) {
		return ;
	}
	scrubChange = 1;
	progessDisplay.innerHTML = asTime(scrub.value);

	if (210 <= event.clientX && event.clientX <= 1340) {
		progessDisplay.style.left = event.clientX - 160 + "px";

		var width = 1 + scrub.value * parseInt(scrub.style.width) / scrub.max;
		progessTrack.style.width = parseInt(width) + "px";
	}
});

function asTime(t) {
	t = Math.floor(t);
	var s = t % 60;
	var m = Math.floor(t / 60);
	    m = m % 60;
	var h = Math.floor(t / 3600);
	return two(h) + ':' + two(m) + ':' + two(s);
}

function two(s) {
	s += "";
	if (s.length < 2) {
		s = "0" + s;
	}
	return s;
}


var video2SyncTime = 0;
var video3SyncTime = 0;

function AutoSyncVideos()
{
	if (video1.ended) {
		return ;
	}

	if (video1.paused) {
		return ;
	}

	if (delayScrub == 1) {
		return ;
	}

	if (scrubChange == 1) {
		return ;
	}

	if (channel >= 2) {
		var offsetStartTime1 = parseInt(video2StartTime - video1StartTime);
		if (offsetStartTime1 <= 0) {
			offsetStartTime1 = 0;
		}

		var syncTime = 0;
		for (var n = 0; n < curVideo1; n++) {
			syncTime = syncTime + getSectionTime(0, n);
		}
		syncTime = syncTime + video1.currentTime - offsetStartTime1;

		var i = 0;
		for (; i < numVideo2; i++) {
			var videoTimes = getSectionTime(1, i);
			if (videoTimes <= syncTime) {
				syncTime = syncTime - videoTimes;
			} else {
				break;
			}
		}
		if (i != curVideo2 && scrubChange == 0) {
			curVideo2 = i;
			video2.src = getVideoSrc(1, i);
			if (sdORhd && hasChange) {
				video2.src = "../../" + sdORhd + "/resource/" + getVideoSrc(1, i);
			}
			video2SyncTime = syncTime;
			video2.oncanplaythrough = function() {
				if (!video1.paused) {
					video2.play();
				}
				video2.muted = true;
				if (video2SyncTime >= 0 && parseInt(scrubChange) == 0) {
					video2.currentTime = video2SyncTime;
					if (!video1.paused) {
						video2.play();
					}
				}
			}
		} else {
			if (syncTime >= 0 && parseInt(scrubChange) == 0) {
				video2SyncTime = syncTime;
				video2.currentTime = video2SyncTime;
				if (!video1.paused) {
					video2.play();
				}
				video2.oncanplaythrough = function() {
					if (video2SyncTime >= 0 && parseInt(scrubChange) == 0) {
						video2.currentTime = video2SyncTime;
						if (!video1.paused) {
							video2.play();
						}
					}
				}
			}
		}
	}

	if (channel >= 3) {
		var offsetStartTime2 = parseInt(video3StartTime - video1StartTime);
		if (offsetStartTime2 <= 0) {
			offsetStartTime2 = 0;
		}

		var syncTime = 0;
		for (var n = 0; n < curVideo1; n++) {
			syncTime = syncTime + getSectionTime(0, n);
		}
		syncTime = syncTime + video1.currentTime - offsetStartTime2;

		var i = 0;
		for (; i < numVideo2; i++) {
			var videoTimes = getSectionTime(1, i);
			if (videoTimes <= syncTime) {
				syncTime = syncTime - videoTimes;
			} else {
				break;
			}
		}
		if (i != curVideo3 && scrubChange == 0) {
			curVideo3 = i;
			video3.src = getVideoSrc(2, i);
			if (sdORhd && hasChange) {
				video3.src = "../../" + sdORhd + "/resource/" + getVideoSrc(2, i);
			}
			video3SyncTime = syncTime;
			video3.oncanplaythrough = function() {
				if (!video1.paused) {
					video3.play();
				}
				video3.muted = true;
				if (video3SyncTime >= 0 && parseInt(scrubChange) == 0) {
					video3.currentTime = video3SyncTime;
					if (!video1.paused) {
						video3.play();
					}
				}
			}
		} else {
			if (syncTime >= 0 && parseInt(scrubChange) == 0) {
				video3SyncTime = syncTime;
				video3.currentTime = video3SyncTime;
				if (!video1.paused) {
					video3.play();
				}
				video3.oncanplaythrough = function() {
					if (video3SyncTime >= 0 && parseInt(scrubChange) == 0) {
						video3.currentTime = video3SyncTime;
						if (!video1.paused) {
							video3.play();
						}
					}
				}
			}
		}
	}
}

setInterval("AutoSyncVideos()", 10000);

/*---------------------------------- Modes --------------------------------- */

var	toggleMode1 = document.getElementById('mode1'),
	toggleMode2 = document.getElementById('mode2'),
	toggleMode3 = document.getElementById('mode3'),
	toggleMode4 = document.getElementById('mode4'),
	toggleMode5 = document.getElementById('mode5'),
	toggleMode6 = document.getElementById('mode6'),
	toggleModes = document.getElementById('modes'),
	toggleDisplayModes = document.getElementById('display_modes'),
	toggleHideModes = document.getElementById('hide_modes');

addEvent(toggleDisplayModes, 'click', function () {
	toggleDisplayModes.style.display = "none";
	toggleHideModes.style.display = "block";
	toggleModes.style.display = "block";
});

addEvent(toggleHideModes, 'click', function () {
	toggleDisplayModes.style.display = "block";
	toggleHideModes.style.display = "none";
	toggleModes.style.display = "none";
});

function displayModes() {
	if (channel >= 2) {
		var mode2 = document.getElementById("mode2");
		var mode3 = document.getElementById("mode3");
		var mode4 = document.getElementById("mode4");
		mode2.style.display = "block";
		mode3.style.display = "block";
		mode4.style.display = "block";
	}
	if (channel >= 3) {
		var mode5 = document.getElementById("mode5");
		var mode6 = document.getElementById("mode6");
		mode5.style.display = "block";
		mode6.style.display = "block";
	}
}

displayModes();

var timer;

function displayPanel() {
	var panel = document.getElementById('control_panel');
	clearTimeout(timer);
	panel.style.visibility = "visible";
}

function hidePanelTimer() {
	var panel = document.getElementById('control_panel');
	panel.style.visibility = 'hidden';
}

function hidePanel() {
	timer = setTimeout("hidePanelTimer()", 2000);
}

hidePanel();

var controlPanel = document.getElementById("control_panel");
var controlsDisplay = document.getElementById("controls_display");

addEvent(controlPanel, 'mouseover', function () {
	displayPanel();
});

addEvent(controlPanel, 'mouseout', function () {
	hidePanel();
});

addEvent(controlsDisplay, 'mouseover', function () {
	displayPanel();
});

function transitionDuration() {
	video1.style.transitionDuration = "0s";
	video2.style.transitionDuration = "0s";
	video3.style.transitionDuration = "0s";
}

function fullScreen(video) {
	document.documentElement.webkitRequestFullScreen();
	var screenSize = getScreenSize();
	var screenWidth = screenSize[0];
	var screenHeight = screenSize[1];

	video.style.display = "block";
	video.style.top = "0px";
	video.style.left = "0px";
	video.style.width = screenWidth + "px";
	video.style.height = screenHeight + "px";
}

function hideWindow(video) {
	video.style.display = "none";
}

function fullWindow(video) {
	video.style.display = "block";
	var winSize = getWindowSize();
	var winWidth = winSize[0];
	var winHeight = winSize[1];
	video.style.top = "0px";
	video.style.left = "0px";
	video.style.width = winWidth + "px";
	video.style.height = winHeight + "px";
}

function halfWindowLeft(video, winSize) {
	video.style.display = "block";
	var winWidth = winSize[0];
	var winHeight = winSize[1];
	video.style.top = "0px";
	video.style.left = "0px";
	video.style.width = winWidth/2 + "px";
	video.style.height = winHeight + "px";
}

function halfWindowRight(video, winSize) {
	video.style.display = "block";
	var winWidth = winSize[0];
	var winHeight = winSize[1];
	video.style.top = "0px";
	video.style.left = winWidth/2 + "px";
	video.style.width = winWidth/2 + "px";
	video.style.height = winHeight + "px";
}

function insideWindow(video, winSize) {
	video.style.display = "block";
	var winWidth = winSize[0];
	var winHeight = winSize[1];
	video.style.top = winHeight/2 + "px";
	video.style.left = winWidth*2/3 + "px";
	video.style.width = winWidth/3 + "px";
	video.style.height = winHeight/2 + "px";
}

function twoWindowLeft(video,winSize) {
	video.style.display = "block";
	var winWidth = winSize[0];
	var winHeight = winSize[1];
	video.style.top = "0px";
	video.style.left = "0px";
	video.style.width = winWidth*2/3 + "px";
	video.style.height = winHeight + "px";
}

function twoWindowRight(video,winSize) {
	video.style.display = "block";
	var winWidth = winSize[0];
	var winHeight = winSize[1];
	video.style.top = winHeight/2 + "px";
	video.style.left = winWidth*2/3 + "px";
	video.style.width = winWidth/3 + "px";
	video.style.height = winHeight/2 + "px";
}

function threeWindowLeft(video,winSize) {
	video.style.display = "block";
	var winWidth = winSize[0];
	var winHeight = winSize[1];
	video.style.top = "0px";
	video.style.left = "0px";
	video.style.width = parseInt(winWidth*2.0/3.0) + "px";
	video.style.height = winHeight + "px";
}

function threeWindowRightUp(video,winSize) {
	video.style.display = "block";
	var winWidth = winSize[0];
	var winHeight = winSize[1];
	video.style.top = "0px";
	video.style.left = parseInt(winWidth*2.0/3.0) + "px";
	video.style.width = parseInt(winWidth/3.0) + "px";
	video.style.height = parseInt(winHeight/2) + "px";
}

function threeWindowRightDown(video,winSize) {
	video.style.display = "block";
	var winWidth = winSize[0];
	var winHeight = winSize[1];
	video.style.top = parseInt(winHeight/2.0) + "px";
	video.style.left = parseInt(winWidth*2.0/3.0) + "px";
	video.style.width = parseInt(winWidth/3.0) + "px";
	video.style.height = parseInt(winHeight/2.0) + "px";
}

function threeWindowRight(video,winSize) {
	video.style.display = "block";
	var winWidth = winSize[0];
	var winHeight = winSize[1];
	video.style.top = "0px";
	video.style.left = winWidth/3 + "px";
	video.style.width = winWidth*2/3 + "px";
	video.style.height = winHeight + "px";
}

function threeWindowLeftUp(video,winSize) {
	video.style.display = "block";
	var winWidth = winSize[0];
	var winHeight = winSize[1];
	video.style.top = "0px";
	video.style.left = "0px";
	video.style.width = winWidth/3 + "px";
	video.style.height = winHeight/2 + "px";
}

function threeWindowLeftDown(video,winSize) {
	video.style.display = "block";
	var winWidth = winSize[0];
	var winHeight = winSize[1];
	video.style.top = winHeight/2 + "px";
	video.style.left = "0px";
	video.style.width = winWidth/3 + "px";
	video.style.height = winHeight/2 + "px";
}

function changeModeDisplay()
{
	transitionDuration();
	var winSize = getWindowSize();

	if (curMode == 1) {
		fullWindow(video1);
		hideWindow(video2);
		hideWindow(video3);
		displayChannels();
	} else if (curMode == 2) {
		halfWindowLeft(video1, winSize)
		halfWindowRight(video2, winSize);
		hideWindow(video3);
		hideChannels();
	} else if (curMode == 3) {
		twoWindowLeft(video1, winSize);
		twoWindowRight(video2, winSize);
		hideWindow(video3);
		hideChannels();
	} else if (curMode == 4) {
		fullWindow(video1);
		video2.style.zIndex = 2;
		insideWindow(video2, winSize);
		hideWindow(video3);
		hideChannels();
	} else if (curMode == 5) {
		threeWindowLeft(video1, winSize);
		threeWindowRightUp(video2, winSize);
		threeWindowRightDown(video3, winSize);
		hideChannels();
	} else if (curMode == 6) {
		threeWindowRight(video1, winSize);
		threeWindowLeftUp(video2, winSize);
		threeWindowLeftDown(video3, winSize);
		hideChannels();
	}

	displayActiveMode();

	var winSize = getWindowSize();
	var winWidth = winSize[0];
	var winHeight = winSize[1];
	document.getElementById("scrub").style.width = winWidth - 420 + "px";
}

function displayActiveMode()
{
	if (curMode == 1) {
		document.getElementById("mode1").style.backgroundImage = "url(./image/oneScreenBtnActive.png)";
	} else {
		document.getElementById("mode1").style.backgroundImage = "url(./image/oneScreenBtnNormal.png)";
	}

	if (curMode == 2) {
		document.getElementById("mode2").style.backgroundImage = "url(./image/TwoScreenNormalBtnActive.png)";
	} else {
		document.getElementById("mode2").style.backgroundImage = "url(./image/TwoScreenNormalBtnNormal.png)";
	}

	if (curMode == 3) {
		document.getElementById("mode3").style.backgroundImage = "url(./image/TwoScreenLeftBtnActive.png)";
	} else {
		document.getElementById("mode3").style.backgroundImage = "url(./image/TwoScreenLeftBtnNormal.png)";
	}

	if (curMode == 4) {
		document.getElementById("mode4").style.backgroundImage = "url(./image/TwoScreenMidBtnActive.png)";
	} else {
		document.getElementById("mode4").style.backgroundImage = "url(./image/TwoScreenMidBtnNormal.png)";
	}

	if (curMode == 5) {
		document.getElementById("mode5").style.backgroundImage = "url(./image/threeScreenLeftBtnActive.png)";
	} else {
		document.getElementById("mode5").style.backgroundImage = "url(./image/threeScreenLeftBtnNormal.png)";
	}

	if (curMode == 6) {
		document.getElementById("mode6").style.backgroundImage = "url(./image/threeScreenRightActive.png)";
	} else {
		document.getElementById("mode6").style.backgroundImage = "url(./image/threeScreenRightNormal.png)";
	}
}

addEvent(toggleMode1, 'click', function () {
	curMode = 1;
	changeModeDisplay();
});

addEvent(toggleMode2, 'click', function () {
	curMode = 2;
	changeModeDisplay();
	activeChannels2 = true;
});

addEvent(toggleMode3, 'click', function () {
	curMode = 3;
	changeModeDisplay();
	activeChannels2 = true;
});

addEvent(toggleMode4, 'click', function () {
	curMode = 4;
	changeModeDisplay();
	activeChannels2 = true;
});

addEvent(toggleMode5, 'click', function () {
	curMode = 5;
	changeModeDisplay();
	activeChannels3 = true;
});

addEvent(toggleMode6, 'click', function () {
	curMode = 6;
	changeModeDisplay();
	activeChannels3 = true;
});

addEvent(toggleMode1, 'mouseover', function () {
	document.getElementById("mode1").style.backgroundImage = "url(./image/oneScreenBtnHover.png)";
});

addEvent(toggleMode2, 'mouseover', function () {
	document.getElementById("mode2").style.backgroundImage = "url(./image/TwoScreenNormalBtnHover.png)";
});

addEvent(toggleMode3, 'mouseover', function () {
	document.getElementById("mode3").style.backgroundImage = "url(./image/TwoScreenLeftBtnHover.png)";
});

addEvent(toggleMode4, 'mouseover', function () {
	document.getElementById("mode4").style.backgroundImage = "url(./image/TwoScreenMidBtnHover.png)";
});

addEvent(toggleMode1, 'mouseout', function () {
	if (curMode == 1) {
		document.getElementById("mode1").style.backgroundImage = "url(./image/oneScreenBtnActive.png)";
	} else {
		document.getElementById("mode1").style.backgroundImage = "url(./image/oneScreenBtnNormal.png)";
	}
});

addEvent(toggleMode2, 'mouseout', function () {
	if (curMode == 2) {
		document.getElementById("mode2").style.backgroundImage = "url(./image/TwoScreenNormalBtnActive.png)";
	} else {
		document.getElementById("mode2").style.backgroundImage = "url(./image/TwoScreenNormalBtnNormal.png)";
	}
});

addEvent(toggleMode3, 'mouseout', function () {
	if (curMode == 3) {
		document.getElementById("mode3").style.backgroundImage = "url(./image/TwoScreenLeftBtnActive.png)";
	} else {
		document.getElementById("mode3").style.backgroundImage = "url(./image/TwoScreenLeftBtnNormal.png)";
	}
});

addEvent(toggleMode4, 'mouseout', function () {
	if (curMode == 4) {
		document.getElementById("mode4").style.backgroundImage = "url(./image/TwoScreenMidBtnActive.png)";
	} else {
		document.getElementById("mode4").style.backgroundImage = "url(./image/TwoScreenMidBtnNormal.png)";
	}
});

addEvent(toggleMode5, 'mouseout', function () {
	if (curMode == 5) {
		document.getElementById("mode5").style.backgroundImage = "url(./image/threeScreenLeftBtnActive.png)";
	} else {
		document.getElementById("mode5").style.backgroundImage = "url(./image/threeScreenLeftBtnNormal.png)";
	}
});

addEvent(toggleMode6, 'mouseout', function () {
	if (curMode == 6) {
		document.getElementById("mode6").style.backgroundImage = "url(./image/threeScreenRightActive.png)";
	} else {
		document.getElementById("mode6").style.backgroundImage = "url(./image/threeScreenRightNormal.png)";
	}
});

addEvent(toggleMode5, 'mouseover', function () {
	document.getElementById("mode5").style.backgroundImage = "url(./image/threeScreenLeftBtnHover.png)";

});

addEvent(toggleMode1, 'mousedown', function () {
	document.getElementById("mode1").style.backgroundImage = "url(./image/oneScreenBtnActive.png)";
});

addEvent(toggleMode2, 'mousedown', function () {
	document.getElementById("mode2").style.backgroundImage = "url(./image/TwoScreenNormalBtnActive.png)";
});

addEvent(toggleMode3, 'mousedown', function () {
	document.getElementById("mode3").style.backgroundImage = "url(./image/TwoScreenLeftBtnActive.png)";
});

addEvent(toggleMode4, 'mousedown', function () {
	document.getElementById("mode4").style.backgroundImage = "url(./image/TwoScreenMidBtnActive.png)";
});

addEvent(toggleMode5, 'mousedown', function () {
	document.getElementById("mode5").style.backgroundImage = "url(./image/threeScreenLeftBtnActive.png)";
});

addEvent(toggleMode6, 'mousedown', function () {
	document.getElementById("mode6").style.backgroundImage = "url(./image/threeScreenRightActive.png)";
});

/*-------------------------------- channels -------------------------------- */

var activeChannels2 = false;
var activeChannels3 = false;
var currentFullScreen = 1;

function displayChannels() {
	if (activeChannels2 || activeChannels3) {
		togglePreChannel.style.display = "block";
		toggleNextChannel.style.display = "block";
	} else {
		togglePreChannel.style.display = "none";
		toggleNextChannel.style.display = "none";
	}
}

function hideChannels() {
	togglePreChannel.style.display = "none";
	toggleNextChannel.style.display = "none";
}

addEvent(togglePreChannel, 'click', function () {
	if (activeChannels2 == false && activeChannels3 == false) {
		return;
	}

	transitionDuration();
	if (activeChannels3) {
		if (currentFullScreen == 1) {
			currentFullScreen = 3;
			fullWindow(video3);
			hideWindow(video1);
			hideWindow(video2);
		} else if (currentFullScreen == 2) {
			currentFullScreen = 1;
			fullWindow(video1);
			hideWindow(video2);
			hideWindow(video3);
		} else if (currentFullScreen == 3) {
			currentFullScreen = 2;
			fullWindow(video2);
			hideWindow(video3);
			hideWindow(video1);
		}
	} else if (activeChannels2) {
		if (currentFullScreen == 1) {
			currentFullScreen = 2;
			fullWindow(video2);
			hideWindow(video1);
		} else if (currentFullScreen == 2) {
			currentFullScreen = 1;
			fullWindow(video1);
			hideWindow(video2);
		}
	}
});

addEvent(toggleNextChannel, 'click', function () {
	if (activeChannels2 == false && activeChannels3 == false) {
		return;
	}

	transitionDuration();
	if (activeChannels3) {
		if (currentFullScreen == 1) {
			currentFullScreen = 2;
			fullWindow(video2);
			hideWindow(video3);
			hideWindow(video1);
		} else if (currentFullScreen == 2) {
			currentFullScreen = 3;
			fullWindow(video3);
			hideWindow(video1);
			hideWindow(video2);
		} else if (currentFullScreen == 3) {
			currentFullScreen = 1;
			fullWindow(video1);
			hideWindow(video2);
			hideWindow(video3);
		}
	} else if (activeChannels2) {
		if (currentFullScreen == 1) {
			currentFullScreen = 2;
			fullWindow(video2);
			hideWindow(video1);
		} else if (currentFullScreen == 2) {
			currentFullScreen = 1;
			fullWindow(video1);
			hideWindow(video2);
		}
	}
});

/*------------------------------- Drag & Drop ------------------------------ */

function allowDrop(ev) {
	ev.preventDefault();
}

addEvent(videos, 'dragover', function () {
	allowDrop(event);
});

function drag(ev) {
	ev.dataTransfer.setData("Text", ev.target.id);
}

function drop(ev) {
	ev.preventDefault();
	var srcId = ev.dataTransfer.getData("Text");

	var srcVideo = document.getElementById(srcId);
	var dstVideo = ev.target;

	srcVideo.style.transitionDuration = "1.5s";
	dstVideo.style.transitionDuration = "1.5s";

	var zIndex = srcVideo.style.zIndex;
	var top = srcVideo.style.top;
	var left = srcVideo.style.left;
	var width = srcVideo.style.width;
	var height = srcVideo.style.height;

	srcVideo.style.zIndex = dstVideo.style.zIndex;
	srcVideo.style.top = dstVideo.style.top;
	srcVideo.style.left = dstVideo.style.left;
	srcVideo.style.width = dstVideo.style.width;
	srcVideo.style.height = dstVideo.style.height;

	dstVideo.style.zIndex = zIndex;
	dstVideo.style.top = top;
	dstVideo.style.left = left;
	dstVideo.style.width = width;
	dstVideo.style.height = height;
}

addEvent(video1, 'dragstart', function () {
	drag(event);
});

addEvent(video1, 'drop', function () {
	drop(event);
});

addEvent(video2, 'dragstart', function () {
	drag(event);
});

addEvent(video2, 'drop', function () {
	drop(event);
});

addEvent(video3, 'dragstart', function () {
	drag(event);
});

addEvent(video3, 'drop', function () {
	drop(event);
});

/*------------------------------- Display Dot ------------------------------ */

var clickIndex = -1;

function jumpToDot(index, value) {
	clickIndex = index;
	scrub.value = value;
	changeScrub();
}

function displayImage(index) {
	var displayImage = document.getElementById("display_image");
	var left = getIndexTime(index) * parseInt(scrub.style.width) / totalTime;
	var time = getIndexTime(index);
	displayImage.style.left = 150 + parseInt(left) + "px";
	displayImage.innerHTML = "<div style='color:#fff; word-break:break-all; width:128px;'>" + asTime(time) + "&ensp;" + getIndexContent(index) + "</div>";
	if (getIndexImage(index)) {
		displayImage.innerHTML += "<image src='" + getIndexImage(index) + "' style='width:128px; height:72px'>";
	}
	displayImage.style.visibility = "visible";
}

function hideImage(index) {
	var displayImage = document.getElementById("display_image");
	displayImage.style.visibility = "hidden";
}

function displayDot() {
	var displayDot = document.getElementById("display_dot");
	displayDot.style.visibility = "visible";

	var displayClickDot = document.getElementById("display_click_dot");
	displayClickDot.style.visibility = "visible";
}

function hideDot() {
	var displayDot = document.getElementById("display_dot");
	displayDot.style.visibility = "hidden";

	var displayClickDot = document.getElementById("display_click_dot");
	displayClickDot.style.visibility = "visible";
}

function setDots() {
	var displayDot = document.getElementById("display_dot");
	displayDot.innerHTML = "";
	var dotNum = getIndexNumber();
	for (var i = 0; i < dotNum; i++) {
		var time = getIndexTime(i);
		var left = getIndexTime(i) * parseInt(scrub.style.width) / totalTime;
		displayDot.innerHTML += "<image src='./image/dot.png' onclick='jumpToDot(" + i + "," + parseInt(time) + ")' onmouseover='displayImage(" + i + ")' onmouseout='hideImage(" + i + ")' style='position:absolute;left:" + parseInt(left) + "px;top:5px;'>";
	}

	var displayClickDot = document.getElementById("display_click_dot");
	displayClickDot.innerHTML = "";
	var clickDotNum = getIndexNumber();
	for (var i = 0; i < clickDotNum; i++) {
		var time = getIndexTime(i);
		var left = getIndexTime(i) * parseInt(scrub.style.width) / totalTime;
		displayClickDot.innerHTML += "<image src='./image/dot0.png' onclick='jumpToDot(" + i + "," + parseInt(time) + ")' onmouseover='displayImage(" + i + ")' onmouseout='hideImage(" + i + ")' style='position:absolute;left:" + parseInt(left) + "px;top:5px;'>";
	}
}

var controls = document.getElementById("controls");

addEvent(controls, 'mouseover', function () {
	displayDot();
});

addEvent(controls, 'mouseout', function () {
	hideDot();
});

function jGetIndexNumber() {
	var objArray = objDots.indexs;
	return objArray.length;
}

function jGetIndexTime(index) {
	var objArray = objDots.indexs;
	var objDot = objArray[index];
	return objDot.time;
}

function jGetIndexImage(index) {
	var objArray = objDots.indexs;
	var objDot = objArray[index];
	return objDot.image;
}

function jGetIndexContent(index) {
	var objArray = objDots.indexs;
	var objDot = objArray[index];
	return objDot.content;
}

function getIndexNumber() {
//	return xGetIndexNumber();
	return jGetIndexNumber();
}

function getIndexTime(index) {
//	return xGetIndexTime(index);
	return jGetIndexTime(index);
}

function getIndexImage(index) {
//	return xGetIndexImage(index);
	return jGetIndexImage(index);
}

function getIndexContent(index) {
//	return xGetIndexContent(index);
	return jGetIndexContent(index);
}

/* ----------------------------- sd hd change ----------------------------- */

function isExistsFile(filepath) {
	if (window.ActiveXObject) {
		var actXObj = new ActiveXObject("Scripting.FileSystemObject");
		if (actXObj.FileExists(filepath)) {
			return true;
		} else {
			return false;
		}
	} else if (document.implementation && document.implementation.createDocument){
		var xmlHttp = new window.XMLHttpRequest();
		xmlHttp.open("GET", filepath, false);
		xmlHttp.send();
		if (xmlHttp.readyState == 4){
			if (xmlHttp.status == 200) {
				return true;
			} else { //xmlHttp.status == 404
				return false;
			}
		}
	} else {
		return false;
	}
}

var sdBtn = document.getElementById("sd_button");
var hdBtn = document.getElementById("hd_button");

var sdORhd = "";
var hasChange = false;

function initHdSd() {
	if (location.href.indexOf("SD") != -1) {
		sdORhd = "HD";
		var path = location.href.replace(/SD/i, sdORhd);
		if (isExistsFile(path)) {
			sdBtn.style.display = "none";
			hdBtn.style.display = "block";
		}
	} else if (location.href.indexOf("HD") != -1) {
		sdORhd = "SD";
		var path = location.href.replace(/HD/i, sdORhd);
		if (isExistsFile(path)) {
			sdBtn.style.display = "block";
			hdBtn.style.display = "none";
		}
	}
}

initHdSd();

addEvent(sdBtn, 'click', function() {
	hasChange = !hasChange;
	sdBtn.style.display = "none";
	hdBtn.style.display = "block";
	jumpScrub(scrub.value, "../../SD/resource/");
});

addEvent(hdBtn, 'click', function() {
	hasChange = !hasChange;
	hdBtn.style.display = "none";
	sdBtn.style.display = "block";
	jumpScrub(scrub.value, "../../HD/resource/");
});

/* ----------------------------- dblclick fullScreen ----------------------------- */

var fullScreen = false;

addEvent(videos, 'dblclick', function() {
	fullScreen = !fullScreen;
	if (fullScreen) {
		toggleFullScreen.click();
	} else {
		toggleNormalScreen.click();
	}
});

addEvent(window, 'keyup', function(e) {
	if (fullScreen && e.keyCode == 27) { // Esc¼ü ÍË³öÈ«ÆÁ
		fullScreen = !fullScreen;
	}
});

/* ----------------------------- version info ----------------------------- */

var version_win = document.getElementById('version_win');
var version_x = document.getElementById("version_x");

addEvent(version_x, 'click', function() {
	version_win.style.display = "none";
});

addEvent(window, 'keyup', function(e) {
	if (e.keyCode == 120) { // F9¼ü °æ±¾ÐÅÏ¢
		document.getElementById("version_win").style.display = "block";
	}
});

/* ----------------------------- key event ----------------------------- */

addEvent(window, 'keyup', function(e) {
	if (e.keyCode == 37) { // left¼ü ºóÍË
		jumpScrub(video1.currentTime - 5);
	}

	if (e.keyCode == 39) { // right¼ü Ç°½ø
		jumpScrub(video1.currentTime + 5);
	}

	if (e.keyCode == 38) { // up¼ü ÉýÒô
		if (video1.volume < 0.1) {
			video1.muted = false;
			videoMutedFlag = false;
			volumeMax.style.display = "block";
			volumeMin.style.display = "none";
		}
		if (video1.volume > 0.8) {
			volumeMin.click();
		} else {
			video1.volume += 0.1;
			thumb.style.margin = 90-parseInt(video1.volume*100)+"px 0px";
			displayV.style.bottom = 60+parseInt(video1.volume*100)+"px";
			lastClientY = displayV.style.bottom;
			displayV.innerHTML = parseInt(video1.volume*100);
			trackV.style.height = parseInt(video1.volume*100)+"px";
			trackV.style.marginTop = parseInt(100-video1.volume*100)+"px";
		}
	}

	if (e.keyCode == 40) { // down¼ü ½µÒô
		if (video1.volume <= 0.2) {
			volumeMax.click();
		} else {
			video1.volume -= 0.1;
			thumb.style.margin = 90-parseInt(video1.volume*100)+"px 0px";
			displayV.style.bottom = 60+parseInt(video1.volume*100)+"px";
			lastClientY = displayV.style.bottom;
			displayV.innerHTML = parseInt(video1.volume*100);
			trackV.style.height = parseInt(video1.volume*100)+"px";
			trackV.style.marginTop = parseInt(100-video1.volume*100)+"px";
		}
	}
});

/* ----------------------------- VOD Interface ----------------------------- */

var objDots;

function vodGetTotalTime() {
	return getTotalTime(0);
}

function vodGetCurrentTime() {
	return video1.currentTime;
}

function vodSetCurrentTime( time ) {
	jumpScrub( time );
}

function vodInitDots( dots ) {
	objDots = dots;
	setDots();
}

function vodSetDots( dot ) {
	objDots.indexs.push( dot );
	setDots();
}

function vodGetDots() {
	return objDots
}

function play() {
	playVideos();
}

