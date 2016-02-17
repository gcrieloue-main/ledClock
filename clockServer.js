var restify = require('restify');
var ledMatrix = require('./ledMatrix.js')
var Twit = require ('twit')
var server = restify.createServer();

var currentWatchedTag = 'Hue'
var T = new Twit({
    consumer_key:         'a2iW2Jax85cfYuouCdnDeIZ6u'
    , consumer_secret:      'bDv23WRDAoUHWZZ1LGD7OMGxQ04BhrQlnv2uuOX5AB67jz7jGa'
    , access_token:         '19644765-F47N3C07M7sDVhE3r5tAwwDxXdvhpYEgTB9HJACAy'
    , access_token_secret:  'Ls28b9ixAp0xG5dDclqiR0onHljbOEOEZYyd9iUNJj9LS'
})

// enable CORS for cross domain queries
server.use(restify.CORS());
server.use(restify.fullResponse());
restify.CORS.ALLOW_HEADERS.push('authorization');

// URL pathes list
server.get('/raw/:command', raw);
server.get('/brightness/:value', setBrightness);
server.get('/mode/:mode', setMode);
server.get('/preset/:preset', setPreset);
server.get('/color/:r/:g/:b', setColor);
server.get('/color2/:r/:g/:b', setColor2);
server.get('/backgroundColor/:r/:g/:b', setBackgroundColor);
server.get('/text/:content/:repetition/:title', setText);
server.get('/text/:content/:repetition', setText);
server.get('/text/:content', setText);
server.get('/animate/:animation', animate);
server.get('/countdown/:min', countdown);
server.get('/help/', help);

function help(req, res, next)
{
    res.contentType = 'json';
    res.send([
        {url:'/brightness/value', description: 'set the led matrix brightness.'},
        {url:'/color/r/g/b', description : 'change the led matrix main color.'}, 
        {url:'/help', description:'you see it right now.'},
        {url:'/text/content(/repetition)', description:'scroll a text a specified number of times (default : one time)'}
    ]);
    return next();
}

function sendCommand()
{
    var command = Array.prototype.join.call(arguments, ':');
    console.log(command);
    try {
        ledMatrix.send(command);
    }
    catch (err){
        console.log(err);
        console.log("command failed");
    }
}

function raw(req, res, next) {
    res.send("'"+req.params.command+"' sent");
    sendCommand(req.params.command)
    next();
}

function isInt(value) {
    var x = parseFloat(value);
    return !isNaN(value) && (x | 0) === x;
}

function isDefined(param)
{
    return typeof(param) != "undefined";
}

function checkIfValueInColorRange(value)
{
    if (!isInt(value) || value <0 || value > 255){
        return new Error(value+" should be an integer between 0 and 255");
    }
    return null;
}

function checkIfIntegerValue(value)
{
    if (!isInt(value)) {
        return new Error("In case you don't know, "+value+" is not an integer");
    }
    return null;
}

function initClockSettings(){
    sendCommand("color",80,80,200);
}

function setBrightness(req, res, next) {
    var value = req.params.value;

    var error = checkIfIntegerValue(value);

    if (error == null){
        sendCommand("bright", value)
        res.send("Brightness set to "+value);
        return next();
    }
    return next(error);
}

function setMode(req, res, next) {
    var mode = req.params.mode;
    var error = checkIfIntegerValue(mode);
    if (error == null){
        sendCommand("mode", mode)
        res.send("Mode set to "+mode);
        return next();
    }
    return next(error);
}

function setPreset(req, res, next) {
    var preset = req.params.preset;
    var error = checkIfIntegerValue(preset);
    if (error == null) {
        res.send("Preset set to "+preset);
        if (preset == 1) 
        {
            sendCommand("backgroundColor",0,0,0);
            sendCommand("color",80,80,200);
            sendCommand("color2",200,80,80);
            sendCommand("brightness",70);
        }
        else if (preset == 2) 
        {
            sendCommand("backgroundColor",2,2,2);
            sendCommand("color",80,80,200);
            sendCommand("color2",200,80,80);
            sendCommand("brightness",70);
        }
        return next();
    }
    return next(error);
}

