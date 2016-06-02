var bIsReplace1 = bIsReplace2 = bIsReplace3 = bInitFinished = false;
var bHaveReplace = bShowVideoMirror = bIsHuaWei = false;
var sStartImage = sEndImage = iStartDuration = indexXmlDoc = infoXmlDoc = sChangePath = null;
var iEndDuration = channelNodes = iTotaltime = iSpceialFlag = iVideoQual = null;
var replacesArry = vReplaceObj = animateArry = vAnimateObj = null; 
var sTitle = sAuthor = sFileStyle = sTime = sDescrip = null;
var VideoWindow1 = VideoWindow2 = VideoWindow3 = controlContent = null;
var iIndexMode = irepchnidx = iCurrentPlayTime = 0;
var bHideGobackButton = bRemark = 0;
var bNeedEncodeUrl = 0;

var isChrome = navigator.userAgent.indexOf('Chrome') != -1;
var isSafari = navigator.userAgent.indexOf('Safari') != -1;
var moz = navigator.userAgent.indexOf('MSIE') == -1;

function getUrlParam(name) {
    var reg = new RegExp("(^|&)" + name + "=([^&]*)(&|$)");
    var r = window.location.search.substr(1).match(reg);
    if (r != null) {
        return unescape(r[2]);
    }
    return null;
}

bNeedEncodeUrl = getUrlParam("flag");
if (bNeedEncodeUrl == null) {
    bNeedEncodeUrl = 0;
}
else {
    bRemark = 1;
}
bHideGobackButton = bNeedEncodeUrl;

function transUrl(sUrl) {
    var sUrlHead = new String(sUrl);
    sUrlHead = sUrlHead.substring(0, 4);

    if (moz) {
        if ((!isChrome) && (!isSafari)) {//FirFox
            if (sUrlHead == "http") {
                return decodeURI(sUrl);
            }
            else {
                return decodeURI(sUrl);
            }
        }
        else {
            if (isChrome) {//chrome
                if (sUrlHead == "http") {
                    return decodeURI(sUrl);
                }
                else {
                    return decodeURI(sUrl);
                }
            }
            else {//safari
                if (sUrlHead == "http") {
                    return encodeURI(sUrl);
                }
                else {
                    return decodeURI(sUrl);
                }
            }
        }
    }
    else {//IE
        if (sUrlHead == "http") {
        	if (bNeedEncodeUrl) {
        		return encodeURI(sUrl);
          }
          else {
          	return sUrl;
          }
        }
        else {
            return sUrl;
        }
    }
}

function DecryptString(sSrcStr) {
    return sSrcStr;
    //return controlContent.ReachPlayerCtrl.DecryptString(sSrcStr);
}

