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
    return command;
}

function raw(req, res, next) {
    sendCommand(req.params.command)
    next();
}

function isDefined(param)
{
    return typeof(param) != "undefined";
}

function Pass(item)
{
    this.item = item;
}

function Fail(item, msg){
    this.item = item;
    this.msg = msg;
}

function ValidationSummary(x, y){
    this.pass = [];
    this.fail = [];
     
    if (isDefined(x))
    {
        this.pass = x;
    }
    if (isDefined(y))
    {
        this.pass = y;
    }

    this.isValid = function()
    {
        return this.fail.length == 0;
    }

    this.formatErrors = function()
    {
        var msg = this.fail.reduce(
                function (a,b){
                    if (a!=null){
                        return a.msg + b.msg;
                    }
                    else {
                        return b.msg;
                    }
                },null);
        return msg;
    }
}

function merge(precedent, current)
{
    var validationSummary;
    if (precedent == null)
    {
        validationSummary = new ValidationSummary()
    }
    else {
        validationSummary = precedent;
    }
    if (current instanceof Pass){
        validationSummary.pass = validationSummary.pass.concat(current);
    }
    else if (current instanceof Fail)
    {
        validationSummary.fail = validationSummary.fail.concat(current);
    }
    else if (current instanceof ValidationSummary)
    {
        validationSummary.pass = validationSummary.pass.concat(current.pass);
        validationSummary.fail = validationSummary.fail.concat(current.fail);
    }
    else {
        throw "invalid merge parameter";
    }
    return validationSummary;
}

function validateValue(valueAndRules)
{
    var value = valueAndRules[0];
    var rules = valueAndRules[1];
    if (rules.length > 0)
    {
        var map = rules.map(function(rule) {
            return rule.apply(rule, [value])
        });
        return map.reduce(merge, null);
    }
    return new ValidationSummary([new Pass(value)]);
}

function validateValues(valuesAndRules)
{
    return valuesAndRules.map(validateValue).reduce(merge, null);
}

function isInt(value) {
    var x = parseFloat(value);
    return !isNaN(value) && (x | 0) === x;
}

function checkIfValueInColorRange(value)
{
    if (!isInt(value) || value <0 || value > 255){
        return new Fail(value, value+" should be an integer between 0 and 255");
    }
    return new Pass(value);
}

function checkIfIntegerValue(value)
{
    if (!isInt(value)) {
        return new Fail(value, "In case you don't know, "+value+" is not an integer");
    }
    return new Pass(value);
}

function initClockSettings(){
    sendCommand("color",80,80,200);
}

function restToLed(command, valuesNamesAndRules)
{
    return function(req,res,next){
        valuesAndRules = valuesNamesAndRules.map(function(valueNameAndRules){
            return [req.params[valueNameAndRules[0]],valueNameAndRules[1]];
        });
        var validation = validateValues(valuesAndRules);
        if (validation.isValid()){
            res.send(
                sendCommand.apply(sendCommand, [command].concat(valuesAndRules.map(
                    function(valueAndRules){
                        return valueAndRules[0]
                    })))
                );
            return next();
        }
        return next(new Error(validation.formatErrors()));
    };
}

function setPreset(req, res, next) {
    var preset = req.params.preset;
    var validation = validateValue([preset, [checkIfIntegerValue]] );
    if (validation.isValid()){
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
    return next(new Error(validation.formatErrors()));
}

// URL pathes list
server.get('/raw/:command', raw);
server.get('/brightness/:value', restToLed("bright",[["value",[checkIfIntegerValue]]]));
server.get('/mode/:mode', restToLed("mode",[["mode",[checkIfIntegerValue]]]));
server.get('/preset/:preset', setPreset);
server.get('/color/:r/:g/:b', restToLed("color", [
            ["r", [checkIfValueInColorRange]],
            ["g", [checkIfValueInColorRange]],
            ["b", [checkIfValueInColorRange]]]));
server.get('/color2/:r/:g/:b', restToLed("color2", [
            ["r", [checkIfValueInColorRange]],
            ["g", [checkIfValueInColorRange]],
            ["b", [checkIfValueInColorRange]]]));
server.get('/backgroundColor/:r/:g/:b', restToLed("backgroundColor", [
            ["r", [checkIfValueInColorRange]],
            ["g", [checkIfValueInColorRange]],
            ["b", [checkIfValueInColorRange]]]));
server.get('/text/:content/:repetition/:title', restToLed("text",[
            ["content", []],
            ["repetition", [checkIfIntegerValue]],
            ["title", []]]));
server.get('/text/:content/:repetition', restToLed("text",[
            ["content", []],
            ["repetition", [checkIfIntegerValue]]]));
server.get('/text/:content', restToLed("text",[
            ["content", []]]));
server.get('/animate/:animation', restToLed("animate",[["animation",[]]]));
server.get('/countdown/:min', restToLed("countdown",[["min",[checkIfIntegerValue]]]));
server.get('/help/', help);

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