function animate(req, res, next) {
    var animation = req.params.animation;
    sendCommand("animate", animation);
    res.send("animate : "+animation);
    next();
}

function countdown(req, res, next) {
    var min = req.params.min;
    sendCommand("countdown", min);
    res.send("countdown : "+min);
    next();
}

function setColor(req, res, next) {
    var r = req.params.r;
    var g = req.params.g;
    var b = req.params.b;

    var error = checkIfValueInColorRange(r);
    if (error == null){
        error = checkIfValueInColorRange(g);
    }
    if (error == null){
        error = checkIfValueInColorRange(b);
    }

    if (error == null){
        sendCommand("color",r,g,b);
        res.send("Color set to rgb("+r
                 +","+g
                 +","+b+")")
        return next();
    }
    else return next(error);
}

function setColor2(req, res, next) {
    var r = req.params.r;
    var g = req.params.g;
    var b = req.params.b;

    var error = checkIfValueInColorRange(r);
    if (error == null){
        error = checkIfValueInColorRange(g);
    }
    if (error == null){
        error = checkIfValueInColorRange(b);
    }

    if (error == null){
        sendCommand("color2",r,g,b);
        res.send("Color2 set to rgb("+r
                 +","+g
                 +","+b+")")
        return next();
    }
    return next(error);
}

function setBackgroundColor(req, res, next) {
    var r = req.params.r;
    var g = req.params.g;
    var b = req.params.b;
    var error = checkIfValueInColorRange(r);
    if (error == null){
        error = checkIfValueInColorRange(g);
    }
    if (error == null){
        error = checkIfValueInColorRange(b);
    }

    if (error == null){
        sendCommand("backgroundColor",r,g,b)
        res.send("BackgroundColor set to rgb("+r
                 +","+g
                 +","+b+")")
        return next();
    }
    return next(error);
}

function setText(req, res, next) {
    var error = null;
    if (isDefined(req.params.repetition))
    {
        error = checkIfIntegerValue(req.params.repetition);
    }
    if (error == null){
        var nbRepetition = 1;
        if (isDefined(req.params.repetition)) {
            nbRepetition = req.params.repetition;
        }
        if (nbRepetition > 5)
        {
            nbRepetition = 5;
        }
        var content = req.params.content;
        content = cleanText(content);
        if (isDefined(req.params.title)) {
            sendCommand("text",content,nbRepetition,req.params.title);
        }
        else {
            sendCommand("text",content,nbRepetition);
        }
        res.send("Text '"+req.params.content+"' will repeat "
                 +nbRepetition+" times");
        return next();
    }
    return next(error);
}

server.listen(8080, function(){
    console.log('server listening…');
});

var cleanText = function(tweet)
{
    var urlRegex =/(\b(https?|ftp|file):\/\/[-A-Z0-9+&@#\/%?=~_|!:,.;]*[-A-Z0-9+&@#\/%=~_|])/ig;
    var cleanString = tweet.replace(urlRegex,'');
    cleanString = cleanString.replace(/[^a-z0-9^_°àáâãäåçèéêëìíîïðòóôõöùúûüýÿ@#\.\(\)\[\]\"\'\-,;\/!\? ]/gi,'').replace(/[\n\r]/g, ' ');
    return cleanString;
};

var stream = T.stream('statuses/filter', { follow: ['57732899','901963182'] })

stream.on('tweet', function (tweet) {
    if ((typeof(tweet.retweeted_status) == 'undefined') 
        && (tweet.in_reply_to_user_id == null)) // neither retweet nor reply
    {
        var tweetToDisplay = cleanText(tweet.text);
        console.log(tweet.text);
        if (!!tweetToDisplay)
        {
            sendCommand('text',tweetToDisplay,1, "TWEET");
        }
    }
});

stream.on('error', function (err) {
    console.log(err);
    sendCommand("text:Error "+err.message+":3");
    stream.stop();
});

initClockSettings();