function playerOnLoad(sender, args) {

    document.getElementById("szReachPlayer").settings.MaxFrameRate = 90;
    controlContent = document.getElementById("szReachPlayer").Content; 
    controlContent.ReachPlayerCtrl.callGoBack = fnGoBack;
    bShowVideoMirror = controlContent.ReachPlayerCtrl.bShowVideoMirror;
    iIndexMode = 1;
    controlContent.ReachPlayerCtrl.iIndexMode = iIndexMode;
    iCurrentPlayTime = 0;
    VideoWindow1 = controlContent.findName('VideoWindow1');
    VideoWindow2 = controlContent.findName('VideoWindow2');
    VideoWindow3 = controlContent.findName('VideoWindow3');

    try {
        fnReadContentInfo(sender, args);
        createSliderDynamic(sender, args);
    }
    catch (e) {
    }

    if (sStartImage) {
        controlContent.ReachPlayerCtrl.sStartImage = DecryptString(sStartImage);
        controlContent.ReachPlayerCtrl.iStartDuration = iStartDuration;

        controlContent.findName('ShowImage')["Source"] = DecryptString(sStartImage);
        controlContent.findName('ShowImage_Animate').Begin();
    }

    if (sEndImage) {
        controlContent.ReachPlayerCtrl.sEndImage = DecryptString(sEndImage);
        controlContent.ReachPlayerCtrl.iEndDuration = iEndDuration;
    }

    controlContent.ReachPlayerCtrl.sTitle = DecryptString(sTitle);
    controlContent.ReachPlayerCtrl.sAuthor = DecryptString(sAuthor);
    controlContent.ReachPlayerCtrl.sTime = DecryptString(sTime);
    controlContent.ReachPlayerCtrl.sDescrip = DecryptString(sDescrip);

    try {
        fnReadVideoInfo();
    }
    catch (e) {
    }

    replacesArry = new Array();
    vReplaceObj = new Object();

    try {
        replacesArry = getReplaces();
    }
    catch (e) {
    }
    bIsReplace1 = false;
    bIsReplace2 = false;
    bIsReplace3 = false;

    if (replacesArry) {
        if (replacesArry.length != 0) {
            if (irepchnidx == 0) {
                bIsReplace1 = true;
            }
            else
                if (irepchnidx == 1) {
                    bIsReplace2 = true;
                }
                else
                    if (irepchnidx == 2) {
                        bIsReplace3 = true;
                    }
        }
    }

    animatesArry = new Array();
    vAnimateObj = new Object();

    try {
        animatesArry = getAnimates();
    }
    catch (e) {
    }

    controlContent.ReachPlayerCtrl.bIsReplace1 = bIsReplace1;
    controlContent.ReachPlayerCtrl.bIsReplace2 = bIsReplace2;
    controlContent.ReachPlayerCtrl.bIsReplace3 = bIsReplace3;

    if (bIsHuaWei) {
        controlContent.ReachPlayerCtrl.getPauseInfo();
        controlContent.ReachPlayerCtrl.getConfTypeInfo()
        controlContent.ReachPlayerCtrl.m_iVideoQual = iVideoQual;
    }

    try {
        controlContent.ReachPlayerCtrl.setChannelCountStatus(getChannelCount());
    }
    catch (e) {
    }
    controlContent.ReachPlayerCtrl.m_bIsHuaWei = bIsHuaWei;
    controlContent.ReachPlayerCtrl.getClipAndReplaceInfo();
    controlContent.ReachPlayerCtrl.getAnimateInfo();
    controlContent.ReachPlayerCtrl.bInitFinished = true;
    controlContent.ReachPlayerCtrl.dTotaltime = iTotaltime;
    controlContent.ReachPlayerCtrl.dCurrentPlayTime = iCurrentPlayTime;
    controlContent.ReachPlayerCtrl.CreateSlider();
    controlContent.ReachPlayerCtrl.PPTLoadMethod();
    controlContent.ReachPlayerCtrl.showActiveStatus();
    controlContent.ReachPlayerCtrl.fnStartPlayTimer();
    showGobackButton();
    loadPage();
}

function CallWebUser() {
    if (window.parent.playInvokedBySilverlight) {
        window.parent.playInvokedBySilverlight();
    }
}

function ChangeSDHD(time) {
    if (window.parent.setCurrentTimePoint) {
        window.parent.setCurrentTimePoint(time);
    }
}

function showStartImage(sender) {
    if (sStartImage) {
        controlContent.findName('ShowImage')["Source"] = DecryptString(sStartImage);
        controlContent.findName('ShowImage_Animate').Begin();
    }
}

function showEndImage(sender) {
    if (sEndImage) {
        controlContent.findName('ShowImage')["Source"] = DecryptString(sEndImage);
        controlContent.findName('ShowImage_Animate').Begin();
    }
}

function showGobackButton() {
    var sUrl = new String(location);
    sUrl = sUrl.substring(0, 4);
    if (sUrl != "http") {
        controlContent.findName('GoBackButton').Visibility = "Collapsed";
        controlContent.findName('ChagQualButton').Visibility = "Collapsed";
        return;
    }

    if (bHideGobackButton == 1 || bRemark == 0) {
        controlContent.findName('GoBackButton').Visibility = "Collapsed";
    }

    if (!bIsHuaWei) {
        controlContent.findName('ChagQualButton').Visibility = "Collapsed";
    }
}

