

var screen = require('./build/Release/screen');

var result = screen.getShape();
console.log(result)

console.log("start")

var twoPI = Math.PI * 2.0;
var screenSize = {height:1080, width:1920}
var height = (screenSize.height / 2) - 10;
var width = screenSize.width;

var x = 0;
var interval = setInterval(function() {
    y = height * Math.sin((twoPI * x) / width) + height;
	screen.warpCursor(x, y);
    x++
}, 3);
  

setTimeout(function() {
    clearInterval(interval);
    screen.setCursorVisible(true)
    console.log("----------stop")
    screen.stop()
}, 13500)


setTimeout(function() {
    // screen.setCursorVisible(false)
    console.log("----------restart")
    var result = screen.start(function (x, y, w, h){
        console.log("update shape", x, y, w, h)
    }, function(x, y) {
        console.log("mouse move", x, y)
    }, function(m, n) {
        console.log("callbacke", m, n)
    });
}, 1500)