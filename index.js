

var screen = require('./build/Release/screen');

var result = screen.getShape();
console.log(result)

console.log("start")
var result = screen.start(function (x, y, w, h){
    console.log("update shape", x, y, w, h)
}, function(x, y) {
    console.log("mouse move", x, y)
}, function(m, n) {
    console.log("callbacke", m, n)
});

setTimeout(function() {
    console.log("----------stop")
    screen.stop()
}, 3500)


setTimeout(function() {
    console.log("----------restart")
    var result = screen.start(function (x, y, w, h){
        console.log("update shape", x, y, w, h)
    }, function(x, y) {
        console.log("mouse move", x, y)
    }, function(m, n) {
        console.log("callbacke", m, n)
    });
}, 6500)