function createSliderDynamic(sender, args) {
    var iLeft, imgSource, slideGrid, sliderGridXaml, tipText, playTime;
    var indexNodes = imageNodes = projNodes = null;

    var xmlFile = new String(location);
    xmlFile = xmlFile.substring(0, xmlFile.lastIndexOf('/') + 1) + ("index.xml");
		xmlFile = transUrl(xmlFile);

    controlContent.findName('btnIndexMode')["Visibility"] = "Collapsed";
    controlContent.findName('PicIndexMoveButton')["Visibility"] = "Collapsed";

    if (moz) {
        if ((!isChrome) && (!isSafari))//FirFox
        {
            indexXmlDoc = document.implementation.createDocument("", "doc", null);
            indexXmlDoc.async = false;
            try {
                indexXmlDoc.load(xmlFile);
            }
            catch (e) {
            }
        }
        else//Chrome Safari
        {
            var xmlhttp = new window.XMLHttpRequest();
            xmlhttp.open("GET", xmlFile, false);
            try {
                xmlhttp.send(null);
            }
            catch (e) {
            }

            indexXmlDoc = xmlhttp.responseXML;
            indexXmlDoc.async = false;
        }
    }
    else//MS IE
    {
        indexXmlDoc = new ActiveXObject("Microsoft.XMLDOM");
        indexXmlDoc.async = false;
        indexXmlDoc.load(xmlFile);

        if (indexXmlDoc.parseError.errorCode != 0) {
            return;
        }
    }

    indexNodes = indexXmlDoc.getElementsByTagName('index');

    try {
        sStartImage = indexXmlDoc.getElementsByTagName("startimage")[0].getAttribute("image");
    }
    catch (e) {
        sStartImage = null;
    }
    try {
        iStartDuration = parseInt(DecryptString(indexXmlDoc.getElementsByTagName("startimage")[0].getAttribute("duration")));
    }
    catch (e) {
        iStartDuration = 0;
    }
    try {
        sEndImage = indexXmlDoc.getElementsByTagName("endimage")[0].getAttribute("image");
    }
    catch (e) {
        sEndImage = null;
    }
    try {
        iEndDuration = parseInt(DecryptString(indexXmlDoc.getElementsByTagName("endimage")[0].getAttribute("duration")));
    }
    catch (e) {
        iEndDuration = 0;
    }

    iStartDuration = iStartDuration * 1000;
    iEndDuration = iEndDuration * 1000;

    var bHaveText = bHaveImage = false;
    for (var i = 0; i < indexNodes.length; i++) {
        try {
            playTime = DecryptString(indexXmlDoc.getElementsByTagName("index")[i].getAttribute("time"));
        }
        catch (e) {
            playTime = 0;
        }
        try {
            imgSource = DecryptString(indexXmlDoc.getElementsByTagName("index")[i].getAttribute("image"));
        }
        catch (e) {
            imgSource = "";
        }
        if (null != indexXmlDoc.getElementsByTagName("index")[i].childNodes[0]) {
            try {
                tipText = DecryptString(indexXmlDoc.getElementsByTagName("index")[i].childNodes[0].nodeValue);
            }
            catch (e) {
                tipText = "";
            }
        }
        else {
            tipText = "";
        }

        if (tipText) {
            bHaveText = true;
        }
        if (imgSource) {
            bHaveImage = true;
        }
        
        controlContent.ReachPlayerCtrl.getImgPPTInfo(imgSource, playTime, tipText);

    }
    if (bHaveText || bHaveImage) {
        controlContent.findName('btnIndexMode')["Visibility"] = "Visible";
        controlContent.findName('PicIndexMoveButton')["Visibility"] = "Visible";
    }
}

function fnChgVideoPage() {
    try {
        location.href = sChangePath;
        opener.location.reload();
    }
    catch (e) {
    }
}

