// Event dispatcher

var Events = {};

function Event(name) {
    this.name = name;
    this.listeners = [];
    Events[name] = this;
}

Event.prototype.addListener = function(cb, context) {
    this.listeners.push({
        "cb": cb,
        "context":context
    });
};

Event.prototype.emit = function(data) {
    for(var i=0;i<this.listeners.length;i++)
        this.listeners[i].cb.call(context, data);
};


function EventInterface() {}
EventInterface.prototype.emitEvent = function (e,data) {
    if(!Events[e]) Events[e] = new Event(e);
    Events[e].emit(data);
};

EventInterface.prototype.onEvent = function (e, cb) {
    if(!Events[e]) Events[e] = new Event(e);
    Events[e].addListener(cb, this);
};

