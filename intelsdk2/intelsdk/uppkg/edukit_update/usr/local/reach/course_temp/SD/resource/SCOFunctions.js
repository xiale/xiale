var startDate;
var exitPageStatus;
var isRecordData;
var isPlayOver;
var supendtime;
//var timer_instance;
//TAG_SHEEP_V3)
function loadPage() {// loadPage方法被ReachPlayer.js的playerOnLoad方法调用，playerOnLoad方法注册在content.htm的object对象上
	
	startDate = new Date().getTime();
	exitPageStatus = false;
	isRecordData 	= false;
	isPlayOver		= false;

	var result = doLMSInitialize();
	
	if(result == "true") {// 连接LMS成功
		
		var mode = doLMSGetValue("cmi.core.lesson_mode");// 三种模式，正常normal、预览browse、复习review
		if(mode == "normal" || mode == "") {// 正常模式才记录学习数据//在moodle2.1下不返回normal而是空（TODO）
			
			isRecordData = true;
			doLMSSetValue("cmi.core.score.min", 0);
			doLMSSetValue("cmi.core.score.max", 100);
			
			var status = doLMSGetValue( "cmi.core.lesson_status" );

			if (status == "not attempted") {// 改变状态
				// the student is now attempting the lesson
				doLMSSetValue( "cmi.core.lesson_status", "incomplete" );
				
			}
			
			//we want to set the exit type to suspend immediately because some LMS's only store data if they get a suspend request
//			doLMSSetValue("cmi.core.exit", "SUSPEND");
			
			var entry = doLMSGetValue("cmi.core.entry");
			if(status != "completed" && entry != "ab-initio") {// 重复学习resume、首次学习ab-initio、其他空
				//自动从上次结束点开始，不再提问，这里需要注意，如果直接注释掉并不能实现，需要延时处理
				//if(confirm("would you like to resume the presentation from the last slide viewed?")) {
					
					supendtime = doLMSGetValue("cmi.core.lesson_location");
					if(supendtime == "")
						supendtime = 0;
					
					if(supendtime > 0){
						setTimeout("beginPlay()", 1000);
						
					}
				//}
			}
		}				
	}
}

function beginPlay(){
	setCurrentPlayTime(Math.round(supendtime));
}

function computeTime() {//doLMSSetValue只是操作本地内存，没有持久化

	if(isRecordData) {
		// 添加学习时间，会自动累加到cmi.core.total_time里
		if ( startDate != 0 ){
			var currentDate = new Date().getTime();
			var formattedTime = convertTotalSeconds( (currentDate - startDate) / 1000 );
			
		}else{
			formattedTime = "00:00:00.0";	
			
		}
		
		doLMSSetValue("cmi.core.session_time", formattedTime);
		
		// 保存播放进度
		var playTime 	= getCurrentPlayTime();//得到的是秒
		doLMSSetValue("cmi.core.lesson_location", (String)(Math.round(playTime)));
		
		// 分数计算
		var totalTime 	= getTotalTime();//得到的是秒
		var score 		= Math.round(playTime * 100 / totalTime);
		doLMSSetValue("cmi.core.score.raw", score);
		
		if (score == 100) {
			doLMSSetValue("cmi.core.lesson_status", "completed");
			
		}
	}
}

function onEnd() {
	
   if (isRecordData) {	   
	   computeTime();
	   doLMSSetValue("cmi.core.score.raw", 100);
	   doLMSSetValue("cmi.core.lesson_status", "completed");
	   doLMSCommit();
	   
   }
   
   isPlayOver = true;
}

function doBack()
{
   doLMSSetValue( "cmi.core.exit", "suspend" );

   //computeTime();
   exitPageStatus = true;
   
   var result;

   result = doLMSCommit();

	// NOTE: LMSFinish will unload the current SCO.  All processing
	//       relative to the current page must be performed prior
	//		 to calling LMSFinish.   
   
   result = doLMSFinish();

}

function doContinue( status )
{
   // Reinitialize Exit to blank
   doLMSSetValue( "cmi.core.exit", "" );

   var mode = doLMSGetValue( "cmi.core.lesson_mode" );

   if ( mode != "review"  &&  mode != "browse" )
   {
      doLMSSetValue( "cmi.core.lesson_status", status );
   }
 
   //computeTime();
   exitPageStatus = true;
   
   var result;
   result = doLMSCommit();
	// NOTE: LMSFinish will unload the current SCO.  All processing
	//       relative to the current page must be performed prior
	//		 to calling LMSFinish.   

   result = doLMSFinish();

}

function doQuit( status )
{
   
   exitPageStatus = true;
   
   if(isRecordData && !isPlayOver){//记录模式	&& 没有播完----------页面被关闭了
	   computeTime();
	   doLMSCommit();
   }

//   result = doLMSSetValue("cmi.core.lesson_status", status);
   
	// NOTE: LMSFinish will unload the current SCO.  All processing
	//       relative to the current page must be performed prior
	//		 to calling LMSFinish.   

   doLMSFinish();
}

/*******************************************************************************
** The purpose of this function is to handle cases where the current SCO may be 
** unloaded via some user action other than using the navigation controls 
** embedded in the content.   This function will be called every time an SCO
** is unloaded.  If the user has caused the page to be unloaded through the
** preferred SCO control mechanisms, the value of the "exitPageStatus" var
** will be true so we'll just allow the page to be unloaded.   If the value
** of "exitPageStatus" is false, we know the user caused to the page to be
** unloaded through use of some other mechanism... most likely the back
** button on the browser.  We'll handle this situation the same way we 
** would handle a "quit" - as in the user pressing the SCO's quit button.
*******************************************************************************/
function unloadPage( status )
{
	
	if (exitPageStatus != true)
	{
		doQuit( status );
	}

	// NOTE:  don't return anything that resembles a javascript
	//		  string from this function or IE will take the
	//		  liberty of displaying a confirm message box.
	
}

/*******************************************************************************
** this function will convert seconds into hours, minutes, and seconds in
** CMITimespan type format - HHHH:MM:SS.SS (Hours has a max of 4 digits &
** Min of 2 digits
*******************************************************************************/
function convertTotalSeconds(ts)
{
   var sec = (ts % 60);

   ts -= sec;
   var tmp = (ts % 3600);  //# of seconds in the total # of minutes
   ts -= tmp;              //# of seconds in the total # of hours

   // convert seconds to conform to CMITimespan type (e.g. SS.00)
   sec = Math.round(sec*100)/100;
   
   var strSec = new String(sec);
   var strWholeSec = strSec;
   var strFractionSec = "";

   if (strSec.indexOf(".") != -1)
   {
      strWholeSec =  strSec.substring(0, strSec.indexOf("."));
      strFractionSec = strSec.substring(strSec.indexOf(".")+1, strSec.length);
   }
   
   if (strWholeSec.length < 2)
   {
      strWholeSec = "0" + strWholeSec;
   }
   strSec = strWholeSec;
   
   if (strFractionSec.length)
   {
      strSec = strSec+ "." + strFractionSec;
   }


   if ((ts % 3600) != 0 )
      var hour = 0;
   else var hour = (ts / 3600);
   if ( (tmp % 60) != 0 )
      var min = 0;
   else var min = (tmp / 60);

   if ((new String(hour)).length < 2)
      hour = "0"+hour;
   if ((new String(min)).length < 2)
      min = "0"+min;

   var rtnVal = hour+":"+min+":"+strSec;

   return rtnVal;
}
 