function fnReadContentInfo(sender, args) {
    var xmlDoc = indexNodes = null;
    var xmlFile = new String(location);
    var CurrentName = "";
    var i;
    var sUrl = xmlFile.substring(0, 4);
    if (sUrl != "http")
        return;

    for (i = 0; i < 3; i++)
    {
        if (i == 2)
            CurrentName = xmlFile.substring(xmlFile.lastIndexOf('/')+1);
        xmlFile = xmlFile.substring(0, xmlFile.lastIndexOf('/'));
    }
    sChangePath = xmlFile;
    xmlFile = xmlFile + "/ContentInfo.xml";
    xmlFile = transUrl(xmlFile);
    if (moz) {
        if ((!isChrome) && (!isSafari))//FirFox
        {
            xmlDoc = document.implementation.createDocument("", "doc", null);
            xmlDoc.async = false;
            try {
                xmlDoc.load(xmlFile);
            }
            catch (e) {
            }
        }
        else//Chrome Safari
        {
            var xmlhttp = new window.XMLHttpRequest();
            xmlhttp.open("GET", xmlFile, false);
            try {
                xmlhttp.send(null);
            }
            catch (e) {
            }
            xmlDoc = xmlhttp.responseXML;
            xmlDoc.async = false;
        }
    }
    else//MS IE
    {
        xmlDoc = new ActiveXObject("Microsoft.XMLDOM");
        xmlDoc.async = false;
        xmlDoc.load(xmlFile);
        if (xmlDoc.parseError.errorCode != 0) {
            return;
        }
    }

    try {
        sFileStyle = xmlDoc.getElementsByTagName("ScmType")[0].childNodes[0].nodeValue;
    }
    catch (e) {
        sFileStyle = "";
    }
    try {
        sTitle = xmlDoc.getElementsByTagName("CName")[0].childNodes[0].nodeValue;
    }
    catch (e) {
        sTitle = "";
    }
    try {
        sAuthor = xmlDoc.getElementsByTagName("MainTeacher")[0].childNodes[0].nodeValue;
    }
    catch (e) {
        sAuthor = "";
    }
    try {
        sTime = xmlDoc.getElementsByTagName("RecDateTime")[0].childNodes[0].nodeValue;
    }
    catch (e) {
        sTime = "";
    }
    try {
        sDescrip = xmlDoc.getElementsByTagName("Notes")[0].childNodes[0].nodeValue;
    }
    catch (e) {
        sDescrip = "";
    }
    try {
        indexNodes = xmlDoc.getElementsByTagName('Type');
        if (indexNodes.length > 1)
        {
            var typename = indexNodes[0].getAttribute("Name");
            if (typename == CurrentName) {
                typename = indexNodes[1].getAttribute("Name");
                sChangePath = sChangePath + "/" + xmlDoc.getElementsByTagName('PC')[1].childNodes[0].nodeValue;
            }
            else {
                sChangePath = sChangePath + "/" + xmlDoc.getElementsByTagName('PC')[0].childNodes[0].nodeValue;
            }
            controlContent.ReachPlayerCtrl.MakeVideoChangeDisp(typename);
        }

    }
    catch (e) {
    }
}

function getAnimateCount(aChannelIndex) {
    if (!animatesArry) {
        return 0;
    }
    else {
        return animatesArry.length;
    }
}

function getAnimateTime(aAnimateIndex) {
    vAnimateObj = animatesArry[aAnimateIndex];

    return vAnimateObj.time;
}

function getAnimateDuration(aAnimateIndex) {
    vAnimateObj = animatesArry[aAnimateIndex];

    return vAnimateObj.duration;
}

function getAnimateShow(aAnimateIndex) {
    vAnimateObj = animatesArry[aAnimateIndex];

    return vAnimateObj.show;
}

function getAnimateAtype(aAnimateIndex) {
    vAnimateObj = animatesArry[aAnimateIndex];

    return vAnimateObj.atype;
}

