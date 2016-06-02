
// JSON 对象打点的列表
var vodDots = {"indexs":[
{"time":"1000", "image":"H5Player/images/1.jpg", "content":"4444"},
{"time":"2000", "image":"H5Player/images/2.jpg", "content":"3333"},
{"time":"3000", "image":"H5Player/images/3.jpg", "content":"2222"},
{"time":"4000", "content":"11111111111111111111111111111111111111111111111111"}
]};

// 打点的索引列表, 以json对象的方式传入
vodInitDots( vodDots );

// 插入打点的json对象
vodSetDots( {"time":"5000", "image":"H5Player/images/4.jpg", "content":"0000"} );

// 播放
//play();

// 跳转到第一个小时开始播放
vodSetCurrentTime( 3600 );

// 获取当前播放的时间
AlertText( vodGetCurrentTime() );

AlertText( "</br>" );

// 获取总的播放的时间
AlertText( vodGetTotalTime() );