function getAnimates() {
    var animatesObj = new Array();
    var i, k;

    k = parseInt(indexXmlDoc.getElementsByTagName("animate").length);

    if (k > 0) {
        for (i = 0; i < k; i++) {
            var vObj = new Object();
            try {
                vObj.time = parseInt(DecryptString(indexXmlDoc.getElementsByTagName("animate")[i].getElementsByTagName("time")[0].childNodes[0].nodeValue));
            }
            catch (e) {
                vObj.time = 0;
            }
            try {
                vObj.duration = parseInt(DecryptString(indexXmlDoc.getElementsByTagName("animate")[i].getElementsByTagName("duration")[0].childNodes[0].nodeValue));
            }
            catch (e) {
                vObj.duration = 0;
            }
            try {
                vObj.show = parseInt(DecryptString(indexXmlDoc.getElementsByTagName("animate")[i].getElementsByTagName("show")[0].childNodes[0].nodeValue));
            }
            catch (e) {
                vObj.show = 1;
            }
            try {
                vObj.atype = parseInt(DecryptString(indexXmlDoc.getElementsByTagName("animate")[i].getElementsByTagName("atype")[0].childNodes[0].nodeValue));
            }
            catch (e) {
                vObj.atype = 1;
            }
            animatesObj[i] = vObj;
        }
        return animatesObj;
    }
}

function fnReadVideoInfo(sender, args) {
    var xmlFile = new String(location);
    xmlFile = xmlFile.substring(0, xmlFile.lastIndexOf('/') + 1) + ("info.xml");
		xmlFile = transUrl(xmlFile);
		
    if (moz) {
        if ((!isChrome) && (!isSafari))//FirFox
        {
            infoXmlDoc = document.implementation.createDocument("", "doc", null);
            infoXmlDoc.async = false;
            try {
                infoXmlDoc.load(xmlFile);
            }
            catch (e) {
            }
        }
        else//Chrome Safari
        {
            var xmlhttp = new window.XMLHttpRequest();
            xmlhttp.open("GET", xmlFile, false);
            try {
                xmlhttp.send(null);
            }
            catch (e) {
            }
            infoXmlDoc = xmlhttp.responseXML;
            infoXmlDoc.async = false;
        }
        channelNodes = infoXmlDoc.documentElement.getElementsByTagName('channel');
    }
    else//MS IE
    {
        infoXmlDoc = new ActiveXObject("Microsoft.XMLDOM");
        infoXmlDoc.async = false;
        infoXmlDoc.load(xmlFile);

        if (infoXmlDoc.parseError.errorCode != 0) {
            return;
        }
        channelNodes = infoXmlDoc.documentElement.childNodes;
    }

    try {
        var str = infoXmlDoc.getElementsByTagName('stream')[0].getAttribute("specialflag");
        if (str != null) {
            iSpceialFlag = parseInt(DecryptString(str));
            bIsHuaWei = true;
        }
    }
    catch (e) {
        iSpceialFlag = 0;
    }
    if (bIsHuaWei) {
        try {
            iVideoQual = parseInt(DecryptString(infoXmlDoc.getElementsByTagName('stream')[0].getAttribute("videoquality")));
        }
        catch (e) {
            iVideoQual = 0;
        }
    }
    try {
        iTotaltime = parseFloat(DecryptString(infoXmlDoc.getElementsByTagName('stream')[0].getAttribute("totaltime")));
    }
    catch (e) {
        iTotaltime = 0.0;
    }
    controlContent.findName('TotalTimeText').Text = formatPlayTime(iTotaltime);
    try {
        irepchnidx = parseInt(DecryptString(infoXmlDoc.getElementsByTagName('stream')[0].getAttribute("repchnidx")));
    }
    catch (e) {
        irepchnidx = 0;
    }
}

function getReplaceCount(aChannelIndex) {
    if (!replacesArry) {
        return 0;
    }
    else {
        return replacesArry.length;
    }
}

function getReplaceTime(aReplaceIndex) {
    vReplaceObj = replacesArry[aReplaceIndex];

    return vReplaceObj.time;
}

function getReplaceDuration(aReplaceIndex) {
    vReplaceObj = replacesArry[aReplaceIndex];

    return vReplaceObj.duration;
}

function getReplaceAttr(aReplaceIndex) {
    vReplaceObj = replacesArry[aReplaceIndex];

    return vReplaceObj.attr;
}

function getReplaceSrc(aReplaceIndex) {
    vReplaceObj = replacesArry[aReplaceIndex];

    return vReplaceObj.src;
}

function getReplaces() {
    var replacesObj = new Array();
    var i, k;

    k = parseInt(infoXmlDoc.getElementsByTagName("replace").length);

    if (k > 0) {
        for (i = 0; i < k; i++) {
            var vObj = new Object();
            try {
                vObj.time = parseInt(DecryptString(infoXmlDoc.getElementsByTagName("replace")[i].getElementsByTagName("time")[0].childNodes[0].nodeValue));
            }
            catch (e) {
                vObj.time = 0;
            }
            try {
                vObj.duration = vObj.time + parseInt(DecryptString(infoXmlDoc.getElementsByTagName("replace")[i].getElementsByTagName("duration")[0].childNodes[0].nodeValue));
            }
            catch (e) {
                vObj.duration = 0;
            }
            try {
                vObj.attr = parseInt(DecryptString(infoXmlDoc.getElementsByTagName("replace")[i].getElementsByTagName("attr")[0].childNodes[0].nodeValue));
            }
            catch (e) {
                vObj.attr = 1;
            }
            try {
                vObj.src = DecryptString(infoXmlDoc.getElementsByTagName("replace")[i].getElementsByTagName("src")[0].childNodes[0].nodeValue);
            }
            catch (e) {
                vObj.src = "";
            }
            replacesObj[i] = vObj;
        }
        return replacesObj;
    }
}

function getChannelCount() {
    var iLen;
    try {
        iLen = infoXmlDoc.getElementsByTagName("channel").length;
    }
    catch (e) {
        iLen = 1;
    }
    if (iLen > 4) {
        iLen = 4;
    }
    return iLen;
}

function getPauseTime(ipauseID) {
    var time;
    try {
        time = parseInt(DecryptString(infoXmlDoc.getElementsByTagName("child")[ipauseID].getAttribute("time")));
    }
    catch (e) {
        time = 0;
    }
    return time;
}

function getPauseDura(ipauseID) {
    var dura;
    try {
        dura = parseInt(DecryptString(infoXmlDoc.getElementsByTagName("child")[ipauseID].getAttribute("duration")));
    }
    catch (e) {
        dura = 0;
    }
    return dura;
}

function getPauseCount() {
    var i, k;
    try {
        k = infoXmlDoc.getElementsByTagName("child").length;
    }
    catch (e) {
        k = 0;
    }
    return k;
}

function getConfTime(ipauseID) {
    var time;
    try {
        time = parseInt(DecryptString(infoXmlDoc.getElementsByTagName("conftype")[ipauseID].getAttribute("begintime")));
    }
    catch (e) {
        time = 0;
    }
    return time;
}

function getConfType(ipauseID) {
    var type;
    try {
        type = parseInt(DecryptString(infoXmlDoc.getElementsByTagName("conftype")[ipauseID].getAttribute("type")));
    }
    catch (e) {
        type = 0;
    }
    return type;
}

function getConfTypeCount() {
    var i, k;
    try {
        k = infoXmlDoc.getElementsByTagName("conftype").length;
    }
    catch (e) {
        k = 0;
    }
    return k;
}

function getFilesType(aChannelIndex) {
    var sType;
    try {
        sType = channelNodes[aChannelIndex].getElementsByTagName("files")[0].getAttribute("type");
    }
    catch (e) {
        sType = "";
    }
    return sType;
}

function getBlueScreenCount(aChannelIndex) {
    var iLen;
    try {
        iLen = channelNodes[aChannelIndex].getElementsByTagName("bluetime").length;
    }
    catch (e) {
        iLen = 0;
    }
    return iLen;
}

function getBlueScreenStart(aChannelIndex, aClipIndex) {
    var iLen;
    try {
        iLen = parseFloat(DecryptString(channelNodes[aChannelIndex].getElementsByTagName("bluetime")[aClipIndex].getAttribute("start")));
    }
    catch (e) {
        iLen = 0;
    }
    return iLen;
}

function getBlueScreenEnd(aChannelIndex, aClipIndex) {
    var iLen;
    try {
        iLen = parseFloat(DecryptString(channelNodes[aChannelIndex].getElementsByTagName("bluetime")[aClipIndex].getAttribute("end")));
    }
    catch (e) {
        iLen = 0;
    }
    return iLen;
}

function getClipVideoCount(aChannelIndex) {
    var iLen;
    try {
        iLen = channelNodes[aChannelIndex].getElementsByTagName("file").length;
    }
    catch (e) {
        iLen = 1;
    }
    return iLen;
}

function getClipVideoFileName(aChannelIndex, aClipIndex) {
    var sFileName;
    try {
        sFileName = DecryptString(channelNodes[aChannelIndex].getElementsByTagName("file")[aClipIndex].childNodes[0].nodeValue);
    }
    catch (e) {
        sFileName = "";
    }
    return sFileName;
}

function getClipVideoStartTime(aChannelIndex, aClipIndex) {
    var iLen;
    try {
        iLen = parseInt(DecryptString(channelNodes[aChannelIndex].getElementsByTagName("file")[aClipIndex].getAttribute("starttime")));
    }
    catch (e) {
        iLen = 0;
    }
    return iLen;
}

function getClipVideoTime(aChannelIndex, aClipIndex) {
    var iLen;
    try {
        iLen = parseInt(DecryptString(channelNodes[aChannelIndex].getElementsByTagName("file")[aClipIndex].getAttribute("totaltime")));
    }
    catch (e) {
        iLen = 0;
    }
    return iLen;
}

function getPicStartTime(aChannelIndex, aClipIndex) {
    var iLen;
    try {
        iLen = parseInt(DecryptString(channelNodes[aChannelIndex].getElementsByTagName("file")[aClipIndex].getAttribute("starttime")));
    }
    catch (e) {
        iLen = 0;
    }
    return iLen;
}

function getClipWidth(aChannelIndex, aClipIndex) {
    var iLen;
    try {
        iLen = parseInt(DecryptString(channelNodes[aChannelIndex].getElementsByTagName("file")[aClipIndex].getAttribute("width")));
    }
    catch (e) {
        iLen = 0;
    }
    return iLen;
}

function getClipHeight(aChannelIndex, aClipIndex) {
    var iLen;
    try {
        iLen = parseInt(DecryptString(channelNodes[aChannelIndex].getElementsByTagName("file")[aClipIndex].getAttribute("height")));
    }
    catch (e) {
        iLen = 0;
    }
    return iLen;
}

function getaVideoNoDataCount(aChannelInde) {
    var iLen;
    try {
        iLen = channelNodes[aChannelInde].getElementsByTagName("nodatatime").length;
    }
    catch (e) {
        iLen = 0;
    }
    return iLen;
}

function getNoDataStartTime(aChannelIndex, aClipIndex) {
    var iLen;
    try {
        iLen = parseInt(DecryptString(channelNodes[aChannelIndex].getElementsByTagName("nodatatime")[aClipIndex].getAttribute("start")));
    }
    catch (e) {
        iLen = 0;
    }
    return iLen;
}

function getNoDataEndTime(aChannelIndex, aClipIndex) {
    var iLen;
    try {
        iLen = parseInt(DecryptString(channelNodes[aChannelIndex].getElementsByTagName("nodatatime")[aClipIndex].getAttribute("end")));
    }
    catch (e) {
        iLen = 0;
    }
    return iLen;
}

function fnChgQulPage(sender, args) {
    try {
        var url = new String(transUrl(location));
        if (iVideoQual == 1)
            url = url.substring(0, url.lastIndexOf('_L/')) + ("_H/player.html");
        else if (iVideoQual == 2)
            url = url.substring(0, url.lastIndexOf('_H/')) + ("_L/player.html");
        else return;
        location.href = url;
        opener.location.reload();
    }
    catch (e) {
    }
}

function fnGoBack(sender, args) {
    try {
        history.go(-1);
    }
    catch (e) {
    }
}

function formatPlayTime(aSeconds) {
    var hrs = Math.floor(aSeconds / 3600);
    var min = Math.floor(aSeconds % 3600 / 60);
    var sec = Math.round(aSeconds % 60);
    var str = "";
    sec > 9 ? str += sec : str += '0' + sec;
    min > 9 ? str = min + ":" + str : str = '0' + min + ":" + str;
    if (hrs <= 0) {
        str = "00:" + str;
    }
    else
        if (hrs > 0 && hrs <= 9) {
            str = '0' + hrs + ":" + str;
        }
        else {
            str = hrs + ":" + str;
        }
    return str;
}

function getCurrentPlayTime() {
    if (controlContent) {
        if (controlContent.ReachPlayerCtrl) {
            return controlContent.ReachPlayerCtrl.dCurrentPlayTime;
        }
    }
    return 0;
}

function setCurrentPlayTime(time) {
    if (controlContent) {
        if (controlContent.ReachPlayerCtrl) {
            controlContent.ReachPlayerCtrl.fnSeekPlayTime(time);
        }
    }
}

function getTotalTime() {
    if (controlContent) {
        if (controlContent.ReachPlayerCtrl) {
            return controlContent.ReachPlayerCtrl.dTotaltime;
        }
    }
    return 0;
}

var xmlHttp;
function createXMLHttpRequest() {
    if (window.ActiveXObject) {
        xmlHttp = new ActiveXObject("Microsoft.XMLHTTP");
    }
    else {
        if (window.XMLHttpRequest) {
            xmlHttp = new XMLHttpRequest();
        }
    }
}

//function sendTimeInfoToServer(time) {
//    if (nUserId == -1 || nFileId == -1) {
//        return;
//    }
//    createXMLHttpRequest();
//    var url = " http://" + sServerIp + "/MakeTime4File?userid=" + nUserId + "&fileid=" + nFileId
//                + "&time=" + time;
//    xmlHttp.open("GET", url, true);
//    xmlHttp.onreadystatechange = function () { goRepeatStatus(); };
//    xmlHttp.send(null);
//}

//function goRepeatStatus() {
//    if (xmlHttp.readyState == 4) {
//        if (xmlHttp.status == 200)
//        { }
//    }
//}

function onSilverlightError(sender, args) {
    var appSource = "";
    if (sender != null && sender != 0) {
        appSource = sender.getHost().Source;
    }
    var errorType = args.ErrorType;
    var iErrorCode = args.ErrorCode;

    if (errorType == "ImageError" || errorType == "MediaError")
        return;

    var errMsg = "Silverlight unsettled error " + appSource + "\n";

    errMsg += "Code: " + iErrorCode + "\n";
    errMsg += "Category: " + errorType + "\n";
    errMsg += "Message: " + args.ErrorMessage + "\n";

    if (errorType == "ParserError") {
        errMsg += "File: " + args.xamlFile + "\n";
        errMsg += "Line: " + args.lineNumber + "\n";
        errMsg += "Position: " + args.charPosition + "\n";
    }
    else if (errorType == "RuntimeError") {
        if (args.lineNumber != 0) {
            errMsg += "Line: " + args.lineNumber + "\n";
            errMsg += "Position: " + args.charPosition + "\n";
        }
        errMsg += "MethodName: " + args.methodName + "\n";
    }

    throw new Error(errMsg);
}